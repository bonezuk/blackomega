#include "engine/silveromega/inc/VSilverCodecComments.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverCodecComments::VSilverCodecComments() : VSilverInfoObject(),
    m_vendor(),
    m_nullStr(),
    m_comments()
{}

//-------------------------------------------------------------------------------------------

VSilverCodecComments::~VSilverCodecComments()
{}

//-------------------------------------------------------------------------------------------

void VSilverCodecComments::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverCodecComments::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecComments::isValid() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodecComments::read(engine::Sequence *seq)
{
    tuint i,noComments;
    QString index,body;
    
    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }
        
    skipPacketIdentifer(seq);
    readString(seq,m_vendor);
        
    noComments = static_cast<tuint>(seq->readBits(32));
    for(i=0;i<noComments;++i)
    {
        tint idx;
        
        readString(seq,body);
        idx = body.indexOf(QChar('='));
        if(idx>=0)
        {
            index = body.left(idx).trimmed().toLower();
            body = body.mid(idx+1);
            if(!index.isEmpty())
            {
                m_comments.insert(index,body);
            }
        }
    }
    
    if(!completePacket(seq))
    {
        printError("read","Information packet not properly terminated");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::vendor() const
{
    return m_vendor;
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::comment(const tchar *str) const
{
    QString s(QString::fromLatin1(str));
    return comment(s);
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::comment(const QString& str) const
{
    QMap<QString,QString>::const_iterator iter = m_comments.find(str);
    
    if(iter!=m_comments.end())
    {
        return iter.value();
    }
    else
    {
        return m_nullStr;
    }
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::title() const
{
    return comment("title");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::version() const
{
    return comment("version");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::album() const
{
    return comment("album");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::trackNumber() const
{
    return comment("tracknumber");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::artist() const
{
    return comment("artist");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::performer() const
{
    return comment("performer");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::copyright() const
{
    return comment("copyright");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::license() const
{
    return comment("license");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::organization() const
{
    return comment("organization");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::description() const
{
    return comment("description");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::genre() const
{
    return comment("genre");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::date() const
{
    return comment("date");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::location() const
{
    return comment("location");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::contact() const
{
    return comment("contact");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::isrc() const
{
    return comment("isrc");
}

//-------------------------------------------------------------------------------------------

const QString& VSilverCodecComments::diskNumber() const
{
    return comment("discnumber");
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
