/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "installerrunner.h"

// Qt includes
#include <QDir>
#include <QIcon>

// KDE includes
#include <KLocalizedString>
#include <KProcess>
#include <KApplicationTrader>

K_PLUGIN_CLASS_WITH_JSON(InstallerRunner, "plasma-runner-installer.json")

InstallerRunner::InstallerRunner(QObject *parent, const KPluginMetaData &metaData)
        : KRunner::AbstractRunner(parent, metaData) {

    setObjectName("Installation Suggestions");
    //setPriority(AbstractRunner::HighestPriority);

    addSyntax(KRunner::RunnerSyntax(QStringLiteral(":q:"), i18n("Suggests the installation of applications if :q: is not found")));
}

InstallerRunner::~InstallerRunner() {
}

void InstallerRunner::match(KRunner::RunnerContext &context) {
    const QString term = context.query();
    if (term.length() < 3) {
        return;
    }

    // Search for applications which are executable and case-insensitively match the search term
    // See http://techbase.kde.org/Development/Tutorials/Services/Traders#The_KTrader_Query_Language
    // if the following is unclear to you.
    QList<KRunner::QueryMatch> matches;

    // TODO it seems it works, but needs to rework at all
    KService::List services = KApplicationTrader::query([&term](const KService::Ptr &service) {
        return !service->exec().isNull() && !service->exec().isEmpty() &&
               service->name().contains(term, Qt::CaseInsensitive);
    });

    if (services.isEmpty()) {
        KProcess process;
        if (QFile::exists(QStringLiteral("/usr/lib/command-not-found"))) {
            process << QStringLiteral("/usr/lib/command-not-found") << term;
        } else if (QFile::exists(QStringLiteral("/usr/bin/command-not-found"))) {
            process << QStringLiteral("/usr/bin/command-not-found") << term;
        } else {
            process << QStringLiteral("/bin/ls") << term; // Play it safe even if it won't work at all
        }
        process.setTextModeEnabled(true);
        process.setOutputChannelMode(KProcess::OnlyStderrChannel);
        process.start();
        process.waitForFinished();

        QString output = QString::fromUtf8(process.readAllStandardError());
        QStringList resultLines = output.split(QChar::fromLatin1('\n'));
        for (const QString &line: resultLines) {
            if (line.startsWith(QLatin1String("sudo"))) {
                QString package = line.split(QChar::fromLatin1(' ')).last();
                KRunner::QueryMatch match(this);
                match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest);
                setupMatch(package, term, match);
                match.setRelevance(1);
                matches << match;
            }
        }
    }

    if (!context.isValid()) {
        return;
    }

    context.addMatches(matches);
}

void InstallerRunner::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) {
    Q_UNUSED(context);
    QString package = match.data().toString();
    if (!package.isEmpty()) {
        KProcess *process = new KProcess(this);
        QStringList args = QStringList() << QStringLiteral("--install") << package;
        process->setProgram(QStringLiteral("/usr/bin/qapt-batch"), args);
        process->start();
    }
}

void InstallerRunner::setupMatch(const QString &package, const QString &term, KRunner::QueryMatch &match) {
    match.setText(i18n("Install %1", package));
    match.setData(package);
    if (term != package) {
        match.setSubtext(i18n("The \"%1\" package contains %2", package, term));
    }

    match.setIcon(QIcon::fromTheme(QStringLiteral("applications-other")));
}

#include "installerrunner.moc"
