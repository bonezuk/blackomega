#include "player/inc/CLIPipe.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

CLIPipe *CLIPipe::m_instance = 0;

//-------------------------------------------------------------------------------------------

CLIPipe::CLIPipe() : m_mailSlot(INVALID_HANDLE_VALUE),
    m_serverMode(false),
    m_cmdList()
{}

//-------------------------------------------------------------------------------------------

CLIPipe::~CLIPipe()
{
    if(m_mailSlot!=INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_mailSlot);
        m_mailSlot = INVALID_HANDLE_VALUE;
    }
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

CLIPipe *CLIPipe::instance()
{
    if(m_instance==0)
    {
        m_instance = new CLIPipe;
        if(!m_instance->init())
        {
            delete m_instance;
            m_instance = 0;
        }
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

bool CLIPipe::isServer() const
{
    return m_serverMode;
}

//-------------------------------------------------------------------------------------------

bool CLIPipe::init()
{
#if defined(OMEGA_DEBUG)
    QString name("\\\\.\\mailslot\\omegainput_debug");
#else
    QString name("\\\\.\\mailslot\\omegainput");
#endif
    bool res = true;

    m_mailSlot = ::CreateMailslotW(reinterpret_cast<LPCWSTR>(name.utf16()),0,MAILSLOT_WAIT_FOREVER,0);
    if(m_mailSlot!=INVALID_HANDLE_VALUE)
    {
        m_serverMode = true;
    }
    else
    {
        m_serverMode = false;
        m_mailSlot = ::CreateFileW(reinterpret_cast<LPCWSTR>(name.utf16()),GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
        if(m_mailSlot==INVALID_HANDLE_VALUE)
        {
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString CLIPipe::cliToXML(const QStringList& cmdList)
{
    int i,stateA=0;
    QStringList x;
    QString cXml;

    x << "<cli>";
    for(i=0;i<cmdList.size();i++)
    {
        const QString& cmd = cmdList.at(i);

        if(stateA==0)
        {
            if(cmd=="-A" || cmd=="--addfiles")
            {
                x << "<command id=\"addfiles\">";
                stateA = 1;
            }
            else if(cmd=="-E" || cmd=="--enqueue")
            {
                x << "<command id=\"enqueue\">";
                stateA = 1;
            }
        }
        else
        {
            QString pStr;
            pStr = "<![CDATA[" + cmd + "]]>";
            x << pStr;
        }
    }
    if(stateA==1)
    {
        x << "</command>";
    }
    x << "</cli>";
    cXml = x.join("");
    return cXml;
}

//-------------------------------------------------------------------------------------------

void CLIPipe::translateXML(const QString& cmd)
{
    if(!cmd.isEmpty())
    {
        xmlDocPtr doc;
        QByteArray iMem;

        iMem = cmd.toUtf8();
        doc = xmlParseMemory(iMem.constData(),iMem.length());
        if(doc!=0)
        {
            xmlNode *rNode = xmlDocGetRootElement(doc);
            if(rNode!=0 && rNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(rNode->name)).toLower()=="cli")
            {
                xmlNode *cNode = rNode->children;
                while(cNode!=0)
                {
                    if(cNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(cNode->name)).toLower()=="command")
                    {
                        QString cTypeName = getXMLAttribute(cNode,"id");
                        xmlNode *tNode = cNode->children;

                        if(tNode!=0 && (tNode->type==XML_TEXT_NODE || tNode->type==XML_CDATA_SECTION_NODE))
                        {
                            QString pName = QString::fromUtf8(reinterpret_cast<const tchar *>(tNode->content));

                            if(cTypeName=="addfiles")
                            {
                                m_cmdList.append(QPair<Command,QString>(eCmdAddFiles,pName));
                            }
                            else if(cTypeName=="enqueue")
                            {
                                m_cmdList.append(QPair<Command,QString>(eCmdQueueFiles,pName));
                            }
                        }
                    }
                    cNode = cNode->next;
                }
            }
            xmlFreeDoc(doc);
        }
    }
}

//-------------------------------------------------------------------------------------------

QString CLIPipe::getXMLAttribute(xmlNode *cNode,const QString& aName)
{
    QString aData;

    if(cNode!=0)
    {
        xmlChar *x = xmlGetProp(cNode,reinterpret_cast<const xmlChar *>(aName.toUtf8().constData()));
        if(x!=0)
        {
            aData = QString::fromUtf8(reinterpret_cast<const tchar *>(x));
            xmlFree(x);
        }
    }
    return aData;
}

//-------------------------------------------------------------------------------------------

bool CLIPipe::cli(const QStringList& cmd)
{
    bool res = true;

    if(cmd.size()>0)
    {
        if(m_mailSlot!=INVALID_HANDLE_VALUE)
        {
            QString cmdXml = cliToXML(cmd);

            if(!m_serverMode)
            {
                DWORD amount;
                QByteArray cmdXmlArr = cmdXml.toUtf8();

                if(!(::WriteFile(m_mailSlot,cmdXmlArr.constData(),static_cast<DWORD>(cmdXmlArr.size()),&amount,0) && amount==static_cast<DWORD>(cmdXmlArr.size())))
                {
                    res = false;
                }
            }
            else
            {
                translateXML(cmdXml);
            }
        }
        else
        {
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool CLIPipe::process()
{
    bool res = false;

    if(m_cmdList.isEmpty())
    {
        DWORD msgSize,amount,bufSize = 256;
        tchar *buffer = new tchar [bufSize];

        if(::GetMailslotInfo(m_mailSlot,0,&msgSize,0,0)!=FALSE)
        {
            if(msgSize!=MAILSLOT_NO_MESSAGE)
            {
                if(msgSize>bufSize)
                {
                    delete [] buffer;
                    bufSize = msgSize;
                    buffer = new tchar [bufSize];
                }

                if(::ReadFile(m_mailSlot,buffer,msgSize,&amount,0)!=FALSE && msgSize==amount)
                {
                    QString cXml = QString::fromUtf8(buffer,msgSize);
                    translateXML(cXml);
                    res = true;
                }
            }
        }
        delete [] buffer;
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

CLIPipe::Command CLIPipe::next(QString& param)
{
    if(!m_cmdList.isEmpty())
    {
        QPair<Command,QString> nP;
        nP = m_cmdList.takeFirst();
        param = nP.second;
        return nP.first;
    }
    else
    {
        return eCmdNone;
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
