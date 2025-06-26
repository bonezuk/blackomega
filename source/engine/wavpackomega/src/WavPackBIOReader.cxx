#include "engine/wavpackomega/inc/WavPackBIOReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace wavpackomega
{
//-------------------------------------------------------------------------------------------

int32_t WavPackBIOReaderReadBytes(void *id, void *data, int32_t bCount)
{
    int32_t amount = 0;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file != 0)
    {
        amount = file->read(reinterpret_cast<tbyte *>(data), bCount);
        if(amount < 0)
        {
            amount = 0;
        }
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

uint32_t WavPackBIOReaderGetPos(void *id)
{
    uint32_t pos;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file!=0)
    {
        pos = static_cast<uint32_t>(file->offset());
    }
    else
    {
        pos = static_cast<uint32_t>(-1L);
    }
    return pos;
}

//-------------------------------------------------------------------------------------------

int WavPackBIOReaderSetPosAbs(void *id, uint32_t pos)
{
    int res = -1;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file!=0)
    {
        res = (file->seek(static_cast<int>(pos),common::e_Seek_Start)) ? 0 : -1;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int WavPackBIOReaderSetPosRel(void *id, int32_t delta, int mode)
{
    int res = -1;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file!=0)
    {
        common::BIOStreamPosition flag;

        if(mode==SEEK_SET)
        {
            flag = common::e_Seek_Start;
        }
        else if(mode==SEEK_END)
        {
            flag = common::e_Seek_End;
        }
        else
        {
            flag = common::e_Seek_Current;
        }
        if(file->seek(delta,flag))
        {
            res = 0;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int WavPackBIOReaderPushBackByte(void *id,int c)
{
    int res = -1;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file!=0)
    {
        if(file->seek(-1,common::e_Seek_Current))
        {
            res = c;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

uint32_t WavPackBIOReaderGetLength(void *id)
{
    uint32_t size = 0;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file!=0)
    {
        int fSize = file->size();
        size = static_cast<uint32_t>((fSize>=0) ? fSize : 0);
    }
    return size;
}

//-------------------------------------------------------------------------------------------

int WavPackBIOReaderCanSeek(void *id)
{
    return (id!=0);
}

//-------------------------------------------------------------------------------------------

int32_t WavPackBIOReaderWriteBytes(void *id,void *data,int32_t bCount)
{
    int32_t amount = 0;
    common::BIOStream *file = reinterpret_cast<common::BIOStream *>(id);

    if(file!=0 && file->writeable())
    {
        amount = file->write(reinterpret_cast<const tbyte *>(data),bCount);
        if(amount < 0)
        {
            amount = 0;
        }
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

void WavPackBIOReaderSetupReader(WavpackStreamReader *reader)
{
    reader->read_bytes = WavPackBIOReaderReadBytes;
    reader->get_pos = WavPackBIOReaderGetPos;
    reader->set_pos_abs = WavPackBIOReaderSetPosAbs;
    reader->set_pos_rel = WavPackBIOReaderSetPosRel;
    reader->push_back_byte = WavPackBIOReaderPushBackByte;
    reader->get_length = WavPackBIOReaderGetLength;
    reader->can_seek = WavPackBIOReaderCanSeek;
}

//-------------------------------------------------------------------------------------------
} // namespace wavpackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
