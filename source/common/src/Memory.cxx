#include "common/inc/Memory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
namespace memory
{
//-------------------------------------------------------------------------------------------

tint32 intFromMemory(const tchar *mem)
{
    tint x;
    
    if(mem!=0)
    {
        x = static_cast<tint>( (static_cast<tuint>(static_cast<tubyte>(mem[0])) << 24) & 0xff000000 ) |
            static_cast<tint>( (static_cast<tuint>(static_cast<tubyte>(mem[1])) << 16) & 0x00ff0000 ) |
            static_cast<tint>( (static_cast<tuint>(static_cast<tubyte>(mem[2])) <<  8) & 0x0000ff00 ) |
            static_cast<tint>( (static_cast<tuint>(static_cast<tubyte>(mem[3]))) & 0x000000ff );
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

void intToMemory(tint32 x,tchar *mem)
{
    tubyte *m = reinterpret_cast<tubyte *>(mem);
    
    if(m!=0)
    {
        m[0] = static_cast<tubyte>((static_cast<tuint>(x) >> 24) & 0x000000ff);
        m[1] = static_cast<tubyte>((static_cast<tuint>(x) >> 16) & 0x000000ff);
        m[2] = static_cast<tubyte>((static_cast<tuint>(x) >>  8) & 0x000000ff);
        m[3] = static_cast<tubyte>((static_cast<tuint>(x)) & 0x000000ff);
    }
}

//-------------------------------------------------------------------------------------------

tint16 shortFromMemory(const tchar *mem)
{
    tint16 x;
    
    if(mem!=0)
    {
        x = static_cast<tint16>( (static_cast<tuint16>(static_cast<tubyte>(mem[0])) << 8 ) & 0xff00 ) |
            static_cast<tint16>( (static_cast<tuint16>(static_cast<tubyte>(mem[1]))) & 0x00ff);
    }
    else
    {
        x = 0;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

void shortToMemory(tint16 x,tchar *mem)
{
    tubyte *m = reinterpret_cast<tubyte *>(mem);
    
    if(m!=0)
    {
        m[0] = static_cast<tubyte>((static_cast<tuint16>(x) >> 8) & 0x00ff);
        m[1] = static_cast<tubyte>((static_cast<tuint16>(x)) & 0x00ff);
    }
}

//-------------------------------------------------------------------------------------------

tint stringFromMemory(BString& str,const tchar *mem,tint total)
{
    tint inc = 4;
    
    if(mem!=0 && total>0)
    {
        tint len = intFromMemory(mem);
        
        if(len>0)
        {
            if(len <= (total-4))
            {
                BString s(&mem[4],static_cast<tuint>(len));
                str = s;
                inc += len;
            }
            else
            {
                inc = -1;
            }
        }
        else
        {
            str = "";
        }
    }
    else
    {
        inc = -1;
    }
    return inc;
}

//-------------------------------------------------------------------------------------------

void stringToMemory(const BString& str,tchar *mem,tint len)
{
    tint i;
    const tchar *s = static_cast<const tchar *>(str);
    
    if(mem!=0 && (str.GetLength() + 4) <= len)
    {
        intToMemory(str.GetLength(),mem);
        for(i=0;i<str.GetLength();++i)
        {
            mem[i+4] = s[i];
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace memory
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
