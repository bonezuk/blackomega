//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_PRODUCTVERSIONINFO_H
#define __OMEGA_COMMON_PRODUCTVERSIONINFO_H
//-------------------------------------------------------------------------------------------

#include <QSharedPointer>
#include "common/inc/DictionaryXMLMap.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT ProductVersionInfo
{
    public:
        ProductVersionInfo();
        virtual ~ProductVersionInfo();
        
        static QSharedPointer<ProductVersionInfo> instance(const QString& fileName);
        static QSharedPointer<ProductVersionInfo> instance();
        
        virtual bool read(const QString& fileName);
        virtual bool write(const QString& fileName);
        
        virtual const int& versionMajor() const;
        virtual const int& versionMinor() const;
        virtual const int& versionRevision() const;
        virtual const int& build() const;
        virtual int& build();
        
        virtual QString version() const;
        virtual const QString& copyright() const;
        
    protected:
    
        static QSharedPointer<ProductVersionInfo> m_instance;
    
        int m_versionMajor;
        int m_versionMinor;
        int m_versionRevision;
        int m_build;
        QString m_copyright;
};

typedef QSharedPointer<ProductVersionInfo> ProductVersionInfoSPtr;

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
