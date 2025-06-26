#include "player/inc/QPlayerApplication.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

QPlayerApplication *QPlayerApplication::m_instance = 0;

//-------------------------------------------------------------------------------------------

QPlayerApplication::QPlayerApplication(int& argc,char **argv) : QApplication(argc,argv)
{
    installEventFilter(this);
    m_instance = this;
#if QT_VERSION >= 0x050000
//    setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
}

//-------------------------------------------------------------------------------------------

QPlayerApplication::~QPlayerApplication()
{
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

QPlayerApplication *QPlayerApplication::playerInstance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

bool QPlayerApplication::eventFilter(QObject *obj,QEvent *evt)
{
    bool res = false;

    if(evt->type()==QEvent::KeyPress || evt->type()==QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(evt);
        if(keyEvent!=0)
        {
            remote::KeyControlServiceSPtr pKeyService = remote::KeyControlService::instance();
            if(pKeyService.data()!=0)
            {
                if(evt->type()==QEvent::KeyPress)
                {
                    res = pKeyService->onKeyPress(keyEvent);
                }
                else
                {
                    res = pKeyService->onKeyRelease(keyEvent);
                }
            }
        }
    }
    if(!res)
    {
        res = QApplication::eventFilter(obj,evt);
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
