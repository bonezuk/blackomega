#include <math.h>
#include "engine/silveromega/inc/VSilverFloorData0.h"
#include "engine/silveromega/inc/VSilverWindow.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

tfloat32 *VSilverWindow::m_windowLookup = 0;

const float c_halfPi = 0.5f * 3.141592653589793238464264338832795f;

const float c_quarterPi = 0.25f * 3.141592653589793238464264338832795f;

//-------------------------------------------------------------------------------------------

VSilverWindow::VSilverWindow(VSilverContainer *info) : m_mode(0),
    m_winLength(0),
    m_window(0),
    m_leftStart(0),
    m_leftEnd(0),
    m_rightStart(0),
    m_rightEnd(0),
    m_centerW(0),
    m_prevBlockMode(0),
    m_currentBlockMode(0),
    m_outCurrent(-1),
    m_outReturn(-1),
    m_pcmEnd(0),
    m_pcm(0),
    m_outPosition(0),
    m_outSize(0),
    m_out(0),
    m_info(info),
    m_alloc()
{
    tint i,j;
    
    m_out = reinterpret_cast<tfloat32 **>(m_alloc.MemAlloc(info->m_information->m_audioChannels,sizeof(tfloat32 *)));
    for(i=0;i<info->m_information->m_audioChannels;++i)
    {
        m_out[i] = reinterpret_cast<tfloat32 *>(m_alloc.MemAlloc(1 << 16,sizeof(tfloat32)));
        for(j=0;j<(1<<16);++j)
        {
            m_out[i][j] = 0.0f;
        }
    }
    m_centerW = m_info->m_information->m_blockSize_1;
}

//-------------------------------------------------------------------------------------------

