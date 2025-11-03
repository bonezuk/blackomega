#ifndef __OMEGA_COMMON_USTRING_H
#define __OMEGA_COMMON_USTRING_H

#include "common/inc/Allocation.h"
#include "common/inc/BString.h"
#include "common/inc/BStringCollection.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LITTLE_ENDIAN)
//-------------------------------------------------------------------------------------------

#define USTRARRAY_CHAR(a,b,c) {a[b] = static_cast<tushort>(static_cast<tuchar>(c));}

#define USTRARRAY_USHORT(a,b,c) {a[b] = c;}

#define USTR_VALUE(a,b) {a = b;}

#define USTR_CHAR(a) (static_cast<tushort>(static_cast<tuchar>(a)))

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

#define USTRARRAY_CHAR(a,b,c) {a[b] = ((static_cast<tushort>(static_cast<tuchar>(c))) << 8) & 0xff00;}

#define USTRARRAY_USHORT(a,b,c) {a[b] = ((c>>8)&0x00ff) | ((c<<8)&0xff00);}

#define USTR_VALUE(a,b) {a = ((b<<8)&0xff00) | ((b>>8)&0x00ff);}

#define USTR_CHAR(a) ((static_cast<tushort>(static_cast<tuchar>(a)) << 8) & 0xff00)

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
//Compatable UTF Types with the old BOOL version

