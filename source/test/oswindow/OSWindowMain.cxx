#include "OSWindowMain.h"

#include <QEvent>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QFocusEvent>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#endif

#include "OSWindowMain.h"

#include <windows.h>

//-------------------------------------------------------------------------------------------
// QWinWidget
//-------------------------------------------------------------------------------------------

QWinWidget::QWinWidget(HWND hParentWnd,QObject *parent,Qt::WindowFlags f) : QWidget(0,f),
    m_hParent(hParentWnd),
    m_prevFocus(0),
    m_reEnableParent(false)
{
    if(parent)
    {
        QObject::setParent(parent);
    }
    init();
}

//-------------------------------------------------------------------------------------------

QWinWidget::~QWinWidget()
{}

//-------------------------------------------------------------------------------------------

void QWinWidget::init()
{
    if(m_hParent)
    {
        SetWindowLong((HWND)winId(),GWL_STYLE,WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

        QWindow *window = windowHandle();
        window->setProperty("_q_embedded_native_parent_handle",(WId)m_hParent);
#if QT_VERSION >= 0x050000
        HWND h = static_cast<HWND>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("handle",window));
#else
        HWND h = static_cast<HWND>(QApplication::platformNativeInterface()->nativeResourceForWindow("handle",window));
#endif
        SetParent(h,m_hParent);
        window->setFlags(Qt::FramelessWindowHint);

        QEvent e(QEvent::EmbeddingControl);
        QApplication::sendEvent(this,&e);
    }
}

//-------------------------------------------------------------------------------------------

HWND QWinWidget::parentWindow() const
{
    return m_hParent;
}

//-------------------------------------------------------------------------------------------

void QWinWidget::childEvent(QChildEvent *e)
{
    QObject *obj = e->child();
    if(obj->isWidgetType())
    {
        if(e->added())
        {
            if(obj->isWidgetType())
            {
                obj->installEventFilter(this);
            }
        }
        else if(e->removed() && m_reEnableParent)
        {
            m_reEnableParent = false;
            EnableWindow(m_hParent,true);
            obj->removeEventFilter(this);
        }
    }
    QWidget::childEvent(e);
}

//-------------------------------------------------------------------------------------------

void QWinWidget::saveFocus()
{
    if(!m_prevFocus)
    {
        m_prevFocus = ::GetFocus();
    }
    if(!m_prevFocus)
    {
        m_prevFocus = parentWindow();
    }
}

//-------------------------------------------------------------------------------------------

void QWinWidget::show()
{
    saveFocus();
    QWidget::show();
}

//-------------------------------------------------------------------------------------------

void QWinWidget::center()
{
    const QWidget *child = findChild<QWidget *>();
    if(child && !child->isWindow())
    {
        qWarning("QWinWidget::center: Call this function only for QWinWidgets with toplevel children");
    }
    RECT r;
    GetWindowRect(m_hParent,&r);
    setGeometry((r.right - r.left)/2 + r.left,(r.bottom - r.top)/2 + r.top,0,0);
}

//-------------------------------------------------------------------------------------------

void QWinWidget::showCentered()
{
    center();
    show();
}

//-------------------------------------------------------------------------------------------

void QWinWidget::resetFocus()
{
    if(m_prevFocus)
    {
        ::SetFocus(m_prevFocus);
    }
    else
    {
        ::SetFocus(parentWindow());
    }
}

//-------------------------------------------------------------------------------------------

