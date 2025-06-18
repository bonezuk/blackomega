#include "common/inc/UString.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
//Global ASCII Unicode encoding type.
//    true  (1) - Encode using XML UTF-16 encoding. i.e. &#xxxx; (Default)
//    false (0) - Encode using standard Unicode encoding. i.e. \uxxxx
//  2         - Encode as UTF8
//bool UString::m_XML_UTF16 = true;
//-------------------------------------------------------------------------------------------

tint UString::m_iUTFEncodingType = XML_UTF16;

//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER tchar BOTree<tchar,tchar>::nullIndex = '\0';
COMMON_TEMPLATE_MEMBER tchar BOTree<tchar,tchar>::nullObject = '\0';

//-------------------------------------------------------------------------------------------
//Method:
//    UString
//
//Purpose:
//    Object constructor. The overloaded members of the UString method
//    proved ways for importing ASCII and Unicode strings into the object
//    via it's constructor.
//
//Access:
//    Public
//
//Parameters:
//    str - The string object to make the instance equal to.
//    
//    len - The length of the string.
//             
//-------------------------------------------------------------------------------------------
//Create an empty string.
//-------------------------------------------------------------------------------------------

UString::UString() : m_Str(0),
    m_Len(0),
    m_memLen(0)
{}

//-------------------------------------------------------------------------------------------
//Import an ASCII C-String.
//-------------------------------------------------------------------------------------------

UString::UString(const tchar *str) : m_Str(0),
    m_Len(0),
    m_memLen(0) 
{
    tint len;
    
    if(str!=0) 
    {
        len = static_cast<tint>(::strlen(str));
        AppendToCurrent(str,len,0);
    }
}

//-------------------------------------------------------------------------------------------
//Import an ASCII C-String for a certain length.
//-------------------------------------------------------------------------------------------

UString::UString(const tchar *str,tint len) : m_Str(0),
    m_Len(0),
    m_memLen(0) 
{
    if(str!=0)
    {
        AppendToCurrent(str,len,0);
    }
}

//-------------------------------------------------------------------------------------------
//Import a Unicode C-String.
//-------------------------------------------------------------------------------------------

UString::UString(const tushort *str) : m_Str(0),
    m_Len(0),
    m_memLen(0) 
{
    tint len;
    
    if(str!=0)
    {
        len = CalcStrLength(str);
        AppendToCurrent(str,len,0);
    }
}

//-------------------------------------------------------------------------------------------
//Import a Unicode C-String for a certain length.
//-------------------------------------------------------------------------------------------

UString::UString(const tushort *str,tint len) : m_Str(0),
    m_Len(0),
    m_memLen(0) 
{
    if(str!=0)
    {
        AppendToCurrent(str,len,0);
    }
}

//-------------------------------------------------------------------------------------------
//Import an ASCII string represented by an instance of BString.
//-------------------------------------------------------------------------------------------

UString::UString(const BString& str) : m_Str(0),
    m_Len(0),
    m_memLen(0) 
{
    AppendToCurrent(static_cast<const tchar *>(str),static_cast<tint>(str.StrLen()),0);
}

//-------------------------------------------------------------------------------------------
//Import a Unicode string represented by another instance of UString.
//-------------------------------------------------------------------------------------------

UString::UString(const UString& str) : m_Str(0),
    m_Len(0),
    m_memLen(0) 
{
    AppendToCurrent(str.m_Str,str.m_Len,0);
}

//-------------------------------------------------------------------------------------------
//Method:
//    ~UString
//
//Purpose:
//    Object deconstructor. Free up string memory buffer if allocated.
//
//Access:
//    Public
//                 
//-------------------------------------------------------------------------------------------

UString::~UString() 
{
    if(m_Str!=0) 
    {
        delete [] m_Str;
        m_Str=0;
    }
}

//-------------------------------------------------------------------------------------------

UString& UString::null()
{
    static UString nullString;
    return nullString;
}

//-------------------------------------------------------------------------------------------

UString UString::null_object;

//-------------------------------------------------------------------------------------------
//Method:
//    PrintError
//
//Purpose:
//    Report an error in UString execution to the hyperterminal.
//
//Access:
//    Private
//
//Parameters:
//    strR   - String containing name of method in which error occurred.
//    
//    strE   - String containing error message.
//             
//-------------------------------------------------------------------------------------------

