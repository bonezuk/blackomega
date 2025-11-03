//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_QKEYLINEEDIT_H
#define __OMEGA_REMOTE_QKEYLINEEDIT_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include <QWidget>
#include <QPaintEvent>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QHBoxLayout>
#include <QToolButton>
#include <QResizeEvent>
#include <QMenu>
#include <QAction>

#include "remote/inc/RemoteDLL.h"
#include "remote/inc/KeyCodesContainer.h"
#include "remote/inc/WinLIRCRemote.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCRemoteProxyIF
{
    public:
        virtual WinLIRCRemoteSPtr getWinLIRCRemote() = 0;
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT QKeyLineEdit : public QWidget
{
    public:
        Q_OBJECT

    public:
        QKeyLineEdit(WinLIRCRemoteProxyIF *remoteProxy,QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~QKeyLineEdit();

        virtual QString text();

        virtual KeyCode& key();
        virtual const KeyCode& key() const;

        static QString textForCode(tuint32 kCode);
        static QString textForKeyCode(Qt::Key key);

        virtual void onKeyReleaseEvent(QKeyEvent *e);

        static tuint32 codeForKeyEvent(QKeyEvent *e);

    protected:
        int m_blinkState;
        QTimer *m_timer;
        QLineEdit *m_dummyEdit;

        KeyCode m_key;

        QPixmap *m_iconKeyboard;
        QPixmap *m_iconRemote;

        WinLIRCRemoteProxyIF *m_remoteProxy;

        virtual void paintEvent(QPaintEvent *e);

        virtual void focusInEvent(QFocusEvent *e);
        virtual void focusOutEvent(QFocusEvent *e);

        static bool isModifier(tuint32 code);

    protected Q_SLOTS:
        void blinkTimer();
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT QKeyAssignEdit : public QWidget
{
    public:
        Q_OBJECT

    public:
        QKeyAssignEdit(WinLIRCRemoteProxyIF *remoteProxy,QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~QKeyAssignEdit();

        const KeyCodesContainer& keys() const;
        KeyCodesContainer& keys();

        void setModelIndex(const QModelIndex& index);

    protected:

        QAbstractItemModel *m_model;
        QModelIndex m_index;

        KeyCodesContainer m_keys;
        int m_editPosition;

        QToolButton *m_buttonBack;
        QToolButton *m_buttonForward;
        QToolButton *m_buttonOptions;
        QKeyLineEdit *m_edit;

        tuint32 m_previousEventCode;

        QMenu *m_menu;
        QAction *m_removeAction;
        QAction *m_clearAction;

        WinLIRCRemoteProxyIF *m_remoteProxy;

        virtual void focusInEvent(QFocusEvent *e);
        virtual void focusOutEvent(QFocusEvent *e);
        virtual void resizeEvent(QResizeEvent *e);

        virtual void initialize();

        virtual bool eventFilter(QObject *obj,QEvent *evt);

        virtual QModelIndex assignedTo(const KeyCode& qKey);
        virtual void unassign(const QModelIndex& index,const KeyCode& qKey);
        virtual bool canAssign(const KeyCode& nKey);
        virtual bool assign(const KeyCode& nKey);

        virtual void refresh();

    protected Q_SLOTS:

        void onBackButton();
        void onForwardButton();
        void onRemoveAssignment();
        void onClear();
        void onProgrammed(QString cmd,int repeat);
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT QKeyLineEditDelegate : public QStyledItemDelegate
{
    public:
        Q_OBJECT

    public:
        QKeyLineEditDelegate(WinLIRCRemoteProxyIF *remoteProxy,QObject *parent = 0);
        virtual ~QKeyLineEditDelegate();

        virtual QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem& option,const QModelIndex& index) const;

        virtual void setEditorData(QWidget *editor,const QModelIndex& index) const;
        virtual void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex& index) const;
        virtual void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem& option,const QModelIndex& index) const;

        virtual void paint(QPainter *painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;

        virtual void resetEditor();

    protected:
        QLineEdit *m_dummyEdit;
        QKeyLineEdit *m_keyEdit;
        QPixmap *m_iconKeyboard;
        QPixmap *m_iconRemote;
        mutable int m_editRow;
        mutable int m_editColumn;
        WinLIRCRemoteProxyIF *m_remoteProxy;
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
