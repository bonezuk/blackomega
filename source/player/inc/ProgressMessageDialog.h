//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_PROGRESSMESSAGEDIALOG_H
#define __OMEGA_PLAYER_PROGRESSMESSAGEDIALOG_H
//-------------------------------------------------------------------------------------------

#include "player/ui_ProgressMessageDialog.h"

#include <QDialog>
#include <QTimer>
#include <QList>
#include <QPixmap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class ProgressMessageDialog : public QDialog
{
    public:
        Q_OBJECT
    
    public:
        ProgressMessageDialog(const QString& txt,QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~ProgressMessageDialog();
        
        void setLabel(const QString& txt);
        
        bool isCancelled() const;

    protected:
    
        Ui::ProgressMessageDialog ui;
        QTimer *m_timer;
        int m_progressIndex;
        QList<QPixmap *> m_waitImageList;
        bool m_cancel;

        bool isRetina() const;
        
    protected slots:
    
        void onTimer();
        void onCancel();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

