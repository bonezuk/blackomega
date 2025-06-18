#include "engine/silveromega/inc/VSilverFloorBase.h"
#include "engine/silveromega/inc/VSilverCodecData.h"
#include "engine/silveromega/inc/VSilverWindow.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverFloorBase::VSilverFloorBase(VSilverCodecInformation *cInfo,VSilverCodecData *cData,VSilverFloorType type) : VSilverInfoObject(),
    m_type(type),
    m_cInformation(cInfo),
    m_cData(cData)
{}

//-------------------------------------------------------------------------------------------

VSilverFloorBase::~VSilverFloorBase()
{
    m_cInformation = 0;
    m_cData = 0;
}

//-------------------------------------------------------------------------------------------

VSilverFloorType VSilverFloorBase::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

void *VSilverFloorBase::decode(VSilverCodecData *info,engine::Sequence *Seq)
{
    return 0;
}

//-------------------------------------------------------------------------------------------

void VSilverFloorBase::product(VSilverWindow *win,void *memo,tfloat32 *out)
{
    tint n,W = m_cData->m_modes[win->m_mode]->m_blockFlag;

    if(!W)
    {
        n = m_cInformation->m_blockSize_0 / 2;
    }
    else
    {
        n = m_cInformation->m_blockSize_1 / 2;
    }
    ::memset(out,0,n * sizeof(tfloat32));
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
