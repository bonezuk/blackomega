#include "network/mime/inc/MimeSection.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

MimeSection::MimeSection() : m_boundary(),
    m_header(),
    m_body(),
    m_parsePosition(-1),
    m_parseResult(true),
    m_endSection(false)
{}

//-------------------------------------------------------------------------------------------

MimeSection::MimeSection(const tchar *mem,tint len) : m_boundary(),
    m_header(),
    m_body(),
    m_parsePosition(-1),
    m_parseResult(true),
    m_endSection(false)
{
    m_parseResult = parse(mem,len);
}

//-------------------------------------------------------------------------------------------

MimeSection::MimeSection(const NetArray& array) : m_boundary(),
    m_header(),
    m_body(),
    m_parsePosition(-1),
    m_parseResult(true),
    m_endSection(false)
{
    m_parseResult = parse(array);
}

//-------------------------------------------------------------------------------------------

MimeSection::MimeSection(const MimeSection& section) : m_boundary(),
    m_header(),
    m_body(),
    m_parsePosition(-1),
    m_parseResult(true),
    m_endSection(false)
{
    copy(section);
}

//-------------------------------------------------------------------------------------------

MimeSection::~MimeSection()
{}

//-------------------------------------------------------------------------------------------

void MimeSection::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "MimeSection::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::parseBoundaryLine(const tchar *mem,tint len,tint& offset)
{
    tint bodyEnd = offset;
    return parseBoundaryLine(mem,len,offset,bodyEnd);
}

//-------------------------------------------------------------------------------------------

