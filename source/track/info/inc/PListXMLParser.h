//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_DICTXMLPARSER_H
#define __OMEGA_TRACK_INFO_DICTXMLPARSER_H
//-------------------------------------------------------------------------------------------

#include <libxml/parser.h>

#include <QString>
#include <QList>
#include <QStack>
#include <QHash>
#include <QVariant>
#include <QSet>
#include <QSharedPointer>
#include <QMultiMap>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>

#include "track/info/inc/InfoDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class PListBaseXMLParser;
typedef QSharedPointer<PListBaseXMLParser> PListBaseXMLParserPtr;

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT PListBaseXMLParser
{
    public:

        typedef enum
        {
            e_parserPList = 0,
            e_parserKey,
            e_parserArray,
            e_parserDict
        } ParserType;

    public:
        PListBaseXMLParser(const QMap<QString,QSet<QString> >& dict);
        PListBaseXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p);
        virtual ~PListBaseXMLParser();

        ParserType parserType() const;

        virtual PListBaseXMLParserPtr startElement(const QString& name,const xmlChar **attrs);
        virtual bool endElement(const QString& name);
        virtual void characters(const QString& c);

        PListBaseXMLParser *parent();

        virtual void processData(PListBaseXMLParserPtr pChild) = 0;

    protected:

        const QMap<QString,QSet<QString> >& m_dict;
        int m_state;
        PListBaseXMLParser *m_parent;
        ParserType m_parserType;

        static PListBaseXMLParserPtr createParser(const QString& name,const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *parent);
};

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT KeyXMLParser : public PListBaseXMLParser
{
    public:

        typedef enum
        {
            e_keyArray = 0,
            e_keyDict,
            e_keyData,
            e_keyString,
            e_keyDate,
            e_keyInteger,
            e_keyReal,
            e_keyBoolean,
            e_keyUnknown
        } KeyType;

    public:
        KeyXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p);
        virtual ~KeyXMLParser();

        virtual PListBaseXMLParserPtr startElement(const QString& name,const xmlChar **attrs);
        virtual bool endElement(const QString& name);
        virtual void characters(const QString& c);

        virtual void processData(PListBaseXMLParserPtr pChild);

        KeyType keyType() const;
        const QString& key() const;
        const QVariant& data() const;

    protected:

        KeyType m_type;
        QString m_key;
        QVariant m_data;
};

//-------------------------------------------------------------------------------------------

inline KeyXMLParser::KeyType KeyXMLParser::keyType() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline const QString& KeyXMLParser::key() const
{
    return m_key;
}

//-------------------------------------------------------------------------------------------

inline const QVariant& KeyXMLParser::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT ArrayXMLParser : public PListBaseXMLParser
{
    public:
        ArrayXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p);
        virtual ~ArrayXMLParser();

        virtual PListBaseXMLParserPtr startElement(const QString& name,const xmlChar **attrs);
        virtual bool endElement(const QString& name);
        virtual void characters(const QString& c);

        virtual void processData(PListBaseXMLParserPtr pChild);

        const QList<QVariant>& data() const;

    protected:

        QList<QVariant> m_data;
};

//-------------------------------------------------------------------------------------------

inline const QList<QVariant>& ArrayXMLParser::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT DictXMLParser : public PListBaseXMLParser
{
    public:
        DictXMLParser(const QMap<QString,QSet<QString> >& dict,PListBaseXMLParser *p);
        virtual ~DictXMLParser();

        virtual PListBaseXMLParserPtr startElement(const QString& name,const xmlChar **attrs);
        virtual bool endElement(const QString& name);
        virtual void characters(const QString& c);

        virtual void processData(PListBaseXMLParserPtr pChild);

        const QMap<QString,QVariant>& data() const;

    protected:

        QMap<QString,QVariant> m_data;
};

//-------------------------------------------------------------------------------------------

inline const QMap<QString,QVariant>& DictXMLParser::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT PListXMLParser : public PListBaseXMLParser
{
    public:
        PListXMLParser(const QMap<QString,QSet<QString> >& dict);
        virtual ~PListXMLParser();

        bool process(const QString& fileName);

        virtual void processData(PListBaseXMLParserPtr pChild);

        const QList<QVariant>& data() const;

    protected:

        QStack<PListBaseXMLParserPtr> m_parserStack;
        bool m_parseResult;
        QList<QVariant> m_data;

        static void startElementImpl(void *userData,const xmlChar *name,const xmlChar **attrs);
        static void endElementImpl(void *userData,const xmlChar *name);
        static void charactersImpl(void *userData,const xmlChar *ch,int len);

        virtual void startElementCB(const xmlChar *name,const xmlChar **attrs);
        virtual void endElementCB(const xmlChar *name);
        virtual void charactersCB(const xmlChar *ch,int len);

        static void error(void *userData,const char *msg,...);
};

//-------------------------------------------------------------------------------------------

inline const QList<QVariant>& PListXMLParser::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