#define STANDARD_UTF16 0
#define XML_UTF16 1
#define STANDARD_UTF8 2
#define STANDARD_ASCII_8BIT 3

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT UString
{
    public:
        UString();
        UString(const tchar *str);
        UString(const tchar *str,tint len);
        UString(const tushort *str);
        UString(const tushort *str,tint len);
        UString(const BString& str);
        UString(const UString& str);

        ~UString();

        static void EncodeUTF8(const UString& in,BString& out);
        static void DecodeUTF8(const BString& in,UString& out);

        static void SetASCIIEncoding(tint type);

        // null object
        static UString& null();
        static UString null_object;

        UString& operator = (const tchar *str);
        UString& operator = (const tushort *str);
        UString& operator = (const BString& str);
        UString& operator = (const UString& str);

        UString& operator += (const tchar *str);
        UString& operator += (const tushort *str);
        UString& operator += (const BString& str);
        UString& operator += (const UString& str);

        tushort operator [] (tint p) const;

        BString AStr() const;

        operator BString() const;

        operator const tushort * () const;

        tushort *Ptr();
        const tushort *Ptr() const;

        tushort *GetString();
        const tushort *GetString() const;

        tushort *getString();
        const tushort *getString() const;

        tint len() const;
        tint length() const;
        tint Length() const;
        tint StrLen() const;
        tint GetLength() const;

        UString mid(tint offset,tint len) const;
        UString mid(tuint offset,tint len) const;
        UString mid(tint offset,tuint len=0) const;
        UString mid(tuint offset,tuint len=0) const;

        UString Mid(tint offset,tint len) const;
        UString Mid(tuint offset,tint len) const;
        UString Mid(tint offset,tuint len=0) const;
        UString Mid(tuint offset,tuint len=0) const;

        UString sub(tint offset,tint len) const;
        UString sub(tuint offset,tint len) const;
        UString sub(tint offset,tuint len=0) const;
        UString sub(tuint offset,tuint len=0) const;

        UString Sub(tint offset,tint len) const;
        UString Sub(tuint offset,tint len) const;
        UString Sub(tint offset,tuint len=0) const;
        UString Sub(tuint offset,tuint len=0) const;

        bool IsEmpty() const;
        bool isEmpty() const;

        friend COMMON_EXPORT tint operator == (const tchar *a,const UString& b);
        friend COMMON_EXPORT tint operator == (const UString& a,const tchar *b);
        friend COMMON_EXPORT tint operator == (const tushort *a,const UString& b);
        friend COMMON_EXPORT tint operator == (const UString& a,const tushort *b);
        friend COMMON_EXPORT tint operator == (const BString& a,const UString& b);
        friend COMMON_EXPORT tint operator == (const UString& a,const BString& b);
        friend COMMON_EXPORT tint operator == (const UString& a,const UString& b);

        friend COMMON_EXPORT tint operator >= (const tchar *a,const UString& b);
        friend COMMON_EXPORT tint operator >= (const UString& a,const tchar *b);
        friend COMMON_EXPORT tint operator >= (const tushort *a,const UString& b);
        friend COMMON_EXPORT tint operator >= (const UString& a,const tushort *b);
        friend COMMON_EXPORT tint operator >= (const BString& a,const UString& b);
        friend COMMON_EXPORT tint operator >= (const UString& a,const BString& b);
        friend COMMON_EXPORT tint operator >= (const UString& a,const UString& b);

        friend COMMON_EXPORT tint operator > (const tchar *a,const UString& b);
        friend COMMON_EXPORT tint operator > (const UString& a,const tchar *b);
        friend COMMON_EXPORT tint operator > (const tushort *a,const UString& b);
        friend COMMON_EXPORT tint operator > (const UString& a,const tushort *b);
        friend COMMON_EXPORT tint operator > (const BString& a,const UString& b);
        friend COMMON_EXPORT tint operator > (const UString& a,const BString& b);
        friend COMMON_EXPORT tint operator > (const UString& a,const UString& b);

        friend COMMON_EXPORT tint operator <= (const tchar *a,const UString& b);
        friend COMMON_EXPORT tint operator <= (const UString& a,const tchar *b);
        friend COMMON_EXPORT tint operator <= (const tushort *a,const UString& b);
        friend COMMON_EXPORT tint operator <= (const UString& a,const tushort *b);
        friend COMMON_EXPORT tint operator <= (const BString& a,const UString& b);
        friend COMMON_EXPORT tint operator <= (const UString& a,const BString& b);
        friend COMMON_EXPORT tint operator <= (const UString& a,const UString& b);

        friend COMMON_EXPORT tint operator < (const tchar *a,const UString& b);
        friend COMMON_EXPORT tint operator < (const UString& a,const tchar *b);
        friend COMMON_EXPORT tint operator < (const tushort *a,const UString& b);
        friend COMMON_EXPORT tint operator < (const UString& a,const tushort *b);
        friend COMMON_EXPORT tint operator < (const BString& a,const UString& b);
        friend COMMON_EXPORT tint operator < (const UString& a,const BString& b);
        friend COMMON_EXPORT tint operator < (const UString& a,const UString& b);

        friend COMMON_EXPORT tint operator != (const tchar *a,const UString& b);
        friend COMMON_EXPORT tint operator != (const UString& a,const tchar *b);
        friend COMMON_EXPORT tint operator != (const tushort *a,const UString& b);
        friend COMMON_EXPORT tint operator != (const UString& a,const tushort *b);
        friend COMMON_EXPORT tint operator != (const BString& a,const UString& b);
        friend COMMON_EXPORT tint operator != (const UString& a,const BString& b);
        friend COMMON_EXPORT tint operator != (const UString& a,const UString& b);

    private:
        //Global ASCII Unicode encoding type.
        //static bool m_XML_UTF16;
        static tint m_iUTFEncodingType;

        //Buffer holding the UTF16 big endain encoded Unicode string.
        tushort *m_Str;

        //Number of characters in the Unicode string.
        tint m_Len;

        //The number of 16-bit character units allocated from memory to hold m_Str.
        tint m_memLen;

        void PrintError(const tchar *strR,const tchar *strE) const;

        void Convert_ASCIIToUnicode(const tchar *src,tint& sLen,tushort *dest,tint& dLen,const tchar *term=0) const;

        void AppendToCurrent(const tushort *nStr,tint nLen,tint offset);
        void AppendToCurrent(const tchar *nStr,tint nLen,tint offset);

        int CalcStrLength(const tushort *str) const;

        BString Encode_StandardUTF16(tushort c) const;
        BString Encode_XMLUTF16(tushort c) const;
        BString Encode_StandardUTF8(tushort c) const;
        BString Encode_StandardASCII(tushort c) const;

        BString Convert_UnicodeToASCII() const;

        static int Compare(const tushort *a,const tushort *b);
};

