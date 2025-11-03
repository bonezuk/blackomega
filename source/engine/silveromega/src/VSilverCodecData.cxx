#include "engine/silveromega/inc/VSilverCodecData.h"
#include "engine/silveromega/inc/VSilverFunctions.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverCodecData::VSilverCodecData(VSilverCodecInformation *info) : VSilverInfoObject(),
    m_alloc(),
    m_information(info),
    m_numCodebooks(0),
    m_codebooks(0),
    m_numFloors(0),
    m_floors(0),
    m_numResidues(0),
    m_residues(0),
    m_numMappings(0),
    m_mappings(0),
    m_numModes(0),
    m_modes(0),
    m_iLog_vorbis_mode_count(0)
{}

//-------------------------------------------------------------------------------------------

VSilverCodecData::~VSilverCodecData()
{
    try
    {
        tint i;

        if(m_codebooks!=0)
        {
            for(i=0;i<m_numCodebooks;++i)
            {
                if(m_codebooks[i]!=0)
                {
                    delete m_codebooks[i];
                }
            }
            m_alloc.Free(m_codebooks);
            m_codebooks = 0;
        }

        if(m_floors!=0)
        {
            for(i=0;i<m_numFloors;++i)
            {
                if(m_floors[i]!=0)
                {
                    delete m_floors[i];
                }
            }
            m_alloc.Free(m_floors);
            m_floors = 0;
        }

        if(m_residues!=0)
        {
            for(i=0;i<m_numResidues;++i)
            {
                if(m_residues[i]!=0)
                {
                    delete m_residues[i];
                }
            }
            m_alloc.Free(m_residues);
            m_residues = 0;
        }

        if(m_mappings!=0)
        {
            for(i=0;i<m_numMappings;++i)
            {
                if(m_mappings[i]!=0)
                {
                    delete m_mappings[i];
                }
            }
            m_alloc.Free(m_mappings);
            m_mappings = 0;
        }

        if(m_modes!=0)
        {
            for(i=0;i<m_numModes;++i)
            {
                if(m_modes[i]!=0)
                {
                    delete m_modes[i];
                }
            }
            m_alloc.Free(m_modes);
            m_modes = 0;
        }

        m_information = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverCodecData::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverCodecData::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecData::isValid() const
{
    tint i;
    common::BString err;
    bool res = true;

    if(m_codebooks!=0)
    {
        for(i=0;i<m_numCodebooks;++i)
        {
            if(m_codebooks[i]==0)
            {
                err = "No codebook at entry " + common::BString::Int(i);
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
            else if(!m_codebooks[i]->isValid())
            {
                err = "Codebook " + common::BString::Int(i) + " is invalid";
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
        }
    }
    else
    {
        printError("read","No codebooks defined");
        res = false;
    }

    if(m_floors!=0)
    {
        for(i=0;i<m_numFloors;++i)
        {
            if(m_floors[i]==0)
            {
                err = "No floor at entry " + common::BString::Int(i);
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
            else if(!m_floors[i]->isValid())
            {
                err = "Floor " + common::BString::Int(i) + " is invalid";
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
        }
    }
    else
    {
        printError("read","No floors defined");
        res = false;
    }

    if(m_residues!=0)
    {
        for(i=0;i<m_numResidues;++i)
        {
            if(m_residues[i]==0)
            {
                err = "No residue at entry " + common::BString::Int(i);
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
            else if(!m_residues[i]->isValid())
            {
                err = "Residue " + common::BString::Int(i) + " is invalid";
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
        }
    }
    else
    {
        printError("isValid","No residues defined");
        res = false;
    }

    if(m_mappings!=0)
    {
        for(i=0;i<m_numMappings;++i)
        {
            if(m_mappings[i]==0)
            {
                err = "No mapping at entry " + common::BString::Int(i);
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
            else if(!m_mappings[i]->isValid())
            {
                err = "Mapping " + common::BString::Int(i) + " is invalid";
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
        }
    }
    else
    {
        printError("isValid","No mappings defined");
        res = false;
    }

    if(m_modes!=0)
    {
        for(i=0;i<m_numModes;++i)
        {
            if(m_modes[i]==0)
            {
                err = "No modes at entry " + common::BString::Int(i);
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
            else if(!m_modes[i]->isValid())
            {
                err = "Mode " + common::BString::Int(i) + " is invalid";
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
        }
    }
    else
    {
        printError("isValid","No modes defined");
        res = false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecData::read(engine::Sequence *seq)
{
    tint i,type,count;
    common::BString err;

    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }

    skipPacketIdentifer(seq);

    // Read in the code book data
    m_numCodebooks = seq->readBits(8) + 1;
    m_codebooks = reinterpret_cast<VSilverCodebook **>(m_alloc.MemAlloc(static_cast<tuint>(m_numCodebooks),sizeof(VSilverCodebook *)));
    ::memset(m_codebooks,0,sizeof(VSilverCodebook *) * static_cast<tuint>(m_numCodebooks));
    for(i=0;i<m_numCodebooks;++i)
    {
        m_codebooks[i] = new VSilverCodebook;
        if(!m_codebooks[i]->read(seq))
        {
            err = "Error reading in codebook " + common::BString::Int(i);
            printError("read",static_cast<const tchar *>(err));
            return false;
        }
    }

    // Skip over any unused time hook data.
    count = seq->readBits(6) + 1;
    for(i=0;i<count;++i)
    {
        if(seq->readBits(16)!=0)
        {
            printError("read","Illegal non-zero time domain transformer found");
            return false;
        }
    }

    // Read in the floor data.
    m_numFloors = seq->readBits(6) + 1;
    m_floors = reinterpret_cast<VSilverFloorBase **>(m_alloc.MemAlloc(static_cast<tuint>(m_numFloors),sizeof(VSilverFloorBase *)));
    ::memset(m_floors,0,sizeof(VSilverFloorBase *) * static_cast<tuint>(m_numFloors));
    for(i=0;i<m_numFloors;++i)
    {
        type = seq->readBits(16);
        if(type==0)
        {
            m_floors[i] = reinterpret_cast<VSilverFloorBase *>(new VSilverFloorData0(m_information,this));
        }
        else if(type==1)
        {
            m_floors[i] = reinterpret_cast<VSilverFloorBase *>(new VSilverFloorData1(m_information,this));
        }
        else
        {
            printError("read","Unknown floor type");
            return false;
        }
        if(!m_floors[i]->read(seq))
        {
            err = "Error reading in floor data " + common::BString::Int(i);
            printError("read",static_cast<const tchar *>(err));
            return false;
        }
    }

    // Read in the residue data
    m_numResidues = seq->readBits(6) + 1;
    m_residues = reinterpret_cast<VSilverResidueBase **>(m_alloc.MemAlloc(static_cast<tuint>(m_numResidues),sizeof(VSilverResidueBase *)));
    ::memset(m_residues,0,sizeof(VSilverResidueBase *) * static_cast<tuint>(m_numResidues));
    for(i=0;i<m_numResidues;++i)
    {
        m_residues[i] = new VSilverResidueBase(this);
        if(!m_residues[i]->read(seq))
        {
            err = "Error reading in residue data " + common::BString::Int(i);
            printError("read",static_cast<const tchar *>(err));
            return false;
        }
    }

    // Read in the mapping data
    m_numMappings = seq->readBits(6) + 1;
    m_mappings = reinterpret_cast<VSilverMapData **>(m_alloc.MemAlloc(static_cast<tuint>(m_numMappings),sizeof(VSilverMapData *)));
    ::memset(m_mappings,0,sizeof(VSilverMapData *) * static_cast<tuint>(m_numMappings));
    for(i=0;i<m_numMappings;++i)
    {
        m_mappings[i] = new VSilverMapData(this,m_information);
        if(!m_mappings[i]->read(seq))
        {
            err = "Error reading in mapping data " + common::BString::Int(i);
            printError("read",static_cast<const tchar *>(err));
            return false;
        }
    }

    // Read in the mode data
    m_numModes = seq->readBits(6) + 1;
    m_modes = reinterpret_cast<VSilverModeData **>(m_alloc.MemAlloc(static_cast<tuint>(m_numModes),sizeof(VSilverModeData *)));
    ::memset(m_modes,0,sizeof(VSilverModeData *)  * static_cast<tuint>(m_numModes));
    for(i=0;i<m_numModes;++i)
    {
        m_modes[i] = new VSilverModeData(this);
        if(!m_modes[i]->read(seq))
        {
            err = "Error reading in mode data " + common::BString::Int(i);
            printError("read",static_cast<const tchar *>(err));
            return false;
        }
    }

    if(!seq->readBit())
    {
        printError("read","Invalid end of packet mark");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecData::setup()
{
    tint i;

    m_iLog_vorbis_mode_count = iLog(m_numModes - 1);

    if(m_codebooks==0)
    {
        printError("setup","No codebooks defined");
        return false;
    }
    for(i=0;i<m_numCodebooks;++i)
    {
        if(!m_codebooks[i]->setup())
        {
            printError("setup","Error initializing given code book structure");
            return false;
        }
    }

    if(m_floors==0)
    {
        printError("setup","No floor setup data defined");
        return false;
    }
    for(i=0;i<m_numFloors;++i)
    {
        if(!m_floors[i]->setup())
        {
            printError("setup","Error initializing floor base data");
            return false;
        }
    }

    if(m_residues==0)
    {
        printError("setup","No residue setup data defined");
        return false;
    }
    for(i=0;i<m_numResidues;++i)
    {
        if(!m_residues[i]->setup())
        {
            printError("setup","Error initializing residue data");
            return false;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
