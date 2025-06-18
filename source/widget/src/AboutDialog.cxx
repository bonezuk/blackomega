#include "widget/inc/AboutDialog.h"
#include "common/inc/ProductVersionInfo.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

BuildVersionDialog::BuildVersionDialog(QDialog *dialog,Qt::WindowFlags f) : QDialog(dialog,f)
{}

//-------------------------------------------------------------------------------------------

BuildVersionDialog::~BuildVersionDialog()
{}

//-------------------------------------------------------------------------------------------

QString BuildVersionDialog::getFormatedVersionText()
{
    common::ProductVersionInfoSPtr pVersion = common::ProductVersionInfo::instance();

    QStringList s;
    s << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";
    s << "<html><head><meta name=\"qrichtext\" content=\"1\" />";
    s << "<style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:";
#if defined(OMEGA_WIN32)
    s << "8";
#else
    s << "12";
#endif
    s << "pt; font-weight:400; font-style:normal;\">";
    s << "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
    s << "<span style=\" font-size:16pt; font-weight:600;\">";
    s << "Black Omega ";
    if(pVersion.data()!=0)
    {
        s << pVersion->version();
    }
    else
    {
        s << "2.1.1";
    }
    s << "</span>";
    s << "</p>";
    s << "</body>";
    s << "</html>";

    QString txt = s.join("");
    return txt;
}

//-------------------------------------------------------------------------------------------

QString BuildVersionDialog::getFormatedCopyrightText()
{
    QString copyright = "Copyright 1999 - 2024 Stuart A. MacLean. All rights reserved.";
    common::ProductVersionInfoSPtr pVersion = common::ProductVersionInfo::instance();
    if(pVersion.data()!=0)
    {
        copyright = pVersion->copyright();
    }
    return copyright;
}

//-------------------------------------------------------------------------------------------

AboutDialog::AboutDialog(QDialog *dialog,Qt::WindowFlags f) : BuildVersionDialog(dialog,f)
{
    ui.setupUi(this);
    ui.m_blackOmegaLabel->setText(getFormatedVersionText());
    ui.m_copyrightLabel->setText(getFormatedCopyrightText());
    
    QApplication *app = dynamic_cast<QApplication *>(QCoreApplication::instance());
    if(app!=0)
    {
#if QT_VERSION >= 0x050000
        if(app->devicePixelRatio() >= 1.25)
        {
            QPixmap pixLogo(":/logo/Resources/omegaLogo48x48@2x.png");
            ui.m_logoImageLabel->setPixmap(pixLogo);
        }
        else
#endif
        {
            QPixmap pixLogo(":/logo/Resources/omegaLogo48x48.png");
            ui.m_logoImageLabel->setPixmap(pixLogo);
        }
    }
}

//-------------------------------------------------------------------------------------------

AboutDialog::~AboutDialog()
{}

//-------------------------------------------------------------------------------------------
} // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
