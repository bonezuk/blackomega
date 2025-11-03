//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WIDGET_IMPORTPLAYLISTDIALOG_H
#define __OMEGA_WIDGET_IMPORTPLAYLISTDIALOG_H
//-------------------------------------------------------------------------------------------

#include "widget/inc/SandBoxMac.h"
#include "widget/ui_ImportPlaylistDialog.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

class WIDGET_EXPORT ImportPlaylistDialog : public QDialog
{
    public:
        Q_OBJECT

    public:
        ImportPlaylistDialog(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~ImportPlaylistDialog();

        void setPlaylistFileName(const QString& fileName);
        void setDirectories(const QStringList& dirList);
        void setFileDependAccessOn();

        void onFolderOpen(const QStringList& files);
        void onCancel();

    protected:

        Ui::ImportPlaylistDialog ui;
        QString m_playlistFileName;
        QStringList m_directoryList;

        void *m_loader;
        int m_index;

        bool m_fileDependOn;

        QString htmlStart();
        QString htmlEnd();
        QString htmlParagraphStart(bool empty);
        QString htmlParagraphEnd();
        QString htmlParagraphEmpty();
        QString htmlSpanItalicPurple();
        QString htmlSpanBold();
        QString htmlSpanDirectory();
        QString htmlSpanUnderline();
        QString htmlSpanEnd();

        QString createHtmlInstructions();
        void updateInstructions();

        void *toUrl(const QString& fileName);

        void processFolder(int index);

    protected slots:

        void onNextButton();
        void onTimer();
};

//-------------------------------------------------------------------------------------------
} // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