void UString::PrintError(const tchar *strR,const tchar *strE) const 
{
    common::Log::g_Log << "UString::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------
//Method:
//    Convert_ASCIIToUnicode
//
//Purpose:
//    This method will convert an ASCII C-string into a null terminated UTF16
//    big endian string. The state parser within the method looks '\u' encoded
//    Unicode characters and XML UTF-8 encoding and converts the strings
//    approriately. Namely of the form &#xxx; or \uxxxx.
//    
//    The parser can also look for certain character codes other than '\0' to
//    complete parsing. This is employed when processing XML text and the parser
//    in the method is used as part of a large parser.
//
//Access:
//    Private
//
//Parameters:
//    src    - Pointer to buffer holding the source ASCII string.
//
//    sLen   - [in] The number of character bytes to parse in the src input buffer.
//             [out] The last source character to be parsed.
//    
//    dest   - Pointer to buffer to place the Unicode string into.
//
//    dLen   - [in] The number of Unicode characters the dest output buffer can hold.
//             [out] The number of Unicode characters converted.
//
//    term   - Pointer to C-string containing the characters to terminate parsing on.
//             
//-------------------------------------------------------------------------------------------

void UString::Convert_ASCIIToUnicode(const tchar *src,tint& sLen,tushort *dest,tint& dLen,const tchar *term) const 
{
    tushort x=0;
    tint i,j,y=0,a,state=0;
    BOTree<tchar,tchar> terminal;
    
    //Setup list of termination characters. 
    if(term!=NULL) 
    {
        for(i=0;term[i]!='\0';i++) 
        {
            if(!terminal.Exist(term[i])) 
            {
                terminal[term[i]] = term[i];
            }
        }
    }
    
    i = 0;
    j = 0;

    if(dest!=0) 
    {
        //Variable 'a' is used to mark start positions for possible unicode
        //encoded characters. If -1 then it implies no marked position.
        a = -1;

        if(src!=0) 
        {
            //Parse source string until either end of source or destination buffers are reached
            //or the a terminal or end of C-string character is detected in the source stream.
            //N.B. The destination buffer ensures that there is space for a null character.
            while(i<sLen && j<(dLen-1) && !terminal.Exist(src[i]) && src[i]!='\0') 
            {
                
                switch(state) 
                {
                    //State - 0 : Look for first character of possible unicode encoding.
                    case 0:
                        {
                            if(src[i]=='&') 
                            {
                                a = i;
                                state = 1;
                            }
                            else if(src[i]=='\\') 
                            {
                                a = i;
                                state = 5;
                            }
                            else 
                            {
                                USTRARRAY_CHAR(dest,j,src[i])
                                j++;
                            }
                            i++;
                        }
                        break;
            
                    //State - 1 : Possible &#xxxx; encoding. Look for the '#'
                    case 1:
                        {
                            if(src[i]=='#') 
                            {
                                i++;
                                state = 2;
                            }
                            else 
                            { //Not &#xxxx; so '&' character is not part of unicode sequence.
                                i = a;
                                state = 6;
                            }
                        }
                        break;
            
                    //State - 2 : &#xxxx; encoding detected. Check to see if number is in decimal or hexidecimal form.
                    case 2:
                        {
                            x = 0; //Reset variable to obtain encoded number to zero.
                            if(src[i]=='x' || src[i]=='X') //Presence of 'x' implies hexidecimal format.
                            {
                                i++;
                                state = 4;
                            }
                            else 
                            {
                                state = 3;
                            }
                        }
                        break;
        
                    //State - 3 : Decode decimal number 'xxx' in &#xxx;
                    case 3:
                        {
                            if(src[i]>='0' && src[i]<='9') 
                            {
                                x *= 10;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - '0'));
                                i++;
                            }
                            else if(src[i]==';') //Detect end of number character.
                            {
                                i++;
                                state = 7;
                            }
                            else //Sequence is not of form &#xxx; so revert back to '&' and continue scan.
                            {
                                i = a;
                                state = 6;
                            }
                        }
                        break;
        
                    //State - 4 : Decode hexidecimal number 'yyy' in &#xyyy;
                    case 4:
                        {
                            if(src[i]>='0' && src[i]<='9') 
                            {
                                x <<= 4;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - '0'));
                                i++;
                            }
                            else if(src[i]>='A' && src[i]<='F') 
                            {
                                x <<= 4;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - 'A')) + 10;
                                i++;
                            }
                            else if(src[i]>='a' && src[i]<='f') 
                            {
                                x <<= 4;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - 'a')) + 10;
                                i++;                    
                            }
                            else if(src[i]==';') //Detect end of number character.
                            {
                                i++;
                                state=7;
                            }
                            else //Sequence is not of form &#xxx; so revert back to '&' and continue scan.
                            {
                                i = a;
                                state = 6;
                            }
                        }
                        break;
        
                    //State - 5 : Possible \uxxxx encoding detected. Look for 'u' in sequence.
                    case 5:
                        {
                            x = 0; //Reset variable to obtain encoded number to zero.
                            if(src[i]=='u') 
                            {
                                y=0;
                                i++;
                                state = 8;
                            }
                            else //Not \uxxxx; so '\' character is not part of unicode sequence.
                            {
                                i = a;
                                state = 6;
                            }
                        }
                        break;
        
                    //State - 6 : Used for moving parser back when possible unicode sequence has been transpired not to be.
                    case 6:
                        {
                            USTRARRAY_CHAR(dest,j,src[i]) //Insert starter character.
                            //Move to next character after start of possible sequence.
                            i++;
                            j++;
                            a = -1;
                            state=0;
                        }
                        break;
            
                    //State - 7 : Insert decoded unicode character.
                    case 7:
                        {
                            USTRARRAY_USHORT(dest,j,x)
                            j++;
                            a = -1;
                            state = 0;
                        }
                        break;

                    //State - 8 : Decode hexidecimal number in form \uxxxx sequence.
                    case 8:
                        {
                            //Decoded number cannot be more than a 4 character sequence.
                            if(x>=0x1000 || y>=4) 
                            {
                                state=7;
                            }
                            else if(src[i]>='0' && src[i]<='9') 
                            {
                                x <<= 4;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - '0'));
                                i++;
                                y++;
                            }
                            else if(src[i]>='A' && src[i]<='F') 
                            {
                                x <<= 4;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - 'A')) + 10;
                                i++;
                                y++;
                            }
                            else if(src[i]>='a' && src[i]<='f') 
                            {
                                x <<= 4;
                                x += static_cast<tushort>(static_cast<tuchar>(src[i] - 'a')) + 10;
                                i++;
                                y++;
                            }
                            else //End of sequence implied so save decoded character.
                            {
                                state=7;
                            }
                        }
                        break;
            
                    default:
                        break;    
                }
            }

            //If the end of stream is arrived at and unicode sequence was being decoded
            //ensure that the correct sequence is put in place.
            if(a!=-1 && j<(dLen-1)) 
            {
                if(state==7 || state==8) //Character is a unicode character.
                {
                    USTRARRAY_USHORT(dest,j,x)
                    j++;
                }
                else //Otherwise put in skipped sequence.
                {
                    i = a;
                    while(i<sLen && j<(dLen-1) && !terminal.Exist(src[i]) && src[i]!='\0') 
                    {
                        USTRARRAY_CHAR(dest,j,src[i])
                        i++;
                        j++;
                    }
                }
            }
        }
        
        dest[j]=0;
    }
    
    sLen = i; //Return the position of where the source was scanned to.
    dLen = j; //Return the last character position entered in on the Unicode array.
}

