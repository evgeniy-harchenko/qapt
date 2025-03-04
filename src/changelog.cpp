/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "changelog.h"

// Qt includes
#include <QtCore/QRegularExpression>
#include <QSharedData>
#include <QStringBuilder>
#include <QStringList>

// QApt includes
#include "package.h"

// apt-pkg includes
#include <apt-pkg/strutl.h>

namespace QApt {

class ChangelogEntryPrivate : public QSharedData
{
public:
    ChangelogEntryPrivate(const QString &entryData, const QString &sourcePkg)
        : QSharedData()
        , data(entryData)
    {
        parseData(sourcePkg);
    }

    ChangelogEntryPrivate(const ChangelogEntryPrivate &other)
        : QSharedData(other)
        , data(other.data)
        , version(other.version)
        , issueDate(other.issueDate)
        , description(other.description)
        , CVEUrls(other.CVEUrls)
    {
    }

    ~ChangelogEntryPrivate() {}

    // Data
    QString data;
    QString version;
    QDateTime issueDate;
    QString description;
    QStringList CVEUrls;

    void parseData(const QString &sourcePackage);
};

    void ChangelogEntryPrivate::parseData(const QString &sourcePackage)
    {
        QStringList lines = data.split(QChar::fromLatin1('\n'));
        QRegularExpression rxInfo(QStringLiteral("^%1 \\((.*)\\)(.*)$").arg(QRegularExpression::escape(sourcePackage)));

        QString versionLine = lines.first();
        lines.removeFirst();
        QRegularExpressionMatch matchInfo = rxInfo.match(versionLine);

        if (matchInfo.hasMatch()) {
            version = matchInfo.captured(1);
        }

        for (const QString &line : lines) {
            if (line.startsWith(QLatin1String("  "))) {
                description.append(line % QChar::fromLatin1('\n'));

                static const QRegularExpression rxCVE(QStringLiteral("CVE-\\d{4}-\\d{4}"));
                QRegularExpressionMatchIterator it = rxCVE.globalMatch(line);
                while (it.hasNext()) {
                    QRegularExpressionMatch matchCVE = it.next();
                    if (matchCVE.isValid())
                        CVEUrls += QStringLiteral("http://web.nvd.nist.gov/view/vuln/detail?vulnId=%1;%1").arg(matchCVE.captured());
                }

                continue;
            }

            static const QRegularExpression rxDate(QStringLiteral("^ -- (.+) (<.+>)  (.+)$"));
            QRegularExpressionMatch matchDate = rxDate.match(line);

            if (matchDate.hasMatch()) {
                time_t issueTime = -1;
                if (RFC1123StrToTime(matchDate.captured(3).toUtf8().data(), issueTime)) {
                    issueDate = QDateTime::fromSecsSinceEpoch(issueTime);
                    break;
                }
            }
        }
    }

ChangelogEntry::ChangelogEntry(const QString &entryData, const QString &sourcePkg)
        : d(new ChangelogEntryPrivate(entryData, sourcePkg))
{
}

ChangelogEntry::ChangelogEntry(const ChangelogEntry &other)
{
    d = other.d;
}

ChangelogEntry::~ChangelogEntry()
{
}

ChangelogEntry &ChangelogEntry::operator=(const ChangelogEntry& rhs)
{
    // Protect against self-assignment
    if (this == &rhs) {
        return *this;
    }
    d = rhs.d;
    return *this;
}

QString ChangelogEntry::entryText() const
{
    return d->data;
}

QString ChangelogEntry::version() const
{
    return d->version;
}

QDateTime ChangelogEntry::issueDateTime() const
{
    return d->issueDate;
}

QString ChangelogEntry::description() const
{
    return d->description;
}

QStringList ChangelogEntry::CVEUrls() const
{
    return d->CVEUrls;
}


class ChangelogPrivate : public QSharedData
{
public:
    ChangelogPrivate(const QString &cData, const QString &sData)
        : QSharedData()
        , data(cData)
        , sourcePackage(sData)
        {
        }
    ChangelogPrivate(const ChangelogPrivate &other)
        : QSharedData(other)
        , data(other.data),
        sourcePackage(other.sourcePackage)
        {
        }
    ~ChangelogPrivate() {}

    QString data;
    QString sourcePackage;
};

Changelog::Changelog(const QString &data, const QString &sourcePackage)
        : d(new ChangelogPrivate(data, sourcePackage))
{
}

Changelog::Changelog(const Changelog &other)
        : d(other.d)
{
}

Changelog::~Changelog()
{
}

Changelog &Changelog::operator=(const Changelog& rhs)
{
    // Protect against self-assignment
    if (this == &rhs) {
        return *this;
    }
    d = rhs.d;
    return *this;
}

QString Changelog::text() const
{
    return d->data;
}

ChangelogEntryList Changelog::entries() const
{

    ChangelogEntryList entries;
    QStringList entryTexts;

    for (const QString &line: d->data.split(QChar::fromLatin1('\n'))) {
        if (line.startsWith(d->sourcePackage)) {
            entryTexts.append(line % QChar::fromLatin1('\n'));
            continue;
        }

        int curIndex = entryTexts.count() -1;
        if (curIndex < 0) {
            continue;
        }
        QString curEntry = entryTexts.at(curIndex);

        curEntry.append(line % QChar::fromLatin1('\n'));
        entryTexts[curIndex] = curEntry;
    }

    for (const QString &stanza : entryTexts) {
        ChangelogEntry entry(stanza, d->sourcePackage);

        entries << entry;
    }

    return entries;
}

ChangelogEntryList Changelog::newEntriesSince(const QString &version) const
{
    ChangelogEntryList newEntries;

    for (const ChangelogEntry &entry : entries()) {
        int res = Package::compareVersion(entry.version(), version);

        // Add entries newer than the given version
        if (res > 0) {
            newEntries << entry;
        }
    }

    return newEntries;
}

}
