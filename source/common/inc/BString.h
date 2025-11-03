#ifndef __OMEGA_COMMON_BSTRING_H
#define __OMEGA_COMMON_BSTRING_H

#include <string.h>
#include <stddef.h>
#include "common/inc/CommonTypes.h"

#define BSTRING_BLOCKSIZE 32L

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class BStringCollection;

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BString 
{
  
    friend class BStringCollection;

    public:
  
        //Constructors
        BString();
        BString(const BString& cpStr);
        BString(tint mSize);
        BString(const BString *nStr);
        BString(const tuchar *cpStr,size_t nLength=0);
        BString(const tchar *cpStr,size_t nLength=0);
  
        //Deconstructor
        virtual ~BString();

        //null object
        static BString& null();
        static BString null_object;
  
        //Return a pointer to the string.
        tchar *GetString();
        const tchar *GetString() const;
        
        tchar *getString();
        const tchar *getString() const;

        const tchar *c_str() const;

        operator const tchar * () const; //lint !e1930 Conversion required for direct memory access.

        //Set the buffer of the string that of another bstring object.
        void IsEqual(const BString *cStr);

        //String append operator methods.
        const BString& operator +=(const BString &s);
        const BString& operator +=(const tuchar *cS);
        const BString& operator +=(const tchar *cS);
        const BString& operator +=(const tchar cS);
  
        //String equal operator method.
        const BString& operator =(const BString &s);
        const BString& operator =(const tuchar *cS);
        const BString& operator =(const tchar *cS);
      
        tchar operator [] (tint pos) const;
        tchar at(tint pos) const;
        
        BString Trim() const; //Remove all spaces from beginning and end of string.
        BString trim() const;
        void SelfTrim();

        BString LTrim() const; //Remove all spaces from left hand side of string.
        BString ltrim() const;
        void SelfLTrim();

        BString RTrim() const; //Remove all spaces from right hand side of string.
        BString rtrim() const;
        void SelfRTrim();

        BString mid(tint offset,tint subLen) const;
        BString mid(tint offset,tuint subLen=0) const;
        BString mid(tuint offset,tint subLen) const;
        BString mid(tuint offset,tuint subLen=0) const;

        BString Mid(tint offset,tint subLen) const;
        BString Mid(tint offset,tuint subLen=0) const;
        BString Mid(tuint offset,tint subLen) const;
        BString Mid(tuint offset,tuint subLen=0) const; //Get the substring of string object
        
        BString sub(tint offset,tint subLen) const;
        BString sub(tint offset,tuint subLen=0) const;
        BString sub(tuint offset,tint subLen) const;
        BString sub(tuint offset,tuint subLen=0) const;
        
        BString Sub(tint offset,tint subLen) const;
        BString Sub(tint offset,tuint subLen=0) const;
        BString Sub(tuint offset,tint subLen) const;
        BString Sub(tuint offset,tuint subLen=0) const; //Get the substring of string object
        
        void SelfSub(tuint offset,tuint subLen=0);

        BString PadL(tchar padChar,tuint newLen); //Perform a right hand string padding.
        void SelfPadL(tchar padChar,tuint newLen);
        
        BString PadR(tchar padChar,tuint newLen); //Perform a right hand padding.
        void SelfPadR(tchar padChar,tuint newLen);
  
        BString UCase() const; //Convert the string to upper case letters.
        BString ucase() const;
        void SelfUCase();
        
        BString LCase() const; //Convert the string to lower case letters.
        BString lcase() const;
        void SelfLCase();
  
        bool IsEmpty() const;
        bool isEmpty() const;
        
        tint len() const;
        tint length() const;
        tint Length() const;
        tint StrLen() const;
        tint GetLength() const;
        
        bool Equal(BString *cStr) const;

        BString XMLString() const;
        BString BOXMLString() const;
        BString ReverseXMLString() const;

        static BString Int(tint x,tint n=0,bool ex=false);
        static BString Int(tuint x,tint n=0,bool ex=false);

        static BString HexInt(tint x,tint n=0,bool ex=false);
        static BString HexInt(tuint x,tint n=0,bool ex=false);

        tint Atoi() const;
        tint Htoi() const;

        void Replace(tchar x,tchar y);
        tint Remove(tchar chRemove);
        tint Insert(tchar chInsert, tint iAt);

        //String addition operators.
        friend COMMON_EXPORT BString operator + (const BString &sA,const BString &sB);
        friend COMMON_EXPORT BString operator + (const BString &sA,const tchar *strB);
        friend COMMON_EXPORT BString operator + (const BString &sA,const tuchar *strB);
        friend COMMON_EXPORT BString operator + (const tchar *strA,const BString &sB);
        friend COMMON_EXPORT BString operator + (const tuchar *strA,const BString &sB);

        //String comparison operator set.
        friend COMMON_EXPORT tint operator == (const BString& a,const BString& b);
        friend COMMON_EXPORT tint operator == (const BString& a,const tuchar *b);
        friend COMMON_EXPORT tint operator == (const BString& a,const tchar *b);
        friend COMMON_EXPORT tint operator == (const tuchar *a,const BString& b);
        friend COMMON_EXPORT tint operator == (const tchar *a,const BString& b);
  
        friend COMMON_EXPORT tint operator != (const BString& a,const BString& b);
        friend COMMON_EXPORT tint operator != (const BString& a,const tuchar *b);
        friend COMMON_EXPORT tint operator != (const BString& a,const tchar *b);
        friend COMMON_EXPORT tint operator != (const tuchar *a,const BString& b);
        friend COMMON_EXPORT tint operator != (const tchar *a,const BString& b);
  
        friend COMMON_EXPORT tint operator >  (const BString& a,const BString& b);
        friend COMMON_EXPORT tint operator >  (const BString& a,const tuchar *b);
        friend COMMON_EXPORT tint operator >  (const BString& a,const tchar *b);
        friend COMMON_EXPORT tint operator >  (const tuchar *a,const BString& b);
        friend COMMON_EXPORT tint operator >  (const tchar *a,const BString& b);
  
        friend COMMON_EXPORT tint operator >= (const BString& a,const BString& b);
        friend COMMON_EXPORT tint operator >= (const BString& a,const tuchar *b);
        friend COMMON_EXPORT tint operator >= (const BString& a,const tchar *b);
        friend COMMON_EXPORT tint operator >= (const tuchar *a,const BString& b);
        friend COMMON_EXPORT tint operator >= (const tchar *a,const BString& b);
  
        friend COMMON_EXPORT tint operator <  (const BString& a,const BString& b);
        friend COMMON_EXPORT tint operator <  (const BString& a,const tuchar *b);
        friend COMMON_EXPORT tint operator <  (const BString& a,const tchar *b);
        friend COMMON_EXPORT tint operator <  (const tuchar *a,const BString& b);
        friend COMMON_EXPORT tint operator <  (const tchar *a,const BString& b);
  
        friend COMMON_EXPORT tint operator <= (const BString& a,const BString& b);
        friend COMMON_EXPORT tint operator <= (const BString& a,const tuchar *b);
        friend COMMON_EXPORT tint operator <= (const BString& a,const tchar *b);
        friend COMMON_EXPORT tint operator <= (const tuchar *a,const BString& b);
        friend COMMON_EXPORT tint operator <= (const tchar *a,const BString& b);

    private:
        void InitializeVariables();

    protected:
  
        //Main character buffer in which the object string is held.
        tuchar *m_str;
        //Defines the length of the string.
        tuint m_strL;
        //Size of memory in bytes.
        tuint m_memSize;

        //AllocateMemory is used to expand the current string buffer.
        void AllocateMemory(size_t newSize);
        //This class of functions are collective used to provide the ability to copy
        //a given character string at set offset into the BString string buffer.
        //The use of the different functions (e.g. CopyToBuffer4) are designned such
        //that the copying procedurer is optimized for the current processor.
        void CopyToBuffer(const tuchar *sStr,tuint offset=0,tint len=-1);
        void CopyToBuffer(const tchar *sStr,tuint offset=0,tint len=-1);
  
        //Method set used to get the length of a particular string buffer.
        unsigned int CountLength(const tchar *sStr) const;
        unsigned int CountLength(const tuchar *sStr) const;
  
        //Method set used to compare the values of two strings.
        static int Compare(const tchar *sA,const tuchar *sB);
        static int Compare(const tchar *sA,const tchar *sB);
        static int Compare(const tuchar *sA,const tchar *sB);
        static int Compare(const tuchar *a,const tuchar *b);
  
        //Method set used to provide an optimized service for copying memory sections.
        //Note the methods are designed such that the contents of memory can be shifted
        //across to the left.
        void CopyMemoryOpt(const tuchar *source,tuchar *dest,tuint copyLen) const;
  
        void InternalTrim();
        void InternalLTrim();
        void InternalRTrim();

        void InternalSub(tuint offset,tuint subLen);

        static BString NumberToString(tuint x,tint n,bool minus,bool ex);
        static BString HexNumberToString(tuint x,tint n,bool minus,bool ex);
};