//-------------------------------------------------------------------------------------------
//Method:
//    AppendToCurrent
//
//Purpose:
//    Append a Unicode string to the current objects string. The method allocates
//    the memory approriately to fit the new string. The last character of the
//    string being appended becomes the new end of string.
//
//Access:
//    Private
//
//Parameters:
//    nStr   - Pointer to buffer containing Unicode string to be appended.
//    nLen   - The length of the new string.
//    offset - Offset in the current string to start appending the new string from.
//             
//-------------------------------------------------------------------------------------------

void UString::AppendToCurrent(const tushort *nStr,tint nLen,tint offset) 
{
    tint i,j;
    tushort *aMem;
    tint aLen;

    //Ensure that operation doesn't start appending outside of current string.
    if(offset<0)
    {
        offset = 0;
    }
    if(offset>m_Len)
    {
        offset = m_Len;
    }
    
    //Ensure that a string append operation is required.
    if(nStr!=0 && nLen>0) 
    {
        
        //Calculate length of new string.
        aLen = offset + nLen;

        //If the current string memory buffer is not large enough then resize
        //it such that new string. (This includes space for null terminator.)
        if(m_memLen<(aLen+1) || m_Str==0) 
        {
            aMem = new tushort [static_cast<tuint>(aLen + 1)];
            if(aMem==0) 
            {
                PrintError("AppendToCurrent","Out of memory"); //Report the error
                return;
            }
            if(m_Str!=0) 
            {
                //Copy the segment of the string that remains.
                for(i=0;i<offset;i++) 
                {
                    aMem[i] = m_Str[i];
                }
                delete [] m_Str;
            }
            m_Str = aMem;
            m_memLen = aLen + 1;
        }
        
        //Append the new string and terminate it approriately.
        for(i=offset,j=0 ; i<aLen ; i++,j++) 
        {
            m_Str[i] = nStr[j];
        }
        m_Str[aLen] = 0;
        m_Len = aLen;
    }
    else {
        //Ensure that if nothing is passed in then current string is appended from position
        //with an empty string.
        if(m_Str!=0) 
        {
            m_Str[offset]=0;
            m_Len=offset;
        }
        else 
        {
            m_Len=0;
        }
    }
}

