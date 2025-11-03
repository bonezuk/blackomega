//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_GREENOMEGA_FLACMETACOMMENT_H
#define __OMEGA_GREENOMEGA_FLACMETACOMMENT_H
//-------------------------------------------------------------------------------------------

#include "engine/greenomega/inc/FLACMetaBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaComment : public FLACMetaBase
{
    public:
        FLACMetaComment();
        virtual ~FLACMetaComment();

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

        QString readString(engine::Sequence *seq);
};

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

