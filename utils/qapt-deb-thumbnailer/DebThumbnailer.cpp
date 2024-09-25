/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *   Copyright © 2014 Harald Sitter <apachelogger@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "DebThumbnailer.h"

#include <QDir>
#include <QStringBuilder>
#include <QImage>
#include <QPainter>
#include <QIcon>
#include <KPluginFactory>

#include <QApt/DebFile>

K_PLUGIN_CLASS_WITH_JSON(DebThumbnailer, "debthumbnailer.json")

DebThumbnailer::DebThumbnailer(QObject *parent, const QVariantList &args)
        : KIO::ThumbnailCreator(parent, args) {
}

DebThumbnailer::~DebThumbnailer() {
}

// Function to extract numbers and non-numeric parts from the string
QStringList tokenizeString(const QString &str) {
    QStringList tokens;
    QRegularExpression re(QStringLiteral("(\\d+|\\D+)"));
    QRegularExpressionMatchIterator i = re.globalMatch(str);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        tokens << match.captured(0);
    }
    return tokens;
}

// Custom natural sorting comparator function (thanks to ChatGPT)
bool naturalCompare(const QString &a, const QString &b) {
    QStringList tokensA = tokenizeString(a);
    QStringList tokensB = tokenizeString(b);

    int n = std::min(tokensA.size(), tokensB.size());

    for (int i = 0; i < n; ++i) {
        bool aIsNum, bIsNum;
        int numA = tokensA[i].toInt(&aIsNum);
        int numB = tokensB[i].toInt(&bIsNum);

        if (aIsNum && bIsNum) {
            if (numA != numB) {
                return numA < numB;
            }
        } else {
            if (tokensA[i] != tokensB[i]) {
                return tokensA[i] < tokensB[i];
            }
        }
    }
    return tokensA.size() < tokensB.size();
}

KIO::ThumbnailResult DebThumbnailer::create(const KIO::ThumbnailRequest &request) {
    const QApt::DebFile debFile(request.url().toLocalFile());

    if (!debFile.isValid()) {
        qDebug() << Q_FUNC_INFO << "debfile not valid";
        return KIO::ThumbnailResult::fail();
    }

    QStringList iconsList = debFile.iconList();

    // Drop everything but pngs and xpms.
    // ::iconList is based on ::fileList which contrary to what the name suggests
    // does a full content list including parent directories.
    // To get sensible results we therefore need to discard everything we cannot
    // identify as supported.
    // TODO: should debfile ever get more sensible this should be changed to
    //       exclude unsupported formats rather than include supported ones.
    QMutableListIterator<QString> it(iconsList);
    while (it.hasNext()) {
    QString icon = it.next();
        if ((!icon.endsWith(QStringLiteral(".png")) &&
        !icon.endsWith(QStringLiteral(".svg")) &&
        !icon.endsWith(QStringLiteral(".svgz"))) ||
        icon.contains(QStringLiteral("symbolic")) ||
        icon.contains(QStringLiteral("/actions/"))) {
            it.remove();
        }
    }

    if (iconsList.isEmpty()) {
        return KIO::ThumbnailResult::fail();
    }

    std::sort(iconsList.begin(), iconsList.end(), naturalCompare);

    QString iconPath = iconsList.last();

    // FIXME: two users at the same time cannot use the thumbnailer or bad things happen
    QDir tempDir = QDir::temp();
    tempDir.mkdir(QStringLiteral("kde-deb-thumbnailer"));

    QString destPath = QDir::tempPath() % QStringLiteral("/kde-deb-thumbnailer/");

    if (!debFile.extractFileFromArchive(iconPath, destPath)) {
        return KIO::ThumbnailResult::fail();
    }

    QPixmap mimeIcon = QIcon::fromTheme(QStringLiteral("application-x-deb")).pixmap(request.targetSize().width(),
                                                                                    request.targetSize().height());
    QPixmap appOverlay = QIcon(destPath % iconPath).pixmap(QSize(request.targetSize().width() / 2,
                                                                 request.targetSize().height() / 2)).scaledToWidth(request.targetSize().width() / 2);

    QPainter painter(&mimeIcon);
    for (int y = 0; y < appOverlay.height(); y += appOverlay.height()) {
        painter.drawPixmap(0, y, appOverlay);
    }

    return KIO::ThumbnailResult::pass(mimeIcon.toImage());
}

#include "DebThumbnailer.moc"