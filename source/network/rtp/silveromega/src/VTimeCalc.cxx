#include "common/inc/Log.h"
#include "engine/inc/SequenceMemory.h"
#include "network/rtp/silveromega/inc/VTimeCalc.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VTimeCalc::VTimeCalc() : m_container(),
    m_prevBlockMode(0),
    m_currentBlockMode(0),
    m_centerW(0),
    m_outCurrent(-1),
    m_outReturn(-1)
{}

//-------------------------------------------------------------------------------------------

VTimeCalc::VTimeCalc(QSharedPointer<engine::silveromega::VSilverContainer> container) : m_container(container),
    m_prevBlockMode(0),
    m_currentBlockMode(0),
    m_centerW(0),
    m_outCurrent(-1),
    m_outReturn(-1)
{}

//-------------------------------------------------------------------------------------------

VTimeCalc::~VTimeCalc()
{}

//-------------------------------------------------------------------------------------------

void VTimeCalc::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VTimeCalc::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VTimeCalc::setConfiguration(RTPArray& info,RTPArray& comments,RTPArray& data)
{
    QSharedPointer<engine::silveromega::VSilverContainer> container(new engine::silveromega::VSilverContainer);
    engine::SequenceMemory seqInfo(info),seqComments(comments),seqData(data);
    bool res = false;

    if(container->readInformation(&seqInfo))
    {
        if(container->readComments(&seqComments))
        {
            if(container->readData(&seqData))
            {
                if(container->setup())
                {
                    m_container = container;
                    initLength();
                    res = true;
                }
                else
                {
                    printError("setConfiguration","Failed to setup vorbis configuration");
                }
            }
            else
            {
                printError("setConfiguration","Failed to decode vorbis data packet");
            }
        }
        else
        {
            printError("setConfiguration","Failed to decode vorbis comments packet");
        }
    }
    else
    {
        printError("setConfiguration","Failed to decode vorbis information packet");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VTimeCalc::setConfiguration(QSharedPointer<engine::silveromega::VSilverContainer> container)
{
    m_container = container;
    initLength();
    return true;
}

//-------------------------------------------------------------------------------------------

void VTimeCalc::initLength()
{
    m_prevBlockMode = 0;
    m_currentBlockMode = 0;
    m_centerW = m_container->m_information->m_blockSize_1;
    m_outCurrent = -1;
    m_outReturn = -1;
}

//-------------------------------------------------------------------------------------------

tint VTimeCalc::calcLength(RTPArray& array)
{
    tint len;

    if(m_container.data()!=0)
    {
        tint mode,n0,n1,thisCenter,prevCenter;
        engine::SequenceMemory seq(array);

        if(!seq.readBit())
        {
            mode = seq.readBits(m_container->m_data->m_iLog_vorbis_mode_count);

            m_prevBlockMode = m_currentBlockMode;
            m_currentBlockMode = m_container->m_data->m_modes[mode]->m_blockFlag;

            n0 = m_container->m_information->m_blockSize_0 / 2;
            n1 = m_container->m_information->m_blockSize_1 / 2;

            if(m_centerW)
            {
                thisCenter = n1;
                prevCenter = 0;
                m_centerW = 0;
            }
            else
            {
                thisCenter = 0;
                prevCenter = n1;
                m_centerW = 1;
            }

            if(m_outReturn==-1)
            {
                m_outReturn = thisCenter;
                m_outCurrent = thisCenter;
            }
            else
            {
                m_outReturn = prevCenter;
                m_outCurrent = prevCenter;
                m_outCurrent += ((!m_prevBlockMode) ? n0 : n1) / 2;
                m_outCurrent += ((!m_currentBlockMode) ? n0 : n1) / 2;
            }
            len = m_outCurrent - m_outReturn;
        }
        else
        {
            printError("calcLength","Invalid vorbis audio packet");
            len = -1;
        }
    }
    else
    {
        printError("calcLength","Vorbis configuration data has not been set");
        len = -1;
    }
    return len;
}

//-------------------------------------------------------------------------------------------

tint VTimeCalc::frequency() const
{
    if(m_container.data()!=0)
    {
        return m_container->m_information->m_audioSampleRate;
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------

