/*
 * sourceslisttest - This is a test program for libqapt
 * Copyright 2013  Michael D. Stemle <themanchicken@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "sourceslisttest.h"

#include <QtTest>

#include <apt-pkg/init.h>
#include <apt-pkg/configuration.h>

QTEST_MAIN(SourcesListTest);

// Verification routines
void SourcesListTest::verifySourceEntry(
    const QString           &label,
    const QApt::SourceEntry &entry,
    const QString           &type,
    const QString           &uri,
    const QString           &dist,
    const QString           &components,
    const QString           &archs,
    const bool              isEnabled,
    const bool              isValid
)
{
    // Verify type...
    QVERIFY2(
        entry.type().compare(type) == 0,
        qPrintable(
            label
            + QStringLiteral(": The type isn't \"")+ type +QStringLiteral("\", it's \"")
            + entry.type()
            + QStringLiteral("\"!")
        )
    );
    
    // Verify URI...
    QVERIFY2(
        entry.uri().compare(uri) == 0,
        qPrintable(
            label
            + QStringLiteral(": The URI isn't \"")+ uri +QStringLiteral("\", it's \"")
            + entry.uri()
            + QStringLiteral("\"!")
        )
    );
    
    // Verify dist...
    QVERIFY2(
        entry.dist().compare(dist) == 0,
        qPrintable(
            label
            + QStringLiteral(": The distro isn't \"")+ dist +QStringLiteral("\", it's \"")
            + entry.dist()
            + QStringLiteral("\"!")
        )
    );
    
    // Verify components...
    QVERIFY2(
        entry.components().join(QStringLiteral("!")).compare(components) == 0,
        qPrintable(
            label
            + QStringLiteral(": The components (joined with !) aren't \"")+ components +QStringLiteral("\", they're \"")
            + entry.components().join(QStringLiteral("!"))
            + QStringLiteral("\"!")
        )
    );
    
    // Verify architectures...
    QVERIFY2(
        entry.architectures().join(QStringLiteral("!")).compare(archs) == 0,
        qPrintable(
            label
            + QStringLiteral(": The archs (joined with !) aren't \"")+ archs +QStringLiteral("\", they're \"")
            + entry.architectures().join(QStringLiteral("!"))
            + QStringLiteral("\"!")
        )
    );
    
    // Verify isEnabled...
    const char *isEnabledMsg = (isEnabled)
        ? "I was expecting this entry to be enabled, but it wasn't."
        : "I was expecting this entry to be disabled, but it wasn't.";
    QVERIFY2(
        entry.isEnabled() == isEnabled,
        qPrintable(label+QStringLiteral(": ")+QString::fromUtf8(isEnabledMsg))
    );
    
    // Verify isValid...
    const char *isValidMsg = (isValid)
        ? "I was expecting this entry to be valid, but it wasn't."
        : "I was expecting this entry to be invalid, but it wasn't.";
    QVERIFY2(
        entry.isValid() == isValid,
        qPrintable(label+QStringLiteral(": ")+QString::fromUtf8(isValidMsg))
    );
}

void SourcesListTest::initTestCase()
{
    pkgInitConfig(*_config);
    
    // Called before the first testfunction is executed
    QString dataPath = QStringLiteral(DATA_DIR);

    sampleSourcesHasOneFile = QStringList({ dataPath + QStringLiteral("/test1.list") });

    sampleSourcesHasTwoFiles = QStringList({
        dataPath + QStringLiteral("/test1.list"),
        dataPath + QStringLiteral("/test2.list")
    });

    sampleSourcesHasDuplicateFiles = QStringList({
        dataPath + QStringLiteral("/test1.list"),
        dataPath + QStringLiteral("/test1.list"),
        dataPath + QStringLiteral("/test2.list")
    });
    
    
    outputFile = QString(dataPath + QStringLiteral("/write_test.list"));
    dummyFile = QString(dataPath + QStringLiteral("/dummy_file.list"));
}

void SourcesListTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
    
    // Let's trash the written file.
    QVERIFY2(
        QFile::remove(outputFile),
        qPrintable(
                QStringLiteral("Unable to remove file \"")
            + outputFile
            + QStringLiteral("\".")
        )
    );
}

void SourcesListTest::init()
{
    // Called before each testfunction is executed
}

void SourcesListTest::cleanup()
{
    // Called after every testfunction
}

void SourcesListTest::testConstructor()
{
    QObject parentObject;
    parentObject.setProperty("propertyName", QStringLiteral("propertyValue"));
    
    QApt::SourcesList subjectDefaultConstructor;
    QStringList shouldntBeEmpty = subjectDefaultConstructor.sourceFiles();
    QVERIFY2(
        subjectDefaultConstructor.parent() == nullptr,
        "The default constructor doesn't have a zero parent?!"
    );
    QVERIFY2(
        shouldntBeEmpty.count() > 0,
        qPrintable(
                QStringLiteral("The default constructor should have given us an empty file list, but it didn't. I got these: ")
            + shouldntBeEmpty.join(QStringLiteral("!"))
            + QStringLiteral(" which is ")
            + QString::number(shouldntBeEmpty.count())
        )
    );
    int previousCount = subjectDefaultConstructor.entries().count();
    subjectDefaultConstructor.reload();
    QVERIFY2(
        previousCount == subjectDefaultConstructor.entries().count(),
        qPrintable(
                QStringLiteral("The default constructor should have given us the same number of entries as the subsequent reload() call. I had ")
            + QString::number(previousCount)
            + QStringLiteral(" and now I have ")
            + QString::number(subjectDefaultConstructor.entries().count())
        )
    );
    
    QApt::SourcesList subjectParentOnly(&parentObject);
    shouldntBeEmpty = subjectParentOnly.sourceFiles();
    QVERIFY2(
        subjectParentOnly.parent() == &parentObject,
        "The parent-only constructor parent isn't the one we sent in?!"
        
    );
    QVERIFY2(
        shouldntBeEmpty.count() > 0,
        "The parent-only constructor should have given us sources anyway, using the default list."
    );
    
    QApt::SourcesList subjectListOnly(nullptr, sampleSourcesHasTwoFiles);
    shouldntBeEmpty = subjectListOnly.sourceFiles();
    QVERIFY2(
        subjectListOnly.parent() == nullptr,
        "The list-only constructor doesn't have a zero parent?!"
    );
    QVERIFY2(
        shouldntBeEmpty.count() == 2,
        "The list-only constructor should have given us an empty file list, but it didn't."
    );

    QApt::SourcesList subjectDuplicateListOnly(nullptr, sampleSourcesHasDuplicateFiles);
    shouldntBeEmpty = subjectDuplicateListOnly.sourceFiles();
    QVERIFY2(
        subjectDuplicateListOnly.parent() == nullptr,
        "The list-only constructor doesn't have a zero parent?!"
    );
    QVERIFY2(
        shouldntBeEmpty.count() == 2,
        "The list-only constructor should have given us an empty file list, but it didn't."
    );
    
    QApt::SourcesList subjectParentAndList(&parentObject, sampleSourcesHasTwoFiles);
    shouldntBeEmpty = subjectListOnly.sourceFiles();
    QVERIFY2(
        subjectParentOnly.parent() == &parentObject,
        "The parent-and-list constructor parent isn't the one we sent in?!"
    );
    QVERIFY2(
        shouldntBeEmpty.count() == 2,
        "The parent-and-list constructor should have given us an empty file list, but it didn't."
    );
}

/*
 * Test plan:
 * 1. Load the sample data file
 * 2. Verify that we loaded the file using entries()
 * 3. Verify that we loaded the file using entries(filename)
 */
