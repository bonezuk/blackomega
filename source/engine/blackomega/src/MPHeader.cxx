#include "engine/blackomega/inc/MPHeader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

int g_bitIndex[2][3][16]={
    {
        {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},
        {0,32,48,56,64,80,96,112,128,160,192,224,256,320,384},
        {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320}
    },
    {
        {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256},
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160},
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160}
    }
};

//-------------------------------------------------------------------------------------------

int g_freqs[9]={44100,48000,32000,22050,24000,16000,11025,12000,8000};

//-------------------------------------------------------------------------------------------
// HeaderItem
//-------------------------------------------------------------------------------------------

HeaderItem::HeaderItem() : m_prev(0),
    m_next(0),
    m_headWidth(0),
    m_dataWidth(0),
    m_headBookmark(-1),
    m_bodyBookmark(-1),
    m_freeBookmark(-1)
{}

//-------------------------------------------------------------------------------------------

void HeaderItem::clear()
{
    m_prev = 0;
    m_next = 0;
    m_headWidth = 0;
    m_dataWidth = 0;
    m_headBookmark = -1;
    m_bodyBookmark = -1;
    m_freeBookmark = -1;
}

//-------------------------------------------------------------------------------------------
// MPHeader
//-------------------------------------------------------------------------------------------

MPHeader::MPHeader() : m_alloc(),
    m_bs(0),
    m_firstItem(0),
    m_lastItem(0),
    m_freeItem(0),
    m_headerBookmark(-1),
    m_startOffset(0),
    m_frequencyIndex(0),
    m_frequency(0),
    m_kbps(0),
    m_stereo(false),
    m_samplesPerSecond(0),
    m_width(0),
    m_widthHeader(0),
    m_widthData(0),
    m_spf(0.0),
    m_xingHeader(0),
    m_length(),
    m_rate(0)
{}

//-------------------------------------------------------------------------------------------

