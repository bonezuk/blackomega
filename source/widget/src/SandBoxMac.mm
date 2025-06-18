#include "widget/inc/SandBoxMac.h"
#include "common/inc/DiskOps.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <QCoreApplication>
#include <QUrl>
#include <QSet>
#include <QFileDialog>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#if __has_include(<UniformTypeIdentifiers/UniformTypeIdentifiers.h>)
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#import <UniformTypeIdentifiers/UTType.h>
#define USE_UNIFORM_TYPE_IDENTIFIERS 1
#elif __has_include(<MobileCoreServices/MobileCoreServices.h>)
#import <MobileCoreServices/MobileCoreServices.h>
#define USE_UNIFORM_TYPE_IDENTIFIERS 0
#else
#import <LaunchServices/UTType.h>
#define USE_UNIFORM_TYPE_IDENTIFIERS 0
#endif

#if defined(OMEGA_MAC_STORE)
#include <sys/types.h>
#include <pwd.h>
#endif

#ifndef NSModalResponseOK
#define NSModalResponseOK NSOKButton
#endif

//-------------------------------------------------------------------------------------------

@interface MacSandServices : NSObject
{
    omega::widget::SBServiceMac *service;
};
@property (assign) NSArray *urls;
- (id)initWithService:(omega::widget::SBServiceMac *)svr;
- (void)dealloc;
- (void)doFileDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir Title:(NSString *)title Filter:(NSArray *)filterArray;
- (void)doFolderDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir Title:(NSString *)title;
- (void)doSaveDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir Title:(NSString *)title Filter:(NSArray *)filterArray;
@end

//-------------------------------------------------------------------------------------------
@implementation MacSandServices
//-------------------------------------------------------------------------------------------

- (id)initWithService:(omega::widget::SBServiceMac *)svr
{
    self = [super init];
    if(self!=nil)
    {
        service = svr;
    }
    return self;
}

//-------------------------------------------------------------------------------------------

- (void)dealloc
{
    [super dealloc];
}

//-------------------------------------------------------------------------------------------

- (void)doFileDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir Title:(NSString *)title Filter:(NSArray *)filterArray
{
    NSOpenPanel *loadPanel = [NSOpenPanel openPanel];

    [loadPanel setCanChooseDirectories:NO];
    [loadPanel setCanChooseFiles:YES];
    [loadPanel setAllowsMultipleSelection:YES];
    [loadPanel setDirectoryURL:dir];
    [loadPanel setTitle:title];
    [loadPanel setCanCreateDirectories:NO];
    if(filterArray!=nil)
    {
        [loadPanel setAllowedContentTypes:filterArray];
    }

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

            service->doLoadFiles(aList);
        }
    }];
#else
    currentOpenPanel = loadPanel;
    [loadPanel beginSheetModalForWindow:win
                modalDelegate:self
                didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:)
                contextInfo:NULL];
#endif
}

#ifndef __clang__
- (void)openPanelDidEnd:(NSOpenPanel *)panel returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
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
        service->doLoadFiles(aList);
    }
    currentOpenPanel = nil;
}
#endif

//-------------------------------------------------------------------------------------------

- (void)doFolderDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir Title:(NSString *)title
{
    NSOpenPanel *loadPanel = [NSOpenPanel openPanel];

    [loadPanel setCanChooseDirectories:YES];
    [loadPanel setCanChooseFiles:NO];
    [loadPanel setDirectoryURL:dir];
    [loadPanel setTitle:title];
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

            if(!aList.isEmpty())
            {
                service->doLoadDirectory(aList.at(0));
            }
        }
    }];
#else
    currentOpenPanel = loadPanel;
    [loadPanel beginSheetModalForWindow:win
                modalDelegate:self
                didEndSelector:@selector(folderPanelDidEnd:returnCode:contextInfo:)
                contextInfo:NULL];
#endif
}

#ifndef __clang__
- (void)folderPanelDidEnd:(NSOpenPanel *)panel returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
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
        if(!aList.isEmpty())
        {
            service->doLoadDirectory(aList.at(0));
        }
    }
    currentOpenPanel = nil;
}
#endif

//-------------------------------------------------------------------------------------------

