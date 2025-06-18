#include "player/inc/SettingsITunesMac.h"
#include "track/db/inc/SBBookmarkTrackDB.h"
#include "track/info/inc/SBBookmarkService.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <QTimer>

#ifndef NSModalResponseOK
#define NSModalResponseOK NSOKButton
#endif

//-------------------------------------------------------------------------------------------

@interface SettingsITunesLoader : NSObject
{
    omega::player::SettingsITunesMac *dialog;
#ifndef __clang__
    NSOpenPanel *currentPanel;
#endif
};
- (id)initWithDialog:(omega::player::SettingsITunesMac *)dlg;
- (void)dealloc;
- (void)doFolderDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir;
@end

//-------------------------------------------------------------------------------------------
@implementation SettingsITunesLoader
//-------------------------------------------------------------------------------------------

- (id)initWithDialog:(omega::player::SettingsITunesMac *)dlg
{
    self = [super init];
    if(self!=nil)
    {
        dialog = dlg;
    }
    return self;
}

//-------------------------------------------------------------------------------------------

- (void)dealloc
{
    [super dealloc];
}

//-------------------------------------------------------------------------------------------

- (void)doFolderDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir
{
    NSOpenPanel *loadPanel = [NSOpenPanel openPanel];

    [loadPanel setCanChooseDirectories:YES];
    [loadPanel setCanChooseFiles:NO];
    [loadPanel setDirectoryURL:dir];
    [loadPanel setCanCreateDirectories:NO];

#ifdef __clang__
    [loadPanel beginSheetModalForWindow:win completionHandler: ^(NSInteger result) {
        if(result == NSModalResponseOK)
        {
            int i;
            QStringList aList;

            NSArray *urls = [NSArray arrayWithArray:[loadPanel URLs]];
            for(i=0;i<[urls count];i++)
            {
                NSURL *u = (NSURL *)[urls objectAtIndex:i];
                NSString *uStr = [u absoluteString];
                const char *x = [uStr UTF8String];
                QUrl qU = QUrl(QString::fromUtf8(x));
                aList.append(qU.path());
            }

            dialog->onFolderOpen(aList);
        }
        else
        {
            dialog->onCancel();
        }
    }];
#else
    currentPanel = loadPanel;
    [loadPanel beginSheetModalForWindow:win
                modalDelegate:self
                didEndSelector:@selector(panelDidEnd:returnCode:contextInfo:)
                contextInfo:NULL];
#endif
}

#ifndef __clang__
- (void)panelDidEnd:(NSOpenPanel *)panel returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if(returnCode == NSModalResponseOK)
    {
        int i;
        QStringList aList;

        NSArray *urls = [NSArray arrayWithArray:[panel URLs]];
        for(i=0;i<[urls count];i++)
        {
            NSURL *u = (NSURL *)[urls objectAtIndex:i];
            NSString *uStr = [u absoluteString];
            const char *x = [uStr UTF8String];
            QUrl qU = QUrl(QString::fromUtf8(x));
            aList.append(qU.path());
        }

        dialog->onFolderOpen(aList);
    }
    else
    {
        dialog->onCancel();
    }
    currentPanel = nil;
}
#endif

//-------------------------------------------------------------------------------------------
@end
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

SettingsITunesMac::SettingsITunesMac(QWidget *parent,Qt::WindowFlags f) : SettingsITunes(parent,f)
{
    SettingsITunesLoader *loader = [[SettingsITunesLoader alloc] initWithDialog:this];
    m_loader = (void *)loader;
}

//-------------------------------------------------------------------------------------------

SettingsITunesMac::~SettingsITunesMac()
{
    SettingsITunesLoader *loader = (SettingsITunesLoader *)(m_loader);
    [loader release];
}

//-------------------------------------------------------------------------------------------

void SettingsITunesMac::processAddDirectory()
{
    NSView *mainView = (NSView *)winId();
    NSWindow *mainWindow = [mainView window];
    SettingsITunesLoader *loader = (SettingsITunesLoader *)(m_loader);
    QString defaultName = defaultMusicFolder();
    NSURL *u = (NSURL *)toUrl(defaultName);
    if(u!=nil)
    {
        [loader doFolderDialogWithWindow:mainWindow Directory:u];
    }
}

//-------------------------------------------------------------------------------------------

void *SettingsITunesMac::toUrl(const QString& fileName)
{
    NSURL *u;

    if(!fileName.isEmpty())
    {
#if defined(OMEGA_MAC_STORE)
        common::BString cStr = fileName.toUtf8().constData();
        const char *x = cStr.getString();
        NSString *nS = [NSString stringWithUTF8String:x];
        u = [NSURL fileURLWithPath:nS];
#else
        common::BString cStr = "file://";
        cStr += fileName.toUtf8().constData();
        NSString *nS = [NSString stringWithUTF8String:cStr.c_str()];
        u = [NSURL URLWithString:nS];
#endif
    }
    else
    {
        u = nil;
    }
    return (void *)u;
}

//-------------------------------------------------------------------------------------------

void SettingsITunesMac::onFolderOpen(const QStringList& files)
{
    QSharedPointer<track::info::SBBookmarkService> pSBService = track::info::SBBookmarkService::instance();

    for(QStringList::const_iterator ppI=files.begin();ppI!=files.end();ppI++)
    {
        pSBService->add(*ppI,true);
    }

    m_loadFileList.clear();
    m_loadFileList.append(files);
    QTimer::singleShot(100,this,SLOT(onLoadTimer()));
}

//-------------------------------------------------------------------------------------------

void SettingsITunesMac::onCancel()
{}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
