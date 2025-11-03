#include "engine/silveromega/inc/VSilverMapData.h"
#include "engine/silveromega/inc/VSilverFunctions.h"
#include "engine/silveromega/inc/VSilverCodecData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverMapData::VSilverMapData(VSilverCodecData *cData,VSilverCodecInformation *info) : VSilverInfoObject(),
    m_submap(0),
    m_couplingSteps(0),
    m_couplingMagnitude(0),
    m_couplingAngle(0),
    m_multiplexes(0),
    m_floorSubmap(0),
    m_residueSubmap(0),
    m_information(info),
    m_cData(cData),
    m_read(false)
{}

//-------------------------------------------------------------------------------------------

VSilverMapData::~VSilverMapData()
{
    try
    {
        if(m_couplingMagnitude!=0)
        {
            delete [] m_couplingMagnitude;
            m_couplingMagnitude = 0;
        }
        if(m_couplingAngle!=0)
        {
            delete [] m_couplingAngle;
            m_couplingAngle = 0;
        }
        if(m_multiplexes!=0)
        {
            delete [] m_multiplexes;
            m_multiplexes = 0;
        }
        if(m_floorSubmap!=0)
        {
            delete [] m_floorSubmap;
            m_floorSubmap = 0;
        }
        if(m_residueSubmap!=0)
        {
            delete [] m_residueSubmap;
            m_residueSubmap = 0;
        }
        m_information = 0;
        m_cData = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverMapData::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverMapData::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverMapData::isValid() const
{
    if(!m_read)
    {
        printError("isValid","Mapping data not decoded correctly");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverMapData::read(engine::Sequence *seq)
{
    tint i,logChannel;
    
    if(m_information==0)
    {
        printError("read","No codec information given");
        return false;
    }
    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }
    if(seq->readBits(16))
    {
        printError("read","Unknown type of mapping");
        return false;
    }
    
    if(seq->readBit())
    {
        m_submap = seq->readBits(4) + 1;
    }
    else
    {
        m_submap = 1;
    }
    
    if(seq->readBit())
    {
        logChannel = iLog2(m_information->m_audioChannels);
        
        m_couplingSteps = seq->readBits(8) + 1;
        m_couplingMagnitude = new tint [static_cast<tuint>(m_couplingSteps)];
        ::memset(m_couplingMagnitude,0,static_cast<tuint>(m_couplingSteps) << 2);
        m_couplingAngle = new tint [static_cast<tuint>(m_couplingSteps)];
        ::memset(m_couplingAngle,0,static_cast<tuint>(m_couplingSteps) << 2);
        
        for(i=0;i<m_couplingSteps;++i)
        {
            m_couplingMagnitude[i] = seq->readBits(logChannel);
            m_couplingAngle[i] = seq->readBits(logChannel);
            
            if(m_couplingMagnitude[i]==m_couplingAngle[i] || m_couplingMagnitude[i]>=m_information->m_audioChannels || m_couplingAngle[i]>=m_information->m_audioChannels)
            {
                printError("read","Polar channel mapping incorrect");
                return false;
            }            
        }
    }
    else
    {
        m_couplingSteps = 0;
    }
    
    if(seq->readBits(2))
    {
        printError("read","Error in mapping data");
        return false;
    }
    
    m_multiplexes = new tint [static_cast<tuint>(m_information->m_audioChannels) + 1];
    ::memset(m_multiplexes,0,static_cast<tuint>(m_information->m_audioChannels + 1) << 2);
    
    if(m_submap>1)
    {
        for(i=0;i<m_information->m_audioChannels;++i)
        {
            m_multiplexes[i] = seq->readBits(4);
            if(m_multiplexes[i]>=m_submap)
            {
                printError("read","Multiplexing map out of range");
                return false;
            }
        }
    }
    
    m_floorSubmap = new tint [static_cast<tuint>(m_submap)];
    ::memset(m_floorSubmap,0,static_cast<tuint>(m_submap) << 2);
    m_residueSubmap = new tint [static_cast<tuint>(m_submap)];
    ::memset(m_residueSubmap,0,static_cast<tuint>(m_submap) << 2);
    
    for(i=0;i<m_submap;++i)
    {
        seq->readBits(8);
        m_floorSubmap[i] = seq->readBits(8);
        if(m_floorSubmap[i] >= m_cData->m_numFloors)
        {
            printError("read","Floor mapping out of range");
            return false;
        }
        m_residueSubmap[i] = seq->readBits(8);
        if(m_residueSubmap[i] >= m_cData->m_numResidues)
        {
            printError("read","Residue mapping out of range");
            return false;
        }
    }
    
    if(seq->isEnd())
    {
        printError("read","End of stream has been detected");
        return false;
    }
    
    m_read = true;
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
