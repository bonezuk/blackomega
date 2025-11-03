#include "engine/redomega/inc/ALACContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACContainer::ALACContainer() : m_description(),
    m_config(),
    m_frameIndex(),
    m_dataOffset(0)
{}

//-------------------------------------------------------------------------------------------

ALACContainer::~ALACContainer()
{}

//-------------------------------------------------------------------------------------------

void ALACContainer::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ALACContainer::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ALACContainer::init(engine::File *f)
{
    return false;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