VSilverWindow::~VSilverWindow()
{
    try
    {
        if(m_window!=0)
        {
            delete [] m_window;
            m_window = 0;
        }
        m_alloc.FreeAll();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverWindow::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::start()
{
    if(m_windowLookup==0)
    {
        tint i;
        tfloat32 tmp;
        
        m_windowLookup = new tfloat32 [65536];
        for(i=0;i<65536;++i)
        {
            tmp = static_cast<tfloat32>(sin((c_quarterPi / 32768.0f) * static_cast<tfloat32>(i)));
            m_windowLookup[i] = static_cast<tfloat32>(sin(c_halfPi * tmp * tmp));
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::stop()
{
    if(m_windowLookup!=0)
    {
        delete [] m_windowLookup;
        m_windowLookup = 0;
    }
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::calculateLeftWindow(tfloat32 *x,const tint shift)
{
    tint i,len = static_cast<tint>(1 << shift);
    
    for(i=0;i<len;++i)
    {
        x[i] = m_windowLookup[(((i<<1) + 1) << (15 - shift))];
    }
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::calculateRightWindow(tfloat32 *x,const tint shift)
{
    tint i,len = static_cast<tint>(1 << shift);
    
    for(i=0;i<len;++i)
    {
        x[i] = m_windowLookup[65536 - ((((i<<1) + 1) << (15 - shift)))];
    }
}

//-------------------------------------------------------------------------------------------

bool VSilverWindow::setupWindow(engine::Sequence *seq)
{
    tuint i=0,shift,previousWindowFlag,nextWindowFlag;
    tuint lS,lE,lN,lShift,rS,rE,rN,rShift;
    VSilverModeData *mode;
    VSilverCodecInformation *info = m_info->m_information;
    
    if(seq->readBit())
    {
        return false;    
    }
    
    m_mode = seq->readBits(m_info->m_data->m_iLog_vorbis_mode_count);
    mode = m_info->m_data->m_modes[m_mode];
    
    if(!mode->m_blockFlag)
    {
        previousWindowFlag = 0;
        nextWindowFlag = 0;
        shift = info->m_shiftBlockSize_0;
        m_winLength = info->m_blockSize_0;
    }
    else
    {
        previousWindowFlag = seq->readBit();
        nextWindowFlag = seq->readBit();
        shift = info->m_shiftBlockSize_1;
        m_winLength = info->m_blockSize_1;
    }
    
    if(m_window==0)
    {
        tint max = (m_info->m_information->m_blockSize_0 > m_info->m_information->m_blockSize_1) ? m_info->m_information->m_blockSize_0 : m_info->m_information->m_blockSize_1;
        m_window = new tfloat32 [max];
    }
    
    if(mode->m_blockFlag && !previousWindowFlag)
    {
        m_leftStart = lS = (m_winLength >> 2) - (info->m_blockSize_0 >> 2);
        m_leftEnd = lE = (m_winLength >> 2) + (info->m_blockSize_0 >> 2);
        lShift = info->m_shiftBlockSize_0 - 1;
        lN = info->m_blockSize_0 >> 1;
    }
    else
    {
        m_leftStart = lS = 0;
        m_leftEnd = lE = lN = m_winLength >> 1;
        lShift = shift - 1;
    }
    
    if(mode->m_blockFlag && !nextWindowFlag)
    {
        m_rightStart = rS = ((m_winLength * 3) >> 2) - (info->m_blockSize_0 >> 2);
        m_rightEnd = rE = ((m_winLength * 3) >> 2) + (info->m_blockSize_0 >> 2);
        rN = info->m_blockSize_0 >> 1;
        rShift = info->m_shiftBlockSize_0 - 1;
    }
    else
    {
        m_rightStart = rS = rN = m_winLength >> 1;
        m_rightEnd = rE = m_winLength;
        rShift = shift - 1;
    }
    
    while(i<lS)
    {
        m_window[i++] = 0.0f;
    }
    
    calculateLeftWindow(&m_window[lS],lShift);
    i = lE;
    
    while(i<rS)
    {
        m_window[i++] = 1.0f;
    }
    
    calculateRightWindow(&m_window[rS],rShift);
    i = rE;

    while(i<static_cast<tuint>(m_winLength))
    {
        m_window[i++] = 0.0f;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverWindow::setupPCM()
{
    tint i,j;
    VSilverModeData *mode = m_info->m_data->m_modes[m_mode];
    
    if(!mode->m_blockFlag)
    {
        m_pcmEnd = m_info->m_information->m_blockSize_0;
    }
    else
    {
        m_pcmEnd = m_info->m_information->m_blockSize_1;
    }
    
    if(m_pcm==0)
    {
        tint max = (m_info->m_information->m_blockSize_0 > m_info->m_information->m_blockSize_1) ? m_info->m_information->m_blockSize_0 : m_info->m_information->m_blockSize_1;
        
        m_pcm = reinterpret_cast<tfloat32 **>(m_alloc.MemAlloc(m_info->m_information->m_audioChannels,sizeof(tfloat32 *)));
        for(i=0;i<m_info->m_information->m_audioChannels;++i)
        {
            m_pcm[i] = reinterpret_cast<tfloat32 *>(m_alloc.MemAlloc(max,sizeof(tfloat32)));
            for(j=0;j<max;++j)
            {
                m_pcm[i][j] = 0.0f;
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverWindow::setup(engine::Sequence *seq)
{
    if(!setupWindow(seq))
    {
        return false;
    }
    if(!setupPCM())
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::window()
{
    tint j,i;
    
    for(j=0;j<m_info->m_information->m_audioChannels;++j)
    {
        i = 0;
        while(i<m_leftStart)
        {
            m_pcm[j][i++] = 0.0f;
        }
        while(i<m_leftEnd)
        {
            m_pcm[j][i] *= m_window[i];
            i++;
        }
        
        i = m_rightStart;
        while(i<m_rightEnd)
        {
            m_pcm[j][i] *= m_window[i];
            i++;
        }
        while(i<m_winLength)
        {
            m_pcm[j][i++] = 0.0f;
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverWindow::synthesis()
{
    tint i,j,n,n0,n1;
    tint thisCenter,prevCenter;
    tfloat32 *pcm,*p;
    
    m_prevBlockMode = m_currentBlockMode;
    m_currentBlockMode = m_info->m_data->m_modes[m_mode]->m_blockFlag;
    
    n = ((!m_currentBlockMode) ? m_info->m_information->m_blockSize_0 : m_info->m_information->m_blockSize_1) / 2;
    n0 = m_info->m_information->m_blockSize_0 / 2;
    n1 = m_info->m_information->m_blockSize_1 / 2;
    
    if(m_centerW)
    {
        thisCenter = n1;
        prevCenter = 0;
    }
    else
    {
        thisCenter = 0;
        prevCenter = n1;
    }
    
    for(j=0;j<m_info->m_information->m_audioChannels;++j)
    {
        if(m_prevBlockMode)
        {
            if(m_currentBlockMode)
            {
                pcm = m_out[j] + prevCenter;
                p = m_pcm[j];
                
                for(i=0;i<n1;++i)
                {
                    pcm[i] += p[i];
                }
            }
            else
            {
                pcm = m_out[j] + prevCenter + n1/2 - n0/2;
                p = m_pcm[j];
                
                for(i=0;i<n0;++i)
                {
                    pcm[i] += p[i];
                }
            }
        }
        else
        {
            if(m_currentBlockMode)
            {
                pcm = m_out[j] + prevCenter;
                p = m_pcm[j] + n1/2 - n0/2;
                
                for(i=0;i<n0;++i)
                {
                    pcm[i] += p[i];
                }
                for(;i<n1/2+n0/2;i++)
                {
                    pcm[i] = p[i];
                }
            }
            else
            {
                pcm = m_out[j] + prevCenter;
                p = m_pcm[j];
                
                for(i=0;i<n0;i++)
                {
                    pcm[i] += p[i];
                }
            }
        }
        
        pcm = m_out[j] + thisCenter;
        p = m_pcm[j] + n;
        
        for(i=0;i<n;++i)
        {
            pcm[i] = p[i];
        }
    }
    
    if(m_centerW)
    {
        m_centerW = 0;
    }
    else
    {
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
}

//-------------------------------------------------------------------------------------------
// PCM len - number of stereo sample pairs in given buffer
//-------------------------------------------------------------------------------------------

tint VSilverWindow::getPCM(sample_t *mem,tint len)
{
    tint i,k,total;
    
    if(mem!=0 && len>0)
    {
        len <<= 1;
        
        if(m_info->m_information->m_audioChannels==1)
        {
            tfloat32 x1;
            
            for(i=0,total=0,k=m_outReturn;i<len && k<m_outCurrent;++k,++total,i+=2)
            {
                x1 = m_out[0][k];
                if(x1 > 1.0f)
                {
                    x1 = 1.0f;
                }
                else if(x1 < -1.0f)
                {
                    x1 = -1.0f;
                }
                
                mem[i] = static_cast<sample_t>(x1);
                mem[i+1] = static_cast<sample_t>(x1);
            }
            m_outReturn += total;
        }
        else if(m_info->m_information->m_audioChannels==2)
        {
            tfloat32 x1,x2;
            
            for(i=0,total=0,k=m_outReturn;i<len && k<m_outCurrent;++k,++total,i+=2)
            {
                x1 = m_out[0][k];
                if(x1>1.0f)
                {
                    x1 = 1.0f;
                }
                else if(x1 < -1.0f)
                {
                    x1 = -1.0f;
                }
                
                x2 = m_out[1][k];
                if(x2>1.0f)
                {
                    x2 = 1.0f;
                }
                else if(x2 < - 1.0f)
                {
                    x2 = -1.0f;
                }
                
                mem[i] = static_cast<sample_t>(x1);
                mem[i+1] = static_cast<sample_t>(x2);
            }
            m_outReturn += total;
        }
        else
        {
            total = 0;
        }
    }
    else
    {
        total = 0;
    }
    return total;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
