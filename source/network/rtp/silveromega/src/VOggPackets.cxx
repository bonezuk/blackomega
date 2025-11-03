#include "network/inc/UDPRead.h"
#include "common/inc/Log.h"
#include "engine/inc/SequenceMemory.h"
#include "network/rtp/silveromega/inc/VOggPackets.h"

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

const tuint c_bufferSize = 1024;

//-------------------------------------------------------------------------------------------

VOggPackets::VOggPackets() : m_file(0),
    m_time(),
    m_timeInc(),
    m_timePacket(),
    m_timePacketEnd(),
    m_streamFile(0),
    m_stream(0),
    m_oggStream(0),
    m_container(0),
    m_seeker(0),
    m_eofFlag(true),
    m_bufferOffset(0),
    m_bufferLength(0),
    m_buffer(0),
    m_serialID(0xffffffff),
    m_packetNo(0),
    m_packetOffsets(),
    m_configInformationMem(),
    m_configCommentsMem(),
    m_configCodecDataMem(),
    m_prevBlockMode(0),
    m_currentBlockMode(0),
    m_centerW(0),
    m_outCurrent(-1),
    m_outReturn(-1)
{}

//-------------------------------------------------------------------------------------------

VOggPackets::~VOggPackets()
{
    try
    {
        VOggPackets::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VOggPackets::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VOggPackets::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::read(tuint amount)
{
    tuint i=0,j,len = m_bufferOffset + amount;

    if(len <= m_bufferLength)
    {
        return true;
    }
    if(m_eofFlag)
    {
        return false;
    }
    if(m_file==0)
    {
        printError("read","No file has been opened to read data from");
        return false;
    }

    if(m_buffer!=0)
    {
        if(amount < m_bufferLength)
        {
            for(j=m_bufferOffset;j<m_bufferLength;++i,++j)
            {
                m_buffer[i] = m_buffer[j];
            }
        }
        else
        {
            tuint remain = m_bufferLength - m_bufferOffset, nLen = m_bufferLength;
            tubyte *nBuffer;

            while(nLen < (amount + remain))
            {
                nLen += c_bufferSize;
            }
            nBuffer = new tubyte [nLen];
            for(j=m_bufferOffset;j<m_bufferLength;++i,++j)
            {
                nBuffer[i] = m_buffer[j];
            }
            m_buffer = nBuffer;
            m_bufferLength = nLen;
        }
    }
    else
    {
        tuint nLen = c_bufferSize;

        while(nLen < amount)
        {
            nLen += c_bufferSize;
        }
        m_buffer = new tubyte [nLen];
        m_bufferLength = nLen;
    }
    m_bufferOffset = 0;

    tint readLen,readAmount;
    bool res = false;

    readAmount = static_cast<tint>(m_bufferLength - i);
    readLen = m_file->read(reinterpret_cast<tchar *>(&m_buffer[i]),readAmount);
    if(readLen>=0)
    {
        if(readAmount!=readLen)
        {
            m_bufferLength = i + readLen;
            m_eofFlag = true;
        }
        res = true;
    }
    else
    {
        printError("read","Error reading data from file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint16 VOggPackets::read16FromMem(tubyte *mem)
{
    return static_cast<tint16>(read16UFromMem(mem));
}


//-------------------------------------------------------------------------------------------

tuint16 VOggPackets::read16UFromMem(tubyte *mem)
{
    tuint16 x;

    x  = static_cast<tuint16>(mem[0]) & 0x00ff;
    x |= (static_cast<tuint16>(mem[1]) << 8) & 0xff00;
    return x;
}

//-------------------------------------------------------------------------------------------

tint32 VOggPackets::read32FromMem(tubyte *mem)
{
    return static_cast<tint32>(read32UFromMem(mem));
}

//-------------------------------------------------------------------------------------------

tuint32 VOggPackets::read32UFromMem(tubyte *mem)
{
    tuint32 x;

    x  = static_cast<tuint32>(mem[0]) & 0x000000ff;
    x |= (static_cast<tuint32>(mem[1]) <<  8) & 0x0000ff00;
    x |= (static_cast<tuint32>(mem[2]) << 16) & 0x00ff0000;
    x |= (static_cast<tuint32>(mem[3]) << 24) & 0xff000000;
    return x;
}

//-------------------------------------------------------------------------------------------

tint64 VOggPackets::read64FromMem(tubyte *mem)
{
    return static_cast<tint64>(read64UFromMem(mem));
}

//-------------------------------------------------------------------------------------------

tuint64 VOggPackets::read64UFromMem(tubyte *mem)
{
    tuint64 x;

    x  = static_cast<tuint64>(mem[0]) & 0x00000000000000ffULL;
    x |= (static_cast<tuint64>(mem[1]) <<  8) & 0x000000000000ff00ULL;
    x |= (static_cast<tuint64>(mem[2]) << 16) & 0x0000000000ff0000ULL;
    x |= (static_cast<tuint64>(mem[3]) << 24) & 0x00000000ff000000ULL;
    x |= (static_cast<tuint64>(mem[4]) << 32) & 0x000000ff00000000ULL;
    x |= (static_cast<tuint64>(mem[5]) << 40) & 0x0000ff0000000000ULL;
    x |= (static_cast<tuint64>(mem[6]) << 48) & 0x00ff000000000000ULL;
    x |= (static_cast<tuint64>(mem[7]) << 56) & 0xff00000000000000ULL;
    return x;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::forwardSequenceSearch()
{
    tuint32 x;
    bool loop = true;

    while(loop)
    {
        if(!read(4))
        {
            loop = false;
        }
        else
        {
            x = read32UFromMem(&m_buffer[m_bufferOffset]);
            if(x == 0x5367674F)
            {
                return true;
            }
            else
            {
                m_bufferOffset++;
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

tint VOggPackets::readHeader(engine::silveromega::OggPageHeader *hdr)
{
    tuint i,t;
    tint res = -1;

    if(read(27))
    {
        if(read32UFromMem(&m_buffer[m_bufferOffset]) == 0x5367674F)
        {
            hdr->version  = static_cast<tint>(m_buffer[m_bufferOffset + 4]);
            if(hdr->version==0)
            {
                tuint hdrLen = 27,bodyLen = 0;

                t = static_cast<tuint>(m_buffer[m_bufferOffset + 5]);
                hdr->continued = (t & 0x1) ? true : false;
                hdr->first = (t & 0x2) ? true : false;
                hdr->last = (t & 0x4) ? true : false;
                hdr->position = read64UFromMem(&m_buffer[m_bufferOffset + 6]);
                hdr->serial = read32UFromMem(&m_buffer[m_bufferOffset + 14]);
                hdr->pageNo = read32UFromMem(&m_buffer[m_bufferOffset + 18]);
                hdr->checksum = read32UFromMem(&m_buffer[m_bufferOffset + 22]);
                hdr->noSegments = static_cast<tuint>(m_buffer[m_bufferOffset + 26]);

                if(read(27 + hdr->noSegments))
                {
                    for(i=0;i<hdr->noSegments;++i)
                    {
                        tubyte v = m_buffer[m_bufferOffset + 27 + i];
                        hdr->segmentTable[i] = v;
                        bodyLen += static_cast<tuint>(v);
                    }
                    hdrLen += hdr->noSegments;

                    if(read(hdrLen + bodyLen))
                    {
                        if(doPageChecksum(&m_buffer[m_bufferOffset],hdrLen + bodyLen))
                        {
                            m_bufferOffset += hdrLen;
                            res = 1;
                        }
                        else
                        {
                            m_bufferOffset += hdrLen + bodyLen;
                            res = 0;
                        }
                    }
                    else
                    {
                        printError("readHeader","Failed to read page body from file");
                    }
                }
            }
        }
        else
        {
            printError("readHeader","Unknown Ogg page version");
        }
    }
    else
    {
        printError("readHeader","Not aligned on page header");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void VOggPackets::getPagePackets(engine::silveromega::OggPageHeader *hdr,QVector<PacketOffset>& packets)
{
    tuint i,start = 0,offset = 0;
    PacketOffset p;

    packets.clear();

    if(hdr->noSegments>0)
    {
        for(i=0;i<hdr->noSegments;++i)
        {
            offset += hdr->segmentTable[i];
            if(hdr->segmentTable[i]!=255)
            {
                p.start = start;
                p.end = offset;
                p.continued = false;
                packets.append(p);
                start = offset;
            }
            else if(i==(hdr->noSegments - 1))
            {
                p.start = start;
                p.end = offset;
                p.continued = true;
                packets.append(p);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::open(const tchar *name)
{
    return open(QString::fromUtf8(name));
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::open(const QString& name)
{
    QString msg;
    bool res = false;

    close();

    m_streamFile = new engine::File;
    if(m_streamFile->open(name))
    {
        m_stream = new engine::Bitstream;
        if(m_stream->open(m_streamFile))
        {
            m_oggStream = new engine::silveromega::VSilverOgg(m_stream);
            m_container = new engine::silveromega::VSilverContainer;

            if(m_container->read(m_oggStream))
            {
                if(m_container->setup())
                {
                    m_seeker = new engine::silveromega::VSilverSeeker(m_container);
                    if(m_seeker->init(name))
                    {
                        if(openStream(name))
                        {
                            res = true;
                        }
                        else
                        {
                            printError("open","Failed to open stream to read packets from");
                        }
                    }
                    else
                    {
                        msg = "Failed to setup seeker service to vorbis file '" + name + "'.";
                        printError("open",msg.toUtf8().constData());
                    }
                }
                else
                {
                    printError("open","Failed to setup ogg stream header contents");
                }
            }
            else
            {
                msg = "Failed to decode vorbis stream header contents for file '" + name + "'.";
                printError("open",msg.toUtf8().constData());
            }
        }
        else
        {
            msg = "Failed to open file bitstream '" + name + "'.";
            printError("open",msg.toUtf8().constData());
        }
    }
    else
    {
        msg = "Failed to open file '" + name + "'.";
        printError("open",msg.toUtf8().constData());
    }

    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void VOggPackets::close()
{
    if(m_streamFile!=0)
    {
        if(m_buffer!=0)
        {
            delete m_buffer;
            m_buffer = 0;
        }
        m_bufferOffset = 0;
        m_bufferLength = 0;
        m_eofFlag = true;

        if(m_file!=0)
        {
            m_file->close();
            delete m_file;
            m_file = 0;
        }

        m_configInformationMem.RemoveAll();
        m_configCommentsMem.RemoveAll();
        m_configCodecDataMem.RemoveAll();

        if(m_seeker!=0)
        {
            delete m_seeker;
            m_seeker = 0;
        }
        if(m_container!=0)
        {
            delete m_container;
            m_container = 0;
        }
        if(m_oggStream!=0)
        {
            delete m_oggStream;
            m_oggStream = 0;
        }
        if(m_stream!=0)
        {
            m_stream->close();
            delete m_stream;
            m_stream = 0;
        }

        m_streamFile->close();
        delete m_streamFile;
        m_streamFile = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::openStream(const QString& name)
{
    QString msg;
    bool res = false;

    m_file = new engine::File;
    if(m_file->open(name))
    {
        tuint done = 0;
        NetArray mem;

        m_eofFlag = false;
        m_serialID = 0xffffffff;
        m_packetNo = 0;
        m_packetOffsets.clear();

        res = true;

        while((done ^ 0x7) && res)
        {
            if(getConfiguration(mem))
            {
                if(mem[0]==0x01)
                {
                    m_configInformationMem.Copy(mem);
                    done |= 1 << static_cast<tint>(e_configInformation);
                }
                else if(mem[0]==0x03)
                {
                    m_configCommentsMem.Copy(mem);
                    done |= 1 << static_cast<tint>(e_configComments);
                }
                else if(mem[0]==0x05)
                {
                    m_configCodecDataMem.Copy(mem);
                    done |= 1 << static_cast<tint>(e_configCodecData);
                }
            }
            else
            {
                printError("openStream","Failed to get configuration information");
                res = false;
            }
        }

        m_time = 0;
        m_timeInc = 0;
        m_timePacketEnd = 0;
        initPCMLength();
    }
    else
    {
        msg = "Failed to open file '" + name + "'.";
        printError("openStream",msg.toUtf8().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::next(NetArray& mem)
{
    common::TimeStamp t;
    return next(mem,t);
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::next(NetArray& mem,common::TimeStamp& t)
{
    bool res = false;

    if(getNextPacket(mem))
    {
        double a,b,c,d;
        tint len = calculatePCMLength(mem);

        m_time += m_timeInc;
        m_timePacket += m_timeInc;
        m_timeInc = static_cast<tfloat64>(len) / static_cast<tfloat64>(m_container->m_information->m_audioSampleRate);

        a = m_time;
        b = m_timeInc;
        c = m_timePacket;
        d = m_timePacketEnd;

        t = m_time;
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::getNextPacket(NetArray& mem)
{
    bool loop = true,res = false;

    mem.RemoveAll();

    while(loop && !res)
    {
        if(m_packetNo>=static_cast<tuint>(m_packetOffsets.size()))
        {
            if(forwardSequenceSearch())
            {
                tint hRes = readHeader(&m_pageHeader);

                if(hRes>0)
                {
                    if(m_serialID==0xffffffff)
                    {
                        m_serialID = m_pageHeader.serial;
                    }
                    if(m_serialID==m_pageHeader.serial)
                    {
                        getPagePackets(&m_pageHeader,m_packetOffsets);
                        m_packetNo = 0;
                    }

                    m_timePacketEnd = static_cast<tfloat64>(m_pageHeader.position) / static_cast<tfloat64>(m_container->m_information->m_audioSampleRate);
                    m_timePacket = 0;
                }
                else if(hRes<0)
                {
                    loop = false;
                }
            }
            else
            {
                loop = false;
            }
        }
        else
        {
            const PacketOffset& p = m_packetOffsets.at(m_packetNo);
            tuint len = p.end - p.start;

            if(read(len))
            {
                mem.AppendRaw(reinterpret_cast<const tchar *>(&m_buffer[m_bufferOffset]),len);
                m_bufferOffset += len;
                m_packetNo++;
                if(!p.continued)
                {
                    res = true;
                }
            }
            else
            {
                loop = false;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::getConfiguration(NetArray& mem)
{
    bool loop = true, res = false;

    while(loop && !res)
    {
        if(getNextPacket(mem))
        {
            if(mem.GetSize()>=7)
            {
                const tubyte *x = mem.GetData();

                if(::memcmp(&x[1],"vorbis",6)==0)
                {
                    res = true;
                }
            }
        }
        else
        {
            loop = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::doPageChecksum(tubyte *mem,tint len)
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

    tint i,j;
    tuint crc = 0;
    tubyte chksum[4];
    bool res = false;

    for(i=0,j=22;i<4;++i,++j)
    {
        chksum[i] = mem[j];
        mem[j] = 0;
    }

    for(i=0;i<len;++i)
    {
        crc = (crc << 8) ^ crc_lookup[((crc >> 24) & 0x000000ff) ^ static_cast<tuint>(mem[i])];
    }

    mem[22] = static_cast<tubyte>(crc & 0x000000ff);
    mem[23] = static_cast<tubyte>((crc >>  8) & 0x000000ff);
    mem[24] = static_cast<tubyte>((crc >> 16) & 0x000000ff);
    mem[25] = static_cast<tubyte>((crc >> 24) & 0x000000ff);

    if(::memcmp(chksum,&mem[22],4)==0)
    {
        res = true;
    }
    else
    {
        printError("doPageChecksum","Page failed CRC checksum");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::configuration(ConfigurationType type,NetArray& mem)
{
    bool res = false;

    switch(type)
    {
        case e_configInformation:
            mem.Copy(m_configInformationMem);
            res = true;
            break;

        case e_configComments:
            mem.Copy(m_configCommentsMem);
            res = true;
            break;

        case e_configCodecData:
            mem.Copy(m_configCodecDataMem);
            res = true;
            break;

        default:
            printError("configuration","Unknown type of configuration");
            break;
    }
    return (res && mem.GetSize()>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

void VOggPackets::initPCMLength()
{
    m_prevBlockMode = 0;
    m_currentBlockMode = 0;
    m_centerW = m_container->m_information->m_blockSize_1;
    m_outCurrent = -1;
    m_outReturn = -1;
}

//-------------------------------------------------------------------------------------------

tint VOggPackets::calculatePCMLength(NetArray& array)
{
    tint mode,n0,n1;
    tint thisCenter,prevCenter;
    engine::SequenceMemory seq(array);

    if(seq.readBit())
    {
        return -1;
    }
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
    return (m_outCurrent - m_outReturn);
}

//-------------------------------------------------------------------------------------------

bool VOggPackets::seek(common::TimeStamp& seekTime,common::TimeStamp& actualTime)
{
    bool res = false;

    if(m_seeker!=0)
    {
        tuint offset = 0;

        if(m_seeker->seek(seekTime,actualTime,offset))
        {
            if(m_file->seek(static_cast<tint>(offset),engine::File::e_startPosition))
            {
                // reset the file buffer.
                if(m_buffer!=0)
                {
                    delete m_buffer;
                    m_buffer = 0;
                }
                m_bufferOffset = 0;
                m_bufferLength = 0;

                // set time to new position
                m_time = actualTime;
                m_timeInc = 0;
                initPCMLength();

                res = true;
            }
            else
            {
                printError("seek","Failed to set file pointer to required seek position");
            }
        }
        else
        {
            printError("seek","Failed to seek to requested time");
        }
    }
    else
    {
        printError("seek","Vorbis seek service not initialized");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp VOggPackets::length() const
{
    return m_seeker->totalTime();
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
