#include "track/info/inc/PListXMLParser.h"
#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// PListBaseXMLParser
//-------------------------------------------------------------------------------------------

PListBaseXMLParser::PListBaseXMLParser(const QMap<QString,QSet<QString> >& dict) : m_dict(dict),
    m_state(0),
    m_parent(0)
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParser::PListBaseXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p) : m_dict(dict),
    m_state(0),
    m_parent(p)
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParser::~PListBaseXMLParser()
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParserPtr PListBaseXMLParser::startElement(const QString& name,const xmlChar **attrs)
{
    return PListBaseXMLParserPtr();
}

//-------------------------------------------------------------------------------------------

bool PListBaseXMLParser::endElement(const QString& name)
{
    return false;
}

//-------------------------------------------------------------------------------------------

void PListBaseXMLParser::characters(const QString& c)
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParser *PListBaseXMLParser::parent()
{
    return m_parent;
}

//-------------------------------------------------------------------------------------------

PListBaseXMLParser::ParserType PListBaseXMLParser::parserType() const
{
    return m_parserType;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<PListBaseXMLParser> PListBaseXMLParser::createParser(const QString& name,const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *parent)
{
    PListBaseXMLParserPtr pParser;

    if(name=="key")
    {
        QSharedPointer<KeyXMLParser> pKeyParser(new KeyXMLParser(dict,parent));
        pParser = pKeyParser.dynamicCast<PListBaseXMLParser>();
    }
    else if(name=="array")
    {
        QSharedPointer<ArrayXMLParser> pArrayParser(new ArrayXMLParser(dict,parent));
        pParser = pArrayParser.dynamicCast<PListBaseXMLParser>();
    }
    else if(name=="dict")
    {
        QSharedPointer<DictXMLParser> pDictParser(new DictXMLParser(dict,parent));
        pParser = pDictParser.dynamicCast<PListBaseXMLParser>();
    }
    return pParser;
}

//-------------------------------------------------------------------------------------------
// KeyXMLParser
//-------------------------------------------------------------------------------------------

KeyXMLParser::KeyXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p) : PListBaseXMLParser(dict,p),
    m_type(e_keyUnknown),
    m_key(),
    m_data()
{
    m_parserType = e_parserKey;
}

//-------------------------------------------------------------------------------------------

KeyXMLParser::~KeyXMLParser()
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParserPtr KeyXMLParser::startElement(const QString& name,const xmlChar **attrs)
{
    PListBaseXMLParserPtr cParser;

    switch(m_state)
    {
        case 0:
            if(name=="key")
            {
                m_state = 1;
            }
            break;

        case 2:
            if(name=="array")
            {
                QSharedPointer<ArrayXMLParser> nParser(new ArrayXMLParser(m_dict,this));
                cParser = nParser.dynamicCast<PListBaseXMLParser>();
                m_type = e_keyArray;
                m_state = 3;
            }
            else if(name=="dict")
            {
                QSharedPointer<DictXMLParser> nParser(new DictXMLParser(m_dict,this));
                cParser = nParser.dynamicCast<PListBaseXMLParser>();
                m_type = e_keyDict;
                m_state = 3;
            }
            else if(name=="string")
            {
                m_type = e_keyString;
                m_state = 3;
            }
            else if(name=="data")
            {
                m_type = e_keyData;
                m_state = 3;
            }
            else if(name=="date")
            {
                m_type = e_keyDate;
                m_state = 3;
            }
            else if(name=="integer")
            {
                m_type = e_keyInteger;
                m_state = 3;
            }
            else if(name=="real")
            {
                m_type = e_keyReal;
                m_state = 3;
            }
            else if(name=="true")
            {
                m_data = QVariant(true);
                m_type = e_keyBoolean;
                m_state = 3;
            }
            else if(name=="false")
            {
                m_data = QVariant(false);
                m_type = e_keyBoolean;
                m_state = 3;
            }
            break;
    }
    return cParser;
}

//-------------------------------------------------------------------------------------------