bool MimeSection::parseBoundaryLine(const tchar *mem,tint len,tint& offset,tint& bodyEnd)
{
    tint state = 0;
    bool res = false;

    if(mem==0 || offset>=len)
    {
        printError("parseBoundaryLine","Requested memory buffer out of range");
        return false;
    }

    if(!m_boundary.isEmpty())
    {
        tint back = offset;
        common::BString str;
        const tchar *s;

        str = "--" + m_boundary;
        s = static_cast<const tchar *>(str);

        while(offset<len && !res)
        {
            state = 0;

            while(offset<len && state<str.length())
            {
                if(s[state]==mem[offset])
                {
                    if(!state)
                    {
                        bodyEnd = offset;
                        back = offset;
                    }
                    state++;
                }
                else if(state)
                {
                    offset = back;
                    state = 0;
                }
                offset++;
            }

            if(state==str.length())
            {
                if(offset<len && mem[offset]=='\n')
                {
                    offset++;
                    m_endSection = false;
                    res = true;
                }
                else if(offset<(len + 1) && ::memcmp(&mem[offset],"\r\n",2)==0)
                {
                    offset += 2;
                    m_endSection = false;
                    res = true;
                }
                else if(offset<(len + 2) && ::memcmp(&mem[offset],"--\n",3)==0)
                {
                    offset += 3;
                    m_endSection = true;
                    res = true;
                }
                else if(offset<(len + 3) && ::memcmp(&mem[offset],"--\r\n",4)==0)
                {
                    offset += 4;
                    m_endSection = true;
                    res = true;
                }
                else if(offset<(len + 2) && ::memcmp(&mem[offset],"--",2)==0)
                {
                    offset += 2;
                    m_endSection = true;
                    res = true;
                }
                else
                {
                    offset = back + 1;
                }
            }
        }
    }
    else
    {
        tint start = offset,end = offset;

        while(offset<len && state<4)
        {
            switch(state)
            {
                case 0:
                    if(mem[offset]=='-')
                    {
                        state = 1;
                    }
                    break;

                case 1:
                    if(mem[offset]=='-')
                    {
                        start = offset + 1;
                        state = 2;
                    }
                    else
                    {
                        state = 0;
                    }
                    break;

                case 2:
                    if(mem[offset]=='\n')
                    {
                        end = offset;
                        state = 4;
                    }
                    else if(mem[offset]=='\r')
                    {
                        end = offset;
                        state = 3;
                    }
                    break;

                case 3:
                    if(mem[offset]=='\n')
                    {
                        state = 4;
                    }
                    else
                    {
                        state = 2;
                    }
                    break;

                default:
                    break;
            }
            if(state==4)
            {
                if(end > start)
                {
                    common::BString str(&mem[start],static_cast<tuint>(end - start));
                    m_boundary = str;
                    res = true;
                }
            }
            offset++;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::parseForHeader(const tchar *mem,tint len,tint& offset,common::BString& hdrStr) const
{
    tint start = offset,state = 0;
    bool res = false,contLine = false;

    if(mem==0 || offset>len)
    {
        printError("parseForHeader","Requested memory buffer out of range");
        return false;
    }

    while(offset<len && !res)
    {
        switch(state)
        {
            case 0:
                if(mem[offset]==':')
                {
                    contLine = true;
                }
                else if(mem[offset]=='\r')
                {
                    state = 1;
                }
                else if(mem[offset]=='\n')
                {
                    state = 2;
                }
                break;

            case 1:
                if(mem[offset]=='\n')
                {
                    state = 2;
                }
                else
                {
                    if(mem[offset]==':')
                    {
                        contLine = true;
                    }
                    state = 0;
                }
                break;

            default:
                break;
        }
        offset++;

        if(state==2)
        {
            if(!contLine)
            {
                common::BString str(&mem[start],static_cast<tuint>(offset - start));
                hdrStr = str;
                res = true;
            }
            contLine = false;
            state = 0;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::parse(const NetArray& array)
{
    return parse(reinterpret_cast<const tchar *>(array.GetData()),array.GetSize());
}

//-------------------------------------------------------------------------------------------

bool MimeSection::parse(const tchar *mem,tint len)
{
    tint i = 0;
    common::BString hdrStr,tmp;
    bool res = false;

    tmp = m_boundary;
    blank();
    m_boundary = tmp;

    if(parseBoundaryLine(mem,len,i))
    {
        if(parseForHeader(mem,len,i,hdrStr))
        {
            m_header = hdrStr;

            if(m_header.isValid())
            {
                tint bodyEnd = i,bodyStart = i;

                if(parseBoundaryLine(mem,len,i,bodyEnd))
                {
                    if(bodyEnd > bodyStart)
                    {
                        m_body.AppendRaw(&mem[bodyStart],bodyEnd - bodyStart);
                    }
                    if(m_endSection)
                    {
                        m_parsePosition = i;
                    }
                    else
                    {
                        m_parsePosition = bodyEnd;
                    }
                    res = true;
                }
                else
                {
                    printError("parse","Failed to locate end boundary for section");
                }
            }
            else
            {
                printError("parse","Parsed mime header is not valid");
            }
        }
        else
        {
            printError("parse","Failed to locate header for mime body");
        }
    }
    else
    {
        printError("parse","Failed to find start boundary for section");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void MimeSection::blank()
{
    m_boundary = "";
    m_header.blank();
    m_body.RemoveAll();
    m_parsePosition = -1;
    m_parseResult = true;
    m_endSection = false;
}

//-------------------------------------------------------------------------------------------

void MimeSection::copy(const MimeSection& section)
{
    blank();
    m_boundary = section.m_boundary;
    m_header = section.m_header;
    m_body.Append(section.m_body);
    m_parsePosition = section.m_parsePosition;
    m_parseResult = section.m_parseResult;
    m_endSection = section.m_endSection;
}

//-------------------------------------------------------------------------------------------

const MimeSection& MimeSection::operator = (const NetArray& array)
{
    m_parseResult = parse(array);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeSection& MimeSection::operator = (const MimeSection& section)
{
    if(&section!=this)
    {
        copy(section);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::set(const tchar *mem,tint len)
{
    m_parseResult = parse(mem,len);
    return m_parseResult;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::set(const NetArray& array)
{
    m_parseResult = parse(array);
    return m_parseResult;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::set(const MimeSection& section)
{
    copy(section);
    return true;
}

//-------------------------------------------------------------------------------------------

MimeHeader& MimeSection::header()
{
    return m_header;
}

//-------------------------------------------------------------------------------------------

const MimeHeader& MimeSection::header() const
{
    return m_header;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setHeader(const tchar *str)
{
    MimeHeader hdr(str);
    return setHeader(hdr);
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setHeader(const common::BString& str)
{
    MimeHeader hdr(str);
    return setHeader(hdr);
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setHeader(const QString& str)
{
    MimeHeader hdr(str);
    return setHeader(hdr);
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setHeader(const MimeHeader& hdr)
{
    if(hdr.isValid())
    {
        m_header = hdr;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------

NetArray& MimeSection::body()
{
    return m_body;
}

//-------------------------------------------------------------------------------------------

const NetArray& MimeSection::body() const
{
    return m_body;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setBody(const tchar *mem,tint len)
{
    m_body.RemoveAll();
    if(mem!=0 && len>0)
    {
        m_body.AppendRaw(mem,len);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setBody(const NetArray& array)
{
    m_body.RemoveAll();
    m_body.Append(array);
    return true;
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeSection::boundary() const
{
    return m_boundary;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setBoundary(const tchar *str)
{
    common::BString s(str);
    return setBoundary(s);
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setBoundary(const common::BString& str)
{
    if(!str.isEmpty())
    {
        m_boundary = str;
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::setBoundary(const QString& str)
{
    common::BString s(str.toUtf8().constData());
    return setBoundary(s);
}

//-------------------------------------------------------------------------------------------

bool MimeSection::compile(NetArray& array,bool doStart) const
{
    common::BString header,boundary;

    if(!isValid())
    {
        printError("compile","Mime section is not valid");
    }

    boundary = "--" + m_boundary + common::c_endl;

    if(doStart)
    {
        array.AppendRaw(static_cast<const tchar *>(boundary),boundary.length());
    }

    m_header.compile(header);
    array.AppendRaw(static_cast<const tchar *>(header),header.length());
    array.Append(m_body);
    array.AppendRaw("\r\n",2);

    if(m_endSection)
    {
        boundary = "--" + m_boundary + "--" + common::c_endl;
    }
    array.AppendRaw(static_cast<const tchar *>(boundary),boundary.length());

    return true;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::isEnd() const
{
    return m_endSection;
}

//-------------------------------------------------------------------------------------------

bool MimeSection::isValid() const
{
    bool res = true;

    if(m_parseResult)
    {
        if(m_boundary.isEmpty())
        {
            printError("isValid","No mime boundary has been defined");
            res = false;
        }
        if(!m_header.isValid())
        {
            printError("isValid","Mime header is not valid");
            res = false;
        }
    }
    else
    {
        printError("isValid","Parsing of original mime section failed");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint MimeSection::parseEndPosition() const
{
    return m_parsePosition;
}

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
