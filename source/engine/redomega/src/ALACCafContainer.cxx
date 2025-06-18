#include "engine/redomega/inc/ALACCafContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACCafContainer::ALACCafContainer() : ALACContainer(),
    m_cafFile()
{}

//-------------------------------------------------------------------------------------------

ALACCafContainer::~ALACCafContainer()
{}

//-------------------------------------------------------------------------------------------

void ALACCafContainer::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ALACCafContainer::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ALACCafContainer::init(engine::File *f)
{
    bool res = false;
    
    if(f!=0)
    {
        if(m_cafFile.init(f))
        {
            ALACSequence *seq;
            
            seq = m_cafFile.getSequence(CAFF_DESC_ID);
            if(seq==0)
            {
                printError("init","Cannot get description block");
                return false;
            }
            if(!m_description.read(seq))
            {
                printError("init","Error reading description data");
                delete seq;
                return false;
            }
            delete seq;
            
            seq = m_cafFile.getSequence(CAFF_KUKI_ID);
            if(seq==0)
            {
                printError("init","Cannot get configuration data");
                return false;
            }
            if(!m_config.read(seq))
            {
                printError("init","Error reading configuration data");
                delete seq;
                return false;
            }
            delete seq;
            
            seq = m_cafFile.getSequence(CAFF_PAKT_ID);
            if(seq==0)
            {
                printError("init","Cannot get frame packet data");
                return false;
            }
            if(!m_frameIndex.read(seq))
            {
                printError("init","Error reading frame packet data");
                delete seq;
                return false;
            }
            delete seq;
            
            m_dataOffset = m_cafFile.blockOffset(CAFF_DATA_ID) + 4;
            m_dataLength = m_cafFile.blockLength(CAFF_DATA_ID) - 4;
            if(m_dataOffset==-1 || m_dataLength==-1)
            {
                printError("init","No data block");
            }
            else
            {
                res = true;
            }
        }
        else
        {
            printError("init","Invalid CAF file");
        }
    }
    else
    {
        printError("init","No file input given");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