COMMON_EXPORT BString& operator << (BString& in,const tchar *strA);
COMMON_EXPORT BString& operator << (BString& in,const tuchar *strA);
COMMON_EXPORT BString& operator << (BString& in,const BString& s);

//-------------------------------------------------------------------------------------------

inline tchar *BString::GetString()
{
    return reinterpret_cast<tchar *>(m_str);
}

//-------------------------------------------------------------------------------------------

inline const tchar *BString::GetString() const
{
    return reinterpret_cast<const tchar *>(reinterpret_cast<tchar *>(m_str));
}

//-------------------------------------------------------------------------------------------

inline tchar *BString::getString()
{
    return reinterpret_cast<tchar *>(m_str);
}

//-------------------------------------------------------------------------------------------

inline const tchar *BString::getString() const
{
    return reinterpret_cast<const tchar *>(reinterpret_cast<tchar *>(m_str));
}

//-------------------------------------------------------------------------------------------

inline const tchar *BString::c_str() const
{
    return reinterpret_cast<const tchar *>(reinterpret_cast<tchar *>(m_str));
}

//-------------------------------------------------------------------------------------------

inline BString::operator const tchar * () const
{
    return reinterpret_cast<const tchar *>(reinterpret_cast<tchar *>(m_str));
}

//-------------------------------------------------------------------------------------------

