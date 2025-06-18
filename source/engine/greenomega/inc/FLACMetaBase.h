//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_GREENOMEGA_FLACMETABASE_H
#define __OMEGA_GREENOMEGA_FLACMETABASE_H
//-------------------------------------------------------------------------------------------

#include <QMap>
#include <QString>
#include <QVector>
#include <QImage>

#include "common/inc/TimeStamp.h"
#include "engine/inc/Sequence.h"
#include "engine/greenomega/inc/GreenOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACMetaBase
{
    public:
    
        typedef enum
        {
            e_StreamInfo  = 0,
            e_Padding     = 1,
            e_Application = 2,
            e_SeekTable   = 3,
            e_Comment     = 4,
            e_Cuesheet    = 5,
            e_Picture     = 6,
            e_Unknown     = -1
        } Type;
    
    public:
        FLACMetaBase(Type t);
        virtual ~FLACMetaBase();
        
        static FLACMetaBase *get(int type,engine::Sequence *seq);
        
        const Type& type() const;
        
        virtual bool read(engine::Sequence *seq);
        
    protected:
    
        Type m_type;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

inline const FLACMetaBase::Type& FLACMetaBase::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

