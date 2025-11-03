#include <stdio.h>
#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega 
{
namespace common
{
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//String constructors.
//-------------------------------------------------------------------------------------------

BString::BString() : m_str(0),
    m_strL(0),
    m_memSize(0)
{}

//-------------------------------------------------------------------------------------------

BString::BString(tint mSize) : m_str(0), 
    m_strL(0), 
    m_memSize(0)
{
    if(mSize>0)
    {
        AllocateMemory(static_cast<tuint>(mSize));
        if(m_str!=0)
        {
            *m_str = '\0';
        }
    }
}

//-------------------------------------------------------------------------------------------

BString::BString(const BString& cpStr) : m_str(0), 
    m_strL(0), 
    m_memSize(0)
{
    BString::CopyToBuffer(cpStr.m_str);
}

//-------------------------------------------------------------------------------------------

BString::BString(const BString *nStr) : m_str(0), 
    m_strL(0), 
    m_memSize(0)
{
    BString::IsEqual(nStr);
}

//-------------------------------------------------------------------------------------------

BString::BString(const tuchar *cpStr,size_t nLength) : m_str(0), 
    m_strL(0), 
    m_memSize(0)
{
    BString::AllocateMemory(BSTRING_BLOCKSIZE);
    if(!nLength) 
    {
        BString::CopyToBuffer(cpStr);
    }
    else 
    {
        BString::CopyToBuffer(cpStr,0,static_cast<tint>(nLength));
    }
}

//-------------------------------------------------------------------------------------------

BString::BString(const tchar *cpStr,size_t nLength) : m_str(0), 
    m_strL(0), 
    m_memSize(0)
{
    BString::AllocateMemory(BSTRING_BLOCKSIZE);
    if(!nLength) 
    {
        BString::CopyToBuffer(cpStr);
    }
    else 
    {
        BString::CopyToBuffer(cpStr,0,static_cast<tint>(nLength));
    }
}

//-------------------------------------------------------------------------------------------
//Deconstructor - ensure that the memory allocated to the string buffer is freed.
//-------------------------------------------------------------------------------------------

BString::~BString() 
{
    if(m_str!=0)
    {
        delete [] m_str;
        m_str = 0;
    }
}


//-------------------------------------------------------------------------------------------
//null object
//-------------------------------------------------------------------------------------------

BString& BString::null()
{
    static BString nullString;
    return nullString;
}

//-------------------------------------------------------------------------------------------
//null object
//-------------------------------------------------------------------------------------------

BString BString::null_object;

//-------------------------------------------------------------------------------------------

void BString::InitializeVariables() 
{
    if(m_str!=0)
    {
        delete [] m_str;
    }
    m_str=0;
    m_strL=0;
    m_memSize=0;
}

//-------------------------------------------------------------------------------------------
//Allocate Memory is designed such that it works on the processors optimal
//bit size. (e.g. 32-bits). Hence it ensures that memory size aligned to 
//the this number of bits and copying is optimized such to handle these memory
//block sizes.
//-------------------------------------------------------------------------------------------

void BString::AllocateMemory(size_t newSize) 
{
    tuchar *newStr;

    newSize = ((newSize >> 2) + 1) << 2;

    if(m_str==0 || newSize>=m_memSize)
    {
        newStr = new tuchar [newSize];
        
        if(m_str!=0)
        {
            if(m_memSize>0)
            {
                ::memcpy(newStr,m_str,m_memSize);
            }
            delete [] m_str;
        }
        m_memSize = static_cast<tuint>(newSize);
        m_str = newStr;
    }
}

//-------------------------------------------------------------------------------------------

void BString::CopyToBuffer(const tchar *sStr,tuint offset,tint len) 
{
    CopyToBuffer(reinterpret_cast<const tuchar *>(sStr),offset,len);
}

//-------------------------------------------------------------------------------------------

void BString::CopyToBuffer(const tuchar *sStr,tuint offset,tint len) 
{
    if(sStr==0) 
    {
        if(offset==0) 
        {
            if(m_str!=0)
            {
                delete [] m_str;
            }
            m_str=0;
            m_strL=0;
            m_memSize=0;
        }
    }
    else
    {
        tint actLen;

        actLen = static_cast<tint>(strlen(reinterpret_cast<const tchar *>(sStr)));
        if(len<0 || actLen<len)
        {
            len = actLen;
        }

        AllocateMemory(offset + static_cast<tuint>(len) + 1);
        if(m_str!=0 && len>=0)
        {
            ::memcpy(&m_str[offset],sStr,static_cast<tuint>(len));
            m_str[offset + static_cast<tuint>(len)] = '\0';
            m_strL = offset + static_cast<tuint>(len);
        }
    }
}

//-------------------------------------------------------------------------------------------

unsigned int BString::CountLength(const tchar *sStr) const
{
    return CountLength(reinterpret_cast<const tuchar *>(sStr));
}

//-------------------------------------------------------------------------------------------

unsigned int BString::CountLength(const tuchar *sStr) const 
{
    tuint32 cl=0,x;
    const tuint32 *source;
    bool run=true;

    if(sStr==0)
    {
        return 0;
    }

    source = reinterpret_cast<const tuint32 *>(sStr);    //lint !e826 Memory allocated in string to multiple of 4 bytes.

    while(run) 
    {
        x=*source++;
        if(!(x&0x000000ff)) 
        {
            run=false;
        }
        else if(!(x&0x0000ff00)) 
        {
            cl++;
            run=false;
        }
        else if(!(x&0x00ff0000)) 
        {
            cl+=2;
            run=false;
        }
        else if(!(x&0xff000000)) 
        {
            cl+=3;
            run=false;
        }
        else 
        {
            cl+=4;
        }
    }
    return cl;
}

//-------------------------------------------------------------------------------------------
//Method set used in the comparison of two strings.
//-------------------------------------------------------------------------------------------

tint BString::Compare(const tchar *sA,const tuchar *sB) 
{
    return Compare(reinterpret_cast<const tuchar *>(sA),sB);
}

//-------------------------------------------------------------------------------------------

tint BString::Compare(const tchar *sA,const tchar *sB) 
{
    return Compare(reinterpret_cast<const tuchar *>(sA),reinterpret_cast<const tuchar *>(sB));
}

//-------------------------------------------------------------------------------------------

tint BString::Compare(const tuchar *sA,const tchar *sB) 
{
    return Compare(sA,reinterpret_cast<const tuchar *>(sB));
}

//-------------------------------------------------------------------------------------------

tint BString::Compare(const tuchar *a,const tuchar *b) 
{
    tint i=0;

    if(a==0) 
    {
        if(b==0 || b[0]=='\0') 
        {
            return 0;
        }
        else // a < b
        { 
            return -1;
        }
    }
    else if(b==0) 
    {
        if(a[0]=='\0') 
        {
            return 0;
        }
        else // a > b
        { 
            return 1;
        }
    }

    while(a[i]!='\0' && b[i]!='\0')
    {
        if(a[i]<b[i])
        {
            return -1;
        }
        else if(a[i]>b[i])
        {
            return 1;
        }
        i++;
    }

    if(a[i]=='\0' && b[i]!='\0')
    {
        return -1;
    }
    else if(a[i]!='\0' && b[i]=='\0')
    {
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------
//Append to current string operator methods.
//-------------------------------------------------------------------------------------------

const BString& BString::operator += (const BString &s) 
{
    CopyToBuffer(s.m_str,m_strL);
    return *this;
}

//-------------------------------------------------------------------------------------------

const BString& BString::operator += (const tuchar *cS) 
{
    CopyToBuffer(cS,m_strL);
    return *this;
}

//-------------------------------------------------------------------------------------------

const BString& BString::operator += (const tchar *cS) 
{
    CopyToBuffer(cS,m_strL);
    return *this;
}

//-------------------------------------------------------------------------------------------

const BString& BString::operator += (const tchar cS)
{
    tchar t[2] = {'\0','\0'};

    t[0] = cS;
    CopyToBuffer(t,m_strL);
    return *this;    
}

//-------------------------------------------------------------------------------------------
//Make string equal operator methods.
//-------------------------------------------------------------------------------------------

const BString& BString::operator = (const BString &s) 
{
    if(&s!=this) 
    {
        CopyToBuffer(s.m_str);
    }
    return *this;    
}

//-------------------------------------------------------------------------------------------

const BString& BString::operator = (const tuchar *cS) 
{
    CopyToBuffer(cS);
    return *this;
}

//-------------------------------------------------------------------------------------------

const BString& BString::operator = (const tchar *cS) 
{
    CopyToBuffer(cS);
    return *this;
}

//-------------------------------------------------------------------------------------------
//String addition operator method set.
//-------------------------------------------------------------------------------------------

BString operator + (const BString &sA,const BString &sB) 
{
    BString retString(sA);
    
    retString+=sB;
    return retString;
}

//-------------------------------------------------------------------------------------------

BString operator + (const BString &sA,const tchar *strB) 
{
    BString retString(sA);
  
    retString.CopyToBuffer(strB,retString.m_strL);
    return retString;
}

//-------------------------------------------------------------------------------------------

BString operator + (const BString &sA,const tuchar *strB) 
{
    BString retString(sA);
  
    retString.CopyToBuffer(strB,retString.m_strL);
    return retString;
}

//-------------------------------------------------------------------------------------------

BString operator + (const tchar *strA,const BString &sB) 
{
    BString retString(strA);
  
    retString+=sB;
    return retString;
}

//-------------------------------------------------------------------------------------------

BString operator + (const tuchar *strA,const BString &sB) 
{
    BString retString(strA);
  
    retString+=sB;
    return retString;
}

//-------------------------------------------------------------------------------------------

BString& operator << (BString& in,const tchar *strA)
{ //lint !e1929 parameter ensures reference exists prior to being called.
    in += strA;
    return in;
}

//-------------------------------------------------------------------------------------------

BString& operator << (BString& in,const tuchar *strA)
{ //lint !e1929 parameter ensures reference exists prior to being called.
    in += strA;
    return in;
}

//-------------------------------------------------------------------------------------------

BString& operator << (BString& in,const BString& s)
{ //lint !e1929 parameter ensures reference exists prior to being called.
    in += s;
    return in;
}

//-------------------------------------------------------------------------------------------
//Equality operator method set.
//-------------------------------------------------------------------------------------------

tint operator ==(const BString& a,const BString& b) 
{
    return (BString::Compare(a.m_str,b.m_str)==0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator ==(const BString& a,const tuchar *b) 
{
    return (BString::Compare(a.m_str,b)==0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator ==(const BString& a,const tchar *b) 
{
    return (BString::Compare(a.m_str,b)==0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator ==(const tuchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator ==(const tchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Inequality operator method set.
//-------------------------------------------------------------------------------------------

tint operator !=(const BString& a,const BString& b) 
{
    return (BString::Compare(a.m_str,b.m_str)==0) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------

tint operator !=(const BString& a,const tuchar *b) 
{
    return (BString::Compare(a.m_str,b)==0) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------

tint operator !=(const BString& a,const tchar *b) 
{
    return (BString::Compare(a.m_str,b)==0) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------

tint operator !=(const tuchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==0) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------

tint operator !=(const tchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==0) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------
//Greater than operator method set.
//-------------------------------------------------------------------------------------------

tint operator > (const BString& a,const BString& b) 
{
    return (BString::Compare(a.m_str,b.m_str)==1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const BString& a,const tuchar *b) 
{
    return (BString::Compare(a.m_str,b)==1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const BString& a,const tchar *b) 
{
    return (BString::Compare(a.m_str,b)==1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const tuchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const tchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Greater than or equal too operator method set.
//-------------------------------------------------------------------------------------------

tint operator >= (const BString& a,const BString& b) 
{
    return (BString::Compare(a.m_str,b.m_str)>=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const BString& a,const tuchar *b) 
{
    return (BString::Compare(a.m_str,b)>=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const BString& a,const tchar *b) 
{
    return (BString::Compare(a.m_str,b)>=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const tuchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)>=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const tchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)>=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Less than operator method set.
//-------------------------------------------------------------------------------------------

tint operator < (const BString& a,const BString& b) 
{
    return (BString::Compare(a.m_str,b.m_str)==-1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const BString& a,const tuchar *b) 
{
    return (BString::Compare(a.m_str,b)==-1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const BString& a,const tchar *b) 
{
    return (BString::Compare(a.m_str,b)==-1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const tuchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==-1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const tchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)==-1) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Less than or equal too operator method set.
//-------------------------------------------------------------------------------------------

tint operator <= (const BString& a,const BString& b) 
{
    return (BString::Compare(a.m_str,b.m_str)<=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const BString& a,const tuchar *b) 
{
    return (BString::Compare(a.m_str,b)<=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const BString& a,const tchar *b) 
{
    return (BString::Compare(a.m_str,b)<=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const tuchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)<=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const tchar *a,const BString& b) 
{
    return (BString::Compare(a,b.m_str)<=0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Set of methods provided to provide a service for copying sections of strings from one
//location to the other. This method set is designed such that it is safe to copy memory
//blocks where the source and destination overlap.
//-------------------------------------------------------------------------------------------

void BString::CopyMemoryOpt(const tuchar *source,tuchar *dest,tuint copyLen) const
{
    static const tuint32 backArray[6]  = {0x000000ff, 0x0000ffff, 0x00ffffff, 0xffffff00, 0xffff0000, 0xff000000};
    static const tuint32 frontArray[6] = {0xff000000, 0xffff0000, 0xffffff00, 0x00ffffff, 0x0000ffff, 0x000000ff};

    const tuint32 *s;
    tuint32 *d,cLen=copyLen;

    if(source==0)
    {
        return;
    }
    if(dest==0)
    {
        return;
    }

    if(reinterpret_cast<ptrdiff_t>(source) > reinterpret_cast<ptrdiff_t>(dest)) //Use shift back copy method.
    { 
        s=reinterpret_cast<const tuint32 *>(source);    //lint !e826 Memory allocated in string to multiple of 4 bytes.
        d=reinterpret_cast<tuint32 *>(dest);            //lint !e826 Memory allocated in string to multiple of 4 bytes.
        while(cLen) 
        {
            if(cLen<4) 
            {
                *d = (*s & backArray[cLen-1]) | (*d & backArray[cLen+2]);
                cLen=0;
            }
            else 
            {
                *d++ = *s++;
                cLen-=4;
            }
        }
    }
    else //Use shift forward method
    { 
        s=reinterpret_cast<const tuint32 *>(&source[cLen]);    //lint !e826 Memory allocated in string to multiple of 4 bytes.
        d=reinterpret_cast<tuint32 *>(&dest[cLen]);            //lint !e826 Memory allocated in string to multiple of 4 bytes.
        while(cLen) 
        {
            if(cLen<4) 
            {
                s--;
                d--;
                *d = (*s & frontArray[cLen-1]) | (*d & frontArray[cLen+2]);
                cLen=0;
            }
            else 
            {
                *--d = *--s;
                cLen-=4;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
//Method used to take the string passed to it and generate a new string
//based on it with left and right spaces removed from it.
//-------------------------------------------------------------------------------------------

void BString::InternalTrim()
{
    InternalLTrim();
    InternalRTrim();
}

//-------------------------------------------------------------------------------------------
//Used to return a string object whose string is the "this" string trimed.
//-------------------------------------------------------------------------------------------

BString BString::Trim() const 
{
    BString trimStr(*this);

    trimStr.InternalTrim();
    return trimStr;
}

//-------------------------------------------------------------------------------------------

BString BString::trim() const
{
    return Trim();
}

//-------------------------------------------------------------------------------------------
//Trim the internal structure of the string.
//-------------------------------------------------------------------------------------------

void BString::SelfTrim() 
{
    InternalTrim();
}

//-------------------------------------------------------------------------------------------

void BString::InternalLTrim() 
{
    tint i,j;

    if(m_str!=0)
    {
        for(i=0,j=0;i<static_cast<tint>(m_strL) && (m_str[i]==' ' || m_str[i]=='\t');i++) ;
        if(i>0)
        {
            while(i<static_cast<tint>(m_strL))
            {
                m_str[j++] = m_str[i++];
            }
            m_str[j] = '\0';
            m_strL = static_cast<tuint>(j);
        }
    }
}

//-------------------------------------------------------------------------------------------

BString BString::LTrim() const 
{
    BString trimStr;

    trimStr.InternalLTrim();
    return trimStr;
}

//-------------------------------------------------------------------------------------------

BString BString::ltrim() const
{
    return LTrim();
}

//-------------------------------------------------------------------------------------------

void BString::SelfLTrim() 
{
    InternalLTrim();
}

//-------------------------------------------------------------------------------------------

void BString::InternalRTrim() 
{
    tint i;

    if(m_str!=0)
    {
        for(i=static_cast<tint>(m_strL)-1;i>=0 && (m_str[i]==' ' || m_str[i]=='\t');i--) ;
        m_strL = static_cast<tuint>(i) + 1;
        m_str[m_strL] = '\0';
    }
}

//-------------------------------------------------------------------------------------------

BString BString::RTrim() const
{
    BString trimStr;

    trimStr.InternalRTrim();
    return trimStr;
}

//-------------------------------------------------------------------------------------------

BString BString::rtrim() const
{
    return RTrim();
}

//-------------------------------------------------------------------------------------------

void BString::SelfRTrim() 
{
    InternalRTrim();
}

//-------------------------------------------------------------------------------------------

void BString::InternalSub(tuint offset,tuint subLen)
{
    tuint i,j;

    if(subLen==0)
    {
        subLen = m_strL - offset;
    }

    if(m_str!=0)
    {
        for(j=0,i=offset;i<m_strL && j<subLen;i++,j++)
        {
            m_str[j] = m_str[i];
        }
        m_str[j] = '\0';
        m_strL = j;
    }
}

//-------------------------------------------------------------------------------------------

BString BString::mid(tint offset,tint subLen) const
{
    return Mid(static_cast<tuint>(offset),static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::mid(tint offset,tuint subLen) const
{
    return Mid(static_cast<tuint>(offset),subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::mid(tuint offset,tint subLen) const
{
    return Mid(offset,static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::mid(tuint offset,tuint subLen) const
{
    return Mid(offset,subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::Mid(tint offset,tint subLen) const
{
    return Mid(static_cast<tuint>(offset),static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::Mid(tint offset,tuint subLen) const
{
    return Mid(static_cast<tuint>(offset),subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::Mid(tuint offset,tint subLen) const
{
    return Mid(offset,static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::Mid(tuint offset,tuint subLen) const 
{
    BString subString(*this);
    
    subString.InternalSub(offset,subLen);
    return subString;
}

//-------------------------------------------------------------------------------------------

BString BString::sub(tint offset,tint subLen) const
{
    return Sub(static_cast<tuint>(offset),static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::sub(tint offset,tuint subLen) const
{
    return Sub(static_cast<tuint>(offset),subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::sub(tuint offset,tint subLen) const
{
    return Sub(offset,static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::sub(tuint offset,tuint subLen) const
{
    return Sub(offset,subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::Sub(tint offset,tint subLen) const
{
    return Sub(static_cast<tuint>(offset),static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::Sub(tint offset,tuint subLen) const
{
    return Sub(static_cast<tuint>(offset),subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::Sub(tuint offset,tint subLen) const
{
    return Sub(offset,static_cast<tuint>(subLen));
}

//-------------------------------------------------------------------------------------------

BString BString::Sub(tuint offset,tuint subLen) const 
{
    BString subString(*this);

    subString.InternalSub(offset,subLen);
    return subString;
}

//-------------------------------------------------------------------------------------------

void BString::SelfSub(tuint offset,tuint subLen) 
{
    InternalSub(offset,subLen);
}

//-------------------------------------------------------------------------------------------

BString BString::UCase() const 
{
    BString uC(*this);

    uC.SelfUCase();
    return uC;
}

//-------------------------------------------------------------------------------------------

BString BString::ucase() const
{
    return UCase();
}

//-------------------------------------------------------------------------------------------

void BString::SelfUCase()
{
    tint i;

    if(m_str!=0)
    {
        for(i=0;i<static_cast<tint>(m_strL);i++)
        {
            if(m_str[i]>='a' && m_str[i]<='z')
            {
                m_str[i] -= ('a' - 'A');
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

BString BString::LCase() const 
{
    BString lC(*this);

    lC.SelfLCase();
    return lC;
}

//-------------------------------------------------------------------------------------------

BString BString::lcase() const
{
    return LCase();
}

//-------------------------------------------------------------------------------------------

void BString::SelfLCase() 
{
    tint i;

    if(m_str!=0)
    {
        for(i=0;i<static_cast<tint>(m_strL);i++)
        {
            if(m_str[i]>='A' && m_str[i]<='Z')
            {
                m_str[i] += ('a' - 'A');
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void BString::IsEqual(const BString *cStr) 
{
    if(cStr!=0)
    {
        const tchar *s = cStr->GetString();

         CopyToBuffer(s,0);
        m_strL=static_cast<tuint>(cStr->StrLen());
    }
    else
    {
        if(m_str!=0)
        {
            delete [] m_str;
            m_memSize = 0;
        }
        m_str = 0;
        m_strL = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool BString::Equal(BString *cStr) const
{
    if(cStr!=0)
    {
        return (Compare(m_str,cStr->GetString())==0) ? true : false;
    }
    else
    {
        return (IsEmpty()) ? true : false;
    }
}

//-------------------------------------------------------------------------------------------

BString BString::XMLString() const
{
    tint i=0,j=0;
    const tchar *x;
    BString out;

    x = reinterpret_cast<const tchar *>(m_str);
    if(x!=0 && m_strL>0) 
    {
        while(j < static_cast<tint>(m_strL))
        {
            if(x[j]=='<') 
            {
                out << Sub(static_cast<tuint>(i),static_cast<tuint>(j-i)) << "&lt;";
                i = j+1;
            }
            else if(x[j]=='>') 
            {
                out << Sub(static_cast<tuint>(i),static_cast<tuint>(j-i)) << "&gt;";
                i = j+1;
            }
            else if(x[j]=='\"') 
            {
                out << Sub(static_cast<tuint>(i),static_cast<tuint>(j-i)) << "&quot;";
                i = j+1;
            }
            else if(x[j]=='\'') 
            {
                out << Sub(static_cast<tuint>(i),static_cast<tuint>(j-i)) << "&apos;";
                i = j+1;
            }
            j++;
        }
        out += Sub(static_cast<tuint>(i));
    }
    return out;
}

//-------------------------------------------------------------------------------------------

BString BString::ReverseXMLString() const
{
    tuint i=0,start=0,state=0;
    const tchar *x = reinterpret_cast<const tchar *>(m_str);
    BString out;

    if(x!=0)
    {
        while(i<m_strL)
        {
            switch(state)
            {
                case 0:
                    if(x[i]!='&')
                    {
                        out += x[i];
                    }
                    else
                    {
                        start = i;
                        state = 1;
                    }
                    break;

                case 1:
                    if(x[i]=='l')
                    {
                        state = 2;
                    }
                    else if(x[i]=='g')
                    {
                        state = 4;
                    }
                    else if(x[i]=='q')
                    {
                        state = 6;
                    }
                    else if(x[i]=='a')
                    {
                        state = 10;
                    }
                    else
                    {
                        state = 50;
                    }
                    break;

                case 2:
                    state = (x[i]=='t') ? 3 : 50;
                    break;

                case 3:
                    if(x[i]==';')
                    {
                        out += '<';
                        state = 0;
                    }
                    else
                    {
                        state = 50;
                    }
                    break;

                case 4:
                    state = (x[i]=='t') ? 5 : 50;
                    break;

                case 5:
                    if(x[i]==';')
                    {
                        out += '>';
                        state = 0;
                    }
                    else
                    {
                        state = 50;
                    }
                    break;

                case 6:
                    state = (x[i]=='u') ? 7 : 50;
                    break;

                case 7:
                    state = (x[i]=='o') ? 8 : 50;
                    break;

                case 8:
                    state = (x[i]=='t') ? 9 : 50;
                    break;

                case 9:
                    if(x[i]==';')
                    {
                        out += '\"';
                        state = 0;
                    }
                    else
                    {
                        state = 50;
                    }
                    break;

                case 10:
                    state = (x[i]=='p') ? 11 : 50;
                    break;

                case 11:
                    state = (x[i]=='o') ? 12 : 50;
                    break;

                case 12:
                    state = (x[i]=='s') ? 13 : 50;
                    break;

                case 13:
                    if(x[i]==';')
                    {
                        out += '\'';
                        state = 0;
                    }
                    else
                    {
                        state = 50;
                    }
                    break;

                default:
                    break;
            }

            if(state==50)
            {
                out += x[start];
                i = start + 1;
                state = 0;
            }
            else
            {
                i++;
            }
        }
    }
    return out;
}

//-------------------------------------------------------------------------------------------

BString BString::BOXMLString() const
{
    tuint i,j;
    BString out;
    const tchar *s;

    s = reinterpret_cast<const tchar *>(m_str);
    if(s!=0 && m_strL>0)
    {
        for(i=0,j=0;i<m_strL;i++)
        {
            if(s[i]=='\'' || s[i]=='\"' || s[i]=='\\')
            {
                if(j<i)
                {
                    out += Sub(j,i-j);
                }
                out += "\\";
                out += s[i];
                j = i + 1;
            }
        }
        if(j<i)
        {
            out += Sub(j,i-j);
        }
    }
    return out;
}

//-------------------------------------------------------------------------------------------

BString BString::NumberToString(tuint x,tint n,bool minus,bool ex) 
{
    BString a;
    tchar t[2]={'\0','\0'};

    if(x || n) 
    {
        if(n) 
        {
            if(ex) 
            {
                while(n>0) 
                {
                    t[0] = static_cast<tchar>(static_cast<tint>((x%10) + '0'));
                    a = t + a;
                    x/=10;
                    n--;
                }
            }
            else 
            {
                while(x || n>0) 
                {
                    t[0] = static_cast<tchar>(static_cast<tint>((x%10) + '0'));
                    a = t + a;
                    x/=10;
                    n--;
                }
            }
        }
        else 
        {
            while(x) 
            {
                t[0] = static_cast<tchar>(static_cast<tint>((x%10) + '0'));
                a = t + a;
                x/=10;
            }
        }
        if(minus) 
        {
            t[0] = '-';
            a = t + a;
        }
    }
    else 
    {
        a="0";
    }
    return a;
}

//-------------------------------------------------------------------------------------------

BString BString::HexNumberToString(tuint x,tint n,bool minus,bool ex)
{
    BString a;
    tchar t[2] = {'\0','\0'};

    if(x || n)
    {
        tuint y;

        if(n)
        {
            if(ex)
            {
                while(n>0)
                {
                    y = x & 0x0000000f;
                    if(y<10)
                    {
                        t[0] = static_cast<tchar>(y) + '0';
                    }
                    else
                    {
                        t[0] = static_cast<tchar>(y - 10) + 'a';
                    }
                    a = t + a;
                    x >>= 4;
                    n--;
                }
            }
            else
            {
                while(x || n>0)
                {
                    y = x & 0x0000000f;
                    if(y<10)
                    {
                        t[0] = static_cast<tchar>(y) + '0';
                    }
                    else
                    {
                        t[0] = static_cast<tchar>(y - 10) + 'a';
                    }
                    a = t + a;
                    x >>= 4;
                    n--;
                }
            }
        }
        else
        {
            while(x)
            {
                y = x & 0x0000000f;
                if(y<10)
                {
                    t[0] = static_cast<tchar>(y) + '0';
                }
                else
                {
                    t[0] = static_cast<tchar>(y - 10) + 'a';
                }
                a = t + a;
                x >>= 4;
            }
        }

        if(minus)
        {
            t[0] = '-';
            a = t + a;
        }
    }
    else
    {
        a = "0";
    }
    return a;
}

//-------------------------------------------------------------------------------------------


BString BString::Int(tint x,tint n,bool ex) 
{
    BString a;

    if(x>=0) 
    {
        a = NumberToString(static_cast<tuint>(x),n,false,ex);
    }
    else 
    {
        a = NumberToString(static_cast<tuint>(-x),n,true,ex);
    }
    return a;
}

//-------------------------------------------------------------------------------------------

BString BString::Int(tuint x,tint n,bool ex) 
{
    return NumberToString(x,n,false,ex);
}

//-------------------------------------------------------------------------------------------

BString BString::HexInt(tint x,tint n,bool ex)
{
    BString a;

    if(x>=0)
    {
        a = HexNumberToString(static_cast<tuint>(x),n,false,ex);
    }
    else
    {
        a = HexNumberToString(static_cast<tuint>(-x),n,true,ex);
    }
    return a;
}

//-------------------------------------------------------------------------------------------

BString BString::HexInt(tuint x,tint n,bool ex)
{
    return HexNumberToString(x,n,false,ex);
}

//-------------------------------------------------------------------------------------------

tint BString::Atoi() const 
{
    tint i,x=0;
    bool minus;
    
    if(m_str!=0) 
    {
        if(m_str[0]=='-') 
        {
            i=1;
            minus=true;
        }
        else 
        {
            i=0;
            minus=false;
        }

        while(i < static_cast<tint>(m_strL)) 
        {
            if(m_str[i]>='0' && m_str[i]<='9') 
            {
                x*=10;
                x+=static_cast<tint>(m_str[i] - '0');
            }
            i++;
        }

        if(minus) 
        {
            x = -x;
        }
    }
    return x;

}

//-------------------------------------------------------------------------------------------

tint BString::Htoi() const
{
    tint i,x=0;
    bool minus;
    
    if(m_str!=0)
    {
        if(m_str[0]=='-')
        {
            i = 1;
            minus = true;
        }
        else
        {
            i = 0;
            minus = false;
        }
        
        while(i < static_cast<tint>(m_strL))
        {
            if(m_str[i]>='0' && m_str[i]<='9')
            {
                x *= 16;
                x += static_cast<tint>(m_str[i] - '0');
            }
            else if(m_str[i]>='a' && m_str[i]<='f')
            {
                x *= 16;
                x += static_cast<tint>(m_str[i] - 'a') + 10;
            }
            else if(m_str[i]>='A' && m_str[i]<='F')
            {
                x *= 16;
                x += static_cast<tint>(m_str[i] - 'A') + 10;
            }
            i++;
        }
        
        if(minus)
        {
            x = -x;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

void BString::Replace(tchar x,tchar y)
{
    tint i;

    if(m_str!=0)
    {
        for(i=0;i<static_cast<tint>(m_strL);i++)
        {
            if(m_str[i]==static_cast<tuchar>(x))
            {
                m_str[i] = static_cast<tuchar>(y);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

tint BString::Remove(tchar chRemove)
{
    tuint orgL = 0;

    if(m_str!=0)
    {
        tuint i,j;
        tuchar *src=m_str,*dest=m_str;

        for(i=0,j=0;i<m_strL;i++)
        {
            if(src[i]!=static_cast<tchar>(chRemove))
            {
                dest[j] = src[i];
                j++;
            }
        }
        dest[j] = '\0';
        orgL = m_strL - j;
        m_strL = j;
    }
    return static_cast<tint>(orgL);
}

//-------------------------------------------------------------------------------------------
//    Inserts character chInsert at position iAt
//-------------------------------------------------------------------------------------------
tint BString::Insert(tchar chInsert, tint iAt)
{

    if (iAt < 0)
        iAt    = 0;

    if ( (m_strL+1) >= m_memSize)
        AllocateMemory(m_memSize+1);

    if(m_str!=0)
    {
        //I.e. If we want to insert char 3 at position 2
        //124567890
        //  ^
        //shift all the data from pos 2 up one
        ::memmove(m_str+iAt+1,m_str+iAt,m_strL-static_cast<tuint>(iAt));

        //Add new character
        m_str[iAt]=static_cast<tuchar>(chInsert);
        m_str[++m_strL]='\0';
    }
    return static_cast<tint>(m_strL);
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
