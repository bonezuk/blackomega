//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYASSIGNMENTMOCK_H
#define __OMEGA_REMOTE_KEYASSIGNMENTMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/KeyAssignment.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class KeyAssignmentMock : public KeyAssignment
{
    public:
        KeyAssignmentMock();
        virtual ~KeyAssignmentMock();
        
        MOCK_METHOD1(loadFromXML,bool(const QString& fileName));
        MOCK_CONST_METHOD1(saveToXML,bool(const QString& fileName));
        MOCK_METHOD1(loadFromModel,bool(QAbstractItemModel *model));
        MOCK_CONST_METHOD1(saveToModel,QStandardItemModel *(QObject *parent));
        MOCK_CONST_METHOD1(assignedTo,KeyAssignment::Key(const KeyCode& k));
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
