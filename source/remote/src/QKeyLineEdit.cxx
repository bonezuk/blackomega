#include "remote/inc/QKeyLineEdit.h"
#include "remote/inc/KeyCodeExclusion.h"

#include <QPainter>
#include <QLineEdit>
#include <QStyleOptionFrame>
#include <QStyle>
#include <QSpinBox>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

QKeyLineEdit::QKeyLineEdit(WinLIRCRemoteProxyIF *remoteProxy,QWidget *parent,Qt::WindowFlags f) : QWidget(parent,f),
    m_blinkState(0),
    m_timer(0),
    m_dummyEdit(0),
    m_key()
{
    m_timer = new QTimer(this);
    QObject::connect(m_timer,SIGNAL(timeout()),this,SLOT(blinkTimer()));
    m_dummyEdit = new QLineEdit;
    setFocusPolicy(Qt::StrongFocus);
    m_iconKeyboard = new QPixmap(":/input/Resources/keyboard.png");
    m_iconRemote = new QPixmap(":/input/Resources/remote.png");
    m_remoteProxy = remoteProxy;
}

//-------------------------------------------------------------------------------------------

QKeyLineEdit::~QKeyLineEdit()
{
    delete m_dummyEdit;
    delete m_iconKeyboard;
    delete m_iconRemote;
}

//-------------------------------------------------------------------------------------------

KeyCode& QKeyLineEdit::key()
{
    return m_key;
}

//-------------------------------------------------------------------------------------------

const KeyCode& QKeyLineEdit::key() const
{
    return m_key;
}

//-------------------------------------------------------------------------------------------