MPHeader::~MPHeader()
{
    try
    {
        HeaderItem *nItem,*item;

        item = m_firstItem;
        while(item!=0)
        {
            nItem = item;
            item = nItem->m_next;
            freeHeaderItem(nItem);
        }

        item = m_freeItem;
        while(item!=0)
        {
            nItem = item;
            item = nItem->m_next;
            delete nItem;
        }
        m_alloc.FreeAll();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

HeaderItem *MPHeader::getHeaderItem()
{
    HeaderItem *item;

    if(m_freeItem==0)
    {
        item = new HeaderItem;
    }
    else
    {
        item = m_freeItem;
        m_freeItem = m_freeItem->m_next;
        item->clear();
    }
    return item;
}

//-------------------------------------------------------------------------------------------

void MPHeader::freeHeaderItem(HeaderItem *item)
{
    if(item!=0)
    {
        item->clear();
        item->m_next = m_freeItem;
        m_freeItem = item;
    }
}

//-------------------------------------------------------------------------------------------

bool MPHeader::validate(tuint h)
{
    tuint layer;

    if((h & 0xffe00000) != 0xffe00000)
    {
        return false;
    }
    if(!(h & 0x00060000))
    {
        return false;
    }
    if(!(h & 0x0000f000))
    {
        return false;
    }
    if((h & 0x0000f000) == 0x0000f000)
    {
        return false;
    }
    if((h & 0x00000c00) == 0x00000c00)
    {
        return false;
    }

    layer = 4 - ((h >> 17) & 0x00000003);
    if(layer!=3)
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void MPHeader::decodeHeader(MPHeaderInfo *hdr,tuint h)
{
    hdr->mpeg_25 = !((h >> 20) & 0x00000001);
    hdr->sampling_frequency = (h >> 10) & 0x00000003;
    if(!hdr->mpeg_25)
    {
        hdr->lsf = ((h >> 19) & 0x00000001) ? 0 : 1;
        hdr->sfreq = hdr->sampling_frequency + (hdr->lsf * 3);
    }
    else
    {
        hdr->lsf = 1;
        hdr->sfreq = 6 + hdr->sampling_frequency;
    }
    hdr->layer = 4 - ((h >> 17) & 0x00000003);
    hdr->error_protection = !((h >> 16) & 0x00000001);
    hdr->bitrate_index = (h >> 12) & 0x0000000f;
    hdr->padding = (h >> 9) & 0x00000001;
    hdr->extension = (h >> 8) & 0x00000001;
    hdr->mode = (h >> 6) & 0x00000003;
    hdr->mode_ext = (h >> 4) & 0x00000003;
    hdr->copyright = (h >> 3) & 0x00000001;
    hdr->original = (h >> 2) & 0x00000001;
    hdr->emphasis = h & 0x00000003;
    hdr->stereo = (hdr->mode==MPG_MD_MONO) ? 1 : 2;
}

//-------------------------------------------------------------------------------------------

tint MPHeader::width(MPHeaderInfo *hdr)
{
    tint hdrSize,dataSize;
    return width(hdr,hdrSize,dataSize);
}

//-------------------------------------------------------------------------------------------

tint MPHeader::width(MPHeaderInfo *hdr,tint& hdrSize,tint& dataSize)
{
    switch(hdr->layer)
    {
        case 1:
            hdrSize = 4;
            dataSize  = g_bitIndex[hdr->lsf][0][hdr->bitrate_index] * 12000;
            dataSize /= g_freqs[hdr->sfreq];
            dataSize  = ((dataSize + hdr->padding) << 2) - 4;
            break;

        case 2:
            hdrSize = 4;
            dataSize  = g_bitIndex[hdr->lsf][1][hdr->bitrate_index] * 144000;
            dataSize /= g_freqs[hdr->sfreq];
            dataSize += hdr->padding - 4;
            break;

        case 3:
            if(hdr->lsf)
            {
                hdrSize = (hdr->stereo==1) ? 9 : 17;
            }
            else
            {
                hdrSize = (hdr->stereo==1) ? 17 : 32;
            }
            if(hdr->error_protection)
            {
                hdrSize += 2;
            }
            hdrSize += 4;
            dataSize  = g_bitIndex[hdr->lsf][2][hdr->bitrate_index] * 144000;
            dataSize /= g_freqs[hdr->sfreq] << (hdr->lsf);
            dataSize += hdr->padding;
            break;
    }
    hdrSize <<= 3;
    dataSize <<= 3;
    dataSize -= hdrSize;
    return hdrSize + dataSize;
}

//-------------------------------------------------------------------------------------------

bool MPHeader::scour(Sequence *seq,tint& offset)
{
    tint i,of,bk;
    tuint h;
    bool res = false;

    bk = seq->bookmark();
    if(bk==-1 || !seq->seek(offset))
    {
        return false;
    }

    h = seq->readBitsI(32);
    if(seq->isEnd())
    {
        return false;
    }

    if(!validate(h))
    {
        if(!seq->seek(-64))
        {
            return false;
        }

        for(i=0,of=-32;i<4 && !res;i++,of+=8)
        {
            h = seq->readBitsI(32);
            if(seq->isEnd())
            {
                return false;
            }
            if(validate(h))
            {
                res = true;
            }
            else
            {
                if(!seq->seek(-24))
                {
                    return false;
                }
            }
        }

        if(!res)
        {
            if(!seq->seek(8))
            {
                return false;
            }

            for(i=0,of=8;i<4 && !res;i++,of+=8)
            {
                h = seq->readBitsI(32);
                if(seq->isEnd())
                {
                    return false;
                }
                if(validate(h))
                {
                    res = true;
                }
                else
                {
                    if(!seq->seek(-24))
                    {
                        return false;
                    }
                }
            }

            if(res)
            {
                offset += of;
            }
        }
    }
    else
    {
        res = true;
    }

    if(!seq->move(bk))
    {
        return false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MPHeader::sequenceSearch(Sequence *seq,tint& offset,bool nFlag)
{
    tint i,bk,a,b,c = 0;
    tuint h;
    MPHeaderInfo hdr;

    if((bk=seq->bookmark())!=-1 && seq->seek(offset))
    {
        while(!seq->isEndStream())
        {
            h = seq->readBitsI(32);
            if(seq->isEndStream())
            {
                return false;
            }
            if(validate(h))
            {
                for(i=0;i<3;++i)
                {
                    decodeHeader(&hdr,h);
                    a = width(&hdr) - 32;
                    if(!scour(seq,a))
                    {
                        break;
                    }
                    if(!seq->seek(a))
                    {
                        return false;
                    }
                    h = seq->readBitsI(32);
                    if(i==0)
                    {
                        b = a + 32;
                    }
                }
                if(i==3)
                {
                    if(nFlag)
                    {
                        offset += c + b;
                    }
                    else
                    {
                        offset += c;
                    }
                    if(!seq->move(bk))
                    {
                        return false;
                    }
                    return true;
                }
                if(!seq->isEnd())
                {
                    if(!seq->move(bk))
                    {
                        return false;
                    }
                    if(!seq->seek(offset + c + 8))
                    {
                        return false;
                    }
                }
            }
            else
            {
                seq->seek(-24);
            }
            c += 8;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

HeaderItem *MPHeader::next()
{
    tint i,offset = 0;
    tuint h;
    HeaderItem *item = m_firstItem;
    Sequence *headSeq,*seq;

    for(i=0;i<3;++i)
    {
        if(item==0)
        {
            seq = m_bs->getSequence(0);

            item = getHeaderItem();

            if(m_headerBookmark>=0)
            {
                seq->move(m_headerBookmark);
            }

            if(item!=0 && sequenceSearch(seq,offset,false))
            {
                if(offset!=0)
                {
                    seq->seek(offset);
                }
                item->m_headBookmark = seq->bookmark();

                if(item->m_headBookmark>=0 && m_bs->mark(1,item->m_headBookmark,0))
                {
                    headSeq = m_bs->getSequence(1);
                    headSeq->move(item->m_headBookmark);

                    h = headSeq->readBitsI(32);
                    if(validate(h))
                    {
                        SideInfo sInfo;

                        decodeHeader(&(item->m_header),h);
                        width(&item->m_header,item->m_headWidth,item->m_dataWidth);

                        m_bs->mark(0,item->m_headBookmark,item->m_headWidth);
                        item->m_bodyBookmark = m_bs->bookmark(item->m_headBookmark,item->m_headWidth);
                        m_headerBookmark = m_bs->bookmark(item->m_bodyBookmark,item->m_dataWidth);

                        if(item->m_header.error_protection)
                        {
                            headSeq->readBitsI(16);
                        }

                        sInfo.set(&(item->m_header));
                        sInfo.set(&(item->m_sideinfo));
                        if(sInfo.read(headSeq))
                        {
                            tint ch,gr,grMax = (item->m_header.lsf) ? 1 : 2;

                            item->m_dataWidth = 0;
                            for(gr=0;gr<grMax;++gr)
                            {
                                for(ch=0;ch<static_cast<tint>(item->m_header.stereo);++ch)
                                {
                                    item->m_dataWidth += item->m_sideinfo.ch[ch].gr[gr].part2_3_length;
                                }
                            }
                        }
                        else
                        {
                            freeHeaderItem(item);
                            item = 0;
                            i--;
                            m_headerBookmark = -1;
                        }
                    }
                    else
                    {
                        freeHeaderItem(item);
                        item = 0;
                        i--;
                        m_headerBookmark = -1;
                    }
                }
                else
                {
                    freeHeaderItem(item);
                    return 0;
                }
            }
            else
            {
                freeHeaderItem(item);
                return 0;
            }

            if(item!=0)
            {
                if(m_lastItem==0)
                {
                    m_firstItem = item;
                }
                else
                {
                    m_lastItem->m_next = item;
                }
                item->m_prev = m_lastItem;
                item->m_next = 0;
                m_lastItem = item;
            }
            else
            {
                item = m_lastItem;
            }
        }
        if(item!=0)
        {
            item = item->m_next;
        }
    }

    item = m_firstItem;
    m_firstItem = item->m_next;
    m_firstItem->m_prev = 0;

    m_bs->move(item->m_bodyBookmark);
    m_bs->mark(2);
    if(item->m_sideinfo.main_data_begin > 0)
    {
        QSet<tint> exSet;

        exSet.insert(1);
        m_bs->moveTo(item->m_bodyBookmark,0 - static_cast<tint>(item->m_sideinfo.main_data_begin <<3),exSet);
        item->m_bodyBookmark = m_bs->bookmark();
        m_bs->mark(2);
        item->m_freeBookmark = item->m_bodyBookmark;
    }
    else
    {
        item->m_freeBookmark = item->m_headBookmark;
    }

    return item;
}

//-------------------------------------------------------------------------------------------

bool MPHeader::start()
{
    tint offset = 0;
    tuint h;
    Sequence *seq = m_bs->getSequence(0);
    MPHeaderInfo hdr;
    bool res = false;

    if(sequenceSearch(seq,offset,false))
    {
        if(seq->seek(offset) && (m_headerBookmark = seq->bookmark())>=0)
        {
            h = seq->readBitsI(32);
            if(validate(h))
            {
                tint hdrSize=0,dataSize=0;
                tuint xHdr;

                decodeHeader(&hdr,h);

                m_startOffset = offset;
                m_frequencyIndex = hdr.sfreq;
                m_frequency = g_freqs[m_frequencyIndex];
                m_kbps = getBitrate(&hdr);
                m_stereo = (hdr.stereo==1) ? false : true;
                m_samplesPerSecond = hdr.stereo * m_frequency;
                m_width = width(&hdr,hdrSize,dataSize);
                m_widthHeader = hdrSize;
                m_widthData = dataSize;

                m_spf = 576000000.0 / static_cast<tfloat64>(m_frequency);
                if(hdr.lsf)
                {
                    m_spf /= 1000000.0;
                }
                else
                {
                    m_spf /= 500000.0;
                }

                seq->seek(hdrSize - 32);
                xHdr = seq->readBitsI(32);
                if(xHdr==0x58696e67)
                {
                    readXingInfo(seq,&hdr);
                }

                tint fileLen = m_bs->file()->length();
                if(fileLen==0)
                {
                    m_length = 0;
                }
                else
                {
                    if(m_xingHeader==0)
                    {
                        tfloat64 mL = (static_cast<tfloat64>(fileLen) * 8.0) - static_cast<tfloat64>(offset);
                        tfloat64 mW = static_cast<tfloat64>(hdrSize + dataSize);
                        tfloat64 fS;

                        if(!hdr.lsf)
                        {
                            fS = 1152.0 / static_cast<tfloat64>(m_frequency);
                        }
                        else
                        {
                            fS = 576.0 / static_cast<tfloat64>(m_frequency);
                        }

                        mL = (mL / mW) * fS;
                        m_length = mL;
                    }
                    else
                    {
                        tfloat64 fS;

                        if(!hdr.lsf)
                        {
                            fS = 1152.0 / static_cast<tfloat64>(m_frequency);
                        }
                        else
                        {
                            fS = 576.0 / static_cast<tfloat64>(m_frequency);
                        }
                        m_length = static_cast<tfloat64>(m_xingHeader->frames) * fS;
                    }
                }

                if(m_xingHeader!=0)
                {
                    m_rate = ((fileLen << 3) - offset) / static_cast<tfloat64>(m_length);
                }
                else
                {
                    m_rate = m_kbps * 1000;
                }

                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint MPHeader::getBitrate(MPHeaderInfo *info)
{
    tint k;

    switch(info->layer)
    {
        case 1:
            k = g_bitIndex[info->lsf][0][info->bitrate_index];
            break;

        case 2:
            k = g_bitIndex[info->lsf][1][info->bitrate_index];
            break;

        case 3:
            k = g_bitIndex[info->lsf][2][info->bitrate_index];
            break;

        default:
            k = 0;
            break;
    }
    return k;
}

//-------------------------------------------------------------------------------------------

void MPHeader::readXingInfo(Sequence *seq,MPHeaderInfo *hdr)
{
    tint i,flags;

    m_xingHeader = reinterpret_cast<XHeadData *>(m_alloc.MemAlloc(1,sizeof(XHeadData)));
    ::memset(m_xingHeader,0,sizeof(XHeadData));
    m_xingHeader->toc = reinterpret_cast<tubyte *>(m_alloc.MemAlloc(100,sizeof(tubyte)));
    ::memset(m_xingHeader->toc,0,100 * sizeof(tubyte));

    m_xingHeader->h_id = !hdr->lsf;
    m_xingHeader->samprate = g_freqs[hdr->sfreq];

    flags = seq->readBitsI(32);
    if(flags & FRAMES_FLAG)
    {
        m_xingHeader->frames = seq->readBitsI(32);
    }
    if(flags & BYTES_FLAG)
    {
        m_xingHeader->bytes = seq->readBitsI(32);
    }
    if(flags & TOC_FLAG)
    {
        for(i=0;i<100;++i)
        {
            m_xingHeader->toc[i] = static_cast<tubyte>(seq->readBitsI(8));
        }
    }
    m_xingHeader->vbr_scale = -1;
    if(flags & VBR_SCALE_FLAG)
    {
        m_xingHeader->vbr_scale = seq->readBitsI(32);
    }
}

//-------------------------------------------------------------------------------------------

bool MPHeader::seek(common::TimeStamp& t,tint& frames)
{
    common::TimeStamp actual(t);
    tint pos;

    if(m_length==0)
    {
        return false;
    }

    if(t <= 0)
    {
        actual = 0;
    }
    else if(t >= m_length)
    {
        return false;
    }

    {
        HeaderItem *nItem,*item = m_firstItem;

        while(item!=0)
        {
            nItem = item;
            item = nItem->m_next;
            freeHeaderItem(nItem);
        }
        m_firstItem = 0;
        m_lastItem = 0;
    }

    if(m_spf < actual)
    {
        actual -= m_spf;
        frames = 1;
        if(actual<=0.0)
        {
            frames = 0;
            actual = 0.0;
        }
    }
    else
    {
        frames = 0;
        actual = 0.0;
    }

    pos = m_startOffset;
    if(m_xingHeader==0)
    {
        tfloat64 f = static_cast<tfloat64>(actual) / m_spf;
        f *= static_cast<tfloat64>(m_width);
        pos += static_cast<tint>(f);
    }
    else
    {
        tint a,b;
        tfloat64 percent;
        tfloat64 fa,fb,fx;

        percent = (static_cast<tfloat64>(actual) / static_cast<tfloat64>(m_length)) * 100.0;
        if(percent<0.0)
        {
            percent = 0.0;
        }
        else if(percent > 100.0)
        {
            percent = 100.0;
        }

        a = static_cast<tint>(percent);
        if(a > 99)
        {
            a = 99;
        }
        fa = static_cast<tfloat64>(m_xingHeader->toc[a]);
        if(a < 99)
        {
            fb = static_cast<tfloat64>(m_xingHeader->toc[a + 1]);
        }
        else
        {
            fb = 256.0;
        }
        fx = fa + (fb - fa) * (percent - static_cast<tfloat64>(a));

        b = (1.0 / 256.0) * fx * static_cast<tfloat64>(m_bs->file()->length() - (m_startOffset >> 3));
        pos += static_cast<tint>(b);
    }
    pos &= 0xfffffff8;

    tint sPos = pos,r = 4096; // maximum main_data_begin number of bits.
    while(r>0)
    {
        sPos -= m_width;
        r -= m_widthData;
    }
    if(sPos<0)
    {
        sPos = 0;
    }

    if(!m_bs->seek(sPos >> 3,File::e_startPosition))
    {
        return false;
    }

    Sequence *seq = m_bs->getSequence(0);

    r = 0;
    while(true)
    {
        tint offset = 0;

        if(sequenceSearch(seq,offset,false))
        {
            if(seq->seek(offset))
            {
                tuint h;
                tint hBk;
                tint hdrWidth = 0,dataWidth = 0;
                MPHeaderInfo hdr;

                hBk = seq->bookmark();
                if(hBk>=0)
                {
                    h = seq->readBitsI(32);
                    if(validate(h))
                    {
                        decodeHeader(&hdr,h);
                        width(&hdr,hdrWidth,dataWidth);

                        if(r>=4096)
                        {
                            m_headerBookmark = hBk;
                            return true;
                        }
                        else
                        {
                            if(!m_bs->mark(1,hBk,0))
                            {
                                break;
                            }
                            if(!m_bs->mark(0,hBk,hdrWidth))
                            {
                                break;
                            }
                            seq = m_bs->getSequence(0);
                            if(!seq->seek(dataWidth))
                            {
                                break;
                            }
                        }
                        r += dataWidth;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

void MPHeader::complete(HeaderItem *item)
{
    if(item!=0)
    {
        m_bs->complete(item->m_freeBookmark);
        freeHeaderItem(item);
    }
}

//-------------------------------------------------------------------------------------------

tint MPHeader::bitrate() const
{
    return m_rate;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