bool QWinWidget::nativeEvent(const QByteArray& eventType,void *message,long *result)
{
    MSG *msg = (MSG *)message;

    if(msg->message==WM_SETFOCUS)
    {
        Qt::FocusReason reason;

        if(::GetKeyState(VK_LBUTTON)<0 || ::GetKeyState(VK_RBUTTON)<0)
        {
            reason = Qt::MouseFocusReason;
        }
        else if(::GetKeyState(VK_SHIFT)<0)
        {
            reason = Qt::BacktabFocusReason;
        }
        else
        {
            reason = Qt::TabFocusReason;
        }
        QFocusEvent e(QEvent::FocusIn,reason);
        QApplication::sendEvent(this,&e);
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool QWinWidget::eventFilter(QObject *o,QEvent *e)
{
    QWidget *w = (QWidget*)o;

    switch(e->type())
    {
        case QEvent::WindowDeactivate:
            if(w->isModal() && w->isHidden())
            {
                BringWindowToTop(m_hParent);
            }
            break;

        case QEvent::Hide:
            if(m_reEnableParent)
            {
                EnableWindow(m_hParent,true);
                m_reEnableParent = false;
            }
            resetFocus();
            if(w->testAttribute(Qt::WA_DeleteOnClose) && w->isWindow())
            {
                deleteLater();
            }
            break;

        case QEvent::Show:
            if(w->isWindow())
            {
                saveFocus();
                hide();
                if(w->isModal() && !m_reEnableParent)
                {
                    EnableWindow(m_hParent,false);
                    m_reEnableParent = true;
                }
            }
            break;

        case QEvent::Close:
            ::SetActiveWindow(m_hParent);
            if(w->testAttribute(Qt::WA_DeleteOnClose))
            {
                deleteLater();
            }
            break;

        default:
            break;
    }
    return QWidget::eventFilter(o,e);
}

//-------------------------------------------------------------------------------------------

void QWinWidget::focusInEvent(QFocusEvent *e)
{
    QWidget *candidate = this;

    switch(e->reason())
    {
        case Qt::TabFocusReason:
        case Qt::BacktabFocusReason:
            while(!(candidate->focusPolicy() & Qt::TabFocus))
            {
                candidate = candidate->nextInFocusChain();
                if(candidate==this)
                {
                    candidate = 0;
                    break;
                }
            }
            if(candidate)
            {
                candidate->setFocus(e->reason());
                if(e->reason()==Qt::BacktabFocusReason || e->reason()==Qt::TabFocusReason)
                {
                    candidate->setAttribute(Qt::WA_KeyboardFocusChange);
                    candidate->window()->setAttribute(Qt::WA_KeyboardFocusChange);
                }
                if(e->reason()==Qt::BacktabFocusReason)
                {
                    QWidget::focusNextPrevChild(false);
                }
            }
            break;

        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------

bool QWinWidget::focusNextPrevChild(bool next)
{
    QWidget *curFocus = focusWidget();

    if(!next)
    {
        if(!curFocus->isWindow())
        {
            QWidget *nextFocus = curFocus->nextInFocusChain();
            QWidget *prevFocus = 0;
            QWidget *topLevel = 0;

            while(nextFocus!=curFocus)
            {
                if(nextFocus->focusPolicy() & Qt::TabFocus)
                {
                    prevFocus = nextFocus;
                    topLevel = 0;
                }
                else if(nextFocus->isWindow())
                {
                    topLevel = nextFocus;
                }
                nextFocus = nextFocus->nextInFocusChain();
            }
            if(!topLevel)
            {
                return QWidget::focusNextPrevChild(false);
            }
        }
    }
    else
    {
        QWidget *nextFocus = curFocus;
        while(true)
        {
            if(nextFocus==0)
            {
                break;
            }
            nextFocus = nextFocus->nextInFocusChain();
            if(nextFocus->isWindow())
            {
                break;
            }
            if(nextFocus->focusPolicy() & Qt::TabFocus)
            {
                return QWidget::focusNextPrevChild(true);
            }
        }
    }
    ::SetFocus(m_hParent);
    return true;
}

//-------------------------------------------------------------------------------------------
// Win32 Implementation
//-------------------------------------------------------------------------------------------

HWND g_winId = 0;

//-------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message)
    {
        /*
        case WM_LBUTTONUP:
            {
                QWinWidget w(hWnd,0,0);
                w.showCentered();
                QMessageBox mb("Qt on Win32 - model","Is this dialog modal?",
                               QMessageBox::NoIcon,
                               QMessageBox::Yes | QMessageBox::Default,
                               QMessageBox::No | QMessageBox::Escape,
                               QMessageBox::NoButton,&w);
                int result = mb.exec();
                Q_UNUSED(result);
            }
            break;

        case WM_RBUTTONUP:
            {
                QWinWidget *w = new QWinWidget(hWnd,0,0);
                w->showCentered();
                QMessageBox *mb = new QMessageBox("Qt on Win32 - modeless","Is this dialog modal?",
                                                  QMessageBox::NoIcon,
                                                  QMessageBox::Yes | QMessageBox::Default,
                                                  QMessageBox::No | QMessageBox::Escape,
                                                  QMessageBox::NoButton,w);
                mb->setModal(false);
                mb->setAttribute(Qt::WA_DeleteOnClose);
                mb->show();
            }
            break;
            */

        case WM_KEYDOWN:
            {
                QString str = "Keypress down\n";
                OutputDebugStringA(str.toLocal8Bit().data());
            }
            if(wParam!=VK_TAB)
            {
                return DefWindowProc(hWnd,message,wParam,lParam);
            }
            SetFocus(g_winId);
            break;

        case WM_KEYUP:
            {
                QString str = "Keypress up\n";
                OutputDebugStringA(str.toLocal8Bit().data());
            }
            break;

        case WM_SETFOCUS:
            {
                QString str("Got focus");
                QWidget *widget = QWidget::find((WId)HWND(wParam));
                if(widget!=0)
                {
                    str += QString(" from %1 (%2)").arg(widget->objectName()).arg(widget->metaObject()->className());
                }
                str += "\n";
                OutputDebugStringA(str.toLocal8Bit().data());
            }
            break;

        case WM_KILLFOCUS:
            {
                QString str("Lost focus");
                QWidget *widget = QWidget::find((WId)HWND(wParam));
                if(widget!=0)
                {
                    str += QString(" to %1 (%2)").arg(widget->objectName()).arg(widget->metaObject()->className());
                }
                str += "\n";
                OutputDebugStringA(str.toLocal8Bit().data());
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd,message,wParam,lParam);
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    WNDCLASSEX wndClass;

    ::memset(&wndClass,0,sizeof(WNDCLASSEX));
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = (WNDPROC)WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = NULL;
    wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"qtest";
    wndClass.hIconSm = NULL;

    ATOM windowClass = RegisterClassEx(&wndClass);

    HWND hWnd = CreateWindow((TCHAR *)windowClass,
                             L"Windows Migration Framework Example",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT,
                             0,
                             CW_USEDEFAULT,
                             0,0,0,
                             hInstance,0);
    if(!hWnd)
    {
        return FALSE;
    }

    int argc = 0;
    QApplication a(argc,0);

    QWinWidget win(hWnd);
    g_winId = (HWND)win.winId();

    QHBoxLayout hbox(&win);
    hbox.setSpacing(5);
    hbox.setMargin(0);
    QPushButton *pb = new QPushButton("Qt command button",&win);
    pb->setObjectName("pb");
    hbox.addWidget(pb);
    QLabel *label = new QLabel("Some label",&win);
    label->setObjectName("label");
    hbox.addWidget(label);
    QLineEdit *le1 = new QLineEdit(&win);
    le1->setObjectName("le1");
    hbox.addWidget(le1);
    QLineEdit *le2 = new QLineEdit(&win);
    le1->setObjectName("le2");
    hbox.addWidget(le2);
    QLineEdit *le3 = new QLineEdit(&win);
    le1->setObjectName("le3");
    hbox.addWidget(le3);

    win.move(0,0);
    win.show();

    ShowWindow(hWnd,nCmdShow);
    UpdateWindow(hWnd);

    return a.exec();
}

//-------------------------------------------------------------------------------------------
