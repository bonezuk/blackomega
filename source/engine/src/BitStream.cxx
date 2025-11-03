#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER tint BOTree<tint,engine::Bookmark *>::nullIndex = -1;
COMMON_TEMPLATE_MEMBER engine::Bookmark *BOTree<tint,engine::Bookmark *>::nullObject = 0;

COMMON_TEMPLATE_MEMBER tint BOTree<tint,engine::Sequence *>::nullIndex = -1;
COMMON_TEMPLATE_MEMBER engine::Sequence *BOTree<tint,engine::Sequence *>::nullObject = 0;

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

common::Allocation Bitstream::m_maskAlloc;
tuint *Bitstream::m_mask[33][8];
Bitstream::BSMask *Bitstream::m_maskI[33];
tuint Bitstream::m_w1maskI[32];

//-------------------------------------------------------------------------------------------
// Bookmark
//-------------------------------------------------------------------------------------------

Bookmark::Bookmark() : m_item(0),
    m_offset(0)
{}

//-------------------------------------------------------------------------------------------

void Bookmark::reset()
{
    m_item = 0;
    m_offset = 0;
}

//-------------------------------------------------------------------------------------------
// Associate
//-------------------------------------------------------------------------------------------

Associate::Associate() : m_stream(-1),
    m_prev(0),
    m_next(0),
    m_sequence(0),
    m_bookmarks(),
    m_buffer(0),
    m_offset(0),
    m_length(0)
{}

//-------------------------------------------------------------------------------------------

void Associate::reset()
{
    m_stream = -1;
    m_prev = 0;
    m_next = 0;
    m_sequence = 0;
    while(m_bookmarks.Start())
    {
        m_bookmarks.Remove(m_bookmarks.CurrentIndex());
    }
    m_buffer = 0;
    m_offset = 0;
    m_length = 0;
}

//-------------------------------------------------------------------------------------------
// Bitstream
//-------------------------------------------------------------------------------------------

Bitstream::Bitstream(Mode m) : m_mode(m),
    m_file(0),
    m_sequences(),
    m_bookmarks(),
    m_firstItem(0),
    m_lastItem(0),
    m_recycleAssociate(),
    m_recycleBookmark(),
    m_recycleSequence(Bitstream::getNewSequence,this),
    m_recycleBuffer(0),
    m_currentItem(0),
    m_bitOffset(0),
    m_fileEnd(false),
    m_nextBookID(0)
{}

//-------------------------------------------------------------------------------------------

