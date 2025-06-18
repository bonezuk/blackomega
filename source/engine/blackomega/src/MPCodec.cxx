#include "engine/blackomega/inc/MPCodec.h"
#if defined(OMEGA_MP3_COMPARE)
#include "engine/inc/Compare.h"
#endif
#include "engine/inc/RData.h"
#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            MPCodecFactory,MPCodec, \
                            "mp3",false)

//-------------------------------------------------------------------------------------------
// MPCodecInitialize
//-------------------------------------------------------------------------------------------

MPCodecInitialize *MPCodecInitialize::m_instance = 0;

//-------------------------------------------------------------------------------------------

MPCodecInitialize::MPCodecInitialize()
{
    Band::instance();
    Subband::start();    
}

//-------------------------------------------------------------------------------------------

MPCodecInitialize::~MPCodecInitialize()
{
    Band *band = Band::instance();
    Subband::stop();
    delete band;    
}

//-------------------------------------------------------------------------------------------

void MPCodecInitialize::start()
{
    if(m_instance==0)
    {
        m_instance = new MPCodecInitialize();
    }
}

//-------------------------------------------------------------------------------------------

void MPCodecInitialize::end()
{
    if(m_instance!=0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------
// MPCodec
//-------------------------------------------------------------------------------------------

MPCodec::MPCodec(QObject *parent) : Codec(Codec::e_codecMp3,parent),
    m_alloc(),
    m_cachedFile(0),
    m_file(0),
    m_bs(0),
    m_state(-1),
    m_time(),
    m_header(0),
    m_polyPhaseIn(0),
    m_grMax(0),
    m_outOffset(0),
    m_frameCount(0)
{
    tint gr,ch;
        
    for(gr=0;gr<2;++gr)
    {
        for(ch=0;ch<2;++ch)
        {
            m_scale[gr][ch] = 0;
            m_reorder[gr][ch] = 0;
            m_antiAlias[gr][ch] = 0;
        }
        m_dequantize[gr] = 0;
        m_dequantizeStereo[gr] = 0;
        m_stereo[gr] = 0;
        m_hybrid[gr] = 0;
        m_subband[gr] = 0;
    }
    
    memset(m_hybridOut,0,sizeof(sample_t) * SBLIMIT * SSLIMIT);
    
    m_polyPhaseIn = reinterpret_cast<sample_t *>(m_alloc.MemAllocAlign(SBLIMIT,sizeof(sample_t),16));

    init();
}

//-------------------------------------------------------------------------------------------

MPCodec::~MPCodec()
{
    try
    {
        close();
        freeStructure();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void MPCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "MPCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void MPCodec::initStructure()
{
    tint gr,ch;
    
    freeStructure();
    
    // scale factor units
    for(gr=0;gr<2;++gr)
    {
        for(ch=0;ch<2;++ch)
        {
            m_scale[gr][ch] = new ScaleFactor;
        }
    }
    
    // dequantization units
    for(gr=0;gr<2;++gr)
    {
        m_dequantize[gr] = new Dequantize;
        m_dequantize[gr]->set(m_scale[gr][0]);
        m_dequantizeStereo[gr] = new DequantizeStereo;
        m_dequantizeStereo[gr]->set(m_scale[gr][1]);
    }
    
    // stereo units
    for(gr=0;gr<2;++gr)
    {
        m_stereo[gr] = new Stereo;
        m_stereo[gr]->set(m_scale[gr][1]);
        m_stereo[gr]->set(reinterpret_cast<sample_t *>(m_dequantize[gr]->get()),reinterpret_cast<sample_t *>(m_dequantizeStereo[gr]->get()));
    }
    
    // reordering and antialias units
    for(gr=0;gr<2;++gr)
    {
        for(ch=0;ch<2;++ch)
        {
            m_reorder[gr][ch] = new Reorder;
            m_antiAlias[gr][ch] = new AntiAlias;
        }
        m_reorder[gr][0]->set(reinterpret_cast<sample_t *>(m_stereo[gr]->getLeft()));
        m_reorder[gr][1]->set(reinterpret_cast<sample_t *>(m_stereo[gr]->getRight()));
        m_antiAlias[gr][0]->set(reinterpret_cast<sample_t *>(m_stereo[gr]->getLeft()));
        m_antiAlias[gr][1]->set(reinterpret_cast<sample_t *>(m_stereo[gr]->getRight()));
    }
    
    // hybrid engine units
    for(gr=0;gr<2;++gr)
    {
        m_hybrid[gr] = new Hybrid;
    }
    
    // subband engine units
    for(gr=0;gr<2;++gr)
    {
        m_subband[gr] = new Subband;
    }
    
    // link scaleinfo backward units
    m_scale[1][0]->set(reinterpret_cast<ScaleInfoS *>(m_scale[0][0]->get()));
    m_scale[1][1]->set(reinterpret_cast<ScaleInfoS *>(m_scale[0][1]->get()));
}

//-------------------------------------------------------------------------------------------

void MPCodec::freeStructure()
{
    tint gr,ch;
    
    for(gr=0;gr<2;++gr)
    {
        for(ch=0;ch<2;++ch)
        {
            if(m_scale[gr][ch]!=0)
            {
                delete m_scale[gr][ch];
                m_scale[gr][ch] = 0;
            }
            if(m_reorder[gr][ch]!=0)
            {
                delete m_reorder[gr][ch];
                m_reorder[gr][ch] = 0;
            }
            if(m_antiAlias[gr][ch]!=0)
            {
                delete m_antiAlias[gr][ch];
                m_antiAlias[gr][ch] = 0;
            }
        }
        if(m_dequantize[gr]!=0)
        {
            delete m_dequantize[gr];
            m_dequantize[gr] = 0;
        }
        if(m_dequantizeStereo[gr]!=0)
        {
            delete m_dequantizeStereo[gr];
            m_dequantizeStereo[gr] = 0;
        }
        if(m_stereo[gr]!=0)
        {
            delete m_stereo[gr];
            m_stereo[gr] = 0;
        }
        if(m_hybrid[gr]!=0)
        {
            delete m_hybrid[gr];
            m_hybrid[gr] = 0;            
        }
        if(m_subband[gr]!=0)
        {
            delete m_subband[gr];
            m_subband[gr] = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

void MPCodec::link(HeaderItem *item)
{
    tint gr,ch;
    
    m_grMax = (item->m_header.lsf) ? 1 : 2;
    for(gr=0;gr<m_grMax;++gr)
    {
        for(ch=0;ch<2;++ch)
        {
            m_scale[gr][ch]->set(reinterpret_cast<MPHeaderInfo *>(&(item->m_header)));
            m_scale[gr][ch]->set(reinterpret_cast<SideInfoS *>(&(item->m_sideinfo)),ch,gr);
            m_reorder[gr][ch]->set(reinterpret_cast<MPHeaderInfo *>(&(item->m_header)));
            m_reorder[gr][ch]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[ch].gr[gr])));
            m_antiAlias[gr][ch]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[ch].gr[gr])));
        }
        m_dequantize[gr]->set(reinterpret_cast<MPHeaderInfo *>(&(item->m_header)));
        m_dequantize[gr]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[0].gr[gr])));
        m_dequantizeStereo[gr]->set(reinterpret_cast<MPHeaderInfo *>(&(item->m_header)));
        m_dequantizeStereo[gr]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[1].gr[gr])));
        m_stereo[gr]->set(reinterpret_cast<MPHeaderInfo *>(&(item->m_header)));
        m_stereo[gr]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[0].gr[gr])));
    }
    m_hybrid[0]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[0].gr[0])),0);
    m_hybrid[0]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[0].gr[1])),1);
    m_hybrid[1]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[1].gr[0])),0);
    m_hybrid[1]->set(reinterpret_cast<GrInfoS *>(&(item->m_sideinfo.ch[1].gr[1])),1);
}

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_MP3_COMPARE)
tint g_debug_frameCount = 254;
#endif

