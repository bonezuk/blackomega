#include "engine/greenomega/inc/FLACMetaComment.h"
#include "engine/greenomega/inc/FLACMetaCuesheet.h"
#include "engine/greenomega/inc/FLACMetaPicture.h"
#include "engine/greenomega/inc/FLACMetaSeekTable.h"
#include "engine/greenomega/inc/FLACMetaStreamInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACMetaBase::FLACMetaBase(Type t) : m_type(t)
{}

//-------------------------------------------------------------------------------------------

FLACMetaBase::~FLACMetaBase()
{}

//-------------------------------------------------------------------------------------------

void FLACMetaBase::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FLACMetaBase::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

FLACMetaBase *FLACMetaBase::get(int type,engine::Sequence *seq)
{
    FLACMetaBase *metaData;

    switch(type)
    {
        case 0:
            metaData = reinterpret_cast<FLACMetaBase *>(new FLACMetaStreamInfo);
            break;
        
        case 1:
            metaData = new FLACMetaBase(e_Padding);
            break;
            
        case 2:
            metaData = new FLACMetaBase(e_Application);
            break;
        
        case 3:
            metaData = reinterpret_cast<FLACMetaBase *>(new FLACMetaSeekTable);
            break;
        
        case 4:
            metaData = reinterpret_cast<FLACMetaBase *>(new FLACMetaComment);
            break;
        
        case 5:
            metaData = reinterpret_cast<FLACMetaBase *>(new FLACMetaCuesheet);
            break;
            
        case 6:
            metaData = reinterpret_cast<FLACMetaBase *>(new FLACMetaPicture);
            break;
        
        default:
            metaData = new FLACMetaBase(e_Unknown);
            break;
    }
    
    if(metaData->read(seq))
    {
        return metaData;
    }
    else
    {
        delete metaData;
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

bool FLACMetaBase::read(engine::Sequence *seq)
{
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
