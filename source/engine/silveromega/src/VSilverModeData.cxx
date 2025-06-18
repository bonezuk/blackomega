#include "engine/silveromega/inc/VSilverModeData.h"
#include "engine/silveromega/inc/VSilverCodecData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverModeData::VSilverModeData(VSilverCodecData *cData) : VSilverInfoObject(),
    m_blockFlag(0),
    m_windowType(0),
    m_transformType(0),
    m_mapping(0),
    m_cData(cData)
{}

//-------------------------------------------------------------------------------------------

VSilverModeData::~VSilverModeData()
{}

//-------------------------------------------------------------------------------------------

void VSilverModeData::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverModesData::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverModeData::isValid() const
{
    bool res = true;
    
    if(m_cData==0)
    {
        printError("isValid","No codec setup data given");
        return false;
    }
    if(m_windowType)
    {
        printError("isValid","Invalid block flag value");
        res = false;
    }
    if(m_transformType)
    {
        printError("isValid","Invalid transform type value");
        res = false;
    }
    if(m_mapping>=m_cData->m_numMappings)
    {
        printError("isValid","Number of mode maps out of range");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverModeData::read(engine::Sequence *seq)
{
    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }
    
    m_blockFlag = seq->readBit();
    m_windowType = seq->readBits(16);
    m_transformType = seq->readBits(16);
    m_mapping = seq->readBits(8);
    
    if(seq->isEnd())
    {
        printError("read","End of stream has been detected");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
