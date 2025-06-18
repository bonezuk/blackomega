//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_UPNPPROVIDERTESTENVIROMENT
#define __OMEGA_COMMON_TEST_UPNPPROVIDERTESTENVIROMENT
//-------------------------------------------------------------------------------------------

#include <QtCore/QPair>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>

#include "common/inc/DiskOps.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
namespace test
{
//-------------------------------------------------------------------------------------------
// Setup and provide a number web directories that provide test cases
// for testing the functionality of UPnP
//-------------------------------------------------------------------------------------------

class UPnPProviderTestEnviroment
{
    public:
        virtual ~UPnPProviderTestEnviroment();
        
        static UPnPProviderTestEnviroment *instance();
        
        QString root(int testCase);
        
    protected:
    
        static UPnPProviderTestEnviroment *m_instance;
    
        UPnPProviderTestEnviroment();
        QString getTempDirectoryPath();
        QString getTempDirectoryPath(int testCase);
        bool setupDirectory(int testCase,const QList<QPair<QString,QString> >& fileList);
        void removeDirectory(int testCase);
};

//-------------------------------------------------------------------------------------------
} // namespace test
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

