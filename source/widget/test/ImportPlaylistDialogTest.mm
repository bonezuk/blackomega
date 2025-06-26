#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "widget/inc/ImportPlaylistDialog.h"
#include "track/db/inc/SandboxBookmarkService.h"

#include <QApplication>

using namespace testing;
using namespace omega;
using namespace omega::widget;

//-------------------------------------------------------------------------------------------
// Test based on following remote links
//
// /Volumes/Data/OmegaAppStoreTest -> Sentinal (Data)
// /Volumes/Music/OmegaAppStoreTest -> Oracle (Music)
// /Volumes/iTunes/OmegaAppStoreTest -> Oracle (ITunes)
//-------------------------------------------------------------------------------------------

TEST(ImportPlaylistDialog,processSingleDirectory)
{
    QString dirName = "/Users/bonez/tmp"; //"/Volumes/Data/OmegaAppStoreTest";

    int argc = 0;
    QApplication app(argc,0);

    QSharedPointer<track::db::SBBookmarkService> pSBService = track::db::SBBookmarkService::instance();

    QStringList dirList;
    dirList << dirName;

    ImportPlaylistDialog dlg;
    dlg.setPlaylistFileName("test.m3u");
    dlg.setDirectories(dirList);
    dlg.show();

    app.exec();

    if(dlg.result()==QDialog::Accepted)
    {
        EXPECT_TRUE(pSBService->has(dirName));
    }
    else
    {
        EXPECT_FALSE(pSBService->has(dirName));
    }

    track::db::SBBookmarkService::release();
}

//-------------------------------------------------------------------------------------------

TEST(ImportPlaylistDialog,processMultipleDirectories)
{
    int argc = 0;
    QApplication app(argc,0);

    QSharedPointer<track::db::SBBookmarkService> pSBService = track::db::SBBookmarkService::instance();

    QStringList dirList;
    dirList << "/Volumes/Data/OmegaAppStoreTest";
    dirList << "/Volumes/Music/OmegaAppStoreTest";
    dirList << "/Volumes/iTunes/OmegaAppStoreTest";

    ImportPlaylistDialog dlg;
    dlg.setPlaylistFileName("essential.pls");
    dlg.setDirectories(dirList);
    dlg.show();

    app.exec();

    if(dlg.result()==QDialog::Accepted)
    {
        EXPECT_TRUE(pSBService->has("/Volumes/Data/OmegaAppStoreTest"));
        EXPECT_TRUE(pSBService->has("/Volumes/Music/OmegaAppStoreTest"));
        EXPECT_TRUE(pSBService->has("/Volumes/iTunes/OmegaAppStoreTest"));
    }
    else
    {
        EXPECT_FALSE(pSBService->has("/Volumes/Data/OmegaAppStoreTest"));
        EXPECT_FALSE(pSBService->has("/Volumes/Music/OmegaAppStoreTest"));
        EXPECT_FALSE(pSBService->has("/Volumes/iTunes/OmegaAppStoreTest"));
    }

    track::db::SBBookmarkService::release();
}

//-------------------------------------------------------------------------------------------