bool KeyXMLParser::endElement(const QString& name)
{
    bool res = true;

    switch(m_state)
    {
        case 1:
            if(name=="key")
            {
                m_state = 2;
            }
            break;

        case 3:
        case 4:
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void KeyXMLParser::characters(const QString& c)
{
    if(m_state==1)
    {
        m_key = c;
    }
    else if(m_state==3)
    {
        switch(m_type)
        {
            case e_keyData:
                {
                    QByteArray dArray = QByteArray::fromBase64(c.toLatin1());
                    m_data = QVariant(dArray);
                }
                break;

            case e_keyDate:
                m_data = QVariant(c);
                break;

            case e_keyInteger:
                m_data = QVariant(c.toInt());
                break;

            case e_keyReal:
                m_data = QVariant(c.toDouble());
                break;

            case e_keyString:
                m_data = QVariant(c);
                break;

            case e_keyBoolean:
            case e_keyArray:
            case e_keyDict:
            case e_keyUnknown:
            default:
                break;
        }
        m_state = 4;
    }
    else if(m_state==4 && m_type==e_keyString)
    {
        QString d = m_data.toString();
        d += c;
        m_data = d;
    }
}

//-------------------------------------------------------------------------------------------

void KeyXMLParser::processData(PListBaseXMLParserPtr pChild)
{
    QMap<QString,QSet<QString> >::const_iterator ppI;

    ppI = m_dict.find(m_key);
    if(ppI!=m_dict.end())
    {
        if(m_type==e_keyArray)
        {
            QSharedPointer<ArrayXMLParser> pParser = pChild.dynamicCast<ArrayXMLParser>();
            if(pParser.data()!=0)
            {
                QVariant nV(pParser->data());
                m_data = nV;
            }
            else
            {
                m_data = QVariant();
            }
        }
        else if(m_type==e_keyDict)
        {
            QSharedPointer<DictXMLParser> pParser = pChild.dynamicCast<DictXMLParser>();
            if(pParser.data()!=0)
            {
                QVariant nV(pParser->data());
                m_data = nV;
            }
            else
            {
                m_data = QVariant();
            }
        }
    }
    else
    {
        PListBaseXMLParser *parentParser = parent();

        while(parentParser!=0)
        {
            KeyXMLParser *keyParser = dynamic_cast<KeyXMLParser *>(parentParser);
            if(keyParser!=0)
            {
                ppI = m_dict.find(keyParser->key());
                if(ppI!=m_dict.end())
                {
                    break;
                }
            }
            parentParser = parentParser->parent();
        }
        if(parentParser!=0)
        {
            if(m_type==e_keyArray)
            {
                QSharedPointer<ArrayXMLParser> pParser = pChild.dynamicCast<ArrayXMLParser>();

                if(pParser.data()!=0)
                {
                    QVariant nV(pParser->data());
                    m_data = nV;
                }
                else
                {
                    m_data = QVariant();
                }
            }
            else if(m_type==e_keyDict)
            {
                QSharedPointer<DictXMLParser> pParser = pChild.dynamicCast<DictXMLParser>();

                if(pParser.data()!=0)
                {
                    QVariant nV(pParser->data());
                    m_data = nV;
                }
                else
                {
                    m_data = QVariant();
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
// ArrayXMLParser
//-------------------------------------------------------------------------------------------

ArrayXMLParser::ArrayXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p) : PListBaseXMLParser(dict,p),
    m_data()
{
    m_parserType = e_parserArray;
}

//-------------------------------------------------------------------------------------------

ArrayXMLParser::~ArrayXMLParser()
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParserPtr ArrayXMLParser::startElement(const QString& name,const xmlChar **attrs)
{
    PListBaseXMLParserPtr cParser;

    switch(m_state)
    {
        case 0:
            if(name=="array")
            {
                m_state = 1;
            }
            break;

        case 1:
            if(name=="dict" || name=="array" || name=="key")
            {
                cParser = createParser(name,m_dict,this);
            }
            break;
    }
    return cParser;
}

//-------------------------------------------------------------------------------------------

bool ArrayXMLParser::endElement(const QString& name)
{
    bool res = true;

    if(m_state==1)
    {
        if(name=="array")
        {
            m_state = 0;
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void ArrayXMLParser::characters(const QString& c)
{}

//-------------------------------------------------------------------------------------------

void ArrayXMLParser::processData(PListBaseXMLParserPtr pChild)
{
    QMap<QString,QSet<QString> >::const_iterator ppI;
    PListBaseXMLParser *parentParser = parent();

    while(parentParser!=0)
    {
        KeyXMLParser *keyParser = dynamic_cast<KeyXMLParser *>(parentParser);
        if(keyParser!=0)
        {
            ppI = m_dict.find(keyParser->key());
            if(ppI!=m_dict.end())
            {
                break;
            }
        }
        parentParser = parentParser->parent();
    }
    if(parentParser!=0)
    {
        QSharedPointer<DictXMLParser> dictParser = pChild.dynamicCast<DictXMLParser>();
        if(dictParser.data()!=0)
        {
            QVariant mapV(dictParser->data());
            m_data.append(mapV);
        }
        else
        {
            QSharedPointer<ArrayXMLParser> arrayParser = pChild.dynamicCast<ArrayXMLParser>();
            if(arrayParser.data()!=0)
            {
                QVariant listV(arrayParser->data());
                m_data.append(listV);
            }
            else
            {
                QSharedPointer<KeyXMLParser> keyParser = pChild.dynamicCast<KeyXMLParser>();
                if(keyParser.data()!=0)
                {
                    QHash<QString,QVariant> hMap;
                    hMap.insert("key",QVariant(keyParser->key()));
                    hMap.insert("data",keyParser->data());
                    QVariant nV(hMap);
                    m_data.append(nV);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
// DictXMLParser
//-------------------------------------------------------------------------------------------

DictXMLParser::DictXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p) : PListBaseXMLParser(dict,p),
    m_data()
{
    m_parserType = e_parserDict;
}

//-------------------------------------------------------------------------------------------

DictXMLParser::~DictXMLParser()
{}

//-------------------------------------------------------------------------------------------

PListBaseXMLParserPtr DictXMLParser::startElement(const QString& name,const xmlChar **attrs)
{
    PListBaseXMLParserPtr cParser;

    switch(m_state)
    {
        case 0:
            if(name=="dict")
            {
                m_state = 1;
            }
            break;

        case 1:
            if(name=="key")
            {
                cParser = createParser(name,m_dict,this);
            }
            break;
    }
    return cParser;
}

//-------------------------------------------------------------------------------------------

bool DictXMLParser::endElement(const QString& name)
{
    bool res = true;

    if(m_state==1 && name=="dict")
    {
        m_state = 0;
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void DictXMLParser::characters(const QString& c)
{}

//-------------------------------------------------------------------------------------------

void DictXMLParser::processData(PListBaseXMLParserPtr pChild)
{
    QSharedPointer<KeyXMLParser> keyParser = pChild.dynamicCast<KeyXMLParser>();
    if(keyParser.data()!=0)
    {
        QString parentKey;
        PListBaseXMLParser *parentParser = parent();
        QMap<QString,QSet<QString> >::const_iterator ppI;

        while(parentParser!=0)
        {
            KeyXMLParser *kParser = dynamic_cast<KeyXMLParser *>(parentParser);
            if(kParser!=0)
            {
                ppI = m_dict.find(kParser->key());
                if(ppI!=m_dict.end())
                {
                    break;
                }
            }
            parentParser = parentParser->parent();
        }
        if(parentParser==0)
        {
            ppI = m_dict.find("");
        }
        if(ppI!=m_dict.end())
        {
            const QSet<QString>& dSet = ppI.value();
            QSet<QString>::const_iterator ppJ = dSet.find(keyParser->key());
            if(ppJ!=dSet.end())
            {
                m_data.insert(keyParser->key(),keyParser->data());
            }
            else if(keyParser->keyType()==KeyXMLParser::e_keyDict && !keyParser->data().toMap().isEmpty())
            {
                m_data.insert(keyParser->key(),keyParser->data());
            }
        }
    }
    else
    {
        QSharedPointer<DictXMLParser> dictParser = pChild.dynamicCast<DictXMLParser>();
        if(dictParser.data()!=0 && parent()->parserType()==PListBaseXMLParser::e_parserPList)
        {
            QVariant mapV(dictParser->data());
            m_data.insert("dict",mapV);
        }
    }
}

//-------------------------------------------------------------------------------------------
// PListXMLParser
//-------------------------------------------------------------------------------------------

PListXMLParser::PListXMLParser(const QMap<QString,QSet<QString> >& dict) : PListBaseXMLParser(dict),
    m_parserStack(),
    m_parseResult(true)
{
    m_parserType = e_parserPList;
}

//-------------------------------------------------------------------------------------------

PListXMLParser::~PListXMLParser()
{}

//-------------------------------------------------------------------------------------------

bool PListXMLParser::process(const QString& fileName)
{
    xmlSAXHandler handler;

#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    if(!(sbBookmark->has(fileName,true) && sbBookmark->checkOut(fileName,true)))
    {
        return false;
    }
#endif

    m_parseResult = true;

    ::memset(&handler,0,sizeof(xmlSAXHandler));
    handler.startElement = PListXMLParser::startElementImpl;
    handler.endElement = PListXMLParser::endElementImpl;
    handler.characters = PListXMLParser::charactersImpl;
    handler.error = PListXMLParser::error;
    handler.fatalError = PListXMLParser::error;

    if(xmlSAXUserParseFile(&handler,reinterpret_cast<void *>(this),fileName.toUtf8().constData())<0)
    {
        m_parseResult = false;
    }

#if defined(OMEGA_MAC_STORE)
    sbBookmark->checkIn(fileName,true);
#endif

    return m_parseResult;
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::processData(PListBaseXMLParserPtr pChild)
{
    QSharedPointer<DictXMLParser> dictParser = pChild.dynamicCast<DictXMLParser>();
    if(dictParser.data()!=0)
    {
        QVariant mapV(dictParser->data());
        m_data.append(mapV);
    }
    else
    {
        QSharedPointer<ArrayXMLParser> arrayParser = pChild.dynamicCast<ArrayXMLParser>();
        if(arrayParser.data()!=0)
        {
            QVariant listV(arrayParser->data());
            m_data.append(listV);
        }
        else
        {
            QSharedPointer<KeyXMLParser> keyParser = pChild.dynamicCast<KeyXMLParser>();
            if(keyParser.data()!=0)
            {
                const QString& k = keyParser->key();
                QMap<QString,QSet<QString> >::const_iterator ppI = m_dict.find("");
                bool found = true;

                if(ppI!=m_dict.end())
                {
                    const QSet<QString>& dSet = ppI.value();
                    QSet<QString>::const_iterator ppJ = dSet.find(k);
                    if(ppJ==dSet.end())
                    {
                        found = false;
                    }
                }

                if(found)
                {
                    QHash<QString,QVariant> hMap;
                    hMap.insert("key",QVariant(keyParser->key()));
                    hMap.insert("data",keyParser->data());
                    QVariant nV(hMap);
                    m_data.append(nV);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::startElementCB(const xmlChar *name,const xmlChar **attrs)
{
    QString n = QString::fromUtf8(reinterpret_cast<const char *>(name));
    if(m_state==0)
    {
        if(n=="plist")
        {
            m_state = 1;
        }
    }
    else
    {
        PListBaseXMLParserPtr pParser;

        if(m_parserStack.isEmpty())
        {
            pParser = createParser(n,m_dict,this);
            if(pParser.data()!=0)
            {
                m_parserStack.push(pParser);
            }
        }
        else
        {
            pParser = m_parserStack.top();
        }
        if(pParser.data()!=0)
        {
            while(pParser = pParser->startElement(n,attrs),pParser.data()!=0)
            {
                m_parserStack.push(pParser);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::endElementCB(const xmlChar *name)
{
    QString n = QString::fromUtf8(reinterpret_cast<const char *>(name));
    if(m_state==1)
    {
        if(m_parserStack.isEmpty())
        {
            if(n=="plist")
            {
                m_state = 0;
            }
        }
        else
        {
            if(!m_parserStack.top()->endElement(n))
            {
                PListBaseXMLParserPtr cParser = m_parserStack.pop();

                if(!m_parserStack.isEmpty())
                {
                    m_parserStack.top()->processData(cParser);
                    if(!m_parserStack.isEmpty() && (cParser->parserType()==PListBaseXMLParser::e_parserArray || cParser->parserType()==PListBaseXMLParser::e_parserDict))
                    {
                        if(m_parserStack.top()->parserType()==PListBaseXMLParser::e_parserKey)
                        {
                            if(!m_parserStack.top()->endElement(n))
                            {
                                cParser = m_parserStack.pop();
                                if(!m_parserStack.isEmpty())
                                {
                                    m_parserStack.top()->processData(cParser);
                                }
                                else
                                {
                                    processData(cParser);
                                }
                            }
                        }
                    }
                }
                else
                {
                    processData(cParser);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::charactersCB(const xmlChar *ch,int len)
{
    if(m_state>0 && !m_parserStack.isEmpty())
    {
        m_parserStack.top()->characters(QString::fromUtf8(reinterpret_cast<const char *>(ch),len));
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::startElementImpl(void *userData,const xmlChar *name,const xmlChar **attrs)
{
    PListXMLParser *pParser = reinterpret_cast<PListXMLParser *>(userData);
    if(pParser!=0)
    {
        pParser->startElementCB(name,attrs);
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::endElementImpl(void *userData,const xmlChar *name)
{
    PListXMLParser *pParser = reinterpret_cast<PListXMLParser *>(userData);
    if(pParser!=0)
    {
        pParser->endElementCB(name);
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::charactersImpl(void *userData,const xmlChar *ch,int len)
{
    PListXMLParser *pParser = reinterpret_cast<PListXMLParser *>(userData);
    if(pParser!=0)
    {
        pParser->charactersCB(ch,len);
    }
}

//-------------------------------------------------------------------------------------------

void PListXMLParser::error(void *userData,const char *msg,...)
{
    PListXMLParser *pParser = reinterpret_cast<PListXMLParser *>(userData);
    if(pParser!=0)
    {
        pParser->m_parseResult = false;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
