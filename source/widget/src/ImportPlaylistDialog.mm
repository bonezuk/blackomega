#include "widget/inc/ImportPlaylistDialog.h"
#include "track/info/inc/SBBookmarkService.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include <QTimer>

//-------------------------------------------------------------------------------------------

@interface ImportPlaylistDialogLoader : NSObject
{
    omega::widget::ImportPlaylistDialog *dialog;
};
- (id)initWithDialog:(omega::widget::ImportPlaylistDialog *)dlg;
- (void)dealloc;
- (void)doFolderDialogWithWindow:(NSWindow *)win Directory:(NSURL *)dir;
@end

//-------------------------------------------------------------------------------------------
@implementation ImportPlaylistDialogLoader
//-------------------------------------------------------------------------------------------

- (id)initWithDialog:(omega::widget::ImportPlaylistDialog *)dlg
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
}

//-------------------------------------------------------------------------------------------
@end
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

ImportPlaylistDialog::ImportPlaylistDialog(QWidget *parent,Qt::WindowFlags f) : QDialog(parent,f),
    m_playlistFileName(),
    m_directoryList(),
    m_index(0),
    m_fileDependOn(false)
{
    ui.setupUi(this);
    QObject::connect(ui.m_nextButton,SIGNAL(clicked()),this,SLOT(onNextButton()));
    
    ImportPlaylistDialogLoader *loader = [[ImportPlaylistDialogLoader alloc] initWithDialog:this];
    m_loader = (void *)loader;
}

//-------------------------------------------------------------------------------------------