- (void)doSaveDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir Title:(NSString *)title Filter:(NSArray *)filterArray
{
    NSSavePanel *savePanel = [NSSavePanel savePanel];

    [savePanel setCanCreateDirectories:YES];
    [savePanel setDirectoryURL:dir];
    [savePanel setTitle:title];
    [savePanel setCanCreateDirectories:NO];

#if USE_UNIFORM_TYPE_IDENTIFIERS
    if(filterArray!=nil)
    {
        NSMutableArray *arr = [NSMutableArray arrayWithCapacity:4];
        [arr addObject: [UTType typeWithFilenameExtension: @"xspf"]];
        [arr addObject: [UTType typeWithFilenameExtension: @"m3u"]];
        [arr addObject: [UTType typeWithFilenameExtension: @"m3u8"]];
        [arr addObject: [UTType typeWithFilenameExtension: @"pls"]];
        [savePanel setAllowedContentTypes:arr];
    }
#else
    if(filterArray != nil && [filterArray count] > 0)
    {
        [savePanel setAllowedFileTypes:filterArray];
    }
#endif

#ifdef __clang__
    [savePanel beginSheetModalForWindow:win completionHandler: ^(NSInteger result) {
        if(result == NSModalResponseOK)
        {
            QStringList aList;

            NSURL *u = (NSURL *)[savePanel URL];
            NSString *uStr = [u absoluteString];
            const char *x = [uStr UTF8String];
            QUrl qU = QUrl(QString::fromUtf8(x));
            aList.append(qU.path());

            if(!aList.isEmpty())
            {
                service->doSaveFile(aList.at(0));
            }
        }
    }];
#else
    currentSavePanel = savePanel;
    [savePanel beginSheetModalForWindow:win
                modalDelegate:self
                didEndSelector:@selector(savePanelDidEnd:returnCode:contextInfo:)
                contextInfo:NULL];
#endif
}

#ifdef __clang__
- (void)savePanelDidEnd:(NSSavePanel *)panel returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if(returnCode == NSModalResponseOK)
    {
        QStringList aList;
        NSURL *u = (NSURL *)[panel URL];
        NSString *uStr = [u absoluteString];
        const char *x = [uStr UTF8String];
        QUrl qU = QUrl(QString::fromUtf8(x));
        aList.append(qU.path());

        if(!aList.isEmpty())
        {
            service->doSaveFile(aList.at(0));
        }
    }
    currentSavePanel = nil;
}
#endif

//-------------------------------------------------------------------------------------------
@end
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(common::SBServiceFactory,common::SBService, \
                            SBServiceMacFactory,SBServiceMac, \
                            "mac",false)

//-------------------------------------------------------------------------------------------

SBServiceMac::SBServiceMac(QObject *parent) : SBServiceBase(parent),
    m_loadDirDialog(0),
    m_loadFilesDialog(0),
    m_saveFileDialog(0)
{
    m_sService = (void *)[[MacSandServices alloc] initWithService:this];
}

//-------------------------------------------------------------------------------------------

SBServiceMac::~SBServiceMac()
{
    if(m_loadDirDialog!=0)
    {
        delete m_loadDirDialog;
        m_loadDirDialog = 0;
    }
    if(m_loadFilesDialog!=0)
    {
        delete m_loadFilesDialog;
        m_loadFilesDialog = 0;
    }
    if(m_saveFileDialog!=0)
    {
        delete m_saveFileDialog;
        m_saveFileDialog = 0;
    }
    MacSandServices *sService = (MacSandServices *)m_sService;
    [sService dealloc];
}

//-------------------------------------------------------------------------------------------

QString SBServiceMac::getHomeDirectory()
{
    NSString *hNSDir = NSHomeDirectory();
    if(hNSDir!=nil)
    {
        const char *x = [hNSDir UTF8String];
        QString hDir = QString::fromUtf8(x);
        return hDir;
    }
    else
    {
        return SBServiceBase::getHomeDirectory();
    }
}

//-------------------------------------------------------------------------------------------

QString SBServiceMac::getTempDirectory()
{
    NSString *hNSDir = NSTemporaryDirectory();
    if(hNSDir!=nil)
    {
        const char *x = [hNSDir UTF8String];
        QString hDir = QString::fromUtf8(x);
        return hDir;
    }
    else
    {
        return SBServiceBase::getTempDirectory();
    }
}

//-------------------------------------------------------------------------------------------

