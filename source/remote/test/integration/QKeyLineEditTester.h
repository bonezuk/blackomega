//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TEST_INTEGRATION_QKEYLINEEDITTESTDIALOG_H
#define __OMEGA_TEST_INTEGRATION_QKEYLINEEDITTESTDIALOG_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/QKeyLineEdit.h"
#include "remote/inc/WinLIRCRemote.h"

#include <QDialog>
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QTableView>
#include <QCloseEvent>

using namespace omega::remote;
using namespace testing;

//-------------------------------------------------------------------------------------------

class QKeyLineEditTestDialog : public QDialog, public WinLIRCRemoteProxyIF
{
    public:
        Q_OBJECT
        
    public:
        QKeyLineEditTestDialog(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~QKeyLineEditTestDialog();
        
        virtual WinLIRCRemoteSPtr getWinLIRCRemote();
        
    protected:
    
        WinLIRCRemoteSPtr m_remote;
    
        virtual void closeEvent(QCloseEvent *evt);
    
    protected slots:
    
        void onStart();
        void onStop();
        void onStatusUpdate(QString text,int statusCode);
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------


