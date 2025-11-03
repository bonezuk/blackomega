//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODECCOMMENTS_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODECCOMMENTS_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QMap>

#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverInfoObject.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverCodecComments : public VSilverInfoObject
{
    public:
        VSilverCodecComments();
        virtual ~VSilverCodecComments();
        
        virtual bool isValid() const;
        
        virtual bool read(engine::Sequence *seq);
        
        const QString& vendor() const;
        
        const QString& comment(const tchar *str) const;
        const QString& comment(const QString& str) const;
        
        const QString& title() const;
        const QString& version() const;
        const QString& album() const;
        const QString& trackNumber() const;
        const QString& artist() const;
        const QString& performer() const;
        const QString& copyright() const;
        const QString& license() const;
        const QString& organization() const;
        const QString& description() const;
        const QString& genre() const;
        const QString& date() const;
        const QString& location() const;
        const QString& contact() const;
        const QString& isrc() const;
        const QString& diskNumber() const;
        
    protected:
    
        QString m_vendor;
        QString m_nullStr;
        QMap<QString,QString> m_comments;
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

