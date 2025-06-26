#include "engine/redomega/inc/ALACFrameIndex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACFrameIndex::ALACFrameIndex() : m_frameList()
{}

//-------------------------------------------------------------------------------------------

ALACFrameIndex::~ALACFrameIndex()
{}

//-------------------------------------------------------------------------------------------

void ALACFrameIndex::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ALACFrameIndex::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ALACFrameIndex::read(ALACSequence *seq)
{
    tuint32 offset = 0,total = 0;
    QVector<QPair<tuint32,tuint32> > tList;
    bool res = true;

    if(seq!=0)
    {
        seq->seek(24 << 3);
        m_frameList.clear();
        tList.reserve(1000);
        while(seq->remain() > 0)
        {
            total = seq->readBERInteger();
            tList.append(QPair<tuint32,tuint32>(offset,total));
            offset += total;
            if(tList.size()>=1000)
            {
                m_frameList += tList;
                tList.clear();
                tList.reserve(1000);
            }
        }
        if(tList.size()>0)
        {
            m_frameList += tList;
        }
    }
    else
    {
        printError("read","No sequence given to read from");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