ImportPlaylistDialog::~ImportPlaylistDialog()
{
    ImportPlaylistDialogLoader *loader = (ImportPlaylistDialogLoader *)m_loader;
    [loader release];
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::setPlaylistFileName(const QString& fileName)
{
    m_playlistFileName = fileName;
    updateInstructions();
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::setDirectories(const QStringList& dirList)
{
    m_directoryList = dirList;
    updateInstructions();
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlStart()
{
    QString x;
    x += QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">");
    x += QString::fromUtf8("<html>");
    x += QString::fromUtf8("<head>");
    x += QString::fromUtf8("<meta name=\"qrichtext\" content=\"1\" />");
    x += QString::fromUtf8("<style type=\"text/css\">");
    x += QString::fromUtf8("p, li { white-space: pre-wrap; }");
    x += QString::fromUtf8("</style>");
    x += QString::fromUtf8("</head>");
    x += QString::fromUtf8("<body style=\" font-family:'.Lucida Grande UI'; font-size:13pt; font-weight:400; font-style:normal;\">");
    return x;
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlEnd()
{
    QString x;
    x += QString::fromUtf8("</body>");
    x += QString::fromUtf8("</html>");
    return x;
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlParagraphStart(bool empty)
{
    QString x;
    x += QString::fromUtf8("<p style=\"");
    if(empty)
    {
        x += QString::fromUtf8("-qt-paragraph-type:empty;");
    }
    x += QString::fromUtf8(" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">");
    return x;
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlParagraphEnd()
{
    return QString::fromUtf8("</p>");
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlParagraphEmpty()
{
    QString x;
    x += htmlParagraphStart(true);
    x += QString::fromUtf8("<br />");
    x += htmlParagraphEnd();
    return x;
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlSpanItalicPurple()
{
    return QString::fromUtf8("<span style=\" font-style:italic; color:#b71ea0;\">");
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlSpanBold()
{
    return QString::fromUtf8("<span style=\" font-weight:600;\">");
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlSpanDirectory()
{
    return QString::fromUtf8("<span style=\" font-weight:600; color:#325fda;\">");
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlSpanUnderline()
{
    return QString::fromUtf8("<span style=\" font-weight:600; text-decoration: underline;\">");
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::htmlSpanEnd()
{
    return QString::fromUtf8("</span>");
}

//-------------------------------------------------------------------------------------------

QString ImportPlaylistDialog::createHtmlInstructions()
{
    QStringList::const_iterator ppI;
    QString x;
    
    x  = htmlStart();
    
    x += htmlParagraphStart(false);
    if(!m_playlistFileName.isEmpty())
    {
        x += QString::fromUtf8("In order to load the music files referenced from playlist file ");
        x += htmlSpanItalicPurple();
#if QT_VERSION >= 0x050000
        x += m_playlistFileName.toHtmlEscaped();
#else
        x += m_playlistFileName;
#endif
        x += htmlSpanEnd() + " the ";
    }
    else
    {
        if(m_fileDependOn)
        {
            x += QString::fromUtf8("In order to properly render the selected music files ");
        }
        else
        {
            x += QString::fromUtf8("In order to load the iTunes music files the ");
        }
    }
    x += htmlSpanBold() + "Black Omega" + htmlSpanEnd();
    if(m_fileDependOn)
    {    
        x += QString::fromUtf8(" app requires your permission to be able to access auxiliary files located in the following ");
    }
    else
    {
        x += QString::fromUtf8(" app requires your permission to be able to access these files located in the following ");
    }
    x += (m_directoryList.size()==1) ? "directory." : "directories.";
    x += htmlParagraphEnd();
    
    x += htmlParagraphEmpty();
    for(ppI=m_directoryList.begin();ppI!=m_directoryList.end();ppI++)
    {
        x += htmlParagraphStart(false);
        x += htmlSpanDirectory();
#if QT_VERSION >= 0x050000
        x += (*ppI).toHtmlEscaped();
#else
        x += *ppI;
#endif
        x += htmlSpanEnd();
        x += htmlParagraphEnd();
    }
    x += htmlParagraphEmpty();
    
    x += htmlParagraphStart(false);
    x += QString::fromUtf8("On clicking ");
    x += htmlSpanUnderline() + "Next" + htmlSpanEnd();
    if(m_directoryList.size()>1)
    {
        x += QString::fromUtf8(", you’ll be presented with a series of folder selection dialogs with the corresponding folders pre-selected. ");
    }
    else
    {
        x += QString::fromUtf8(", you’ll be presented a folder selection dialog with the given folder pre-selected. ");
    }
    x += QString::fromUtf8("Please select ");
    x += htmlSpanUnderline() + "Open" + htmlSpanEnd();
    x += QString::fromUtf8(" for each.");
    x += htmlParagraphEnd();
    
    x += htmlParagraphEmpty();
    x += htmlParagraphStart(false);
    x += QString::fromUtf8("Selecting ");
    x += htmlSpanUnderline() + "Cancel" + htmlSpanEnd();
    x += QString::fromUtf8(" will stop the files located in the given folder from being loaded into the playlist.");
    x += htmlParagraphEnd();
    
    x += htmlEnd();

    return x;
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::updateInstructions()
{
    QString txt = createHtmlInstructions();
    ui.m_instructionText->setText(txt);
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::onNextButton()
{
    ui.m_pages->setCurrentIndex(1);
    m_index = 0;
    processFolder(m_index);
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::onTimer()
{
    m_index++;
    processFolder(m_index);
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::processFolder(int index)
{
    if(index>=0 && index<m_directoryList.size())
    {
        NSView *mainView = (NSView *)winId();
        NSWindow *mainWindow = [mainView window];
        ImportPlaylistDialogLoader *loader = (ImportPlaylistDialogLoader *)m_loader;
        NSURL *u = (NSURL *)toUrl(m_directoryList.at(index));
        if(u!=nil)
        {
            [loader doFolderDialogWithWindow:mainWindow Directory:u];
        }        
    }
    else
    {
        QSharedPointer<track::info::SBBookmarkService> pSBService = track::info::SBBookmarkService::instance();
        bool res = false;
        
        for(QStringList::const_iterator ppI=m_directoryList.begin();ppI!=m_directoryList.end();ppI++)
        {
            if(pSBService->has(*ppI,true))
            {
                res = true;
            }
        }
        
        done((res) ? QDialog::Accepted : QDialog::Rejected);
    }
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::onFolderOpen(const QStringList& files)
{
    QSharedPointer<track::info::SBBookmarkService> pSBService = track::info::SBBookmarkService::instance();
    
    for(QStringList::const_iterator ppI=files.begin();ppI!=files.end();ppI++)
    {
        pSBService->add(*ppI,true);
    }
    QTimer::singleShot(500,this,SLOT(onTimer()));
}

//-------------------------------------------------------------------------------------------

void ImportPlaylistDialog::onCancel()
{
    QTimer::singleShot(500,this,SLOT(onTimer()));
}

//-------------------------------------------------------------------------------------------

void *ImportPlaylistDialog::toUrl(const QString& fileName)
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

void ImportPlaylistDialog::setFileDependAccessOn()
{
    m_fileDependOn = true;
    updateInstructions();
}

//-------------------------------------------------------------------------------------------
} // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