#define SOURCES_COUNT 14
#define TEST1_MD5     "56717306e28a529f89b56c6ee6082375"
void SourcesListTest::testLoadSourcesOneFile()
{
    QVERIFY2(sampleSourcesHasOneFile.count() == 1, "Verify we have only one source...");

    QApt::SourcesList subject (nullptr, sampleSourcesHasOneFile);
    
    QApt::SourceEntryList entries = subject.entries();
    qDebug() << "I have " << entries.count() << " entries.";
    for ( const QApt::SourceEntry &one : entries ) {
        qDebug() << "Entry " << one.toString();
    }
    
    // Since this test depends so much on the test1.list file,
    // let's do everything we can to make sure people know what it
    // should look like!
    QVERIFY2(
        entries.count() == SOURCES_COUNT,
        qPrintable(
                QStringLiteral("We don't have ")
            + QString::number(SOURCES_COUNT)
            + QStringLiteral(" entries, we have ")
            + QString::number(entries.count())
            + QStringLiteral("! Please check the file, the MD5 should be \"")
            + QStringLiteral(TEST1_MD5)
            + QStringLiteral("\"")
        )
    );
    
    // Now we're going to go through each of the lines in the file and verify that
    // it loaded properly...
    QVERIFY2(
        entries[0].toString().compare(QStringLiteral("## First test")) == 0,
        qPrintable(
                QStringLiteral("I was expecting \"## First test\" as the first line, but I got ")
            + entries[0].toString()
        )
    );
    // A commented line is inactive and invalid
    verifySourceEntry(
            QStringLiteral("Line #0"),
        entries[0],
        QString(),
        QString(),
        QString(),
        QString(),
        QString(),
        false,
        false
    );
    verifySourceEntry(
            QStringLiteral("Line #1"),
        entries[1],
            QStringLiteral("deb"),
            QStringLiteral("http://apttest/ubuntu"),
            QStringLiteral("saucy"),
            QStringLiteral("partner"),
            QStringLiteral(""),
        true,
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #2"),
        entries[2],
            QStringLiteral("deb-src"),
            QStringLiteral("http://apttest/ubuntu"),
            QStringLiteral("saucy"),
            QStringLiteral("partner"),
            QStringLiteral(""),
        true,
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #3"),
        entries[3],
        QStringLiteral("deb"),
        QStringLiteral("http://apttest/ubuntu"),
        QStringLiteral("saucy"),
        QStringLiteral("contrib!main!partner"),
        QStringLiteral(""),
        true,
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #4"),
        entries[4],
        QStringLiteral("deb"),
        QStringLiteral("http://apttest/ubuntu"),
        QStringLiteral("saucy"),
        QStringLiteral("partner"),
        QStringLiteral("i386"),
        true,
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #5"),
        entries[5],
        QStringLiteral("deb"),
        QStringLiteral("http://apttest/ubuntu"),
        QStringLiteral("saucy"),
        QStringLiteral("partner"),
        QStringLiteral("i386!ppc"),
        true,
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #6"),
        entries[6],
        QStringLiteral("deb"),
        QStringLiteral("http://apttest/ubuntu"),
        QStringLiteral("saucy"),
        QStringLiteral("contrib!main!partner"),
        QStringLiteral("i386!ppc"),
        true,
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #7"),
        entries[7],
                           QStringLiteral("deb"),
                                   QStringLiteral("https://apttest/ubuntu"), // <-- Note, https this time
                                   QStringLiteral("saucy"),
                                   QStringLiteral("contrib!main!partner"),
                                   QStringLiteral("i386!ppc"),
        true,
        true
    );
    QVERIFY2(
        entries[8].toString().compare(QStringLiteral("")) == 0,
        qPrintable(
                QStringLiteral("I was expecting an empty string as the ninth line (index 8), but I got ")
            + entries[8].toString()
        )
    );
    // An empty line isn't invalid
    verifySourceEntry(
            QStringLiteral("Line #8"),
        entries[8],
        QString(),
        QString(),
        QString(),
        QString(),
        QString(),
        true,
        false
    );
    verifySourceEntry(
            QStringLiteral("Line #9"),
        entries[9],
                           QStringLiteral("deb"),
                                   QStringLiteral("http://apttest/ubuntu"),
                                   QStringLiteral("saucy"),
                                   QStringLiteral("contrib!main!partner"),
                                   QStringLiteral("i386!ppc"),
        false, // <-- Note, disabled!
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #10"),
        entries[10],
                           QStringLiteral("deb"),
                                   QStringLiteral("http://apttest/ubuntu"),
                                   QStringLiteral("saucy"),
                                   QStringLiteral("contrib!main!partner"),
                                   QStringLiteral("i386!ppc"),
        false, // <-- Note, disabled!
        true
    );
    verifySourceEntry(
            QStringLiteral("Line #11"),
        entries[11],
                           QStringLiteral("deb"),
                                   QStringLiteral("http://apttest/ubuntu"),
                                   QStringLiteral("saucy"),
                                   QStringLiteral("contrib!main!partner"),
                                   QStringLiteral("i386!ppc"),
        false, // <-- Note, disabled!
        true
    );
    // This line ensures that lines only containing comment characters (e.g. ##)
    // are discarded as invalid.
    verifySourceEntry(
            QStringLiteral("Line #12"),
        entries[12],
                           QStringLiteral(""),
                                   QStringLiteral(""),
                                   QStringLiteral(""),
                                   QStringLiteral(""),
                                   QStringLiteral(""),
        false, // <-- Note, disabled!
        false
    );
    verifySourceEntry(
            QStringLiteral("Line #13"),
        entries[13],
                           QStringLiteral("deb"),
                                   QStringLiteral("cdrom:[Kubuntu 11.10 _Oneiric Ocelot_ - Release amd64 (20111012)]/"),
                                   QStringLiteral("oneiric"),
                                   QStringLiteral("main!restricted"),
                                   QStringLiteral(""),
        true,
        true
    );
}

