#include "common/inc/DiskIF.h"
#include "common/test/UPnPProviderTestEnviroment.h"
#include "common/inc/SBService.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
namespace test
{
//-------------------------------------------------------------------------------------------

UPnPProviderTestEnviroment *UPnPProviderTestEnviroment::m_instance = 0;

//-------------------------------------------------------------------------------------------

UPnPProviderTestEnviroment::UPnPProviderTestEnviroment()
{}

//-------------------------------------------------------------------------------------------

UPnPProviderTestEnviroment::~UPnPProviderTestEnviroment()
{
    removeDirectory(0);
    removeDirectory(1);
    removeDirectory(2);
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

UPnPProviderTestEnviroment *UPnPProviderTestEnviroment::instance()
{
    if(m_instance==0)
    {
        bool success = true;

        m_instance = new UPnPProviderTestEnviroment;

        QList<QPair<QString,QString> > blankList;
        if(!m_instance->setupDirectory(0,blankList))
        {
            success = false;
        }

        QList<QPair<QString,QString> > dlnaWebList;
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/lifetimeSCPD.xml","lifetimeSCPD.xml"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/simple.html","simple.html"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/simple_p1.html","simple_p1.html"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/simple_p2.html","simple_p2.html"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/supertoasterDDD.xml","supertoasterDDD.xml"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/toastercontrolSCPD.xml","toastercontrolSCPD.xml"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/toasterstatusSCPD.xml","toasterstatusSCPD.xml"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/background.gif","images/background.gif"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/bottom_footer.gif","images/bottom_footer.gif"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/orange_arrow.gif","images/orange_arrow.gif"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/title_toast_numbers.gif","images/title_toast_numbers.gif"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/title_toast_time.gif","images/title_toast_time.gif"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/toaster.gif","images/toaster.gif"));
        dlnaWebList.append(QPair<QString,QString>(":/dlnaweb/Resources/dlnaweb/images/top_header.gif","images/top_header.gif"));
        if(!m_instance->setupDirectory(1,dlnaWebList))
        {
            success = false;
        }

        QList<QPair<QString,QString> > freedbList;
        freedbList.append(QPair<QString,QString>(":/freedb/Resources/freedb/travis.txt","travis.txt"));
        freedbList.append(QPair<QString,QString>(":/freedb/Resources/freedb/kanon.txt","kanon.txt"));
        if(!m_instance->setupDirectory(2,freedbList))
        {
            success = false;
        }

        if(!success)
        {
            delete m_instance;
            m_instance = 0;
        }
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

QString UPnPProviderTestEnviroment::root(int testCase)
{
    return getTempDirectoryPath(testCase);
}

//-------------------------------------------------------------------------------------------

QString UPnPProviderTestEnviroment::getTempDirectoryPath()
{
    return common::SBService::tempDirectory();
}

//-------------------------------------------------------------------------------------------

QString UPnPProviderTestEnviroment::getTempDirectoryPath(int testCase)
{
    QString path = getTempDirectoryPath() + "/" + "upnpprovidertest" + QString::number(testCase);
    path = common::DiskOps::toNativeSeparators(path);
    return path;
}

//-------------------------------------------------------------------------------------------

bool UPnPProviderTestEnviroment::setupDirectory(int testCase,const QList<QPair<QString,QString> >& fileList)
{
    QString dPath = getTempDirectoryPath(testCase);
    bool res = true;

    if(common::DiskOps::path(dPath,true))
    {
        QList<QPair<QString,QString> >::const_iterator ppI;

        for(ppI=fileList.begin();ppI!=fileList.end() && res;ppI++)
        {
            const QPair<QString,QString>& p = *ppI;
            QString destPath = dPath + "/" + p.second;
            destPath = common::DiskOps::toNativeSeparators(destPath);
            if(common::DiskOps::path(destPath))
            {
                if(QFile::exists(destPath))
                {
                    common::DiskOps::deleteFile(destPath);
                }
                QFile file(p.first);
                res = file.copy(destPath);
            }
            else
            {
                res = false;
            }
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void UPnPProviderTestEnviroment::removeDirectory(int testCase)
{
    QString dPath = getTempDirectoryPath(testCase);
    common::DiskOps::deleteDirectory(dPath);
}

//-------------------------------------------------------------------------------------------
} // namespace test
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
