//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_INTEGRATION_APPLEREMOTESERVICE_H
#define __OMEGA_REMOTE_INTEGRATION_APPLEREMOTESERVICE_H
//-------------------------------------------------------------------------------------------
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------

class AppleRemoteServiceApplication : public QApplication
{
    public:
        Q_OBJECT
        
    public:
        AppleRemoteServiceApplication(int testType,int& argc,char **argv);
        virtual ~AppleRemoteServiceApplication();

    protected:
    
        int m_testType;
        int m_count;

    protected slots:
        void onInit();
        void onRepeatOpen();
        void onRepeatClose();
        void onCountToClose();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