#define SOURCES_COUNT_2   3
#define ALL_SOURCES_COUNT (SOURCES_COUNT + SOURCES_COUNT_2)
#define TEST2_MD5         "af99c3d972146a82c7af125c2022c581"
void SourcesListTest::testLoadSourcesManyFiles()
{
    QVERIFY2(sampleSourcesHasTwoFiles.count() == 2,
        qPrintable(
                QStringLiteral("We don't have the 2 files I was expecting, we have ")
            + QString::number(sampleSourcesHasTwoFiles.count())
        )
    );

    QApt::SourcesList subject (nullptr, sampleSourcesHasTwoFiles);
    
    QApt::SourceEntryList entries = subject.entries();
    qDebug() << "I have " << entries.count() << " entries.";
    for ( const QApt::SourceEntry &one : entries ) {
        qDebug() << one.file() << ": Entry " << one.toString();
    }
    
    // Since this test depends so much on the test1.list file,
    // let's do everything we can to make sure people know what it
    // should look like!
    QVERIFY2(
        entries.count() == ALL_SOURCES_COUNT,
        qPrintable(
                QStringLiteral("We don't have ")
            + QString::number(ALL_SOURCES_COUNT)
            + QStringLiteral(" entries, we have ")
            + QString::number(entries.count())
            + QStringLiteral("! Please check the files test1.list and test2.list, the MD5's should be \"")
            + QStringLiteral(TEST1_MD5)
            + QStringLiteral("\" and \"")
            + QStringLiteral(TEST2_MD5)
            + QStringLiteral("\" respectively.")
        )
    );
    
    // Verify that grabbing each source by file returns the correct number of records.
    QApt::SourceEntryList test1 = subject.entries(sampleSourcesHasTwoFiles[0]);
    QApt::SourceEntryList test2 = subject.entries(sampleSourcesHasTwoFiles[1]);    
    QVERIFY2(
        test1.count() == SOURCES_COUNT,
        qPrintable(
            sampleSourcesHasTwoFiles[0]
            + QStringLiteral("\nLooks like test1.list doesn't have ")
            + QString::number(SOURCES_COUNT)
            + QStringLiteral(" entries afterall; it has ")
            + QString::number(test1.count())
            + QStringLiteral(" entries.")
        )
    );
    QVERIFY2(
        test2.count() == SOURCES_COUNT_2,
        qPrintable(
            sampleSourcesHasTwoFiles[0]
            + QStringLiteral("\nLooks like test2.list doesn't have ")
            + QString::number(SOURCES_COUNT_2)
            + QStringLiteral(" entries afterall; it has ")
            + QString::number(test1.count())
            + QStringLiteral(" entries.")
        )
    );
}