inline tchar BString::operator [](tint pos) const
{
    if(m_str==0)
    {
        return '\0';
    }
    return static_cast<tchar>(m_str[pos]);
}

//-------------------------------------------------------------------------------------------

inline tchar BString::at(tint pos) const
{
    if(m_str==0)
    {
        return '\0';
    }
    return static_cast<tchar>(m_str[pos]);
}

//-------------------------------------------------------------------------------------------

inline bool BString::IsEmpty() const 
{
    return (m_strL) ? false : true;
}

//-------------------------------------------------------------------------------------------

inline bool BString::isEmpty() const
{
    return (m_strL) ? false : true;
}

//-------------------------------------------------------------------------------------------

inline tint BString::len() const
{
    return static_cast<tint>(m_strL);
}

//-------------------------------------------------------------------------------------------

inline tint BString::length() const
{
    return static_cast<tint>(m_strL);
}

//-------------------------------------------------------------------------------------------

inline tint BString::Length() const
{
    return static_cast<tint>(m_strL);
}

//-------------------------------------------------------------------------------------------

inline tint BString::StrLen() const
{
    return static_cast<tint>(m_strL);
}

//-------------------------------------------------------------------------------------------

inline tint BString::GetLength() const
{
    return static_cast<tint>(m_strL);
}

//-------------------------------------------------------------------------------------------
} // common namespace
} // omega namespace
//-------------------------------------------------------------------------------------------

#endif

