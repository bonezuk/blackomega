#include "engine/silveromega/inc/VSilverSeeker.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverSeeker::VSilverSeeker(VSilverContainer *container) : m_container(container),
    m_file(0),
    m_bitstream(0),
    m_sequenceNo(1),
    m_serialNo(0xffffffff),
    m_firstOffset(0),
    m_lastOffset(0),
    m_firstPCM(0),
    m_lastPCM(0)
{}

//-------------------------------------------------------------------------------------------

VSilverSeeker::~VSilverSeeker()
{
    try
    {
        if(m_bitstream!=0)
        {
            delete m_bitstream;
            m_bitstream = 0;
        }
        if(m_file!=0)
        {
            delete m_file;
            m_file = 0;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverSeeker::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverSeeker::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::forwardSequenceSearch(tuint64& total,tint seqNo)
{
    tuint x;
    bool res = false;
    engine::Sequence *seq = m_bitstream->getSequence(seqNo);

    if(seq!=0)
    {
        do
        {
            total += 32;
            x = static_cast<tuint>(seq->readBits(32));
            if(x==0x5367674F)
            {
                total -= 32;
                if(!seq->seek(-32))
                {
                    printError("forwardSequenceSearch","Failed to rewind to beginning of page");
                    return false;
                }
                res = true;
            }
            else if(seq->isEnd())
            {
                return false;
            }
            else
            {
                total -= 24;
                if(!seq->seek(-24))
                {
                    printError("forwardSequenceSearch","Error rewinding sequence");
                    return false;
                }
            }
        } while(!res);
    }
    else
    {
        printError("forwardSequenceSearch","Failed to get root sequence");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::readHeader(OggPageHeader *hdr,tuint64& total,tint seqNo)
{
    tuint x1,x2;
    bool res = false;
    engine::Sequence *seq = m_bitstream->getSequence(seqNo);

    if(hdr!=0 && seq!=0)
    {
        total += 32;
        x1 = static_cast<tuint>(seq->readBits(32));
        if(x1==0x5367674F)
        {
            ::memset(hdr,0,sizeof(OggPageHeader));

            total += 8;
            hdr->version = seq->readBits(8);
            if(hdr->version==0)
            {
                total += 8;
                x1 = static_cast<tuint>(seq->readBits(8));
                hdr->continued = (x1 & 0x1) ? true : false;
                hdr->first = (x1 & 0x2) ? true : false;
                hdr->last = (x1 & 0x4) ? true : false;

                total += 64;
                x1 = static_cast<tuint>(seq->readBits(32));
                x2 = static_cast<tuint>(seq->readBits(32));
                hdr->position = ((static_cast<tuint64>(x2) << 32) & 0xffffffff00000000ULL) | (static_cast<tuint64>(x1) & 0x00000000ffffffffULL);

                total += 96;
                hdr->serial = static_cast<tuint>(seq->readBits(32));
                hdr->pageNo = static_cast<tuint>(seq->readBits(32));
                hdr->checksum = static_cast<tuint>(seq->readBits(32));

                total += 8;
                hdr->noSegments = static_cast<tuint>(seq->readBits(8));
                for(x1=0;x1<hdr->noSegments;++x1)
                {
                    total += 8;
                    hdr->segmentTable[x1] = static_cast<tubyte>(seq->readBits(8));
                }

                res = true;
            }
            else
            {
                printError("readHeader","Unknown Ogg stream page version");
            }
        }
        else
        {
            printError("readHeader","Bitstream no on page boundary");
        }
    }
    else
    {
        printError("readHeader","Page header and bitstream sequence required");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::readPage(OggPageHeader *hdr,common::BOQueueTree<engine::Sequence *>& list)
{
    tuint64 total = 0;
    return readPage(hdr,list,total);
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::readPage(OggPageHeader *hdr,common::BOQueueTree<engine::Sequence *>& list,tuint64& total)
{
    tint bodyStart,count,seqNo=m_sequenceNo,offset=0;
    tuint i;
    bool res = false,flag;
    engine::Sequence *pSeq,*seq = m_bitstream->getSequence(0);

    clearSequence(list);

    if(hdr!=0 && seq!=0)
    {
        if(forwardSequenceSearch(total))
        {
            if(readHeader(hdr,total))
            {
                bodyStart = seq->bookmark();
                if(bodyStart>=0)
                {
                    for(res=true,i=0;res && i<hdr->noSegments;)
                    {
                        if(m_bitstream->move(bodyStart,offset << 3))
                        {
                            if(m_bitstream->mark(seqNo))
                            {
                                flag = false;
                                count = 0;
                                while(i<hdr->noSegments && hdr->segmentTable[i]==255)
                                {
                                    count += 255;
                                    i++;
                                }
                                if(i<hdr->noSegments)
                                {
                                    flag = true;
                                    count += hdr->segmentTable[i];
                                    i++;
                                }

                                offset += count;
                                if(m_bitstream->move(bodyStart,offset << 3))
                                {
                                    if(m_bitstream->mark(0))
                                    {
                                        if(flag)
                                        {
                                            pSeq = m_bitstream->getSequence(seqNo);
                                            count = list.Size();
                                            list.Add(count,pSeq);
                                            seqNo++;
                                        }
                                    }
                                    else
                                    {
                                        printError("readPage","Error marking end of packet sequence");
                                        res = false;
                                    }
                                }
                                else
                                {
                                    printError("readPage","Error moving bitstream to end of packet");
                                }
                            }
                            else
                            {
                                printError("readPage","Failed to mark new sequence in bitstream");
                                res = false;
                            }
                        }
                        else
                        {
                            printError("readPage","Failed to move bitstream to start of packet");
                            res = false;
                        }
                    }
                    total += offset << 3;
                    m_sequenceNo = seqNo;
                }
                else
                {
                    printError("readPage","Error placing bookmark at page body start location");
                }
            }
            else
            {
                printError("readPage","Failed to res in Ogg page header");
            }
        }
        else
        {
            printError("readPage","Error locating next page in stream");
        }
    }
    else
    {
        printError("readPage","Page header and bitstream sequence required");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::checkCodecHeader(engine::Sequence *seq)
{
    tint i;
    bool res = false;
    tchar tmp[6];

    if(seq!=0)
    {
        i = seq->readBits(8);
        for(i=0;i<6;++i)
        {
            tmp[i] = static_cast<tchar>(seq->readBits(8));
        }
        if(::memcmp("vorbis",tmp,6)==0)
        {
            res = true;
        }
        else
        {
            printError("checkCodecHeader","Packet is not a codec header packet");
        }
    }
    else
    {
        printError("checkCodecHeader","No sequence instance given");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void VSilverSeeker::clearSequence(common::BOQueueTree<engine::Sequence *>& list)
{
    while(list.Size()>0)
    {
        list.Delete(0);
    }
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::packetBlocksize(engine::Sequence *seq,tint& size)
{
    tint modeNo;
    VSilverModeData *mode;
    bool res = false;

    if(seq!=0)
    {
        if(!seq->readBit())
        {
            modeNo = seq->readBits(m_container->m_data->m_iLog_vorbis_mode_count);
            mode = m_container->m_data->m_modes[modeNo];
            if(!mode->m_blockFlag)
            {
                size = m_container->m_information->m_blockSize_0;
            }
            else
            {
                size = m_container->m_information->m_blockSize_1;
            }
            res = true;
        }
        else
        {
            printError("packetBlocksize","Invalid audio packet");
        }
    }
    else
    {
        printError("packetBlocksize","No packet sequence instance given");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::calcPageBlocksize(OggPageHeader *hdr,common::BOQueueTree<engine::Sequence *>& list,tuint64& blockSize)
{
    tint i,thisBlock,lastBlock = -1;

    blockSize = 0;
    for(i=0;i<list.Size();++i)
    {
        if(packetBlocksize(list.Find(i),thisBlock))
        {
            if(lastBlock!=-1)
            {
                blockSize += (lastBlock + thisBlock) >> 2;
            }
            lastBlock = thisBlock;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

const tuint c_seekBackwardStep = engine::c_bitstreamBufferSize * 8;

bool VSilverSeeker::readLastHeader(OggPageHeader *hdr,tuint& offset)
{
    tint bk,count;
    tuint x;
    engine::Sequence *seq;
    tuint64 blockOffset = 0,dumb = 0;
    bool loop = true,res = false;

    blockOffset = m_bitstream->file()->length();
    blockOffset <<= 3;

    while(loop)
    {
        blockOffset -= c_seekBackwardStep;
        if(m_bitstream->seek(static_cast<tint>(blockOffset >> 3),File::e_startPosition))
        {
            bk = m_bitstream->bookmark();
            if(bk>=0)
            {
                if(m_bitstream->mark(1,bk))
                {
                    seq = m_bitstream->getSequence(1);
                    if(seq!=0)
                    {
                        if(seq->seek(c_seekBackwardStep - 32))
                        {
                            count = c_seekBackwardStep - 32;
                            while(count>=0 && loop)
                            {
                                count += 32;
                                x = seq->readBits(32);
                                if(x==0x5367674F) // 'OggS' bound mark.
                                {
                                    count -= 32;
                                    if(seq->seek(-32))
                                    {
                                        bk = seq->bookmark();
                                        if(bk>=0)
                                        {
                                            if(readHeader(hdr,dumb,1) && hdr->serial==m_serialNo)
                                            {
                                                blockOffset += count;
                                                offset = static_cast<tuint>(blockOffset >> 3);
                                                loop = false;
                                                res = true;
                                            }
                                            else
                                            {
                                                if(seq->move(bk))
                                                {
                                                    count -= 40;
                                                    if(!seq->seek(-40))
                                                    {
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    printError("readLastHeader","Error moving back onto bookmark");
                                                }
                                            }
                                        }
                                        else
                                        {
                                            printError("readLastHeader","Error placing bookmark in sequence");
                                            loop = false;
                                        }
                                    }
                                    else
                                    {
                                        printError("readLastHeader","Failed to rewind to start of header");
                                        loop = false;
                                    }
                                }
                                else
                                {
                                    count -= 40;
                                    if(!seq->seek(-40))
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            printError("readLastHeader","Failed to locate start of search position");
                            loop = false;
                        }
                    }
                    else
                    {
                        printError("readLastHeader","Failed to get search sequence");
                        loop = false;
                    }
                }
                else
                {
                    printError("readLastHeader","Error marking out stream sequence");
                    loop = false;
                }
            }
            else
            {
                printError("readLastHeader","Error placing bookmark at start of search sequence");
                loop = false;
            }
        }
        else
        {
            printError("readLastHeader","Failed to locate seek to required position");
            loop = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::calcLastPCM(tuint& offset,tuint& lastPCM)
{
    OggPageHeader hdr;
    bool res = false;

    if(readLastHeader(&hdr,offset))
    {
        lastPCM = static_cast<tuint>(hdr.position & 0x00000000ffffffff);
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::getSeekData()
{
    tint i,count = 0;
    OggPageHeader hdrA;
    common::BOQueueTree<engine::Sequence *> list;
    tuint64 firstOffset = 0,accumulated = 0;
    bool res = true;

    if(forwardSequenceSearch(firstOffset))
    {
        while(res && count<3)
        {
            if(readPage(&hdrA,list,firstOffset))
            {
                if(m_serialNo==0xffffffff)
                {
                    m_serialNo = hdrA.serial;
                }
                if(m_serialNo==hdrA.serial)
                {
                    for(i=0;res && count<3 && i<list.Size();++i)
                    {
                        if(checkCodecHeader(list.Find(i)))
                        {
                            count++;
                        }
                        else
                        {
                            printError("getSeekData","Codec packet header expected");
                            res = false;
                        }
                    }
                }
            }
            else
            {
                printError("getSeekData","Failed to load logical page");
                res = false;
            }
        }

        if(res)
        {
            if(forwardSequenceSearch(firstOffset))
            {
                m_firstOffset = static_cast<tuint>(firstOffset >> 3);

                if(readPage(&hdrA,list))
                {
                    accumulated = 0;
                    if(calcPageBlocksize(&hdrA,list,accumulated))
                    {
                        if(hdrA.position > accumulated)
                        {
                            accumulated = hdrA.position - accumulated;
                        }
                        else
                        {
                            accumulated = 0;
                        }
                        m_firstPCM = static_cast<tuint>(accumulated);

                        if(calcLastPCM(m_lastOffset,m_lastPCM))
                        {
                            res = true;
                        }
                        else
                        {
                            printError("getSeekData","Failed to get end of stream position data");
                        }
                    }
                    else
                    {
                        printError("getSeekData","Error calculating page blocksize");
                    }
                }
                else
                {
                    printError("getSeekData","Failed to read in first audio page");
                    res = false;
                }
            }
            else
            {
                printError("getSeekData","Failed to find audio ogg stream");
                res = false;
            }
        }
    }
    else
    {
        printError("getSeekData","Failed to find ogg stream");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::init(const tchar *filename)
{
    return init(QString::fromLatin1(filename));
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::init(const QString& filename)
{
    bool res = false;

    if(m_container==0)
    {
        printError("init","No codec container information given");
        return false;
    }

    m_file = new engine::File;
    if(m_file->open(filename))
    {
        m_bitstream = new engine::Bitstream;
        if(m_bitstream->open(m_file))
        {
            if(getSeekData())
            {
                res = true;
            }
            else
            {
                printError("init","Failed to get required seek information");
            }
        }
        else
        {
            printError("init","Failed to open required bitstream");
        }
    }
    else
    {
        printError("init","Failed to open file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp VSilverSeeker::totalTime() const
{
    common::TimeStamp tLength;
    tfloat64 approx;

    approx = static_cast<tfloat64>(m_lastPCM - m_firstPCM) / static_cast<tfloat64>(m_container->m_information->m_audioSampleRate);
    tLength = approx;
    return tLength;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::getPosition(tuint offset,tuint& position,tuint& actualOffset)
{
    OggPageHeader hdr;
    tuint64 current = 0,a = 0,b = 0;
    bool loop = true,res = false;

    if(offset < m_firstOffset)
    {
        offset = m_firstOffset;
    }
    if(offset > m_lastOffset)
    {
        offset = m_lastOffset;
    }

    if(m_bitstream->seek(offset,File::e_startPosition))
    {
        if(m_bitstream->mark(1))
        {
            engine::Sequence *seq = m_bitstream->getSequence(1);

            if(seq!=0)
            {
                current = offset;
                current <<= 3;
                while(loop)
                {
                    if(forwardSequenceSearch(a,1))
                    {
                        current += a;
                        if(readHeader(&hdr,b,1))
                        {
                            if(m_serialNo==hdr.serial)
                            {
                                actualOffset = static_cast<tuint>(current >> 3);
                                position = static_cast<tuint>(hdr.position);
                                loop = false;
                                res = true;
                            }
                            else
                            {
                                b -= 32;
                                if(!seq->seek(-static_cast<tint>(b & 0x00000000ffffffff)))
                                {
                                    loop = false;
                                }
                            }
                        }
                        else
                        {
                            loop = false;
                        }
                    }
                    else
                    {
                        loop = false;
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::seekPosition(tuint position,tuint& offset,tuint& actualOffset)
{
    tuint currentPosition;
    tuint nextOffset,lowerOffset = m_firstOffset,upperOffset = m_lastOffset;
    bool res = true;

    if(lowerOffset < upperOffset)
    {
        while(res && lowerOffset<(upperOffset - 1))
        {
            nextOffset = ((upperOffset - lowerOffset) >> 1) + lowerOffset;

            if(getPosition(nextOffset,currentPosition,offset))
            {
                if(position < currentPosition)
                {
                    upperOffset = nextOffset;
                }
                else
                {
                    lowerOffset = nextOffset;
                }
            }
            else
            {
                res = false;
            }
        }
        actualOffset = currentPosition;
    }
    else
    {
        offset = m_firstOffset;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverSeeker::seek(common::TimeStamp& seekTime,common::TimeStamp& actualTime,tuint& offset)
{
    tuint position,actualOffset;
    tfloat64 a;
    bool res = false;

    a = static_cast<tfloat64>(seekTime);
    a *= static_cast<tfloat64>(m_container->m_information->m_audioSampleRate);
    position = static_cast<tuint>(a);
    if(seekPosition(position,offset,actualOffset))
    {
        a = static_cast<tfloat64>(actualOffset) / static_cast<tfloat64>(m_container->m_information->m_audioSampleRate);
        actualTime = a;
        res = a;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint VSilverSeeker::offset() const
{
    return m_firstOffset;
}

//-------------------------------------------------------------------------------------------

tint VSilverSeeker::bitrate() const
{
    common::TimeStamp total(totalTime());
    return static_cast<tint>((static_cast<tfloat64>(m_lastOffset - m_firstOffset) * 8.0) / static_cast<tfloat64>(total));
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
