//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_DISKMOCKIF_H
#define __OMEGA_COMMON_DISKMOCKIF_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/DiskLayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class DiskMockIF : public DiskIF
{
    public:
        DiskMockIF();
        virtual ~DiskMockIF();

        MOCK_CONST_METHOD1(isFile,bool(const QString& name));
        MOCK_CONST_METHOD1(isDirectory,bool(const QString& name));

        MOCK_CONST_METHOD1(openDirectory,DiskIF::DirHandle(const QString& name));
        MOCK_CONST_METHOD1(nextDirectoryEntry,QString(DiskIF::DirHandle h));
        MOCK_CONST_METHOD1(closeDirectory,void(DiskIF::DirHandle h));
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