Bitstream::~Bitstream()
{
    try
    {
        tubyte *buf = 0,*buffer;
        Associate *item,*nItem;
        
        close();
        
        for(item=m_firstItem;item!=0;item=nItem)
        {
            nItem = item->m_next;
            if(buf!=item->m_buffer)
            {
                returnFileBuffer(item->m_buffer);
                buf = item->m_buffer;
            }
            while(item->m_bookmarks.Start())
            {
                item->m_bookmarks.Remove(item->m_bookmarks.CurrentIndex());
            }
            m_recycleAssociate.put(item);
        }
        m_firstItem = m_lastItem = m_currentItem = 0;
        
        while(m_recycleBuffer!=0)
        {
            buffer = getFileBuffer();
            delete [] buffer;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Bitstream::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Bitstream::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void Bitstream::start()
{
    static const tuint mask[32][9] = {
        { 0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff }, //  0
        { 0x00000000, 0x00000002, 0x00000006, 0x0000000e, 0x0000001e, 0x0000003e, 0x0000007e, 0x000000fe, 0x000001fe }, //  1
        { 0x00000000, 0x00000004, 0x0000000c, 0x0000001c, 0x0000003c, 0x0000007c, 0x000000fc, 0x000001fc, 0x000003fc }, //  2
        { 0x00000000, 0x00000008, 0x00000018, 0x00000038, 0x00000078, 0x000000f8, 0x000001f8, 0x000003f8, 0x000007f8 }, //  3
        { 0x00000000, 0x00000010, 0x00000030, 0x00000070, 0x000000f0, 0x000001f0, 0x000003f0, 0x000007f0, 0x00000ff0 }, //  4
        { 0x00000000, 0x00000020, 0x00000060, 0x000000e0, 0x000001e0, 0x000003e0, 0x000007e0, 0x00000fe0, 0x00001fe0 }, //  5
        { 0x00000000, 0x00000040, 0x000000c0, 0x000001c0, 0x000003c0, 0x000007c0, 0x00000fc0, 0x00001fc0, 0x00003fc0 }, //  6
        { 0x00000000, 0x00000080, 0x00000180, 0x00000380, 0x00000780, 0x00000f80, 0x00001f80, 0x00003f80, 0x00007f80 }, //  7
        { 0x00000000, 0x00000100, 0x00000300, 0x00000700, 0x00000f00, 0x00001f00, 0x00003f00, 0x00007f00, 0x0000ff00 }, //  8
        { 0x00000000, 0x00000200, 0x00000600, 0x00000e00, 0x00001e00, 0x00003e00, 0x00007e00, 0x0000fe00, 0x0001fe00 }, //  9
        { 0x00000000, 0x00000400, 0x00000c00, 0x00001c00, 0x00003c00, 0x00007c00, 0x0000fc00, 0x0001fc00, 0x0003fc00 }, // 10
        { 0x00000000, 0x00000800, 0x00001800, 0x00003800, 0x00007800, 0x0000f800, 0x0001f800, 0x0003f800, 0x0007f800 }, // 11
        { 0x00000000, 0x00001000, 0x00003000, 0x00007000, 0x0000f000, 0x0001f000, 0x0003f000, 0x0007f000, 0x000ff000 }, // 12
        { 0x00000000, 0x00002000, 0x00006000, 0x0000e000, 0x0001e000, 0x0003e000, 0x0007e000, 0x000fe000, 0x001fe000 }, // 13
        { 0x00000000, 0x00004000, 0x0000c000, 0x0001c000, 0x0003c000, 0x0007c000, 0x000fc000, 0x001fc000, 0x003fc000 }, // 14
        { 0x00000000, 0x00008000, 0x00018000, 0x00038000, 0x00078000, 0x000f8000, 0x001f8000, 0x003f8000, 0x007f8000 }, // 15
        { 0x00000000, 0x00010000, 0x00030000, 0x00070000, 0x000f0000, 0x001f0000, 0x003f0000, 0x007f0000, 0x00ff0000 }, // 16
        { 0x00000000, 0x00020000, 0x00060000, 0x000e0000, 0x001e0000, 0x003e0000, 0x007e0000, 0x00fe0000, 0x01fe0000 }, // 17
        { 0x00000000, 0x00040000, 0x000c0000, 0x001c0000, 0x003c0000, 0x007c0000, 0x00fc0000, 0x01fc0000, 0x03fc0000 }, // 18
        { 0x00000000, 0x00080000, 0x00180000, 0x00380000, 0x00780000, 0x00f80000, 0x01f80000, 0x03f80000, 0x07f80000 }, // 19
        { 0x00000000, 0x00100000, 0x00300000, 0x00700000, 0x00f00000, 0x01f00000, 0x03f00000, 0x07f00000, 0x0ff00000 }, // 20
        { 0x00000000, 0x00200000, 0x00600000, 0x00e00000, 0x01e00000, 0x03e00000, 0x07e00000, 0x0fe00000, 0x1fe00000 }, // 21
        { 0x00000000, 0x00400000, 0x00c00000, 0x01c00000, 0x03c00000, 0x07c00000, 0x0fc00000, 0x1fc00000, 0x3fc00000 }, // 22
        { 0x00000000, 0x00800000, 0x01800000, 0x03800000, 0x07800000, 0x0f800000, 0x1f800000, 0x3f800000, 0x7f800000 }, // 23
        { 0x00000000, 0x01000000, 0x03000000, 0x07000000, 0x0f000000, 0x1f000000, 0x3f000000, 0x7f000000, 0xff000000 }, // 24
        { 0x00000000, 0x02000000, 0x06000000, 0x0e000000, 0x1e000000, 0x3e000000, 0x7e000000, 0xfe000000, 0xfe000000 }, // 25
        { 0x00000000, 0x04000000, 0x0c000000, 0x1c000000, 0x3c000000, 0x7c000000, 0xfc000000, 0xfc000000, 0xfc000000 }, // 26
        { 0x00000000, 0x08000000, 0x18000000, 0x38000000, 0x78000000, 0xf8000000, 0xf8000000, 0xf8000000, 0xf8000000 }, // 27
        { 0x00000000, 0x10000000, 0x30000000, 0x70000000, 0xf0000000, 0xf0000000, 0xf0000000, 0xf0000000, 0xf0000000 }, // 28
        { 0x00000000, 0x20000000, 0x60000000, 0xe0000000, 0xe0000000, 0xe0000000, 0xe0000000, 0xe0000000, 0xe0000000 }, // 29
        { 0x00000000, 0x40000000, 0xc0000000, 0xc0000000, 0xc0000000, 0xc0000000, 0xc0000000, 0xc0000000, 0xc0000000 }, // 30
        { 0x00000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000 }  // 31
    };

    tuint i,j,k,l,n,shift,*bMask;
    
    stop();
    
    for(i=0;i<33;++i)
    {
        for(j=0;j<8;++j)
        {
            m_mask[i][j] = bMask = reinterpret_cast<tuint *>(m_maskAlloc.MemAlloc(15,sizeof(tuint)));
            
            n = i;
            k = ((8 - j) < i) ? (8 - j) : i;
            bMask[0] = j;
            bMask[1] = mask[0][k];
            bMask[2] = k;
            bMask += 3;
            l = 3;
            n -= k;
            shift = k;
            
            while(n>0)
            {
                k = (8 < n) ? 8 : n;
                bMask[0] = shift;
                bMask[1] = mask[shift][k];
                bMask[2] = k;
                bMask += 3;
                shift += 8;
                n -= k;
                l += 3;
            }
            
            while(l<15)
            {
                bMask[0] = 0;
                bMask[1] = 0;
                bMask[2] = 0;
                bMask += 3;
                l += 3;
            }
        }
        
        m_maskI[i] = reinterpret_cast<BSMask *>(m_maskAlloc.MemAlloc(32,sizeof(BSMask)));
        for(j=0;j<32;++j)
        {
            BSMask *ms = &(m_maskI[i][j]);
            k = i + j;
            if(k <= 32)
            {
                ms->rShiftA = 32 - k;
                ms->rMaskA  = (i==32) ? 0xffffffff : static_cast<unsigned int>((1 << i) - 1);
                ms->rShiftB = 0;
                ms->rMaskB  = 0;
                ms->wShiftA = 32 - k;
                ms->wMask1A = (i==32) ? 0xffffffff : static_cast<unsigned int>(((1 << i) - 1) << ms->rShiftA);
                ms->wMask2A = 0xffffffff ^ ms->wMask1A;
                ms->wShiftB = 0;
                ms->wMask1B = 0;
                ms->wMask2B = 0;            
            }
            else
            {
                ms->rShiftA = k - 32;
                ms->rMaskA  = static_cast<tuint>((1 << (32 - j)) - 1) << ms->rShiftA;
                ms->rShiftB = 32 - ms->rShiftA;
                ms->rMaskB  = static_cast<unsigned int>((1 << ms->rShiftA) - 1);
                ms->wShiftA = k - 32;
                ms->wMask1A = static_cast<unsigned int>((1 << (i - ms->wShiftA)) - 1);
                ms->wMask2A = 0xffffffff ^ ms->wMask1A;
                ms->wShiftB = 32 - ms->wShiftA;
                ms->wMask1B = static_cast<unsigned int>(((1 << ms->wShiftA) - 1) << ms->wShiftB);
                ms->wMask2B = 0xffffffff ^ ms->wMask1B;
            }
            ms->width = (k>=32) ? 2 : 1;
            ms->required = static_cast<tint>(i);
            ms->newOffset = k & 0x0000001f;
        }
    }
    
    for(i=0;i<32;++i)
    {
        m_w1maskI[i] = 0xffffffff ^ (1 << (31 - i));
    }
}

//-------------------------------------------------------------------------------------------

void Bitstream::stop()
{
    tint i,j;
    
    m_maskAlloc.FreeAll();
    for(i=0;i<33;++i)
    {
        for(j=0;j<8;++j)
        {
            m_mask[i][j] = 0;
        }
    }
}


//-------------------------------------------------------------------------------------------

bool Bitstream::open(File *file)
{
    close();
    
    if(file==0)
    {
        printError("open","No file object given to read from");
        return false;
    }
    m_file = file;
    return true;
}

//-------------------------------------------------------------------------------------------

void Bitstream::close()
{
    reset();
    
    if(m_file!=0)
    {
        m_file->close();
    }
}

//-------------------------------------------------------------------------------------------

void Bitstream::reset()
{
    tubyte *buf = 0;
    Sequence *seq;
    Associate *item = m_firstItem,*nItem;
    Bookmark *bk;
    
    while(item!=0)
    {
        nItem = item->m_next;
        if(buf!=item->m_buffer)
        {
            returnFileBuffer(item->m_buffer);
            buf = item->m_buffer;
        }
        while(item->m_bookmarks.Start())
        {
            item->m_bookmarks.Remove(item->m_bookmarks.CurrentIndex());
        }
        m_recycleAssociate.put(item);
        item = nItem;
    }
    
    while(m_sequences.Start())
    {
        seq = m_sequences.Current();
        m_sequences.Remove(m_sequences.CurrentIndex());
        m_recycleSequence.put(seq);
    }
    
    while(m_bookmarks.Start())
    {
        bk = m_bookmarks.Current();
        m_bookmarks.Remove(m_bookmarks.CurrentIndex());
        m_recycleBookmark.put(bk);
    }
    
    m_firstItem = m_lastItem = m_recycleAssociate.get();
    m_firstItem->m_length = 0xffffffff;
    m_firstItem->m_stream = 0;
    
    m_currentItem = m_firstItem;
    m_bitOffset = 0;
}

//-------------------------------------------------------------------------------------------

tubyte *Bitstream::getFileBuffer()
{
    tubyte *buffer;
    
    if(m_recycleBuffer!=0)
    {
        buffer = m_recycleBuffer;
        m_recycleBuffer = *(reinterpret_cast<tubyte **>(&buffer[c_bitstreamBufferSize]));
    }
    else
    {
        buffer = new tubyte [c_bitstreamBufferSize + sizeof(tubyte *)];
    }
    return buffer;
}

//-------------------------------------------------------------------------------------------

void Bitstream::returnFileBuffer(tubyte *buffer)
{
    if(buffer!=0)
    {
        tubyte **pBuffer = reinterpret_cast<tubyte **>(&buffer[c_bitstreamBufferSize]);
        pBuffer[0] = m_recycleBuffer;
        m_recycleBuffer = buffer;
    }
}

//-------------------------------------------------------------------------------------------

void Bitstream::associateWithSequence(Associate *item,Sequence *seq)
{
    item->m_sequence = seq;
    seq->m_bitstream = this;
    seq->m_item = item;
    seq->m_buffer = item->m_buffer;
    seq->m_length = item->m_length;
    seq->m_bitOffset = item->m_offset;
    if(seq->m_buffer!=0)
    {
        seq->m_end = false;
    }
    else
    {
        seq->m_end = true;
    }
}

//-------------------------------------------------------------------------------------------

bool Bitstream::readInNextBuffer()
{
    tint cID,nID,len;
    Associate *item,*pItem;
    Bookmark *bk;
    tubyte *buffer;
    tuint offset;
    
    if(m_lastItem==0)
    {
        printError("readInNextBuffer","Bitstream has not been properly initialized");
        return false;
    }
    
    if(m_fileEnd)
    {
        return false;
    }
    
    // Get buffer and read file buffer contents into it.
    if(m_file==0)
    {
        printError("readInNextBuffer","No file open to read buffer from");
        return false;
    }
    buffer = getFileBuffer();
    len = m_file->read(reinterpret_cast<tbyte *>(buffer),c_bitstreamBufferSize);
    if(len<=0)
    {
        if(len==-1)
        {
            printError("readInNextBuffer","Failed to read data from file");
        }
        else
        {
            m_fileEnd = true;
        }
        returnFileBuffer(buffer);
        return false;
    }
    else if(len < c_bitstreamBufferSize)
    {
        m_fileEnd = true;
    }
    
    // Get associate instance and set it to see buffer and continuation of end stream.
    item = m_recycleAssociate.get();
    item->m_buffer = buffer;
    item->m_offset = 0;
    item->m_length = static_cast<tuint>(len) << 3;
    item->m_stream = m_lastItem->m_stream;
    
    if(m_mode==e_streamMode2 && isLittleEndian())
    {
        tint i = 0,t = len >> 2;
        tuint32 x,*m = reinterpret_cast<tuint32 *>(buffer);
        
        while(i < t)
        {
            x = m[i];
            m[i] = ((x << 24) & 0xff000000 ) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
            i++;
        }
        
        t = len & 0x00000003;
        if(t)
        {
            x = m[i];
            switch(t)
            {
                case 1:
                    x &= 0x000000ff;
                    break;
                
                case 2:
                    x &= 0x0000ffff;
                    break;
                
                case 3:
                    x &= 0x00ffffff;
                    break;

                default:
                    break;
            }
            m[i] = ((x << 24) & 0xff000000 ) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
        }
    }
    
    // Move any existing bookmarks that falls into the area of the newly loaded buffer.
    if(m_lastItem->m_bookmarks.Start())
    {
        while(!m_lastItem->m_bookmarks.IsEnd())
        {
            bk = m_lastItem->m_bookmarks.Current();
            if(bk->m_offset < item->m_length)
            {
                cID = m_lastItem->m_bookmarks.NextIndex();
                nID = m_lastItem->m_bookmarks.CurrentIndex();
                item->m_bookmarks[cID] = bk;
                bk->m_item = item;
                m_lastItem->m_bookmarks.Remove(cID);
                if(nID>=0)
                {
                    m_lastItem->m_bookmarks.Start(nID);
                }
                else
                {
                    break;
                }
            }
            else
            {
                bk->m_offset -= item->m_length;
                m_lastItem->m_bookmarks.Next();
            }
        }
    }
    
    // If a sequence is associated with the last item, adjust it accordingly.
    if(m_lastItem->m_sequence!=0)
    {
        Sequence *seq = m_lastItem->m_sequence;
        
        if(seq->m_bitOffset < item->m_length)
        {
            offset = seq->m_bitOffset;
            associateWithSequence(item,seq);
            seq->m_bitOffset = offset;
            m_lastItem->m_sequence = 0;
        }
        else
        {
            seq->m_bitOffset -= item->m_length;
        }
    }
    
    // Check to see if the bitstream file pointer lies in current region.
    if(m_currentItem==m_lastItem)
    {
        if(m_bitOffset < item->m_length)
        {
            m_currentItem = item;
        }
        else
        {
            m_bitOffset -= item->m_length;
        }
    }
    
    // Place the new begin the last (infinite) item in the bitstream list.
    pItem = m_lastItem->m_prev;
    if(pItem==0)
    {
        m_firstItem = item;
    }
    else
    {
        pItem->m_next = item;
    }
    m_lastItem->m_prev = item;
    item->m_prev = pItem;
    item->m_next = m_lastItem;
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool Bitstream::move(tint bkID)
{
    if(bkID==-1)
    {
        printError("move","Trying to move on undefined bookmark");
        return false;
    }
    return move(bkID,0,false);
}

//-------------------------------------------------------------------------------------------

bool Bitstream::move(tint bkID,tint offset,bool toStream)
{
    tint streamID;
    
    if(bkID==-1)
    {
        if(m_currentItem==0)
        {
            printError("move","No item is defined to move from");
            return false;
        }
    }
    else
    {
        Bookmark *book;
        
        if(!m_bookmarks.Exist(bkID))
        {
            printError("move","Cannot find specified bookmark");
            return false;
        }
        book = m_bookmarks.Find(bkID);
        m_currentItem = book->m_item;
        m_bitOffset = book->m_offset;
    }
    
    streamID = m_currentItem->m_stream;
    
    if(offset>0)
    {
        while(offset>0)
        {
            if(m_currentItem==m_lastItem)
            {
                m_bitOffset += static_cast<tuint>(offset);
                offset = 0;
            }
            else
            {
                if((m_bitOffset + static_cast<tuint>(offset)) < m_currentItem->m_length)
                {
                    m_bitOffset += static_cast<tuint>(offset);
                    offset = 0;
                }
                else
                {
                    offset -= static_cast<tint>(m_currentItem->m_length - m_bitOffset);
                    
                    if(toStream)
                    {
                        do
                        {
                            m_currentItem = m_currentItem->m_next;
                        }
                        while(m_currentItem!=0 && m_currentItem->m_stream!=streamID);
                    }
                    else
                    {
                        m_currentItem = m_currentItem->m_next;
                    }
                    if(m_currentItem==0)
                    {
                        printError("move","Failed to move to next position");
                        return false;
                    }
                    m_bitOffset = m_currentItem->m_offset;
                }
            }
        }
    }
    else
    {
        offset = -offset;
        
        while(offset>0)
        {
            if(static_cast<tint>(m_bitOffset - static_cast<tuint>(offset)) >= static_cast<tint>(m_currentItem->m_offset))
            {
                m_bitOffset -= static_cast<tuint>(offset);
                offset = 0;
            }
            else
            {
                offset -= static_cast<tint>(m_bitOffset - m_currentItem->m_offset);
                if(toStream)
                {
                    do
                    {
                        m_currentItem = m_currentItem->m_prev;
                    }
                    while(m_currentItem!=0 && m_currentItem->m_stream!=streamID);
                }
                else
                {
                    m_currentItem = m_currentItem->m_prev;
                }
                if(m_currentItem==0)
                {
                    printError("move","Failed to move to previous position");
                    return false;
                }
                m_bitOffset = m_currentItem->m_length;
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool Bitstream::moveTo(tint bkID,tint offset,const QSet<tint>& exSeqSet)
{
    QSet<tint>::const_iterator ppI;
    
    if(bkID==-1)
    {
        if(m_currentItem==0)
        {
            printError("moveTo","No item is defined to move from");
            return false;
        }
    }
    else
    {
        Bookmark *book;
        
        if(!m_bookmarks.Exist(bkID))
        {
            printError("moveTo","Cannot find specified bookmark");
            return false;
        }
        book = m_bookmarks.Find(bkID);
        m_currentItem = book->m_item;
        m_bitOffset = book->m_offset;
    }
    
    if(offset>0)
    {
        while(offset>0)
        {
            if(m_currentItem==m_lastItem)
            {
                m_bitOffset += static_cast<tuint>(offset);
                offset = 0;
            }
            else
            {
                if((m_bitOffset + static_cast<tuint>(offset)) < m_currentItem->m_length)
                {
                    m_bitOffset += static_cast<tuint>(offset);
                    offset = 0;
                }
                else
                {
                    offset -= static_cast<tint>(m_currentItem->m_length - m_bitOffset);
                    
                    do 
                    {
                        m_currentItem = m_currentItem->m_next;
                        if(m_currentItem!=0)
                        {
                            ppI = exSeqSet.find(m_currentItem->m_stream);
                        }
                    } while(m_currentItem!=0 && ppI!=exSeqSet.end());

                    if(m_currentItem==0)
                    {
                        printError("moveTo","Failed to move to next position");
                        return false;
                    }
                    m_bitOffset = m_currentItem->m_offset;
                }
            }
        }
    }
    else
    {
        offset = -offset;
        
        while(offset>0)
        {
            if(static_cast<tint>(m_bitOffset - static_cast<tuint>(offset)) >= static_cast<tint>(m_currentItem->m_offset))
            {
                m_bitOffset -= static_cast<tuint>(offset);
                offset = 0;
            }
            else
            {
                offset -= static_cast<tint>(m_bitOffset - m_currentItem->m_offset);

                do 
                {
                    m_currentItem = m_currentItem->m_prev;
                    if(m_currentItem!=0)
                    {
                        ppI = exSeqSet.find(m_currentItem->m_stream);
                    }
                } while(m_currentItem!=0 && ppI!=exSeqSet.end());

                if(m_currentItem==0)
                {
                    printError("moveTo","Failed to move to previous position");
                    return false;
                }
                m_bitOffset = m_currentItem->m_length;
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

tint Bitstream::newBookmark(Associate *item,tint offset)
{
    tint id;
    Bookmark *book;
    
    if(item==0)
    {
        printError("newBookmark","No item given to associate bookmark with");
        return -1;
    }
    if(item->m_offset<=static_cast<tuint>(offset) && static_cast<tuint>(offset)<item->m_length)
    {
        id = m_nextBookID++;
        book = m_recycleBookmark.get();
        m_bookmarks[id] = book;
        book->m_item = item;
        book->m_offset = static_cast<tuint>(offset);
        item->m_bookmarks[id] = book;
    }
    else
    {
        printError("newBookmark","Offset out of range of associate item");
        return -1;
    }
    return id;
}

//-------------------------------------------------------------------------------------------

void Bitstream::complete(tint bkMark)
{
    tint bkID;
    Associate *item,*pItem;
    Bookmark *book;
    common::BOTree<tint,Sequence *> list;
    Sequence *seq;
    tubyte *buf;
    
    if(bkMark==-1 || !m_bookmarks.Exist(bkMark))
    {
        return;
    }
    book = m_bookmarks.Find(bkMark);
    
    // Move all sequences to start of stream if in complete section.
    
    // 1. Note all sequence instances in complete section;
    item = book->m_item->m_prev;
    while(item!=0)
    {
        if(item->m_sequence!=0)
        {
            list[item->m_stream] = item->m_sequence;
            item->m_sequence = 0;
        }
        item = item->m_prev;
    }
    
    // 2. Look for stream belonging to sequence and move it to beginning of its stream section.
    item = book->m_item;
    while(item!=0)
    {
        if(list.Exist(item->m_stream))
        {
            bkID = newBookmark(item,static_cast<tint>(item->m_offset));
            if(bkID!=-1)
            {
                seq = list.Find(item->m_stream);
                if(seq->move(bkID))
                {
                    list.Remove(item->m_stream);
                }
            }
        }
        item = item->m_next;
    }
    
    // 3. Recycle all unallocated sequences.
    if(list.Start())
    {
        while(!list.IsEnd())
        {
            m_sequences.Remove(list.CurrentIndex());
            m_recycleSequence.put(list.Next());
        }
    }
    
    // Free up the associate items, bookmarks and buffers in complete section.
    item = book->m_item->m_prev;
    buf = book->m_item->m_buffer;

    while(item!=0)
    {
        pItem = item->m_prev;
        if(buf!=item->m_buffer)
        {
            returnFileBuffer(item->m_buffer);
            buf = item->m_buffer;
        }
        while(item->m_bookmarks.Start())
        {
            m_recycleBookmark.put(item->m_bookmarks.Current());
            m_bookmarks.Remove(item->m_bookmarks.CurrentIndex());
            item->m_bookmarks.Remove(item->m_bookmarks.CurrentIndex());
        }
        m_recycleAssociate.put(item);
        item = pItem;
    }
    
    item = book->m_item;
    if(item!=0)
    {
        m_firstItem = item;
        item->m_prev = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool Bitstream::mark(tint stream)
{
    return mark(stream,-1,0);
}

//-------------------------------------------------------------------------------------------

bool Bitstream::mark(tint stream,tint bkMark,tint offset)
{
    tint cID,nID,streamID;
    Associate *pItem,*item;
    Bookmark *book;
    Sequence *seq = 0;
    
    if(m_currentItem==0)
    {
        printError("mark","Bitstream pointer not set to anything");
        return false;
    }
    
    if(!move(bkMark,offset,false))
    {
        printError("mark","Failed to move bitstream position to requested location");
        return false;
    }
    
    // Ensure that the marked split is done on data region where file has been read.
    while(m_currentItem->m_buffer==0)
    {
        if(m_fileEnd)
        {
            return false;
        }
        if(!readInNextBuffer())
        {
            printError("mark","Failed to read in buffer region");
            return false;
        }
    }
    
    // If the stream being asked for is the same as the place where it is to be put then no need for operation.
    if(stream==m_currentItem->m_stream)
    {
        return true;
    }
    
    // Ensure boundary exists on byte aligned border otherwise split cannot be done.
    if(m_bitOffset & 0x00000007)
    {
        printError("mark","Mark boundary not byte aligned");
        return false;
    }
    
    streamID = m_currentItem->m_stream;
    
    // Split the current item, if so required and move bookmarks.
    if(m_currentItem->m_offset < m_bitOffset)
    {
        item = m_recycleAssociate.get();
        
        item->m_prev = m_currentItem;
        item->m_next = m_currentItem->m_next;
        item->m_stream = streamID;
        item->m_buffer = m_currentItem->m_buffer;
        item->m_offset = m_bitOffset;
        item->m_length = m_currentItem->m_length;
        
        m_currentItem->m_next->m_prev = item;
        m_currentItem->m_next = item;
        m_currentItem->m_length = m_bitOffset;
        
        if(m_currentItem->m_bookmarks.Start())
        {
            while(!m_currentItem->m_bookmarks.IsEnd())
            {
                book = m_currentItem->m_bookmarks.Current();
                if(book->m_offset>=m_currentItem->m_length)
                {
                    cID = m_currentItem->m_bookmarks.NextIndex();
                    nID = m_currentItem->m_bookmarks.CurrentIndex();
                    book->m_item = item;
                    item->m_bookmarks[cID] = book;
                    m_currentItem->m_bookmarks.Remove(cID);
                    if(nID!=-1)
                    {
                        m_currentItem->m_bookmarks.Start(nID);
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    m_currentItem->m_bookmarks.Next();
                }
            }
        }
        
        // Move sequence over if in marked region.
        if(m_currentItem->m_sequence!=0)
        {
            if(m_currentItem->m_sequence->m_bitOffset < m_currentItem->m_length)
            {
                m_currentItem->m_sequence->m_length = m_currentItem->m_length;
                item->m_sequence = 0;
            }
            else
            {
                // This is adjusted properly by later section of mark method.
                associateWithSequence(item,m_currentItem->m_sequence);
                m_currentItem->m_sequence = 0;
            }
        }
        else
        {
            item->m_sequence = 0;
        }    
    }
    else
    {
        item = m_currentItem;
    }
    
    // Set the stream number for the sequence. Note any sequence instance in the stream.
    pItem = item;
    while(item!=0 && item->m_stream==streamID)
    {
        if(item->m_sequence!=0)
        {
            seq = item->m_sequence;
            item->m_sequence = 0;
        }
        item->m_stream = stream;
        item = item->m_next;
    }
    
    // Find position to replace the moved sequence
    if(seq!=0)
    {
        // 1. Search forwards
        while(item!=0)
        {
            if(item->m_stream==streamID)
            {
                cID = newBookmark(item,static_cast<int>(item->m_offset));
                if(seq->move(cID))
                {
                    break;
                }
            }
            item = item->m_next;
        }
        
        // 2. Search backwards
        if(item==0)
        {
            item = pItem->m_prev;
            while(item!=0)
            {
                if(item->m_stream==streamID)
                {
                    associateWithSequence(item,seq);
                    seq->m_bitOffset = item->m_length;
                    seq->m_end = true;
                    break;
                }
                item = item->m_prev;
            }
        }
        
        // 3. If nothing found recycle the sequence
        if(item==0)
        {
            m_sequences.Remove(streamID);
            m_recycleSequence.put(seq);
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------

tint Bitstream::bookmark()
{
    return newBookmark(m_currentItem,static_cast<tint>(m_bitOffset));
}

//-------------------------------------------------------------------------------------------

tint Bitstream::bookmark(tint bkID,tint offset,bool toStream)
{
    if(!move(bkID,offset,toStream))
    {
        printError("bookmark","Failed to move to given stream position");
        return -1;
    }
    return bookmark();
}

//-------------------------------------------------------------------------------------------

Sequence *Bitstream::getSequence(tint stream)
{
    Sequence *seq = 0;
    
    if(!m_sequences.Exist(stream))
    {
        Associate *item = m_firstItem;
        
        while(item!=0 && item->m_stream!=stream)
        {
            item = item->m_next;
        }
        if(item!=0)
        {
            seq = m_recycleSequence.get();
            associateWithSequence(item,seq);
            m_sequences[stream] = seq;
        }
        else
        {
            printError("getSequence","Stream does not exist in bit stream");
        }
    }
    else
    {
        seq = m_sequences.Find(stream);
    }
    return seq;
}

//-------------------------------------------------------------------------------------------

bool Bitstream::seek(tint offset,File::Position position)
{
    bool res = false;
    
    if(m_file!=0)
    {
        if(m_file->seek(offset,position))
        {
            reset();
            res = true;
        }
        else
        {
            printError("seek","Failed to seek requested location in file");
        }
    }
    else
    {
        printError("seek","No file open in bitstream instance");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const File *Bitstream::file() const
{
    return m_file;
}

//-------------------------------------------------------------------------------------------

Sequence *Bitstream::getNewSequence(void *p)
{
    Bitstream *bs = reinterpret_cast<Bitstream *>(p);
    if(bs!=0)
    {
        return bs->newSequence();
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

Sequence *Bitstream::newSequence()
{
    Sequence *seq = new Sequence;
    return seq;
}

//-------------------------------------------------------------------------------------------

bool Bitstream::isLittleEndian()
{
    static bool initF = false;
    static bool res = true;

    if(!initF)
    {
        static tubyte m[4] = {0x01,0x02,0x03,0x04};
        tuint32 *b = reinterpret_cast<tuint32 *>(m);

        if((*b)==0x01020304)
        {
            res = false;
        }
        initF = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
