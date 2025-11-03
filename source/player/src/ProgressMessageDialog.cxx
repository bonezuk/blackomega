#include "player/inc/ProgressMessageDialog.h"
#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

ProgressMessageDialog::ProgressMessageDialog(const QString& txt,QWidget *parent,Qt::WindowFlags f) : QDialog(parent,f),
    m_timer(0),
    m_progressIndex(0)
{
    bool retinaFlag = isRetina();

    ui.setupUi(this);
    ui.m_label->setText(txt);

    for(int i=0;i<12;i++)
    {
        QString rIName = ":/progress/Resources/progress/alphaWait";
        rIName += common::BString::Int(i+1,2,true);
        rIName += (retinaFlag) ? "@2x.png" : ".png";
        QPixmap *wImage = new QPixmap(rIName);
        m_waitImageList.append(wImage);
    }

    onTimer();

    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);
    QObject::connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    m_timer->start(200);

    QObject::connect(ui.m_cancelButton,SIGNAL(clicked()),this,SLOT(onCancel()));
    m_cancel = false;
}

//-------------------------------------------------------------------------------------------

ProgressMessageDialog::~ProgressMessageDialog()
{
    delete m_timer;
}

//-------------------------------------------------------------------------------------------

void ProgressMessageDialog::setLabel(const QString& txt)
{
    ui.m_label->setText(txt);
}

//-------------------------------------------------------------------------------------------

bool ProgressMessageDialog::isRetina() const
{
    bool retinaFlag = false;

#if QT_VERSION >= 0x050000
    if(QPlayerApplication::playerInstance()->devicePixelRatio() >= 1.25)
    {
        retinaFlag = true;
    }
#endif
    return retinaFlag;
}

//-------------------------------------------------------------------------------------------

void ProgressMessageDialog::onTimer()
{
    ui.m_icon->setPixmap(*(m_waitImageList.at(m_progressIndex)));

    m_progressIndex++;
    if(m_progressIndex >= 12)
    {
        m_progressIndex = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool ProgressMessageDialog::isCancelled() const
{
    return m_cancel;
}

//-------------------------------------------------------------------------------------------

void ProgressMessageDialog::onCancel()
{
    m_cancel = true;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