//-------------------------------------------------------------------------------------------
//This variation of the method is used Append ASCII strings that require convertion to Unicode
//before they can be appended to the object's string.
//-------------------------------------------------------------------------------------------

void UString::AppendToCurrent(const tchar *nStr,tint nLen,tint offset) 
{
    tint iLen,oLen,pos;
    tushort tmp[256];
    
    if(nStr!=0) 
    {
        pos = 0;
        do 
        {
            iLen = nLen - pos;
            oLen = 256;
            Convert_ASCIIToUnicode(&nStr[pos],iLen,tmp,oLen);
            AppendToCurrent(tmp,oLen,pos+offset);
            pos += iLen;
        } while(pos < nLen);
    }
    else 
    {
        AppendToCurrent(tmp,0,offset); //lint !e603 As no length is passed then tmp array contents are not used.
    }
}

//-------------------------------------------------------------------------------------------
//Method:
//    CalcStrLength
//
//Purpose:
//    Obtain the length of the Unicode string by looking for the null character.
//
//Access:
//    Private
//
//Parameters:
//    str - Pointer to Unicode string.
//             
//-------------------------------------------------------------------------------------------

tint UString::CalcStrLength(const tushort *str) const 
{
    tint i=0;
    
    if(str!=0) 
    {
        while(str[i]!=0)
        {
            i++;
        }
    }
    return i;
}



//-------------------------------------------------------------------------------------------
//Method:
//    operator =
//
//Purpose:
//    String equality operator set of methods.
//    
//Access:
//    Public
//
//Parameters:
//    str - The string which UString is to equate to.   
//
//-------------------------------------------------------------------------------------------

UString& UString::operator = (const tchar *str) 
{
    tint len;
    
    if(str!=NULL)
    {
        len = static_cast<tint>(::strlen(str));
    }
    else
    {
        len = 0;
    }
    AppendToCurrent(str,len,0);
    return *this;
}

//-------------------------------------------------------------------------------------------

UString& UString::operator = (const tushort *str) 
{
    tint len;
    
    len = CalcStrLength(str);
    AppendToCurrent(str,len,0);
    return *this;
}

//-------------------------------------------------------------------------------------------

UString& UString::operator = (const BString& str) 
{
    AppendToCurrent(static_cast<const tchar *>(str),static_cast<tint>(str.StrLen()),0);
    return *this;
}

//-------------------------------------------------------------------------------------------