void SourcesListTest::testAddSource()
{
    QStringList outfilesListJustOne (dummyFile);
    QApt::SourcesList subjectSingleEntry(nullptr, outfilesListJustOne);
    
    QVERIFY2(
        subjectSingleEntry.entries().count() == 0,
        qPrintable(
                QStringLiteral("I expected to have a new, empty file with zero entries. I found ")
            + QString::number(subjectSingleEntry.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    QApt::SourceEntry entryOne;
    entryOne.setType(QStringLiteral("deb"));
    entryOne.setUri(QStringLiteral("https://foo.com/bar"));
    entryOne.setComponents({QStringLiteral("main")});
    entryOne.setDist(QStringLiteral("saucy"));
    entryOne.setEnabled(true);
    // Verify the item we just created...
    verifySourceEntry(
            QStringLiteral("New Entry #0"),
        entryOne,
                           QStringLiteral("deb"),
                                   QStringLiteral("https://foo.com/bar"),
                                   QStringLiteral("saucy"),
                                   QStringLiteral("main"),
        QString(),
        true,
        true
    );
    QApt::SourceEntry entryTwo;
    entryTwo.setType(QStringLiteral("deb"));
    entryTwo.setUri(QStringLiteral("https://foo.com/bar2"));
    entryTwo.setComponents({QStringLiteral("main")});
    entryTwo.setDist(QStringLiteral("saucy"));
    entryTwo.setEnabled(true);
    
    // Verify the item we just created...
    verifySourceEntry(
            QStringLiteral("New Entry #1"),
        entryTwo,
                           QStringLiteral("deb"),
                                   QStringLiteral("https://foo.com/bar2"),
                                   QStringLiteral("saucy"),
                                   QStringLiteral("main"),
        QString(),
        true,
        true
    );
    
    subjectSingleEntry.addEntry(entryOne);
    QVERIFY2(
        subjectSingleEntry.entries().count() == 1,
        qPrintable(
                QStringLiteral("I expected to have a single entry now. I found ")
            + QString::number(subjectSingleEntry.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    QVERIFY2(
        subjectSingleEntry.containsEntry(entryOne),
        qPrintable(
                QStringLiteral("I totally thought I had an entry \"")
            + entryOne.toString()
            + QStringLiteral("\", but I don't.")
        )
    );
    
    QApt::SourcesList subjectMultipleFiles(nullptr, sampleSourcesHasTwoFiles);
    QVERIFY2(
        subjectMultipleFiles.entries().count() == ALL_SOURCES_COUNT,
        qPrintable(
                QStringLiteral("I expected to have both files 1 and 2 loaded with ")
            + QString::number(ALL_SOURCES_COUNT)
            + QStringLiteral(" entries. I found ")
            + QString::number(subjectMultipleFiles.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    subjectMultipleFiles.addEntry(entryOne);
    QVERIFY2(
        subjectMultipleFiles.entries().count() == ALL_SOURCES_COUNT+1,
        qPrintable(
                QStringLiteral("I expected to have a new, empty file with ")
            + QString::number(ALL_SOURCES_COUNT+1)
            + QStringLiteral(" entries. I found ")
            + QString::number(subjectMultipleFiles.entries().count())
            + QStringLiteral(" entries.")
        )
    );
}

void SourcesListTest::testRemoveSource()
{
    QStringList outfilesListJustOne (dummyFile);
    QApt::SourcesList subject(nullptr, outfilesListJustOne);
    QApt::SourceEntry entryOne;
    QApt::SourceEntry entryTwo;

    subject.reload();
    
    // Construct our two sources
    entryOne.setType(QStringLiteral("deb"));
    entryOne.setUri(QStringLiteral("https://foo.com/bar"));
    entryOne.setComponents({QStringLiteral("main")});
    entryOne.setDist(QStringLiteral("saucy"));
    entryOne.setEnabled(true);
    entryTwo.setType(QStringLiteral("deb"));
    entryTwo.setUri(QStringLiteral("https://foo.com/bar2"));
    entryTwo.setComponents({QStringLiteral("main")});
    entryTwo.setDist(QStringLiteral("saucy"));
    entryTwo.setEnabled(true);
    
    subject.addEntry(entryOne);
    subject.addEntry(entryTwo);
    
    QVERIFY2(
        subject.entries().count() == 2,
        qPrintable(
                QStringLiteral("I expected to have 2 sources. I found ")
            + QString::number(subject.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    subject.removeEntry(entryOne);
    QVERIFY2(
        subject.entries().count() == 1,
        qPrintable(
                QStringLiteral("Now, I expect to have 1 sources. I found ")
            + QString::number(subject.entries().count())
            + QStringLiteral(" entries.")
        )
    );    
}

void SourcesListTest::testSaveSources()
{
    QStringList outfilesListJustOne (outputFile);
    QApt::SourcesList subject(nullptr, outfilesListJustOne);
    QApt::SourceEntry entryOne;
    QApt::SourceEntry entryTwo;

    // Construct our two sources
    entryOne.setType(QStringLiteral("deb"));
    entryOne.setUri(QStringLiteral("https://foo.com/bar"));
    entryOne.setComponents({QStringLiteral("main")});
    entryOne.setDist(QStringLiteral("saucy"));
    entryOne.setEnabled(true);
    entryTwo.setType(QStringLiteral("deb"));
    entryTwo.setUri(QStringLiteral("https://foo.com/bar2"));
    entryTwo.setComponents({QStringLiteral("main")});
    entryTwo.setDist(QStringLiteral("saucy"));
    entryTwo.setEnabled(true);
    
    subject.addEntry(entryOne);
    
    qDebug() << "Here's your sources list:\n" << subject.toString();
    
    QVERIFY2(
                subject.containsEntry(entryOne),
                qPrintable(
                        QStringLiteral("I expected to have the entry ")
                    + entryOne.toString()
                )
    );
    QVERIFY2(
        subject.entries().count() == 1,
        qPrintable(
                QStringLiteral("I expected to have 1 sources. I found ")
            + QString::number(subject.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    subject.save();
    
    QApt::SourcesList loadingAfterSave(nullptr, outfilesListJustOne);
    QVERIFY2(
        loadingAfterSave.entries().count() == 1,
        qPrintable(
                QStringLiteral("I expected to have 1 sources after loading. I found ")
            + QString::number(loadingAfterSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );

    loadingAfterSave.addEntry(entryTwo);
    loadingAfterSave.save();
    QVERIFY2(
        loadingAfterSave.entries().count() == 2,
        qPrintable(
                QStringLiteral("I expected to have 2 sources after adding another. I found ")
            + QString::number(loadingAfterSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );

    QApt::SourcesList loadingAfterSecondSave(nullptr, outfilesListJustOne);
    QVERIFY2(
        loadingAfterSave.entries().count() == 2,
        qPrintable(
                QStringLiteral("I expected to have 2 sources after loading again. I found ")
            + QString::number(loadingAfterSecondSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );

    loadingAfterSecondSave.save();
    QVERIFY2(
        loadingAfterSecondSave.entries().count() == 2,
        qPrintable(
                QStringLiteral("I expected to have 2 sources after adding another. I found ")
            + QString::number(loadingAfterSecondSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );

    QApt::SourcesList loadingAfterSecondSaveB(nullptr, outfilesListJustOne);
    QVERIFY2(
        loadingAfterSave.entries().count() == 2,
        qPrintable(
                QStringLiteral("I expected to have 2 sources after loading again. I found ")
            + QString::number(loadingAfterSecondSaveB.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    loadingAfterSecondSave.removeEntry(entryOne);
    loadingAfterSecondSave.removeEntry(entryTwo);
    
    QVERIFY2(
        loadingAfterSecondSave.entries().count() == 0,
        qPrintable(
                QStringLiteral("I expected to have 0 sources after removing the two I had previously added. I found ")
            + QString::number(loadingAfterSecondSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    loadingAfterSecondSave.save();
    
    QApt::SourcesList loadingAfterThirdSave(nullptr, outfilesListJustOne);
    QVERIFY2(
        loadingAfterThirdSave.entries().count() == 3,
        qPrintable(
                QStringLiteral("I expected to have 3 sources (all the default comments) after saving following the remove. I found ")
            + QString::number(loadingAfterThirdSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );
    
    loadingAfterSecondSave.reload();
    QVERIFY2(
        loadingAfterSecondSave.entries().count() == 3,
        qPrintable(
                QStringLiteral("I expected to have 3 sources (all the default comments) after saving following the remove (using reload()). I found ")
            + QString::number(loadingAfterSecondSave.entries().count())
            + QStringLiteral(" entries.")
        )
    );
}

// #include "../t/sourceslisttest.moc"