void QKeyLineEdit::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    QStyleOptionFrame panel;
    panel.initFrom(m_dummyEdit);
    panel.fontMetrics = painter.fontMetrics();

    QSize lSize = style()->sizeFromContents(QStyle::CT_LineEdit,&panel,size(),m_dummyEdit);

    panel.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth,&panel,m_dummyEdit);
    panel.rect = QRect(0,0,width(),height());
    panel.midLineWidth = 0;
    panel.state |= QStyle::State_Sunken;
    if(hasFocus())
    {
        panel.state |= QStyle::State_HasFocus | QStyle::State_KeyboardFocusChange;
    }
    panel.features = QStyleOptionFrame::None;

    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &painter, this);

    int offset = style()->pixelMetric(QStyle::PM_DefaultFrameWidth,&panel,m_dummyEdit);

    QPixmap *icon = 0;
    if(!m_key.isEmpty())
    {
        icon = (m_key.isRemote()) ? m_iconRemote : m_iconKeyboard;
    }

    QString sText = text();
    QFontMetrics fontMetrics = painter.fontMetrics();
    QSize s = fontMetrics.size(Qt::TextSingleLine,sText);

    QTextOption textOptions(Qt::AlignLeft);
    textOptions.setWrapMode(QTextOption::NoWrap);

    tfloat64 cYPos = static_cast<tfloat64>(geometry().height()) / 2.0;
    tfloat64 tYPos = cYPos - (static_cast<tfloat64>(fontMetrics.height()) / 2.0);
    tfloat64 iYPos = cYPos - (static_cast<tfloat64>((icon!=0) ? icon->height() : 0) / 2.0);

    int tIYPos = static_cast<int>(::floor(tYPos));
    int iIYPos = static_cast<int>(::floor(iYPos));
    int xPos = offset + fontMetrics.horizontalAdvance(QChar(' '));

    QRect rIcon(xPos,iIYPos,(icon!=0) ? icon->width() : 0, (icon!=0) ? icon->height() : 0);
    QRect rText(rIcon.right()+4,tIYPos,lSize.width() - rIcon.right() + 4,s.height());

    if(icon!=0)
    {
        painter.drawPixmap(rIcon.left(),rIcon.top(),*icon);
    }
    painter.drawText(rText,sText,textOptions);

    if(hasFocus() && m_blinkState)
    {
        int blinkXPos = rText.left() + s.width() + 1;
        if(blinkXPos < rText.right())
        {
            int blinkYEnd = tIYPos + s.height();
            painter.drawLine(blinkXPos,tIYPos,blinkXPos,blinkYEnd);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QKeyLineEdit::blinkTimer()
{
    m_blinkState = (m_blinkState==0) ? 1 : 0;
    repaint();
}

//-------------------------------------------------------------------------------------------

void QKeyLineEdit::focusInEvent(QFocusEvent *e)
{
    if(e->gotFocus())
    {
        m_timer->start(QApplication::cursorFlashTime() / 2);
        repaint();
    }
    QWidget::focusInEvent(e);
}

//-------------------------------------------------------------------------------------------

void QKeyLineEdit::focusOutEvent(QFocusEvent *e)
{
    if(e->lostFocus())
    {
        m_timer->stop();
    }
    QWidget::focusOutEvent(e);
}

//-------------------------------------------------------------------------------------------

void QKeyLineEdit::onKeyReleaseEvent(QKeyEvent *e)
{
    if(hasFocus())
    {
        tuint32 keyRelease = static_cast<tuint32>(e->key()) & 0x01ffffff;
        
        if(!isModifier(keyRelease))
        {
            keyRelease = codeForKeyEvent(e);
            m_key.setKeyCode(keyRelease);
        }
    }
    else
    {
        QWidget::keyReleaseEvent(e);
    }
}

//-------------------------------------------------------------------------------------------

tuint32 QKeyLineEdit::codeForKeyEvent(QKeyEvent *e)
{
    tuint32 keyRelease = static_cast<tuint32>(e->key()) & 0x01ffffff;
        
    if(!isModifier(keyRelease))
    {
        Qt::KeyboardModifiers m = e->modifiers();
            
        if(m & Qt::ShiftModifier)
        {
            keyRelease |= static_cast<tuint32>(Qt::ShiftModifier);
        }
        if(m & Qt::ControlModifier)
        {
            keyRelease |= static_cast<tuint32>(Qt::ControlModifier);
        }
        if(m & Qt::AltModifier)
        {
            keyRelease |= static_cast<tuint32>(Qt::AltModifier);
        }
        if(m & Qt::MetaModifier)
        {
            keyRelease |= static_cast<tuint32>(Qt::MetaModifier);
        }
    }
    return keyRelease;
}

//-------------------------------------------------------------------------------------------

bool QKeyLineEdit::isModifier(tuint32 code)
{
    Qt::Key k = static_cast<Qt::Key>(code);
    
    if(k==Qt::Key_Shift || k==Qt::Key_Control || k==Qt::Key_Alt || k==Qt::Key_Meta)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

QString QKeyLineEdit::text()
{
    if(m_key.isEmpty())
    {
        return QString();
    }
    else if(m_key.isRemote())
    {
        return m_key.remote();
    }
    else
    {
        return textForCode(m_key.keyCode());
    }
}

//-------------------------------------------------------------------------------------------

QString QKeyLineEdit::textForCode(tuint32 kCode)
{
    QString kText,kS;
    bool plusFlag = false;
    
    kS = textForKeyCode(static_cast<Qt::Key>(kCode & 0x01ffffff));
    if(!kS.isEmpty())
    {
        if(kCode & static_cast<tuint32>(Qt::ControlModifier))
        {
            kText = textForCode(Qt::Key_Control);
            plusFlag = true;
        }
        if(kCode & static_cast<tuint32>(Qt::MetaModifier))
        {
            if(plusFlag)
            {
                kText += " + ";
            }
            kText += textForCode(Qt::Key_Meta);
            plusFlag = true;
        }
        if(kCode & static_cast<tuint32>(Qt::ShiftModifier))
        {
            if(plusFlag)
            {
                kText += " + ";
            }
            kText += textForCode(Qt::Key_Shift);
            plusFlag = true;
        }
        if(kCode & static_cast<tuint32>(Qt::AltModifier))
        {
            if(plusFlag)
            {
                kText += " + ";
            }
            kText += textForCode(Qt::Key_Alt);
            plusFlag = true;
        }
        if(plusFlag)
        {
            kText += " + ";
        }
        kText += kS;
    }
    return kText;
}

//-------------------------------------------------------------------------------------------

QString QKeyLineEdit::textForKeyCode(Qt::Key key)
{
    QString k;

    switch(key)
    {
        case Qt::Key_Escape:
            k = "Esc";
            break;
        case Qt::Key_Tab:
            k = QChar(0x21e5);
            break;
        case Qt::Key_Backtab:
            k = QChar(0x21e4);
            break;
        case Qt::Key_Backspace:
            k = QChar(0x21e6);
            break;
        case Qt::Key_Return:
            k = QChar(0x21a9);
            break;
        case Qt::Key_Enter:
            k = "Enter";
            break;
        case Qt::Key_Insert:
            k = "Ins";
            break;
        case Qt::Key_Delete:
            k = "Del";
            break;
        case Qt::Key_Pause:
            k = "Pause";
            break;
        case Qt::Key_Print:
            k = "Print";
            break;
        case Qt::Key_SysReq:
            k = "SysReq";
            break;
        case Qt::Key_Clear:
            k = "Clear";
            break;
        case Qt::Key_Home:
            k = "Home";
            break;
        case Qt::Key_End:
            k = "End";
            break;
        case Qt::Key_Left:
            k = QChar(0x2190);
            break;
        case Qt::Key_Up:
            k = QChar(0x2191);
            break;
        case Qt::Key_Right:
            k = QChar(0x2192);
            break;
        case Qt::Key_Down:
            k = QChar(0x2193);
            break;
        case Qt::Key_PageUp:
            k = "Page-Up";
            break;
        case Qt::Key_PageDown:
            k = "Page-Down";
            break;
        case Qt::Key_Shift:
            {
                QString kArrow = QChar(0x21e7);
                k = "Shift (" + kArrow + ")";
            }
            break;
        case Qt::Key_Control:
#if defined(Q_OS_MAC)
            k = QChar(0x2318);
#else
            k = "Ctrl";
#endif
            break;
        case Qt::Key_Meta:
#if defined(Q_OS_MAC)
            k = "Ctrl";
#else
            k = QChar(0x00ff);
#endif
            break;
        
        case Qt::Key_Alt:
            k = "Alt";
            break;
        case Qt::Key_AltGr:
            k = "Alt-Gr";
            break;
        case Qt::Key_CapsLock:
            k = "Caps Lock";
            break;
        case Qt::Key_NumLock:
            k = "Num Lock";
            break;
        case Qt::Key_ScrollLock:
            k = "Scroll Lock";
            break;
        case Qt::Key_F1:
            k = "F1";
            break;
        case Qt::Key_F2:
            k = "F2";
            break;
        case Qt::Key_F3:
            k = "F3";
            break;
        case Qt::Key_F4:
            k = "F4";
            break;
        case Qt::Key_F5:
            k = "F5";
            break;
        case Qt::Key_F6:
            k = "F6";
            break;
        case Qt::Key_F7:
            k = "F7";
            break;
        case Qt::Key_F8:
            k = "F8";
            break;
        case Qt::Key_F9:
            k = "F9";
            break;
        case Qt::Key_F10:
            k = "F10";
            break;
        case Qt::Key_F11:
            k = "F11";
            break;
        case Qt::Key_F12:
            k = "F12";
            break;
        case Qt::Key_F13:
            k = "F13";
            break;
        case Qt::Key_F14:
            k = "F14";
            break;
        case Qt::Key_F15:
            k = "F15";
            break;
        case Qt::Key_F16:
            k = "F16";
            break;
        case Qt::Key_F17:
            k = "F17";
            break;
        case Qt::Key_F18:
            k = "F18";
            break;
        case Qt::Key_F19:
            k = "F19";
            break;
        case Qt::Key_F20:
            k = "F20";
            break;
        case Qt::Key_F21:
            k = "F21";
            break;
        case Qt::Key_F22:
            k = "F22";
            break;
        case Qt::Key_F23:
            k = "F23";
            break;
        case Qt::Key_F24:
            k = "F24";
            break;
        case Qt::Key_F25:
            k = "F25";
            break;
        case Qt::Key_F26:
            k = "F26";
            break;
        case Qt::Key_F27:
            k = "F27";
            break;
        case Qt::Key_F28:
            k = "F28";
            break;
        case Qt::Key_F29:
            k = "F29";
            break;
        case Qt::Key_F30:
            k = "F30";
            break;
        case Qt::Key_F31:
            k = "F31";
            break;
        case Qt::Key_F32:
            k = "F32";
            break;
        case Qt::Key_F34:
            k = "F34";
            break;
        case Qt::Key_F35:
            k = "F35";
            break;
        case Qt::Key_Super_L:
            k = "Super-Left";
            break;
        case Qt::Key_Super_R:
            k = "Super-Right";
            break;
        case Qt::Key_Menu:
            k = "Menu";
            break;
        case Qt::Key_Hyper_L:
            k = "Hyper-Left";
            break;
        case Qt::Key_Hyper_R:
            k = "Hyper-Right";
            break;
        case Qt::Key_Help:
            k = "Help";
            break;
        case Qt::Key_Direction_L:
            k = QChar(0x21d0);
            break;
        case Qt::Key_Direction_R:
            k = QChar(0x21d2);
            break;
        case Qt::Key_Space:
            k = "Space";
            break;
        case Qt::Key_Exclam:
            k = "!";
            break;
        case Qt::Key_NumberSign:
            k = QChar(0x0023);
            break;
        case Qt::Key_Dollar:
            k = "$";
            break;
        case Qt::Key_Percent:
            k = "%%";
            break;
        case Qt::Key_Ampersand:
            k = "&";
            break;
        case Qt::Key_Apostrophe:
            k = "'";
            break;
        case Qt::Key_ParenLeft:
            k = "(";
            break;
        case Qt::Key_ParenRight:
            k = ")";
            break;
        case Qt::Key_Asterisk:
            k = "*";
            break;
        case Qt::Key_Plus:
            k = "+";
            break;
        case Qt::Key_Comma:
            k = ",";
            break;
        case Qt::Key_Minus:
            k = "-";
            break;
        case Qt::Key_Period:
            k = ".";
            break;
        case Qt::Key_Slash:
            k = "/";
            break;
        case Qt::Key_0:
            k = "0";
            break;
        case Qt::Key_1:
            k = "1";
            break;
        case Qt::Key_2:
            k = "2";
            break;
        case Qt::Key_3:
            k = "3";
            break;
        case Qt::Key_4:
            k = "4";
            break;
        case Qt::Key_5:
            k = "5";
            break;
        case Qt::Key_6:
            k = "6";
            break;
        case Qt::Key_7:
            k = "7";
            break;
        case Qt::Key_8:
            k = "8";
            break;
        case Qt::Key_9:
            k = "9";
            break;
        case Qt::Key_Colon:
            k = ":";
            break;
        case Qt::Key_Semicolon:
            k = ";";
            break;
        case Qt::Key_Less:
            k = "<";
            break;
        case Qt::Key_Equal:
            k = "=";
            break;
        case Qt::Key_Greater:
            k = ">";
            break;
        case Qt::Key_Question:
            k = "?";
            break;
        case Qt::Key_At:
            k = "@";
            break;
        case Qt::Key_A:
            k = "A";
            break;
        case Qt::Key_B:
            k = "B";
            break;
        case Qt::Key_C:
            k = "C";
            break;
        case Qt::Key_D:
            k = "D";
            break;
        case Qt::Key_E:
            k = "E";
            break;
        case Qt::Key_F:
            k = "F";
            break;
        case Qt::Key_G:
            k = "G";
            break;
        case Qt::Key_H:
            k = "H";
            break;
        case Qt::Key_I:
            k = "I";
            break;
        case Qt::Key_J:
            k = "J";
            break;
        case Qt::Key_K:
            k = "K";
            break;
        case Qt::Key_L:
            k = "L";
            break;
        case Qt::Key_M:
            k = "M";
            break;
        case Qt::Key_N:
            k = "N";
            break;
        case Qt::Key_O:
            k = "O";
            break;
        case Qt::Key_P:
            k = "P";
            break;
        case Qt::Key_Q:
            k = "Q";
            break;
        case Qt::Key_R:
            k = "R";
            break;
        case Qt::Key_S:
            k = "S";
            break;
        case Qt::Key_T:
            k = "T";
            break;
        case Qt::Key_U:
            k = "U";
            break;
        case Qt::Key_V:
            k = "V";
            break;
        case Qt::Key_W:
            k = "W";
            break;
        case Qt::Key_X:
            k = "X";
            break;
        case Qt::Key_Y:
            k = "Y";
            break;
        case Qt::Key_Z:
            k = "Z";
            break;
        case Qt::Key_BracketLeft:
            k = "[";
            break;
        case Qt::Key_Backslash:
            k = "\\";
            break;
        case Qt::Key_BracketRight:
            k = "]";
            break;
        case Qt::Key_AsciiCircum:
            k = "^";
            break;
        case Qt::Key_Underscore:
            k = "_";
            break;
        case Qt::Key_QuoteLeft:
            k = "`";
            break;
        case Qt::Key_BraceLeft:
            k = "{";
            break;
        case Qt::Key_Bar:
            k = "|";
            break;
        case Qt::Key_BraceRight:
            k = "}";
            break;
        case Qt::Key_AsciiTilde:
            k = "~";
            break;
        case Qt::Key_nobreakspace:
            k = "No-Break-Space";
            break;
        case Qt::Key_exclamdown:
            k = QChar(0x00a1);
            break;
        case Qt::Key_cent:
            k = QChar(0x00a2);
            break;
        case Qt::Key_sterling:
            k = QChar(0x00a3);
            break;
        case Qt::Key_currency:
            k = QChar(0x00a4);
            break;
        case Qt::Key_yen:
            k = QChar(0x00a5);
            break;
        case Qt::Key_brokenbar:
            k = QChar(0x00a6);
            break;
        case Qt::Key_section:
            k = QChar(0x00a7);
            break;
        case Qt::Key_diaeresis:
            k = QChar(0x00a8);
            break;
        case Qt::Key_copyright:
            k = QChar(0x00a9);
            break;
        case Qt::Key_ordfeminine:
            k = QChar(0x00aa);
            break;
        case Qt::Key_guillemotleft:
            k = QChar(0x00ab);
            break;
        case Qt::Key_notsign:
            k = QChar(0x00ac);
            break;
        case Qt::Key_hyphen:
            k = QChar(0x00ad);
            break;
        case Qt::Key_registered:
            k = QChar(0x00ae);
            break;
        case Qt::Key_macron:
            k = QChar(0x00af);
            break;
        case Qt::Key_degree:
            k = QChar(0x00b0);
            break;
        case Qt::Key_plusminus:
            k = QChar(0x00b1);
            break;
        case Qt::Key_twosuperior:
            k = QChar(0x00b2);
            break;
        case Qt::Key_threesuperior:
            k = QChar(0x00b3);
            break;
        case Qt::Key_acute:
            k = QChar(0x00b4);
            break;
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        case Qt::Key_mu:
#else
        case Qt::Key_micro:
#endif
            k = QChar(0x00b5);
            break;
        case Qt::Key_paragraph:
            k = QChar(0x00b6);
            break;
        case Qt::Key_periodcentered:
            k = QChar(0x00b7);
            break;
        case Qt::Key_cedilla:
            k = QChar(0x00b8);
            break;
        case Qt::Key_onesuperior:
            k = QChar(0x00b9);
            break;
        case Qt::Key_masculine:
            k = QChar(0x00ba);
            break;
        case Qt::Key_guillemotright:
            k = QChar(0x00bb);
            break;
        case Qt::Key_onequarter:
            k = QChar(0x00bc);
            break;
        case Qt::Key_onehalf:
            k = QChar(0x00bd);
            break;
        case Qt::Key_threequarters:
            k = QChar(0x00be);
            break;
        case Qt::Key_questiondown:
            k = QChar(0x00bf);
            break;
        case Qt::Key_Agrave:
            k = QChar(0x00c0);
            break;
        case Qt::Key_Aacute:
            k = QChar(0x00c1);
            break;
        case Qt::Key_Acircumflex:
            k = QChar(0x00c2);
            break;
        case Qt::Key_Atilde:
            k = QChar(0x00c3);
            break;
        case Qt::Key_Adiaeresis:
            k = QChar(0x00c4);
            break;
        case Qt::Key_Aring:
            k = QChar(0x00c5);
            break;
        case Qt::Key_AE:
            k = QChar(0x00c6);
            break;
        case Qt::Key_Ccedilla:
            k = QChar(0x00c7);
            break;
        case Qt::Key_Egrave:
            k = QChar(0x00c8);
            break;
        case Qt::Key_Eacute:
            k = QChar(0x00c9);
            break;
        case Qt::Key_Ecircumflex:
            k = QChar(0x00ca);
            break;
        case Qt::Key_Ediaeresis:
            k = QChar(0x00cb);
            break;
        case Qt::Key_Igrave:
            k = QChar(0x00cc);
            break;
        case Qt::Key_Iacute:
            k = QChar(0x00cd);
            break;
        case Qt::Key_Icircumflex:
            k = QChar(0x00ce);
            break;
        case Qt::Key_Idiaeresis:
            k = QChar(0x00cf);
            break;
        case Qt::Key_ETH:
            k = QChar(0x00d0);
            break;
        case Qt::Key_Ntilde:
            k = QChar(0x00d1);
            break;
        case Qt::Key_Ograve:
            k = QChar(0x00d2);
            break;
        case Qt::Key_Oacute:
            k = QChar(0x00d3);
            break;
        case Qt::Key_Ocircumflex:
            k = QChar(0x00d4);
            break;
        case Qt::Key_Otilde:
            k = QChar(0x00d5);
            break;
        case Qt::Key_Odiaeresis:
            k = QChar(0x00d6);
            break;
        case Qt::Key_multiply:
            k = QChar(0x00d7);
            break;
        case Qt::Key_Ooblique:
            k = QChar(0x00d8);
            break;
        case Qt::Key_Ugrave:
            k = QChar(0x00d9);
            break;
        case Qt::Key_Uacute:
            k = QChar(0x00da);
            break;
        case Qt::Key_Ucircumflex:
            k = QChar(0x00db);
            break;
        case Qt::Key_Udiaeresis:
            k = QChar(0x00de);
            break;
        case Qt::Key_Yacute:
            k = QChar(0x00dd);
            break;
        case Qt::Key_THORN:
            k = QChar(0x00de);
            break;
        case Qt::Key_ssharp:
            k = QChar(0x00df);
            break;
        case Qt::Key_division:
            k = QChar(0x00f7);
            break;
        case Qt::Key_ydiaeresis:
            k = QChar(0x00ff);
            break;
        case Qt::Key_Back:
            k = QChar(0x27fd);
            break;
        case Qt::Key_Forward:
            k = QChar(0x27fe);
            break;
        case Qt::Key_Stop:
            k = "Stop";
            break;
        case Qt::Key_Refresh:
            k = "Refresh";
            break;
        case Qt::Key_VolumeDown:
            k = "Volume-Down";
            break;
        case Qt::Key_VolumeMute:
            k = "Volume-Mute";
            break;
        case Qt::Key_VolumeUp:
            k = "Volume-Up";
            break;
        case Qt::Key_BassBoost:
            k = "Bass-Boost";
            break;
        case Qt::Key_BassUp:
            k = "Bass-Up";
            break;
        case Qt::Key_BassDown:
            k = "Bass-Down";
            break;
        case Qt::Key_TrebleUp:
            k = "Treble-Up";
            break;
        case Qt::Key_TrebleDown:
            k = "Treble-Down";
            break;
        case Qt::Key_MediaPlay:
            k = "Play";
            break;
        case Qt::Key_MediaStop:
            k = "Stop";
            break;
        case Qt::Key_MediaPrevious:
            k = "Previous";
            break;
        case Qt::Key_MediaNext:
            k = "Next";
            break;
        case Qt::Key_MediaRecord:
            k = "Record";
            break;
        case Qt::Key_MediaPause:
            k = "Pause";
            break;
        case Qt::Key_MediaTogglePlayPause:
            k = "Play/Pause";
            break;
        case Qt::Key_HomePage:
            k = "Home";
            break;
        case Qt::Key_Favorites:
            k = "Favourites";
            break;
        case Qt::Key_Search:
            k = "Search";
            break;
        case Qt::Key_Standby:
            k = "Standby";
            break;
        case Qt::Key_OpenUrl:
            k = "Open URL";
            break;
        case Qt::Key_LaunchMail:
            k = "Mail";
            break;
        case Qt::Key_LaunchMedia:
            k = "Media";
            break;
        case Qt::Key_Launch0:
            k = "Launch 0";
            break;
        case Qt::Key_Launch1:
            k = "Launch 1";
            break;
        case Qt::Key_Launch2:
            k = "Launch 2";
            break;
        case Qt::Key_Launch3:
            k = "Launch 3";
            break;
        case Qt::Key_Launch4:
            k = "Launch 4";
            break;
        case Qt::Key_Launch5:
            k = "Launch 5";
            break;
        case Qt::Key_Launch6:
            k = "Launch 6";
            break;
        case Qt::Key_Launch7:
            k = "Launch 7";
            break;
        case Qt::Key_Launch8:
            k = "Launch 8";
            break;
        case Qt::Key_Launch9:
            k = "Launch 9";
            break;
        case Qt::Key_LaunchA:
            k = "Launch A";
            break;
        case Qt::Key_LaunchB:
            k = "Launch B";
            break;
        case Qt::Key_LaunchC:
            k = "Launch C";
            break;
        case Qt::Key_LaunchD:
            k = "Launch D";
            break;
        case Qt::Key_LaunchE:
            k = "Launch E";
            break;
        case Qt::Key_LaunchF:
            k = "Launch F";
            break;
        case Qt::Key_LaunchG:
            k = "Launch G";
            break;
        case Qt::Key_LaunchH:
            k = "Launch H";
            break;
        case Qt::Key_MonBrightnessUp:
            k = "Brightness Up";
            break;
        case Qt::Key_MonBrightnessDown:
            k = "Brightness Down";
            break;
        case Qt::Key_KeyboardLightOnOff:
            k = "Keyboard Light On/Off";
            break;
        case Qt::Key_KeyboardBrightnessUp:
            k = "Keyboard Light Up";
            break;
        case Qt::Key_KeyboardBrightnessDown:
            k = "Keyboard Light Down";
            break;
        case Qt::Key_PowerOff:
            k = "Power Off";
            break;
        case Qt::Key_WakeUp:
            k = "Wake Up";
            break;
        case Qt::Key_Eject:
            k = "Eject";
            break;
        case Qt::Key_ScreenSaver:
            k = "Screen Saver";
            break;
        case Qt::Key_WWW:
            k = "WWW";
            break;
        case Qt::Key_Memo:
            k = "Memo";
            break;
        case Qt::Key_LightBulb:
            k = "Light Bulb";
            break;
        case Qt::Key_Shop:
            k = "Shop";
            break;
        case Qt::Key_History:
            k = "History";
            break;
        case Qt::Key_AddFavorite:
            k = "Add Favorite";
            break;
        case Qt::Key_HotLinks:
            k = "Hot Links";
            break;
        case Qt::Key_BrightnessAdjust:
            k = "Brightness Adjust";
            break;
        case Qt::Key_Finance:
            k = "Finance";
            break;
        case Qt::Key_Community:
            k = "Community";
            break;
        case Qt::Key_AudioRewind:
            k = "Rewind";
            break;
        case Qt::Key_BackForward:
            k = "Back/Forward";
            break;
        case Qt::Key_ApplicationLeft:
            k = "App Left";
            break;
        case Qt::Key_ApplicationRight:
            k = "App Right";
            break;
        case Qt::Key_Book:
            k = "Book";
            break;
        case Qt::Key_CD:
            k = "CD";
            break;
        case Qt::Key_Calculator:
            k = "Calculator";
            break;
        case Qt::Key_ToDoList:
            k = "TODO";
            break;
        case Qt::Key_ClearGrab:
            k = "Clear Grab";
            break;
        case Qt::Key_Close:
            k = "Close";
            break;
        case Qt::Key_Copy:
            k = "Copy";
            break;
        case Qt::Key_Cut:
            k = "Cut";
            break;
        case Qt::Key_Display:
            k = "Display";
            break;
        case Qt::Key_DOS:
            k = "DOS";
            break;
        case Qt::Key_Documents:
            k = "Documents";
            break;
        case Qt::Key_Excel:
            k = "Excel";
            break;
        case Qt::Key_Explorer:
            k = "Explorer";
            break;
        case Qt::Key_Game:
            k = "Game";
            break;
        case Qt::Key_Go:
            k = "Go";
            break;
        case Qt::Key_iTouch:
            k = "iTouch";
            break;
        case Qt::Key_LogOff:
            k = "Log Off";
            break;
        case Qt::Key_Market:
            k = "Market";
            break;
        case Qt::Key_Meeting:
            k = "Meeting";
            break;
        case Qt::Key_MenuKB:
            k = "Menu KB";
            break;
        case Qt::Key_MenuPB:
            k = "Menu PB";
            break;
        case Qt::Key_MySites:
            k = "My Sites";
            break;
        case Qt::Key_News:
            k = "News";
            break;
        case Qt::Key_OfficeHome:
            k = "Office";
            break;
        case Qt::Key_Option:
            k = "Option";
            break;
        case Qt::Key_Paste:
            k = "Paste";
            break;
        case Qt::Key_Phone:
            k = "Phone";
            break;
        case Qt::Key_Calendar:
            k = "Calendar";
            break;
        case Qt::Key_Reply:
            k = "Reply";
            break;
        case Qt::Key_Reload:
            k = "Reload";
            break;
        case Qt::Key_RotateWindows:
            k = "Rotate";
            break;
        case Qt::Key_RotationPB:
            k = "Rotation PB";
            break;
        case Qt::Key_RotationKB:
            k = "Rotation KB";
            break;
        case Qt::Key_Save:
            k = "Save";
            break;
        case Qt::Key_Send:
            k = "Send";
            break;
        case Qt::Key_Spell:
            k = "Spell";
            break;
        case Qt::Key_SplitScreen:
            k = "Split Screen";
            break;
        case Qt::Key_Support:
            k = "Support";
            break;
        case Qt::Key_TaskPane:
            k = "Task Pane";
            break;
        case Qt::Key_Terminal:
            k = "Terminal";
            break;
        case Qt::Key_Tools:
            k = "Tools";
            break;
        case Qt::Key_Travel:
            k = "Travel";
            break;
        case Qt::Key_Video:
            k = "Video";
            break;
        case Qt::Key_Word:
            k = "Word";
            break;
        case Qt::Key_Xfer:
            k = "Transfer";
            break;
        case Qt::Key_ZoomIn:
            k = "Zoom In";
            break;
        case Qt::Key_ZoomOut:
            k = "Zoom Out";
            break;
        case Qt::Key_Away:
            k = "Away";
            break;
        case Qt::Key_Messenger:
            k = "Messenger";
            break;
        case Qt::Key_WebCam:
            k = "Web Cam";
            break;
        case Qt::Key_MailForward:
            k = "Mail Forward";
            break;
        case Qt::Key_Pictures:
            k = "Pictures";
            break;
        case Qt::Key_Music:
            k = "Music";
            break;
        case Qt::Key_Battery:
            k = "Battery";
            break;
        case Qt::Key_Bluetooth:
            k = "Bluetooth";
            break;
        case Qt::Key_WLAN:
            k = "WLAN";
            break;
        case Qt::Key_UWB:
            k = "UWB";
            break;
        case Qt::Key_AudioForward:
            k = "Forward";
            break;
        case Qt::Key_AudioRepeat:
            k = "Repeat";
            break;
        case Qt::Key_AudioRandomPlay:
            k = "Play Random";
            break;
        case Qt::Key_Subtitle:
            k = "Subtitle";
            break;
        case Qt::Key_AudioCycleTrack:
            k = "Cycle Tracks";
            break;
        case Qt::Key_Time:
            k = "Time";
            break;
        case Qt::Key_Hibernate:
            k = "Hibernate";
            break;
        case Qt::Key_View:
            k = "View";
            break;
        case Qt::Key_TopMenu:
            k = "Top Menu";
            break;
        case Qt::Key_PowerDown:
            k = "Power Down";
            break;
        case Qt::Key_Suspend:
            k = "Suspend";
            break;
        case Qt::Key_ContrastAdjust:
            k = "Adjust Contrast";
            break;
#if QT_VERSION >= 0x050000
        case Qt::Key_TouchpadToggle:
            k = "Toggle Touchpad";
            break;
        case Qt::Key_TouchpadOn:
            k = "Touchpad On";
            break;
        case Qt::Key_TouchpadOff:
            k = "Touchpad Off";
            break;
        case Qt::Key_MicMute:
            k = "Mute Mic";
            break;
        case Qt::Key_Red:
            k = "Red";
            break;
        case Qt::Key_Green:
            k = "Green";
            break;
        case Qt::Key_Yellow:
            k = "Yellow";
            break;
        case Qt::Key_Blue:
            k = "Blue";
            break;
        case Qt::Key_ChannelUp:
            k = "Channel Up";
            break;
        case Qt::Key_ChannelDown:
            k = "Channel Down";
            break;
#endif
        case Qt::Key_MediaLast:
            k = "Media Last";
            break;
        case Qt::Key_unknown:
            k = "unknown";
            break;
        case Qt::Key_Call:
            k = "Call";
            break;
        case Qt::Key_Camera:
            k = "Camera";
            break;
        case Qt::Key_CameraFocus:
            k = "Camera Focus";
            break;
        case Qt::Key_Context1:
            k = "Context 1";
            break;
        case Qt::Key_Context2:
            k = "Context 2";
            break;
        case Qt::Key_Context3:
            k = "Context 3";
            break;
        case Qt::Key_Context4:
            k = "Context 4";
            break;
        case Qt::Key_Flip:
            k = "Flip";
            break;
        case Qt::Key_Hangup:
            k = "Hangup";
            break;
        case Qt::Key_No:
            k = "No";
            break;
        case Qt::Key_Select:
            k = "Select";
            break;
        case Qt::Key_Yes:
            k = "Yes";
            break;
        case Qt::Key_ToggleCallHangup:
            k = "Hangup";
            break;
        case Qt::Key_VoiceDial:
            k = "Dial";
            break;
        case Qt::Key_LastNumberRedial:
            k = "Redial";
            break;
        case Qt::Key_Execute:
            k = "Execute";
            break;
        case Qt::Key_Printer:
            k = "Printer";
            break;
        case Qt::Key_Play:
            k = "Play";
            break;
        case Qt::Key_Sleep:
            k = "Sleep";
            break;
        case Qt::Key_Zoom:
            k = "Zoom";
            break;
        case Qt::Key_Cancel:
            k = "Cancel";
            break;
        default:
            k = "";
            break;
    }
    return k;
}

//-------------------------------------------------------------------------------------------
// QKeyAssignEdit
//-------------------------------------------------------------------------------------------

QKeyAssignEdit::QKeyAssignEdit(WinLIRCRemoteProxyIF *remoteProxy,QWidget *parent,Qt::WindowFlags f) : QWidget(parent,f),
    m_model(0),
    m_index(),
    m_keys(),
    m_editPosition(0),
    m_buttonBack(0),
    m_buttonForward(0),
    m_buttonOptions(0),
    m_edit(0),
    m_previousEventCode(0),
    m_remoteProxy(remoteProxy)
{
    QHBoxLayout *hBox = new QHBoxLayout(this);
    hBox->setSpacing(0);
    hBox->setContentsMargins(0,0,0,0);
    
    m_buttonBack = new QToolButton(this);
    m_buttonBack->setIcon(QIcon(":/arrow/Resources/back_arrow.png"));
    m_buttonBack->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    hBox->addWidget(m_buttonBack);
    
    m_buttonForward = new QToolButton(this);
    m_buttonForward->setIcon(QIcon(":/arrow/Resources/forward_arrow.png"));
    m_buttonForward->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    hBox->addWidget(m_buttonForward);
    
    m_edit = new QKeyLineEdit(m_remoteProxy,this);
    hBox->addWidget(m_edit);
    
    m_buttonOptions = new QToolButton(this);
    m_buttonOptions->setText("...");
    m_buttonOptions->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    hBox->addWidget(m_buttonOptions);
    
    installEventFilter(this);
    
    QObject::connect(m_buttonBack,SIGNAL(clicked()),this,SLOT(onBackButton()));
    QObject::connect(m_buttonForward,SIGNAL(clicked()),this,SLOT(onForwardButton()));
    
    m_menu = new QMenu(this);
    m_removeAction = m_menu->addAction("Remove",this,SLOT(onRemoveAssignment()));
    m_clearAction = m_menu->addAction("Clear All",this,SLOT(onClear()));
    
    m_buttonOptions->setPopupMode(QToolButton::InstantPopup);
    m_buttonOptions->setMenu(m_menu);
    
    WinLIRCRemoteSPtr remote = remoteProxy->getWinLIRCRemote();
    if(remote.data()!=0)
    {
        QObject::connect(remote.data(),SIGNAL(onProgrammed(QString,int)),this,SLOT(onProgrammed(QString,int)));
    }
}

//-------------------------------------------------------------------------------------------

QKeyAssignEdit::~QKeyAssignEdit()
{}

//-------------------------------------------------------------------------------------------

const KeyCodesContainer& QKeyAssignEdit::keys() const
{
    return m_keys;
}

//-------------------------------------------------------------------------------------------

KeyCodesContainer& QKeyAssignEdit::keys()
{
    return m_keys;
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    
    if(e->gotFocus())
    {
        initialize();
        m_edit->setFocus(Qt::MouseFocusReason);
    }
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::initialize()
{
    m_editPosition = 0;
    m_previousEventCode = 0;
    refresh();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::refresh()
{
    bool prevFlag = false, nextFlag = false, optionFlag = false, navFlag = false;

    if(m_editPosition>=0 && m_editPosition<m_keys.size())
    {
        m_edit->key() = m_keys.at(m_editPosition);
    }
    else
    {
        m_edit->key() = KeyCode();
    }
    
    if(!m_keys.isEmpty())
    {
        if(m_keys.size() > 1)
        {
            navFlag = true;
            if(m_editPosition > 0)
            {
                prevFlag = true;
            }
            if(m_editPosition < m_keys.size() - 1)
            {
                nextFlag = true;
            }
        }
        optionFlag = true;    
    }
    
    m_buttonBack->setVisible(navFlag);
    m_buttonBack->setEnabled(prevFlag);
    m_buttonForward->setVisible(navFlag);
    m_buttonForward->setEnabled(nextFlag);
    m_buttonOptions->setVisible(optionFlag);
    m_buttonOptions->setEnabled(optionFlag);
    
    m_edit->update();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::resizeEvent(QResizeEvent *e)
{
    int h = e->size().height();
    m_buttonBack->setFixedSize(24,h);
    m_buttonForward->setFixedSize(24,h);
    m_buttonOptions->setFixedSize(24,h);
    m_edit->setFixedHeight(h);
    QWidget::resizeEvent(e);
}

//-------------------------------------------------------------------------------------------

bool QKeyAssignEdit::eventFilter(QObject *obj,QEvent *evt)
{
    if(evt->type()==QEvent::KeyPress && m_edit->hasFocus())
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(evt);
        if(keyEvent!=0)
        {
            tuint32 currentCode = m_edit->codeForKeyEvent(keyEvent);
            if(currentCode!=m_previousEventCode)
            {
                KeyCode prevCode = m_edit->key();
                m_edit->onKeyReleaseEvent(keyEvent);
                if(!assign(m_edit->key()))
                {
                    m_edit->key() = prevCode;
                    refresh();
                }
                m_previousEventCode = currentCode;
            }
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::setModelIndex(const QModelIndex& index)
{
    if(index.isValid())
    {
        m_index = index;
        m_model = const_cast<QAbstractItemModel *>(index.model());
    }
    else
    {
        m_index = QModelIndex();
        m_model = 0;
    }
}

//-------------------------------------------------------------------------------------------

QModelIndex QKeyAssignEdit::assignedTo(const KeyCode& qKey)
{
    if(m_model!=0)
    {
        for(int iRow=0;iRow<m_model->rowCount();iRow++)
        {
            for(int iColumn=0;iColumn<m_model->columnCount();iColumn++)
            {
                KeyCodesContainer keys;
                QModelIndex index = m_model->index(iRow,iColumn);
                
                keys = m_model->data(index,Qt::EditRole);
                for(int k=0;k<keys.size();k++)
                {
                    if(keys.at(k)==qKey)
                    {
                        return index;
                    }
                }
            }
        }
    }
    return QModelIndex();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::unassign(const QModelIndex& index,const KeyCode& qKey)
{
    if(index.isValid() && m_model!=0)
    {
        KeyCodesContainer keys;
        QList<KeyCode>::iterator ppI;
        
        keys = m_model->data(index,Qt::EditRole);
        ppI=keys.list().begin();
        while(ppI!=keys.list().end())
        {
            KeyCode& k = *ppI;
            if(k==qKey)
            {
                ppI = keys.list().erase(ppI);
            }
            else
            {
                ppI++;
            }
        }
        m_model->setData(index,keys.variant(),Qt::EditRole);
    }
}

//-------------------------------------------------------------------------------------------
// The key shortcut "Ctrl+A" is reserved for the Select All action.
// The key shortcut "Ctrl+V" is reserved for the Paste action.
//-------------------------------------------------------------------------------------------

bool QKeyAssignEdit::canAssign(const KeyCode& nKey)
{
    bool res = true;
    KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();
    if(!pKeyExclusion->isAvailable(nKey))
    {
        QString msg;
        QString qTitle("Cannot Assign Key");
        
        msg  = "The key shortcut \"" + QKeyLineEdit::textForCode(nKey.keyCode());
        msg += " is reserved for the " + pKeyExclusion->description(nKey) + " action.";

        QMessageBox::information(dynamic_cast<QWidget *>(this->parent()),qTitle,msg);
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool QKeyAssignEdit::assign(const KeyCode& nKey)
{
    QModelIndex cIndex;
    bool assignFlag = true;

    cIndex = assignedTo(nKey);
    
    if(!canAssign(nKey))
    {
        assignFlag = false;
    }
    else if(cIndex.isValid())
    {
        if(!(cIndex.row()==m_index.row() && cIndex.column()==m_index.column()))
        {
            QString qTitle("Reassign ");
            qTitle += (nKey.isRemote()) ? "remote button" : "key press";
        
            QString qQuestion;
            QString cmdName = m_model->data(cIndex,Qt::UserRole).toString();
            if(nKey.isKeyboard())
            {
                qQuestion  = "The key assignment \"" + QKeyLineEdit::textForCode(nKey.keyCode()) + "\" is currently assigned to ";
                qQuestion += cmdName + ". Do you wish to reassign this keyboard shortcut to " + cmdName + "?";
            }
            else
            {
                qQuestion  = "The remote button \"" + nKey.remote() + "\" is currently assigned to ";
                qQuestion += cmdName + ". Do you wish to reassign this remote button to " + cmdName + "?";
            }
        
            if(QMessageBox::question(dynamic_cast<QWidget *>(this->parent()),qTitle,qQuestion,QMessageBox::Yes | QMessageBox::No,QMessageBox::No)==QMessageBox::Yes)
            {
                unassign(cIndex,nKey);
            }
            else
            {
                assignFlag = false;
            }
        }
        else
        {
            int eIndex = m_keys.list().indexOf(nKey);
            if(eIndex>=0)
            {
                m_editPosition = eIndex;
                assignFlag = false;
            }
        }
    }

    if(assignFlag)
    {
        m_keys.list().append(nKey);
        m_editPosition = m_keys.size() - 1;
        if(m_model!=0 && m_index.isValid())
        {
            m_model->setData(m_index,m_keys.variant(),Qt::EditRole);
        }
        refresh();
    }    
    return assignFlag;
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::onBackButton()
{
    m_editPosition--;
    if(m_editPosition < 0)
    {
        m_editPosition = 0;
    }
    refresh();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::onForwardButton()
{
    m_editPosition++;
    if(m_editPosition >= m_keys.size())
    {
        m_editPosition = m_keys.size() - 1;
    }
    refresh();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::onRemoveAssignment()
{
    if(m_editPosition>=0 && m_editPosition<m_keys.size())
    {
        m_keys.list().removeAt(m_editPosition);
        if(m_editPosition>=0 && m_editPosition<m_keys.size())
        {
            m_edit->key() = m_keys.at(m_editPosition);
        }
        else
        {
            m_edit->key() = KeyCode();
            m_editPosition = 0;
        }
        if(m_model!=0 && m_index.isValid())
        {
            m_model->setData(m_index,m_keys.variant(),Qt::EditRole);
        }
    }
    refresh();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::onClear()
{
    m_keys.list().clear();
    m_edit->key() = KeyCode();
    m_editPosition = 0;
    if(m_model!=0 && m_index.isValid())
    {
        m_model->setData(m_index,m_keys.variant(),Qt::EditRole);
    }
    refresh();
}

//-------------------------------------------------------------------------------------------

void QKeyAssignEdit::onProgrammed(QString cmd,int repeat)
{
    if(m_remoteProxy->getWinLIRCRemote().data()!=0 && (hasFocus() || m_edit->hasFocus()))
    {
        WinLIRCRemoteSPtr remote = m_remoteProxy->getWinLIRCRemote();
        KeyCode remoteKey;
        remoteKey.setRemote(cmd);
        assign(remoteKey);
    }
}

//-------------------------------------------------------------------------------------------
// QKeyLineEditDelegate
//-------------------------------------------------------------------------------------------

QKeyLineEditDelegate::QKeyLineEditDelegate(WinLIRCRemoteProxyIF *remoteProxy,QObject *parent) : QStyledItemDelegate(parent),
    m_remoteProxy(remoteProxy)
{
    m_dummyEdit = new QLineEdit;
    m_iconKeyboard = new QPixmap(":/input/Resources/keyboard.png");
    m_iconRemote = new QPixmap(":/input/Resources/remote.png");
    m_editRow = -1;
    m_editColumn = -1;
}

//-------------------------------------------------------------------------------------------

QKeyLineEditDelegate::~QKeyLineEditDelegate()
{
    delete m_dummyEdit;
    delete m_iconKeyboard;
    delete m_iconRemote;
}

//-------------------------------------------------------------------------------------------

QWidget *QKeyLineEditDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem& option,const QModelIndex& index) const
{
    QKeyAssignEdit *editor = new QKeyAssignEdit(m_remoteProxy,parent);
    editor->setModelIndex(index);
    return editor;
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditDelegate::setEditorData(QWidget *editor,const QModelIndex& index) const
{
    KeyCodesContainer keys;
    keys = index.model()->data(index,Qt::EditRole);
    QKeyAssignEdit *keyEdit = static_cast<QKeyAssignEdit *>(editor);
    keyEdit->keys() = keys;
    keyEdit->setModelIndex(index);
    m_editRow = index.row();
    m_editColumn = index.column();
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex& index) const
{
    QKeyAssignEdit *keyEdit = static_cast<QKeyAssignEdit *>(editor);
    model->setData(index,keyEdit->keys().variant(),Qt::EditRole);
    m_editRow = -1;
    m_editColumn = -1;
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem& option,const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditDelegate::resetEditor()
{
    m_editRow = -1;
    m_editColumn = -1;
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditDelegate::paint(QPainter *painter,const QStyleOptionViewItem& option,const QModelIndex& index) const
{
    KeyCodesContainer keys(index.model()->data(index,Qt::EditRole));

    painter->fillRect(option.rect,option.palette.color(QPalette::Window));
    if(!keys.isEmpty() && !(index.row()==m_editRow && index.column()==m_editColumn))
    {
        QList<KeyCode>::const_iterator ppI;
        float backCHue,backCSat,backCVal;

        option.palette.color(QPalette::Base).getHsvF(&backCHue,&backCSat,&backCVal);
        backCVal += (backCVal < 0.5f) ? 0.06f : -0.06f;
        if(backCVal > 1.0)
        {
            backCVal = 0.94f;
        }
        else if(backCVal < 0.0f)
        {
            backCVal = 0.06f;
        }
        QColor backC = QColor::fromHsvF(backCHue,backCSat,backCVal);

        int tXPos = option.rect.left() + 2;

        for(ppI=keys.list().begin();ppI!=keys.list().end();ppI++)
        {
            QPixmap *icon;
            QString kText;
            const KeyCode& k = *ppI;

            if(k.isKeyboard())
            {
                kText = QKeyLineEdit::textForCode(k.keyCode());
                icon = m_iconKeyboard;
            }
            else
            {
                kText = k.remote();
                icon = m_iconRemote;
            }

            tfloat64 cYPos = static_cast<tfloat64>(option.rect.top()) + (static_cast<tfloat64>(option.rect.height()) / 2.0);
            tfloat64 tYPos = cYPos - (static_cast<tfloat64>(option.fontMetrics.height()) / 2.0);
            tfloat64 iYPos = cYPos - (static_cast<tfloat64>(icon->height()) / 2.0);

            int tIYPos = static_cast<int>(::floor(tYPos));
            int iIYPos = static_cast<int>(::floor(iYPos));

            QRect rIcon(tXPos+3,iIYPos,icon->width(),icon->height());
            QRect rText(rIcon.right()+4,tIYPos,option.fontMetrics.horizontalAdvance(kText),option.fontMetrics.height());

            QPoint ptBack(tXPos,((iIYPos < tIYPos) ? iIYPos : tIYPos));
            QSize sBack((rText.right() + 4) - ptBack.x(),((rIcon.height() > rText.height()) ? rIcon.height() : rText.height()));
            QRect rBack(ptBack,sBack);

            QTextOption textOptions(Qt::AlignLeft);
            textOptions.setWrapMode(QTextOption::NoWrap);

            QBrush backBrush(backC);

            painter->setRenderHint(QPainter::Antialiasing);
            painter->setRenderHint(QPainter::TextAntialiasing);
            painter->setRenderHint(QPainter::Antialiasing);

            painter->setPen(QPen(option.palette.color(QPalette::Base)));
            painter->setBrush(backBrush);
            painter->drawRoundedRect(rBack,3.0,3.0);
            
            painter->setPen(QPen(option.palette.color(QPalette::Text)));
            painter->drawPixmap(rIcon.left(),rIcon.top(),*icon);
            painter->drawText(rText,kText,textOptions);

            tXPos += sBack.width() + 8;
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
