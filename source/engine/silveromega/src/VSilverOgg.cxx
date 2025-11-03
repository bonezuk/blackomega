#include "engine/silveromega/inc/VSilverOgg.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER engine::Sequence *BOQueueTree<engine::Sequence *>::nullObject = 0;

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverOgg::VSilverOgg(engine::Bitstream *bs) : m_bitstream(bs),
    m_sequenceNo(0),
    m_packets(),
    m_bookmark(-1),
    m_streamID(-1),
    m_pSeq(0),
    m_bkLast(-1)
{}

//-------------------------------------------------------------------------------------------

VSilverOgg::~VSilverOgg()
{
    try
    {
        clearList();
        m_bitstream = 0;
        m_pSeq = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverOgg::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverOgg::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::sequenceSearch()
{
    tuint x;
    bool res = false;
    engine::Sequence *seq = m_bitstream->getSequence(0);

    if(seq!=0)
    {
        if(m_bookmark>=0)
        {
            if(!seq->move(m_bookmark))
            {
                printError("sequenceSearch","Failed to locate bookmark position");
                return false;
            }
        }

        do
        {
            x = static_cast<tuint>(seq->readBits(32));
            if(x==0x5367674F) // 'OggS' boundary mark
            {
                res = true;
            }
            else if(seq->isEnd())
            {
                return false;
            }
            else
            {
                if(!seq->seek(-24))
                {
                    printError("sequenceSearch","Error rewinding sequence");
                    return false;
                }
            }
        } while(!res);
    }
    else
    {
        printError("sequenceSearch","Failed to get root sequence");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// Move sequence to start position of where the next Ogg page given the bookmark position
// for the current page
//-------------------------------------------------------------------------------------------

bool VSilverOgg::nextPage(engine::Sequence *seq,tint bkStart) const
{
    tint i,size,count;
    bool res = false;

    if(seq->move(bkStart))
    {
        if(seq->seek(26 * 8))
        {
            size = seq->readBits(8);
            for(i=0,count=0;i<size;++i)
            {
                count += seq->readBits(8);
            }
            count += size;
            count = static_cast<tint>(static_cast<tuint>(count) << 3);

            if(seq->seek(count))
            {
                if(!seq->isEnd())
                {
                    if(m_bookmark>=0)
                    {
                        res = true;
                    }
                    else
                    {
                        printError("nextPage","No bookmark defined");
                    }
                }
            }
            else
            {
                printError("nextPage","Failed to seek to start of next page");
            }
        }
        else
        {
            printError("nextPage","Failed to seek to page segment data for page");
        }
    }
    else
    {
        printError("nextPage","Failed to move to start position of current page");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::generatePackets(engine::Sequence *seq,tint bkStart,const OHeader *hdr,bool prevFlag)
{
    tint i,index,count,bkBodyStart;
    bool res = true;

    if(seq->move(bkStart))
    {
        if(!seq->seek(static_cast<tint>(static_cast<tuint>(hdr->headerLength) << 3)))
        {
            return false;
        }
        bkBodyStart = seq->bookmark();
        if(bkBodyStart>=0)
        {
            for(i=0,index=0,count=0;res && i<hdr->noPackets;++i)
            {
                count += hdr->packets[i];

                if(hdr->packets[i]<255)
                {
                    if(buildPacketSequence(bkBodyStart,index,count,prevFlag))
                    {
                        tint pos = m_packets.Size();

                        if(!m_packets.Add(pos,m_pSeq))
                        {
                            printError("generatePackets","Error adding sequence to packet list");
                            res = false;
                        }
                        m_pSeq = 0;
                    }
                    prevFlag = false;
                    index += count;
                    count = 0;
                }
            }
            if(res && count>0)
            {
                if(!buildPacketSequence(bkBodyStart,index,count,prevFlag))
                {
                    printError("generatePackets","Failed to insert final packet in page");
                    res = false;
                }
            }
        }
        else
        {
            printError("generatePackets","Error placing bookmark");
            res = false;
        }
    }
    else
    {
        printError("generatePackets","Failed to move to start position of current page");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::buildPacketSequence(tint bkFrom,tint startIndex,tint length,bool prevFlag)
{
    tint seqNo;
    bool res = false;

    if(m_pSeq!=0)
    {
        seqNo = m_sequenceNo;
    }
    else
    {
        seqNo = ++m_sequenceNo;
    }

    if(m_bitstream->move(bkFrom,static_cast<tint>(static_cast<tuint>(startIndex) << 3)))
    {
        if(m_bitstream->mark(seqNo))
        {
            if(m_bitstream->move(-1,static_cast<tint>(static_cast<tuint>(length) << 3)))
            {
                if(m_bitstream->mark(0))
                {
                    if(m_pSeq==0)
                    {
                        engine::Sequence *seq = m_bitstream->getSequence(seqNo);

                        if(seq!=0)
                        {
                            m_pSeq = seq;
                            res = true;
                        }
                        else
                        {
                            printError("buildPacketSequence","Failed to get new sequence instance");
                        }
                    }
                    else
                    {
                        res = true;
                    }
                }
                else
                {
                    printError("buildPacketSequence","Error marking end of packet position");
                }
            }
            else
            {
                printError("buildPacketSequence","Failed to move stream to end of packet position");
            }
        }
        else
        {
            printError("buildPacketSequence","Error marking start of packet position");
        }
    }
    else
    {
        printError("buildPacketSequence","Failed to move stream to start of packet position");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::readHeader(engine::Sequence *seq,tint bkStart,OHeader *hdr)
{
    tint i;
    bool res = false;

    if(seq->move(bkStart))
    {
        if(seq->seek(26 * 8))
        {
            hdr->noPackets = seq->readBits(8);
            hdr->headerLength = 27 + hdr->noPackets;
            hdr->bodyLength = 0;
            for(i=0;i<hdr->noPackets;++i)
            {
                hdr->packets[i] = seq->readBits(8);
                hdr->bodyLength += hdr->packets[i];
            }

            if(seq->seek((static_cast<tint>(static_cast<tuint>(hdr->bodyLength) << 3)) - 8))
            {
                seq->readBits(8);
                if(!seq->isEnd())
                {
                    m_bookmark = seq->bookmark();
                    if(m_bookmark>=0)
                    {
                        res = true;
                    }
                }
                else
                {
                    printError("readHeader","Page ending is unexpected");
                }
            }
            else
            {
                printError("readHeader","Failed to seek to end of page");
            }
        }
        else
        {
            printError("readHeader","Failed to goto packet segment of page header");
        }
    }
    else
    {
        printError("readHeader","Failed to find start of page");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::checksumPage(engine::Sequence *seq,tint bkStart,const OHeader *hdr) const
{
    static const tuint crc_lookup[256] = {
        0x00000000,0x04c11db7,0x09823b6e,0x0d4326d9,0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
        0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
        0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
        0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
        0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
        0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
        0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
        0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
        0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
        0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,0x018aeb13,0x054bf6a4,0x0808d07d,0x0cc9cdca,
        0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
        0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
        0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
        0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
        0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
        0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
        0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
        0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
        0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
        0x0315d626,0x07d4cb91,0x0a97ed48,0x0e56f0ff,0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
        0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
        0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
        0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
        0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
        0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
        0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
        0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,0x029f3d35,0x065e2082,0x0b1d065b,0x0fdc1bec,
        0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
        0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
        0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
        0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
        0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
    };

    tuint crc = 0;
    bool res = false;

    if(seq->move(bkStart))
    {
        tint i;
        tint j,len;
        tchar chksum[4],*mem;

        len = hdr->headerLength + hdr->bodyLength;
        mem = new tchar [static_cast<tuint>(len)];
        if(seq->copy(mem,len)==len)
        {
            for(i=0,j=22;i<4;i++,j++)
            {
                chksum[i] = mem[j];
                mem[j] = 0;
            }

            for(i=0;i<len;i++)
            {
                crc = (crc << 8) ^ crc_lookup[((crc >> 24) & 0x000000ff) ^ static_cast<tuint>(static_cast<tubyte>(mem[i]))];
            }

            mem[22] = static_cast<tbyte>(static_cast<tubyte>(crc & 0x000000ff));
            mem[23] = static_cast<tbyte>(static_cast<tubyte>((crc >> 8) & 0x000000ff));
            mem[24] = static_cast<tbyte>(static_cast<tubyte>((crc >> 16) & 0x000000ff));
            mem[25] = static_cast<tbyte>(static_cast<tubyte>((crc >> 24) & 0x000000ff));

            if(::memcmp(chksum,&mem[22],4)==0)
            {
                res = true;
            }
        }
        else
        {
            printError("checksumPage","Failed to copy page into memory buffer");
        }
        delete [] mem;
    }
    else
    {
        printError("checksumPage","Failed to fin start of page");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::doPage()
{
    tint bkStart, count = 0;
    engine::Sequence *seq;
    bool prevFlag;

    while(sequenceSearch() && count < 10)
    {
        seq = m_bitstream->getSequence(0);
        if(seq!=0 && seq->seek(8))
        {
            // Read in the header flag byte and get previous packet information.
            if(seq->readBits(8) & 0x1)
            {
                prevFlag = true;
            }
            else
            {
                prevFlag = false;
            }

            if(seq->seek(-48))
            {
                // bookmark start position of page.
                bkStart = seq->bookmark();
                // move sequence to stream number data.
                if(bkStart!=-1 && seq->seek(112))
                {
                    // read in page ID and ensure that it matches original logical stream.
                    if(m_streamID==-1 || m_streamID==seq->readBits(32))
                    {
                        OHeader hdr;

                        if(m_streamID==-1)
                        {
                            m_streamID = seq->readBits(32);
                        }

                        if(readHeader(seq,bkStart,&hdr))
                        {
                            if(checksumPage(seq,bkStart,&hdr))
                            {
                                if(generatePackets(seq,bkStart,&hdr,prevFlag))
                                {
                                    return true;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                m_pSeq = 0;
                                if(!nextPage(seq,bkStart))
                                {
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if(nextPage(seq,bkStart))
                        {
                            count++;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    printError("doPage","Failed to place start bookmark");
                }
            }
            else
            {
                printError("doPage","Failed to move to beginning of page");
            }
        }
        else
        {
            printError("doPage","Failed to get root sequence");
            return false;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

engine::Sequence *VSilverOgg::next()
{
    engine::Sequence *seq;

    // Remove redundant data.
    if(m_bkLast>=0)
    {
        m_bitstream->complete(m_bkLast);
        m_bkLast = -1;
    }

    while(m_packets.Size()<1)
    {
        if(!doPage())
        {
            return 0;
        }
    }

    seq = m_packets.Find(0);
    if(!m_packets.Delete(0))
    {
        printError("next","Error popping packet sequence from list");
        return 0;
    }
    if(seq!=0)
    {
        m_bkLast = seq->bookmark();
    }
    return seq;
}

//-------------------------------------------------------------------------------------------

void VSilverOgg::clearList()
{
    while(m_packets.Size()>0)
    {
        m_packets.Delete(0);
    }
    m_pSeq = 0;
    m_bkLast = -1;
    m_bookmark = -1;
}

//-------------------------------------------------------------------------------------------

bool VSilverOgg::seek(tint offset)
{
    bool res = false;

    if(offset>0)
    {
        clearList();
        if(m_bitstream->seek(offset,File::e_startPosition))
        {
            res = true;
        }
        else
        {
            printError("seek","Failed to set file to given offset");
        }
    }
    else
    {
        printError("seek","Cannot seek with a negative offset from start");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namesapce engine
} // namespace omega
//-------------------------------------------------------------------------------------------
