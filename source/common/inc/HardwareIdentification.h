//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_HARDWAREIDENTIFICATION_H
#define __OMEGA_COMMON_HARDWAREIDENTIFICATION_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QVector>
#include <QPair>

#include "common/inc/Allocation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT HardwareIdentification
{
    public:

        typedef enum
        {
            e_Motherboard = 1,
            e_HardDisk = 2,
            e_NetworkMACAddress = 0
        } HardwareType;

    public:
        ~HardwareIdentification();

        static HardwareIdentification& instance();

        virtual bool getIdentity(QVector<QPair<HardwareType,QString> >& idList) const;

    protected:

        static HardwareIdentification *m_instance;

        QVector<QPair<HardwareType,QString> > m_idList;

        HardwareIdentification();

#if defined(OMEGA_WIN32)
        bool getWDIInfomation(QVector<QPair<HardwareType,QString> >& idList);
        bool getMACAddresses(QVector<QPair<HardwareType,QString> >& idList);
        QString getMACString(const BYTE *addr,int len);
#elif defined(OMEGA_MACOSX)
        bool getMacUUID(QVector<QPair<HardwareType,QString> >& idList);
#endif
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