UString& UString::operator = (const UString& str) 
{
    if(&str!=this) 
    {
        AppendToCurrent(str.m_Str,str.m_Len,0);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------
//Method:
//    operator +=
//
//Purpose:
//    Add string onto end of current string.
//    
//Access:
//    Public
//
//Parameters:
//    str - The string which UString is to equate to.   
//
//-------------------------------------------------------------------------------------------

UString& UString::operator += (const tchar *str) 
{
    tint len;
    
    if(str!=NULL)
    {
        len = static_cast<tint>(::strlen(str));
    }
    else
    {
        len = 0;
    }
    AppendToCurrent(str,len,m_Len);
    return *this;
}

//-------------------------------------------------------------------------------------------

UString& UString::operator += (const tushort *str) 
{
    tint len;
    
    len = CalcStrLength(str);
    AppendToCurrent(str,len,m_Len);
    return *this;
}

//-------------------------------------------------------------------------------------------

UString& UString::operator += (const BString& str) 
{
    AppendToCurrent(static_cast<const tchar *>(str),static_cast<tint>(str.StrLen()),m_Len);
    return *this;    
}

//-------------------------------------------------------------------------------------------

UString& UString::operator += (const UString& str) 
{
    AppendToCurrent(str.m_Str,str.m_Len,m_Len);
    return *this;    
}

//-------------------------------------------------------------------------------------------
//Method:
//    operator <<
//
//Purpose:
//    Append string onto end of current string.
//    
//Access:
//    Public
//
//Parameters:
//    str - The string which UString is to equate to.   
//
//-------------------------------------------------------------------------------------------

UString& operator << (UString& in,const tchar *str) 
{
    in += str;
    return in;
}

//-------------------------------------------------------------------------------------------

UString& operator << (UString& in,const tushort *str) 
{
    in += str;
    return in;
}

//-------------------------------------------------------------------------------------------

UString& operator << (UString& in,const BString& str) 
{
    in += str;
    return in;
}

//-------------------------------------------------------------------------------------------

UString& operator << (UString& in,const UString& str) 
{
    in += str;
    return in;
}

//-------------------------------------------------------------------------------------------
//Method:
//    SetASCIIEncoding
//
//Purpose:
//    Set the global UString flag for specifying weither ASCII is encoded using
//    either XML or Standard Unicode character output.
//    
//Access:
//    Public
//
//Parameters:
//    type - The new value of the flag.
//             true  : Encode using XML UTF-16 encoding. i.e. &#xxxx;
//             false : Encode using standard Unicode encoding. i.e. \uxxxx
//
//-------------------------------------------------------------------------------------------

void UString::SetASCIIEncoding(tint type) 
{
    m_iUTFEncodingType = type;
}

//-------------------------------------------------------------------------------------------
//Method:
//    Encode_StandardUTF16
//
//Purpose:
//    Transform a Unicode character from UTF-16 into ASCII encoded for the
//    standard encoding mechanism. i.e. \uxxxx where xxxx is a four digit
//    hexidecimal number. N.B. The value of the character is in the endian
//    format native to the processor it is being executed on.
//    
//Access:
//    Private
//
//Parameters:
//    c - The Unicode character to be encoded.
//    
//Returns:
//    A BString instance encapsulating the character encoded in ASCII.
//
//-------------------------------------------------------------------------------------------

BString UString::Encode_StandardUTF16(tushort c) const 
{
    tint x,y,n=0;
    tchar t[2]={'\0','\0'};
    BString a;
    
    x = static_cast<tint>(c);
    
    while(n<4) 
    {
        y = x&0x0000000f;
        if(y>=10) 
        {
            t[0] = static_cast<tchar>(y-10) + 'a';
        }
        else 
        {
            t[0] = static_cast<tchar>(y) + '0';
        }
        x = static_cast<tint>(static_cast<tuint>(x) >> 4);
        a = t + a;
        n++;    
    }
    a = "\\u" + a;
    return a;
}

//-------------------------------------------------------------------------------------------
//Method:
//    Encode_XMLUTF16
//
//Purpose:
//    Transform a Unicode character from UTF-16 into ASCII encoded following
//    the XML encoding mechanism. i.e. &#xxxx; where xxxx is a decimal digit
//    number. N.B. The value of the character is in the endian
//    format native to the processor it is being executed on.
//    
//Access:
//    Private
//
//Parameters:
//    c - The Unicode character to be encoded.
//    
//Returns:
//    A BString instance encapsulating the character encoded in XML ASCII.
//
//-------------------------------------------------------------------------------------------

BString UString::Encode_XMLUTF16(tushort c) const 
{
    tint x;
    tchar t[2]={'\0','\0'};
    BString a(";");
    
    x = static_cast<tint>(c);
    
    if(x) 
    {
        while(x) 
        {
            t[0] = static_cast<tchar>(x%10) + '0';
            a = t + a;
            x/=10;
        }
        a = "&#" + a;
    }
    else 
    {
        a = "&#0;";
    }
    return a;
}

//-------------------------------------------------------------------------------------------

BString UString::Encode_StandardUTF8(tushort c) const 
{
    tuchar encoded[4];
    BString b;
    
    if(c < 0x80)
    {
        encoded[0] = static_cast<tuchar>(c);
        encoded[1] = '\0';
    }
    else if(c < 0x0800)
    {
        encoded[0] = static_cast<tuchar>(0xC0) | static_cast<tuchar>((c & 0x07C0) >> 6);
        encoded[1] = static_cast<tuchar>(0x80) | static_cast<tuchar>(c & 0x003F);
        encoded[2] = '\0';
    }
    else
    {
        encoded[0] = static_cast<tuchar>(0xE0) | static_cast<tuchar>((c & 0xF000) >> 12);
        encoded[1] = static_cast<tuchar>(0x80) | static_cast<tuchar>((c & 0x0FC0) >> 6);
        encoded[2] = static_cast<tuchar>(0x80) | static_cast<tuchar>(c & 0x003F);
        encoded[3] = '\0';
    }
    b << encoded;
    return b;
}

//-------------------------------------------------------------------------------------------

BString UString::Encode_StandardASCII(tushort c) const
{
    BString b;
    tuchar t;

    t = static_cast<tuchar>(c & static_cast<tushort>(0x00ff));
    b += static_cast<tchar>(t);
    return b;
}

//-------------------------------------------------------------------------------------------
//Method:
//    Convert_UnicodeToASCII
//
//Purpose:
//    Converts the instance's Unicode string into an ASCII string using the
//    format perscribed by the m_XML_UTF16 global flag.
//    
//Access:
//    Private
//
//Returns:
//    The ASCII encoded variation of this instance's string, encapsulated as
//    an instance of BString.
//
//-------------------------------------------------------------------------------------------

BString UString::Convert_UnicodeToASCII() const 
{
    tint i;
    tushort c;
    BString a,b;
    BStringCollection x;
    char t[2] = {'\0','\0'};
    
    if(m_Str!=0) 
    {
        for(i=0;i<m_Len;i++) 
        {
            USTR_VALUE(c,m_Str[i])
            if(c<128) 
            {
                t[0] = static_cast<tchar>(c);
                x << t; //lint !e534 Return from operation is reference pointer to self.i.e. 'x'.
            }
            else 
            {
                if(XML_UTF16 == m_iUTFEncodingType) 
                {
                    b = Encode_XMLUTF16(c);
                }
                else if (STANDARD_UTF16 == m_iUTFEncodingType)
                {
                    b = Encode_StandardUTF16(c);
                }
                else if (STANDARD_UTF8 == m_iUTFEncodingType)
                {
                    b = Encode_StandardUTF8(c);
                }
                else if(STANDARD_ASCII_8BIT == m_iUTFEncodingType)
                {
                    b = Encode_StandardASCII(c);
                }
                x << b; //lint !e534 Return from operation is reference pointer to self.i.e. 'x'.
            }
        }
    }
    
    if(!x.Group(a)) 
    {
        PrintError("Convert_UnicodeToASCII","Error in grouping final string");
    }
    return a;
}

//-------------------------------------------------------------------------------------------
//Method:
//    operator BString
//
//Purpose:
//    Provide a method for implicit convertion from 16-bit Unicode to a ASCII
//    BString.
//    
//Access:
//    Public
//
//Returns:
//    The ASCII encoded variation of this instance's string, encapsulated as
//    an instance of BString.
//
//-------------------------------------------------------------------------------------------

BString UString::AStr() const 
{
    return Convert_UnicodeToASCII();
}

//-------------------------------------------------------------------------------------------

UString::operator BString() const
{
    return Convert_UnicodeToASCII();
}

//-------------------------------------------------------------------------------------------
//Method:
//    Compare
//
//Purpose:
//    Compares the contents of Unicode string buffers.
//    
//Access:
//    Private
//
//Parameters:
//    a - Pointer to buffer for first string.
//    
//    b - Pointer to buffer for second string.
//    
//Returns:
//    a < b  -> -1
//    a == b ->  0
//    a > b  ->  1
//
//-------------------------------------------------------------------------------------------

tint UString::Compare(const tushort *a,const tushort *b) 
{
    tint i=0;
    
    if(a==0) 
    {
        if(b==0) 
        {
            return 0;        //a == b
        }
        else 
        {
            return -1;        //a < b
        }
    }
    else if(b==0) 
    {
        return 1;            //a > b
    }

    while(a[i]!=0 && b[i]!=0) 
    {
        if(a[i] < b[i]) 
        {
            return -1;        //a < b
        }
        else if(a[i] > b[i]) 
        {
            return 1;        //a > b
        }
        i++;
    }
    
    if(a[i]==0) 
    {
        if(b[i]==0) 
        {
            return 0;        //a == b
        }
        else 
        {
            return -1;        //a < b
        }
    }
    //Implies a[i]!=0 && b[i]==0 -> a > b
    return 1;
}

//-------------------------------------------------------------------------------------------
//Function:
//    operator ==
//
//Purpose:
//    The set of equality comparison functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    1 if a == b else 0.
//
//-------------------------------------------------------------------------------------------

tint operator == (const tchar *a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator == (const UString& a,const tchar *b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator == (const tushort *a,const UString& b) 
{
    return (UString::Compare(a,b.m_Str) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator == (const UString& a,const tushort *b) 
{
    return (UString::Compare(a.m_Str,b) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator == (const BString& a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator == (const UString& a,const BString& b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator == (const UString& a,const UString& b) 
{
    return (UString::Compare(a.m_Str,b.m_Str) == 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Function:
//    operator >=
//
//Purpose:
//    The set of greater than or equal to comparison functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    1 if a >= b else 0.
//
//-------------------------------------------------------------------------------------------

tint operator >= (const tchar *a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const UString& a,const tchar *b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const tushort *a,const UString& b) 
{
    return (UString::Compare(a,b.m_Str) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const UString& a,const tushort *b) 
{
    return (UString::Compare(a.m_Str,b) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const BString& a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const UString& a,const BString& b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator >= (const UString& a,const UString& b) 
{
    return (UString::Compare(a.m_Str,b.m_Str) >= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Function:
//    operator >
//
//Purpose:
//    The set of greater than comparison functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    1 if a > b else 0.
//
//-------------------------------------------------------------------------------------------

tint operator > (const tchar *a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const UString& a,const tchar *b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const tushort *a,const UString& b) 
{
    return (UString::Compare(a,b.m_Str) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const UString& a,const tushort *b) 
{
    return (UString::Compare(a.m_Str,b) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const BString& a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const UString& a,const BString& b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator > (const UString& a,const UString& b) 
{
    return (UString::Compare(a.m_Str,b.m_Str) > 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Function:
//    operator <=
//
//Purpose:
//    The set of less than or equal to comparison functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    1 if a <= b else 0.
//
//-------------------------------------------------------------------------------------------

tint operator <= (const tchar *a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const UString& a,const tchar *b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const tushort *a,const UString& b) 
{
    return (UString::Compare(a,b.m_Str) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const UString& a,const tushort *b) 
{
    return (UString::Compare(a.m_Str,b) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const BString& a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const UString& a,const BString& b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator <= (const UString& a,const UString& b) 
{
    return (UString::Compare(a.m_Str,b.m_Str) <= 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Function:
//    operator <
//
//Purpose:
//    The set of less than comparison functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    1 if a < b else 0.
//
//-------------------------------------------------------------------------------------------

tint operator < (const tchar *a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const UString& a,const tchar *b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const tushort *a,const UString& b) 
{
    return (UString::Compare(a,b.m_Str) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const UString& a,const tushort *b) 
{
    return (UString::Compare(a.m_Str,b) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const BString& a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const UString& a,const BString& b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator < (const UString& a,const UString& b) 
{
    return (UString::Compare(a.m_Str,b.m_Str) < 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Function:
//    operator !=
//
//Purpose:
//    The set of inequality comparison functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    1 if a == b else 0.
//
//-------------------------------------------------------------------------------------------

tint operator != (const tchar *a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator != (const UString& a,const tchar *b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator != (const tushort *a,const UString& b) 
{
    return (UString::Compare(a,b.m_Str) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator != (const UString& a,const tushort *b) 
{
    return (UString::Compare(a.m_Str,b) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator != (const BString& a,const UString& b) 
{
    UString c(a);
    return (UString::Compare(c.m_Str,b.m_Str) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator != (const UString& a,const BString& b) 
{
    UString d(b);
    return (UString::Compare(a.m_Str,d.m_Str) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

tint operator != (const UString& a,const UString& b) 
{
    return (UString::Compare(a.m_Str,b.m_Str) != 0) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------
//Function:
//    Sub
//
//Purpose:
//    Create and return an instance of UString holding a specified sub-string of
//    the called instance.
//    
//Parameters:
//    offset - The offset from the current string to start the sub-string.
//    len    - The length of the sub-string. If zero then the rest of the string is
//             taken.
//    
//Returns:
//    An instance of UString containing the sub-string.
//
//-------------------------------------------------------------------------------------------

UString UString::mid(tint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::mid(tuint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::mid(tint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::mid(tuint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Mid(tint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Mid(tuint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Mid(tint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Mid(tuint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::sub(tint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::sub(tuint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::sub(tint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::sub(tuint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Sub(tuint offset,tint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Sub(tint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Sub(tuint offset,tuint len) const
{
    return Sub(static_cast<tint>(offset),static_cast<tint>(len));
}

//-------------------------------------------------------------------------------------------

UString UString::Sub(tint offset,tint len) const 
{
    tint remain;
    UString a;
    
    if(m_Str!=0) 
    {
        //Restrict offset to be in the boundaries of the string.
        if(offset < 0)
        {
            offset = 0;
        }
        if(offset > m_Len)
        {
            offset = m_Len;
        }
        
        //Restrict the length of the sub-string to be in bounds.
        remain = m_Len - offset;
        if(len<=0)
        {
            len = remain;
        }
        if(len > remain)
        {
            len = remain;
        }

        a.AppendToCurrent(&m_Str[offset],len,0);
    }
    return a;
}

//==============================================================================
//Function:
//    operator +
//
//Purpose:
//    The set of addition operator functions associated to UString.
//    
//Parameters:
//    a - First string.
//    b - Second string.
//    
//Returns:
//    UString instance of a + b
//
//==============================================================================

UString operator + (const tchar *a,const UString& b) 
{
    UString c(a);
    c += b;
    return c;
}

//------------------------------------------------------------------------------

UString operator + (const UString& a,const tchar *b) 
{
    UString c(a);
    c += b;
    return c;    
}

//------------------------------------------------------------------------------

UString operator + (const tushort *a,const UString& b) 
{
    UString c(a);
    c += b;
    return c;
}

//------------------------------------------------------------------------------

UString operator + (const UString& a,const tushort *b) 
{
    UString c(a);
    c += b;
    return c;
}

//------------------------------------------------------------------------------

UString operator + (const BString& a,const UString& b) 
{
    UString c(a);
    c += b;
    return c;    
}

//------------------------------------------------------------------------------

UString operator + (const UString& a,const BString& b) 
{
    UString c(a);
    c += b;
    return c;
}

//------------------------------------------------------------------------------

UString operator + (const UString& a,const UString& b) 
{
    UString c(a);
    c += b;
    return c;
}

//------------------------------------------------------------------------------

void UString::EncodeUTF8(const UString& in,BString& out)
{
    tint i,offset,len = in.Length();
    const tushort *x = static_cast<const tushort *>(in);
    tuchar tmp[128];
    
    out = "";
    for(i=0,offset=0;i<len;i++)
    {
        if(offset>=124)
        {
            tmp[offset] = '\0';
            out += tmp;
            offset = 0;
        }
        
        if(x[i]>=0x0000 && x[i]<=0x007F)
        {
            tmp[offset] = static_cast<tuchar>(x[i] & 0x007F);
            offset++;
        }
        else if(x[i]>=0x0080 && x[i]<=0x07FF)
        {
            tmp[offset] = static_cast<tuchar>(0xC0) | static_cast<tuchar>((x[i] >> 6) & 0x001F);
            tmp[offset + 1] = static_cast<tuchar>(0x80) | static_cast<tuchar>(x[i] & 0x003F);
            offset += 2;
        }
        else
        {
            tmp[offset + 0] = static_cast<tuchar>(0xE0) | static_cast<tuchar>((x[i] >> 12) & 0x000F);
            tmp[offset + 1] = static_cast<tuchar>(0x80) | static_cast<tuchar>((x[i] >> 6) & 0x003F);
            tmp[offset + 2] = static_cast<tuchar>(0x80) | static_cast<tuchar>(x[i] & 0x003F);
            offset += 3;
        }
    }
    tmp[offset] = '\0';
    out += tmp;
}

//------------------------------------------------------------------------------

void UString::DecodeUTF8(const BString& in,UString& out)
{
    tint i,offset,len = in.GetLength();
    const tuchar *x = reinterpret_cast<const tuchar *>(static_cast<const tchar *>(in));
    tushort tmp[128];
    
    out = "";
    for(i=0,offset=0;i<len;offset++)
    {
        if(offset>=127)
        {
            tmp[offset] = 0;
            out += tmp;
            offset = 0;
        }
            
        if((x[i] & 0xC0) != 0x80)
        {
            if(!(x[i] & 0x80))
            {
                tmp[offset] = static_cast<tushort>(x[i]);
                i++;
            }
            else if(((x[i] & 0xE0) == 0xC0) && i<(len-1))
            {
                tmp[offset] = (static_cast<tushort>(x[i] & 0x1F) << 6) 
                    | static_cast<tushort>(x[i + 1] & 0x3F);
                i += 2;
            }
            else if(((x[i] & 0xF0) == 0xE0) && i<(len-2))
            {
                tmp[offset] = (static_cast<tushort>(x[i] & 0x0F) << 12)
                    | (static_cast<tushort>(x[i + 1] & 0x3F) << 6)
                    | static_cast<tushort>(x[i + 2] & 0x3F);
                i += 3;
            }
            else
            {
                i++;
                offset--;
            }
        }
        else
        {
            i++;
            offset--;
        }
    }
    tmp[offset] = 0;
    out += tmp;
}


//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