COMMON_EXPORT UString& operator << (UString& in,const tchar *str);
COMMON_EXPORT UString& operator << (UString& in,const tushort *str);
COMMON_EXPORT UString& operator << (UString& in,const BString& str);
COMMON_EXPORT UString& operator << (UString& in,const UString& str);

COMMON_EXPORT UString operator + (const tchar *a,const UString& b);
COMMON_EXPORT UString operator + (const UString& a,const tchar *b);
COMMON_EXPORT UString operator + (const tushort *a,const UString& b);
COMMON_EXPORT UString operator + (const UString& a,const tushort *b);
COMMON_EXPORT UString operator + (const BString& a,const UString& b);
COMMON_EXPORT UString operator + (const UString& a,const BString& b);
COMMON_EXPORT UString operator + (const UString& a,const UString& b);

//-------------------------------------------------------------------------------------------
//Method:
//    Ptr
//
//Purpose:
//    Return back the pointer to string buffer mantained by UString instance.
//
//Access:
//    Public
//
//-------------------------------------------------------------------------------------------

inline tushort *UString::Ptr()
{
    return m_Str;
}

//-------------------------------------------------------------------------------------------

inline const tushort *UString::Ptr() const
{
    return m_Str;
}

//-------------------------------------------------------------------------------------------

inline tushort *UString::GetString()
{
    return m_Str;
}

//-------------------------------------------------------------------------------------------

inline const tushort *UString::GetString() const
{
    return m_Str;
}

//-------------------------------------------------------------------------------------------

inline tushort *UString::getString()
{
    return m_Str;
}

//-------------------------------------------------------------------------------------------

inline const tushort *UString::getString() const
{
    return m_Str;
}

//-------------------------------------------------------------------------------------------

inline UString::operator const tushort * () const
{
    return reinterpret_cast<const tushort *>(m_Str);
}

//-------------------------------------------------------------------------------------------
//Method:
//    Length
//
//Purpose:
//    Return back the length of the string of UString instance
//
//Access:
//    Public
//
//-------------------------------------------------------------------------------------------

inline tint UString::len() const
{
    return m_Len;
}

//-------------------------------------------------------------------------------------------

inline tint UString::length() const
{
    return m_Len;
}

//-------------------------------------------------------------------------------------------

inline tint UString::Length() const
{
    return m_Len;
}

//-------------------------------------------------------------------------------------------

inline tint UString::StrLen() const
{
    return m_Len;
}

//-------------------------------------------------------------------------------------------

inline tint UString::GetLength() const
{
    return m_Len;
}

//------------------------------------------------------------------------------
//Function:
//    operator []
//
//Purpose:
//    Returns back the character at a specified position in string array.
//
//Parameters:
//    p - position of the character
//
//Returns:
//    The character at the position in the array if it exists. Otherwise 0.
//
//------------------------------------------------------------------------------

inline tushort UString::operator [] (tint p) const
{
    tushort c;

    if(m_Str!=0 && p>=0 && p<m_Len)
    {
        c = m_Str[p];
    }
    else
    {
        c = 0;
    }
    return c;
}

//------------------------------------------------------------------------------

inline bool UString::IsEmpty() const
{
    return (m_Str==0 || m_Len<=0) ? true : false;
}

//------------------------------------------------------------------------------

inline bool UString::isEmpty() const
{
    return (m_Str==0 || m_Len<=0) ? true : false;
}


//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