QString SBServiceMac::getApplicationDataDirectory()
{
#if defined(OMEGA_MAC_STORE)
    QString hDir = getHomeDirectory();
    if(hDir.at(hDir.length()-1)!=QChar('/') && hDir.at(hDir.length()-1)!=QChar('\\'))
    {
        hDir += "/";
    }
    hDir = QDir::toNativeSeparators(hDir);
    omega::common::DiskOps::path(hDir,true);
    return hDir;
#else
    return SBServiceBase::getApplicationDataDirectory();
#endif
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::loadDirDialog(QObject *parent,const QString& title,const QString& dirName)
{
    QWidget *parentWidget = dynamic_cast<QWidget *>(parent);

#if defined(OMEGA_MAC_STORE)
    NSView *mainView = (NSView *)parentWidget->winId();
    NSWindow *mainWindow = [mainView window];

    NSURL *dirURL = (NSURL *)toUrl(dirName);
    NSString *titleStr = [NSString stringWithUTF8String:(title.toUtf8().constData())];

    MacSandServices *sService = (MacSandServices *)m_sService;
    [sService doFolderDialogWithWindow:mainWindow Directory:dirURL Title:titleStr];
#else
    if(m_loadDirDialog!=0)
    {
        delete m_loadDirDialog;
        m_loadDirDialog = 0;
    }
    m_loadDirDialog = new QFileDialog(parentWidget,Qt::Sheet);
    m_loadDirDialog->setAcceptMode(QFileDialog::AcceptOpen);
    m_loadDirDialog->setFileMode(QFileDialog::Directory);
    m_loadDirDialog->setViewMode(QFileDialog::Detail);
    m_loadDirDialog->setWindowTitle(title);
    m_loadDirDialog->setDirectory(dirName);
    m_loadDirDialog->open(this,SLOT(doLoadDirectory(const QString&)));
#endif
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::loadFilesDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter)
{
    QWidget *parentWidget = dynamic_cast<QWidget *>(parent);

#if defined(OMEGA_MAC_STORE)
    NSView *mainView = (NSView *)parentWidget->winId();
    NSWindow *mainWindow = [mainView window];

    NSURL *dirURL = (NSURL *)toUrl(dirName);
    NSString *titleStr = [NSString stringWithUTF8String:(title.toUtf8().constData())];
    NSArray *filterArray = (NSArray *)fromFilter(filter);

    MacSandServices *sService = (MacSandServices *)m_sService;
    [sService doFileDialogWithWindow:mainWindow Directory:dirURL Title:titleStr Filter:filterArray];
#else
    if(m_loadFilesDialog!=0)
    {
        delete m_loadFilesDialog;
        m_loadFilesDialog = 0;
    }
    m_loadFilesDialog = new QFileDialog(parentWidget,Qt::Sheet);
    m_loadFilesDialog->setAcceptMode(QFileDialog::AcceptOpen);
    m_loadFilesDialog->setFileMode(QFileDialog::ExistingFiles);
    m_loadFilesDialog->setViewMode(QFileDialog::Detail);
    m_loadFilesDialog->setWindowTitle(title);
    m_loadFilesDialog->setNameFilter(filter);
    m_loadFilesDialog->setDirectory(dirName);
    m_loadFilesDialog->open(this,SLOT(doLoadFiles(const QStringList&)));
#endif
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::saveFileDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter)
{
    QWidget *parentWidget = dynamic_cast<QWidget *>(parent);

#if defined(OMEGA_MAC_STORE)
    NSView *mainView = (NSView *)parentWidget->winId();
    NSWindow *mainWindow = [mainView window];

    NSURL *dirURL = (NSURL *)toUrl(dirName);
    NSString *titleStr = [NSString stringWithUTF8String:(title.toUtf8().constData())];
    NSArray *filterArray = (NSArray *)fromFilter(filter);

    MacSandServices *sService = (MacSandServices *)m_sService;
    [sService doSaveDialogWithWindow:mainWindow Directory:dirURL Title:titleStr Filter:filterArray];
#else
    if(m_saveFileDialog!=0)
    {
        delete m_saveFileDialog;
        m_saveFileDialog = 0;
    }
    m_saveFileDialog = new QFileDialog(parentWidget,Qt::Sheet);
    m_saveFileDialog->setAcceptMode(QFileDialog::AcceptSave);
    m_saveFileDialog->setFileMode(QFileDialog::AnyFile);
    m_saveFileDialog->setViewMode(QFileDialog::Detail);
    m_saveFileDialog->setWindowTitle(title);
    m_saveFileDialog->setNameFilter(filter);
    m_saveFileDialog->setDirectory(dirName);
    m_saveFileDialog->open(this,SLOT(doSaveFile(const QString&)));
#endif
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::doLoadDirectory(const QString& dirName)
{
#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    if(!sbBookmark->has(dirName,true))
    {
        sbBookmark->add(dirName,true);
    }
#endif
    emit onLoadDirectory(dirName);
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::doLoadFiles(const QStringList& fileList)
{
#if defined(OMEGA_MAC_STORE)
    QStringList::const_iterator ppI;
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    for(ppI=fileList.begin();ppI!=fileList.end();ppI++)
    {
        const QString& fileName = *ppI;
        if(!sbBookmark->has(fileName,isReadOnly(fileName)))
        {
            sbBookmark->add(fileName,isReadOnly(fileName));
        }
    }
#endif
    emit onLoadFiles(fileList);
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::doSaveFile(const QString& fileName)
{
#if defined(OMEGA_MAC_STORE)
    QString selFilter("XML Shareable Playlist (*.xspf)");
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    if(!sbBookmark->has(fileName,true))
    {
        sbBookmark->add(fileName,true);
    }
#else
    QString selFilter = m_saveFileDialog->selectedNameFilter();
#endif

    common::Log::g_Log.print("%s\n",fileName.toUtf8().constData());

    emit onSaveFile(fileName,selFilter);
}

//-------------------------------------------------------------------------------------------

void *SBServiceMac::toUrl(const QString& in)
{
    QUrl u = QUrl::fromLocalFile(in);
    const char *x = u.toString().toUtf8().constData();
    NSString *nS = [NSString stringWithUTF8String:x];
    NSURL *oUrl = [NSURL URLWithString:nS];
    return (void *)oUrl;
}

//-------------------------------------------------------------------------------------------

QString SBServiceMac::fromUrl(void *in)
{
    NSURL *inUrl = (NSURL *)in;
    NSString *iStr = [inUrl absoluteString];
    const char *x = [iStr UTF8String];
    QUrl u = QUrl(QString::fromUtf8(x));
    return u.toLocalFile();
}

//-------------------------------------------------------------------------------------------

void *SBServiceMac::fromFilter(const QString& filter)
{
    if(!filter.isEmpty())
    {
        int i,state = 0,start = 0;
        QSet<QString> fArray;
        QSet<QString>::iterator ppI;

        for(i=0;i<filter.size();i++)
        {
            switch(state)
            {
                case 0:
                    if(filter.at(i)==QChar('*'))
                    {
                        state = 1;
                    }
                    break;

                case 1:
                    if(filter.at(i)==QChar('.'))
                    {
                        start = i + 1;
                        state = 2;
                    }
                    break;

                case 2:
                    if(!filter.at(i).isLetterOrNumber())
                    {
                        if(i>start)
                        {
                            fArray.insert(filter.mid(start,i-start));
                        }
                        state = 0;
                    }
                    break;
            }
        }

        if(!fArray.isEmpty())
        {
            NSMutableArray *arr = [NSMutableArray arrayWithCapacity:(fArray.size())];
            for(ppI=fArray.begin();ppI!=fArray.end();ppI++)
            {
                const QString& s = *ppI;
                NSString *nS = [NSString stringWithUTF8String:(s.toUtf8().constData())];
#if USE_UNIFORM_TYPE_IDENTIFIERS
                UTType *type = [UTType typeWithFilenameExtension: nS];
                if(type != nil)
                {
                    [arr addObject:type];
                }
#else
                CFStringRef uti = UTTypeCreatePreferredIdentifierForTag(
                kUTTagClassFilenameExtension, (CFStringRef)nS, NULL
                );
                if (uti) {
                    [arr addObject:(NSString *)uti];
                    CFRelease(uti);
                }
#endif
            }
            return (void *)arr;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

bool SBServiceMac::isReadOnly(const QString& fileName)
{
    return common::SBBookmark::isReadOnly(fileName);
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void *SBServiceMac::allocatePool()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    return (void *)pool;
}

//-------------------------------------------------------------------------------------------

void SBServiceMac::releasePool(void *pPtr)
{
    NSAutoreleasePool *pool = (NSAutoreleasePool *)pPtr;
    [pool release];
}

//-------------------------------------------------------------------------------------------
} // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
