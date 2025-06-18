#include "engine/greenomega/inc/FLACMetaComment.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACMetaComment::FLACMetaComment() : FLACMetaBase(e_Comment),
    m_vendor(),
    m_nullStr(),
    m_comments()
{}

//-------------------------------------------------------------------------------------------

FLACMetaComment::~FLACMetaComment()
{}

//-------------------------------------------------------------------------------------------

QString FLACMetaComment::readString(engine::Sequence *seq)
{
    tint i,len;
    tchar *tmp;
    QString s;
    
    len = seq->readBitsIL(32);
        
    tmp = new tchar [len + 1];
    for(i=0;i<len;i++)
    {
        tmp[i] = static_cast<tchar>(seq->readBitsI(8));
    }
    tmp[len] = '\0';
        
    s = QString::fromUtf8(tmp,len);
    delete [] tmp;
    return s;
}

//-------------------------------------------------------------------------------------------

bool FLACMetaComment::read(engine::Sequence *seq)
{
    tint len,i,noComments;
    QString index,body;
    
    len = seq->readBitsI(24) << 3;
    m_vendor = readString(seq);

    noComments = seq->readBitsIL(32);
    for(i=0;i<noComments;i++)
    {
        tint idx;
            
        body = readString(seq);
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
    return true;
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::vendor() const
{
    return m_vendor;
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::comment(const tchar *str) const
{
    QString s(QString::fromLatin1(str));
    return comment(s);
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::comment(const QString& str) const
{
    QMap<QString,QString>::const_iterator ppI = m_comments.find(str);
    
    if(ppI!=m_comments.end())
    {
        return ppI.value();
    }
    else
    {
        return m_nullStr;
    }
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::title() const
{
    return comment("title");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::version() const
{
    return comment("version");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::album() const
{
    return comment("album");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::trackNumber() const
{
    return comment("tracknumber");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::artist() const
{
    return comment("artist");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::performer() const
{
    return comment("performer");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::copyright() const
{
    return comment("copyright");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::license() const
{
    return comment("license");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::organization() const
{
    return comment("organization");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::description() const
{
    return comment("description");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::genre() const
{
    return comment("genre");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::date() const
{
    return comment("date");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::location() const
{
    return comment("location");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::contact() const
{
    return comment("contact");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::isrc() const
{
    return comment("isrc");
}

//-------------------------------------------------------------------------------------------

const QString& FLACMetaComment::diskNumber() const
{
    return comment("discnumber");
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
