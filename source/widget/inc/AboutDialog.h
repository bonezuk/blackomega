//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WIDGET_ABOUTDIALOG_H
#define __OMEGA_WIDGET_ABOUTDIALOG_H
//-------------------------------------------------------------------------------------------

#include <QDialog>
#include <QClipboard>
#include <QMessageBox>

#include "widget/ui_AboutDialog.h"
#include "widget/inc/WidgetDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

class WIDGET_EXPORT BuildVersionDialog : public QDialog
{
    Q_OBJECT
    
    public:
        BuildVersionDialog(QDialog *dialog = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~BuildVersionDialog();
    
    protected:
    
        virtual QString getFormatedVersionText();
        virtual QString getFormatedCopyrightText();
};

//-------------------------------------------------------------------------------------------

class WIDGET_EXPORT AboutDialog : public BuildVersionDialog
{
    Q_OBJECT
    
    public:
        AboutDialog(QDialog *dialog = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~AboutDialog();
        
    protected:
    
        Ui::AboutDialog ui;
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