bool MPCodec::decode()
{
    tint ch,gr,sb,ss;
    sample_t *hybridIn;
    HeaderItem *item;
    bool res = false;
    
    if(m_header!=0)
    {
        item = m_header->next();
        if(item!=0)
        {
            BSequence *seq = dynamic_cast<BSequence *>(m_bs->getSequence(2));
            
            if(seq!=0 && seq->move(item->m_bodyBookmark))
            {
                link(item);
                
                for(gr=0;gr<m_grMax;++gr)
                {
                    for(ch=0;ch<static_cast<tint>(item->m_header.stereo);++ch)
                    {
                        seq->startGrain(item->m_sideinfo.ch[ch].gr[gr].part2_3_length);
                        m_scale[gr][ch]->read(seq);
                        if(!ch)
                        {
                            if(!m_dequantize[gr]->read(seq))
                            {
                                printError("decode","Error dequantizing stream");
                                return false;
                            }
                            
#if defined(OMEGA_MP3_COMPARE)
                            if(g_Compare.isThreadA())
                            {
                                tint frame = g_Compare.frameA();
                                common::Log::g_Log.print("%d Dequantize gr-%d ch-%d frame-%d\n",m_frameCount,gr,ch,frame);
                                g_Compare.compareA(reinterpret_cast<tfloat32 *>(m_dequantize[gr]->get()),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                                frame = g_Compare.frameA();
                            }
                            else
                            {
                                tint frame = g_Compare.frameB();
                                common::Log::g_Log.print("%d Dequantize gr-%d ch-%d frame-%d\n",m_frameCount,gr,ch,frame);
                                g_Compare.compareB(reinterpret_cast<tfloat32 *>(m_dequantize[gr]->get()),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                                frame = g_Compare.frameB();
                            }
#endif
                            
                        }
                        else
                        {
                            if(!m_dequantizeStereo[gr]->read(seq))
                            {
                                printError("decode","Error dequantizing stereo stream");
                                return false;
                            }
#if defined(OMEGA_MP3_COMPARE)
                            if(g_Compare.isThreadA())
                            {
                                tint frame = g_Compare.frameA();
                                common::Log::g_Log.print("%d DequantizeStereo gr-%d ch-%d frame-%d\n",m_frameCount,gr,ch,frame);
                                g_Compare.compareA(reinterpret_cast<tfloat32 *>(m_dequantizeStereo[gr]->get()),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                                frame = g_Compare.frameA();
                            }
                            else
                            {
                                tint frame = g_Compare.frameB();
                                common::Log::g_Log.print("%d DequantizeStereo gr-%d ch-%d frame-%d\n",m_frameCount,gr,ch,frame);
                                g_Compare.compareB(reinterpret_cast<tfloat32 *>(m_dequantizeStereo[gr]->get()),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                                frame = g_Compare.frameB();
                            }
#endif
                        }

                        if(seq->remain() > 0)
                        {
                            seq->seek(seq->remain());
                        }
                    }

                    m_stereo[gr]->set(m_dequantizeStereo[gr]->getStereo(DequantizeStereo::e_stereoLong),
                                      m_dequantizeStereo[gr]->getStereo(DequantizeStereo::e_stereoShort0),
                                      m_dequantizeStereo[gr]->getStereo(DequantizeStereo::e_stereoShort1),
                                      m_dequantizeStereo[gr]->getStereo(DequantizeStereo::e_stereoShort2));
                    m_stereo[gr]->process();

#if defined(OMEGA_MP3_COMPARE)
                    if(g_Compare.isThreadA())
                    {
                        tint frame = g_Compare.frameA();
                        common::Log::g_Log.print("%d Stereo gr-%d frame-%d\n",m_frameCount,gr,frame);
                        g_Compare.compareA(m_stereo[gr]->getLeft(),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                        frame = g_Compare.frameA();
                        g_Compare.compareA(m_stereo[gr]->getRight(),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                        frame = g_Compare.frameA();
                    }
                    else
                    {
                        tint frame = g_Compare.frameB();
                        common::Log::g_Log.print("%d Stereo gr-%d frame-%d\n",m_frameCount,gr,frame);
                        g_Compare.compareB(m_stereo[gr]->getLeft(),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                        frame = g_Compare.frameB();
                        g_Compare.compareB(m_stereo[gr]->getRight(),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                        frame = g_Compare.frameB();
                    }
#endif

                    for(ch=0;ch<static_cast<tint>(item->m_header.stereo);++ch)
                    {
                        m_reorder[gr][ch]->process();
                        hybridIn = m_antiAlias[gr][ch]->process();
                        
                        if(!item->m_header.lsf)
                        {
                            m_hybrid[ch]->swap();
                        }
                        for(sb=0;sb<32;++sb)
                        {
                            m_hybrid[ch]->transform(&hybridIn[sb*18],&m_hybridOut[sb][0],sb);
                        }
                        for(ss=1;ss<SSLIMIT;ss+=2)
                        {
                            for(sb=1;sb<SBLIMIT;sb+=2)
                            {
                                m_hybridOut[sb][ss] = -m_hybridOut[sb][ss];
                            }
                        }

#if defined(OMEGA_MP3_COMPARE)
                        if(g_Compare.isThreadA())
                        {
                            tint frame = g_Compare.frameA();
                            common::Log::g_Log.print("%d Hybrid gr-%d ch-%d frame-%d\n",m_frameCount,gr,ch,frame);
                            g_Compare.compareA(reinterpret_cast<tfloat32 *>(m_hybridOut),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                            frame = g_Compare.frameA();
                        }
                        else
                        {
                            tint frame = g_Compare.frameB();
                            common::Log::g_Log.print("%d Hybrid gr-%d ch-%d frame-%d\n",m_frameCount,gr,ch,frame);
                            g_Compare.compareB(reinterpret_cast<tfloat32 *>(m_hybridOut),SBLIMIT * SSLIMIT,(g_debug_frameCount==m_frameCount) ? true : false);
                            frame = g_Compare.frameB();                            
                        }
#endif

                        for(ss=0;ss<SSLIMIT;++ss)
                        {
                            sample_t *pcm;
                            
                            for(sb=0;sb<SBLIMIT;++sb)
                            {
                                m_polyPhaseIn[sb] = m_hybridOut[sb][ss];
                            }
                            
                            pcm = &m_out[(gr * SBLIMIT * SSLIMIT * 2) + ((ss * 64) + ch)];
                            m_subband[ch]->synthesis(m_polyPhaseIn,pcm);
                        }
                    }
                    
                    if(item->m_header.stereo==1)
                    {
                        sample_t *pcm = &m_out[gr * SBLIMIT * SSLIMIT * 2];
                        
                        for(ss=0;ss<(2 * SBLIMIT * SSLIMIT);ss+=2)
                        {
                            pcm[ss + 1] = pcm[ss];
                        }
                    }
                    
                    m_frameCount++;
                }

                res = true;
            }
            else
            {
                printError("decode","Error getting body sequence");
            }
            m_header->complete(item);
        }
    }
    else
    {
        printError("decode","No mp3 stream has been defined to decode from");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::open(const QString& name)
{
    QString err;
    bool res = false;
    
    close();
    
    m_cachedFile = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
    m_file = new engine::IOFile(m_cachedFile);
    
    if(m_file->open(name))
    {
        m_bs = new BBitstream;
        if(m_bs->open(m_file))
        {
            m_header = new MPHeader;
            m_header->set(m_bs);
            if(m_header->start())
            {
                m_state = 0;
                m_time = 0;
                res = true;
            }
            else
            {
                err = "Failed to start decoding of mp3 file '" + name + "'";
                printError("open",err.toLatin1().constData());
            }
        }
        else
        {
            printError("open","Error opening bitstream for given file");
        }
    }
    else
    {
        err = "Failed to mp3 file '" + name + "'";
        printError("open",err.toLatin1().constData());
    }
    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void MPCodec::close()
{
    if(m_header!=0)
    {
        delete m_header;
        m_header = 0;
    }
    if(m_bs!=0)
    {
        delete m_bs;
        m_bs = 0;
    }
    if(m_file!=0)
    {
        delete m_file;
        m_file = 0;
    }
    if(m_cachedFile!=0)
    {
        delete m_cachedFile;
        m_cachedFile = 0;
    }
    m_state = -1;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::init()
{
    initStructure();
    m_initFlag = true;
    return true;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::next(AData& data)
{
    tint i,len;
    sample_t *buffer;
    bool res = true;
    engine::RData& rData = dynamic_cast<engine::RData &>(data);
    
    if(!rData.noParts())
    {
        data.start() = m_time;
    }
        
    if(m_state>=0)
    {
        engine::RData::Part *part = &(rData.nextPart());
        
        buffer = rData.partData(rData.noParts() - 1);
        part->start() = m_time;
        
        i = 0;
        len = rData.rLength();
        
        while(i<len && res)
        {
            switch(m_state)
            {
                case 0:
                    {
                        if(decode())
                        {
                            m_outOffset = 0;
                            m_state = 1;
                        }
                        else
                        {
                            part->end() = m_time;
                            part->done() = true;
                            m_state = -1;
                            res = false;
                        }
                    }
                    break;
                
                case 1:
                    {
                        tint amount,oAmount,noChan = noChannels();
                        
                        amount = len - i;
                        oAmount = (SBLIMIT * SSLIMIT * m_grMax) - m_outOffset;
                        if(amount > oAmount)
                        {
                            amount = oAmount;
                        }
                        if(amount > 0)
                        {
                            memcpy(&buffer[i * noChan],&m_out[m_outOffset * noChan],amount * noChan * sizeof(sample_t));
                            m_outOffset += amount;
                            i += amount;
                            m_time += (static_cast<tfloat64>(amount) / static_cast<tfloat64>(frequency()));
                        }
                        
                        if(m_outOffset >= (SBLIMIT * SSLIMIT * m_grMax))
                        {
                            m_state = 0;
                        }
                    }
                    break;
            }
        }
        
        part->length() = i;
        part->end() = m_time;
        part->done() = true;
        data.end() = m_time;
    }
    else
    {
        res = false;
    }
    
    return res;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::isSeek() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::seek(const common::TimeStamp& v)
{
    tint i,frames;
    bool res = false;
    
    if(m_header!=0)
    {
        m_time = v;
        
        if(m_header->seek(m_time,frames))
        {        
            m_state = 0;
            m_outOffset = 0;

            res = true;
            for(i=0;i<frames && res;i++)
            {
                if(!decode())
                {
                    res = false;
                }
            }
        }
        else
        {
            close();
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::isComplete() const
{
    return (m_state>=0) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::isRemote() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool MPCodec::isBuffered(tfloat32& percent)
{
    percent = 100.0f;
    return true;
}

//-------------------------------------------------------------------------------------------

tint MPCodec::frequency() const
{
    if(m_header!=0)
    {
        return m_header->frequency();
    }
    else
    {
        return 44100;
    }
}

//-------------------------------------------------------------------------------------------

tint MPCodec::noChannels() const
{
    return 2;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp MPCodec::length() const
{
    common::TimeStamp t;
    
    if(m_header!=0)
    {
        t = m_header->length();
    }
    return t;
}

//-------------------------------------------------------------------------------------------

tint MPCodec::bitrate() const
{
    tint rate = 0;
    
    if(m_header!=0)
    {
        rate = m_header->bitrate();
    }
    return rate;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
