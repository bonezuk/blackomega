#include "network/mime/inc/MimeForm.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

MimeForm::MimeForm() : m_boundary(),
    m_sections(),
    m_parseResult(true),
    m_parsePosition(-1)
{}

//-------------------------------------------------------------------------------------------

MimeForm::MimeForm(const tchar *mem,tint len) : m_boundary(),
    m_sections(),
    m_parseResult(true),
    m_parsePosition(-1)
{
    m_parseResult = parse(mem,len);
}

//-------------------------------------------------------------------------------------------

MimeForm::MimeForm(const NetArray& array) : m_boundary(),
    m_sections(),
    m_parseResult(true),
    m_parsePosition(-1)
{
    m_parseResult = parse(array);
}

//-------------------------------------------------------------------------------------------

MimeForm::MimeForm(const MimeForm& form) : m_boundary(),
    m_sections(),
    m_parseResult(true),
    m_parsePosition(-1)
{
    copy(form);
}

//-------------------------------------------------------------------------------------------

MimeForm::~MimeForm()
{}

//-------------------------------------------------------------------------------------------

void MimeForm::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "MimeForm::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::parse(const tchar *mem,tint len)
{
    tint i = 0,offset = 0;
    bool res = true;
    common::BString errStr;
    MimeSection part;

    if(mem==0 || len<=0)
    {
        printError("parse","No buffer given to parse");
        return false;
    }

    blank();

    while(res)
    {
        if(part.set(&mem[offset],len - offset))
        {
            if(part.isValid())
            {
                m_sections.append(part);
                offset += part.parseEndPosition();
                i++;

                if(part.isEnd())
                {
                    break;
                }
            }
            else
            {
                errStr = "Mime section number " + common::BString::Int(i) + " failed validation";
                printError("parse",static_cast<const tchar *>(errStr));
                res = false;
            }
        }
        else
        {
            errStr = "Error parsing mime section number " + common::BString::Int(i);
            printError("parse",static_cast<const tchar *>(errStr));
            res = false;
        }
    }

    if(res)
    {
        if(i>0)
        {
            m_boundary = part.boundary();
            m_parsePosition = offset;
        }
        else
        {
            printError("parse","No sections found in mime form");
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::parse(const NetArray& array)
{
    return parse(reinterpret_cast<const tchar *>(array.GetData()),array.GetSize());
}

//-------------------------------------------------------------------------------------------

void MimeForm::blank()
{
    m_boundary = "";
    m_parseResult = true;
    m_parsePosition = -1;
    m_sections.clear();
}

//-------------------------------------------------------------------------------------------

void MimeForm::copy(const MimeForm& form)
{
    blank();
    m_boundary = form.m_boundary;
    m_parseResult = form.m_parseResult;
    m_parsePosition = form.m_parsePosition;
    m_sections = form.m_sections;
}

//-------------------------------------------------------------------------------------------

const MimeForm& MimeForm::operator = (const NetArray& array)
{
    m_parseResult = parse(array);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeForm& MimeForm::operator = (const MimeForm& form)
{
    if(this!=&form)
    {
        copy(form);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::set(const tchar *mem,tint len)
{
    m_parseResult = parse(mem,len);
    return m_parseResult;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::set(const NetArray& array)
{
    m_parseResult = parse(array);
    return m_parseResult;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::set(const MimeForm& form)
{
    copy(form);
    return true;
}

//-------------------------------------------------------------------------------------------

tint MimeForm::size() const
{
    return m_sections.size();
}

//-------------------------------------------------------------------------------------------

MimeSection& MimeForm::section(tint idx)
{
    return const_cast<MimeSection&>(m_sections.at(idx));
}

//-------------------------------------------------------------------------------------------

const MimeSection& MimeForm::section(tint idx) const
{
    return m_sections.at(idx);
}

//-------------------------------------------------------------------------------------------

bool MimeForm::append(const MimeSection& section)
{
    bool res = false;

    if(section.isValid())
    {
        if(m_sections.size()>0)
        {
            m_sections[m_sections.size() - 1].m_endSection = false;
        }
        m_sections.append(section);
        m_sections[m_sections.size() - 1].m_endSection = true;
        res = true;

        if(m_boundary.isEmpty())
        {
            m_boundary = section.boundary();
        }
        else
        {
            m_sections[m_sections.size() - 1].setBoundary(m_boundary);
        }
    }
    else
    {
        printError("append","The mime section to append has failed validation");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeForm::boundary() const
{
    return m_boundary;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::setBoundary(const tchar *str)
{
    m_boundary = str;
    return true;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::setBoundary(const common::BString& str)
{
    m_boundary = str;
    return true;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::setBoundary(const QString& str)
{
    m_boundary = str.toUtf8().constData();
    return true;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::compile(NetArray& array) const
{
    tint i = 0;
    QList<MimeSection>::const_iterator ppI;
    bool res = true;

    if(!isValid())
    {
        printError("compile","Mime form failed validation");
        return false;
    }

    for(ppI=m_sections.begin();ppI!=m_sections.end();++ppI,++i)
    {
        const MimeSection& section = *ppI;

        if(section.compile(array,(!i) ? true : false))
        {
            printError("compile","Error compiling mime section");
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MimeForm::isValid() const
{
    tint i=0,size=m_sections.size();
    common::BString errStr;
    bool res = true;

    if(m_boundary.isEmpty())
    {
        printError("isValid","No mime boundary defined");
        res = false;
    }

    if(size)
    {
        while(i<size)
        {
            if(!m_sections.at(i).isValid())
            {
                errStr = "Mime section number " + common::BString::Int(i) + "failed validation";
                printError("isValid",static_cast<const tchar *>(errStr));
                res = false;
            }
            if(i < (size - 1))
            {
                if(m_sections.at(i).isEnd())
                {
                    printError("isValid","Mime section is defined as end section when others still proceed it");
                    res = false;
                }
            }
            else
            {
                if(!m_sections.at(i).isEnd())
                {
                    printError("isValid","Final mime section is not defined as end section");
                    res = false;
                }
            }
            i++;
        }
    }
    else
    {
        printError("isValid","No mime sections defined");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint MimeForm::parseEndPosition() const
{
    return m_parsePosition;
}

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
