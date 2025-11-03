#include "common/inc/RegistryScript.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

RegistryScript::RegistryScript()
{}

//-------------------------------------------------------------------------------------------

RegistryScript::~RegistryScript()
{}

//-------------------------------------------------------------------------------------------

bool RegistryScript::exec(const QString& xmlStr)
{
#if defined(OMEGA_WIN32)
    bool res = false;

    if(!xmlStr.isEmpty())
    {
        QByteArray iMem;
        xmlDocPtr doc;

        iMem = xmlStr.toUtf8();
        doc = xmlParseMemory(iMem.constData(),iMem.length());
        if(doc!=0)
        {
            xmlNode *rNode = xmlDocGetRootElement(doc);
            if(rNode!=0)
            {
                RegistryScript regScript;
                res = regScript.processRegistryXML(rNode);
            }
            xmlFreeDoc(doc);
        }
    }
    return res;
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

void RegistryScript::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "RegistryScript::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::deleteRegistryKey(HKEY parent)
{
    DWORD i,noKeys,len,kLen;
    LONG result;
    LPWSTR mem;
    FILETIME fAccess;
    HKEY child;
    bool res = true;

    result = ::RegQueryInfoKeyW(parent,0,0,0,&noKeys,&len,0,0,0,0,0,0);
    if(result==ERROR_SUCCESS)
    {
        mem = reinterpret_cast<LPWSTR>(new tuint16 [len + 1]);

        for(i=0;i<noKeys && result==ERROR_SUCCESS && res;i++)
        {
            kLen = len + 1;
            result = ::RegEnumKeyExW(parent,0,mem,&kLen,0,0,0,&fAccess);
            if(result==ERROR_SUCCESS)
            {
                result = ::RegOpenKeyExW(parent,mem,0,KEY_ALL_ACCESS,&child);
                if(result==ERROR_SUCCESS)
                {
                    res = deleteRegistryKey(child);
                    if(res)
                    {
                        result = ::RegDeleteKeyW(parent,mem);
                    }
                }
                else
                {
                    res = false;
                }
            }
        }
        if((result==ERROR_SUCCESS || result==ERROR_NO_MORE_ITEMS) && res)
        {
            if(::RegCloseKey(parent)!=ERROR_SUCCESS)
            {
                printError("deleteRegistryKey","Failed to close registry key");
            }
            res = true;
        }
        delete [] mem;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::processKey(xmlNode *cNode,HKEY parentKey)
{
    bool r = true;

    if(cNode==0)
    {
        printError("processKey","No XML object given");
        return false;
    }

    if(cNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(cNode->name)).toLower()=="key")
    {
        QString idS,valueS;

        idS = getXMLAttribute(cNode,"id");
        valueS = getXMLAttribute(cNode,"value");
        if(!idS.isEmpty())
        {
            HKEY key;
            LONG res;
            DWORD type,amount;
            bool writeFlag = true,deleteFlag = false,existFlag;

            res = ::RegOpenKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
            if(res==ERROR_SUCCESS)
            {
                existFlag = true;
                res = ::RegQueryValueExW(key,0,0,&type,0,&amount);
                if(res==ERROR_SUCCESS && type==REG_SZ)
                {
                    if(amount>0)
                    {
                        if(!valueS.isEmpty())
                        {
                            tuint16 *data = new tuint16 [(amount>>1) + 1];

                            res = ::RegQueryValueExW(key,0,0,&type,reinterpret_cast<LPBYTE>(data),&amount);
                            if(res==ERROR_SUCCESS)
                            {
                                data[amount>>1] = 0;
                                if(valueS==QString::fromUtf16(reinterpret_cast<const char16_t*>(data)))
                                {
                                    writeFlag = false;
                                }
                            }
                            delete [] data;
                        }
                    }
                    else if(valueS.isEmpty())
                    {
                        writeFlag = false;
                    }
                }
                else if(valueS.isEmpty())
                {
                    writeFlag = false;
                }
            }
            else
            {
                existFlag = false;
            }

            if(writeFlag)
            {
                res = ERROR_SUCCESS;

                if(deleteFlag && getXMLAttribute(cNode,"delete").toLower()!="false")
                {
                    if(!deleteRegistryKey(key))
                    {
                        printError("processKey","Failed to remove items under registry key");
                        return false;
                    }
                    res = ::RegDeleteKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()));
                    if(res!=ERROR_SUCCESS)
                    {
                        printError("processKey","Failed to remove registry key");
                        return false;
                    }
                    res = ::RegCreateKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
                }
                else
                {
                    if(!existFlag)
                    {
                        res = ::RegCreateKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
                    }
                }

                if(res==ERROR_SUCCESS)
                {
                    if(!valueS.isEmpty())
                    {
                        res = ::RegSetValueW(key,0,REG_SZ,reinterpret_cast<LPCWSTR>(valueS.utf16()),static_cast<DWORD>(valueS.length() * sizeof(tuint16)));
                        if(res!=ERROR_SUCCESS)
                        {
                            printError("processKey","Failed to write default value to key");
                            r = false;
                        }
                    }
                    if(::RegCloseKey(key)!=ERROR_SUCCESS)
                    {
                        printError("processKey","Failed to close registry key");
                    }
                }
                else
                {
                    printError("processKey","Failed to create registry key");
                    r = false;
                }
            }
            else
            {
                if(::RegCloseKey(key)!=ERROR_SUCCESS)
                {
                    printError("processKey","Failed to close registry key");
                }
            }
        }
        else
        {
            printError("processKey","'id' is a required attribute");
            r = false;
        }
    }
    else
    {
        printError("processKey","Given XML does not represent a registry key");
        r = false;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::processString(xmlNode *node,HKEY key)
{
    DWORD type,amount;
    LONG res;
    bool writeFlag=true,deleteFlag=false;
    bool r = true;

    if(node==0)
    {
        printError("processString","No XML object given");
        return false;
    }
    if(node->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(node->name)).toLower()=="string")
    {
        QString idS,valueS;

        idS = getXMLAttribute(node,"id");
        valueS = getXMLAttribute(node,"value");
        if(!idS.isEmpty())
        {
            res = ::RegQueryValueExW(key,reinterpret_cast<LPCWSTR>(idS.utf16()),0,&type,0,&amount);
            if(res==ERROR_SUCCESS)
            {
                if(type==REG_SZ)
                {
                    tuint16 *data = new tuint16 [(amount>>1) + 1];

                    res = ::RegQueryValueExW(key,reinterpret_cast<LPCWSTR>(idS.utf16()),0,&type,reinterpret_cast<LPBYTE>(data),&amount);
                    if(res==ERROR_SUCCESS)
                    {
                        data[amount>>1] = 0;
                        if(valueS==QString::fromUtf16(reinterpret_cast<const char16_t*>(data)))
                        {
                            writeFlag = false;
                        }
                    }
                    delete [] data;
                    deleteFlag = true;
                }
                else if(type!=REG_NONE)
                {
                    deleteFlag = false;
                }
            }

            if(writeFlag)
            {
                if(deleteFlag)
                {
                    res = ::RegDeleteValueW(key,reinterpret_cast<LPCWSTR>(idS.utf16()));
                    if(res!=ERROR_SUCCESS)
                    {
                        printError("processString","Failed to delete registry value");
                        return false;
                    }
                }

                res = ::RegSetValueExW(key,reinterpret_cast<LPCWSTR>(idS.utf16()),0,REG_SZ,reinterpret_cast<CONST BYTE *>(valueS.utf16()),static_cast<DWORD>(valueS.length() * sizeof(tuint16)));
                if(res!=ERROR_SUCCESS)
                {
                    printError("processString","Error setting registry value");
                    r = false;
                }
            }
        }
        else
        {
            printError("processString","'id' and 'value' are required attributes");
            r = false;
        }
    }
    else
    {
        printError("processString","Given XML does not represent a registry string");
        r = false;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::openRootKey(const QString& name,HKEY& key)
{
    QString n = name.toUpper();
    bool res = true;

    if(n=="HKEY_CLASSES_ROOT")
    {
        key = HKEY_CLASSES_ROOT;
    }
    else if(n=="HKEY_CURRENT_CONFIG")
    {
        key = HKEY_CURRENT_CONFIG;
    }
    else if(n=="HKEY_CURRENT_USER")
    {
        key = HKEY_CURRENT_USER;
    }
    else if(n=="HKEY_LOCAL_MACHINE")
    {
        key = HKEY_LOCAL_MACHINE;
    }
    else if(n=="HKEY_USERS")
    {
        key = HKEY_USERS;
    }
    else
    {
        printError("openRootKey","Unknown top node registry key");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::processNode(xmlNode *cNode,HKEY parentKey)
{
    bool r = true;

    if(cNode==0)
    {
        printError("processNode","No XML object given");
        return false;
    }
    if(cNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(cNode->name)).toLower()=="node")
    {
        HKEY key;
        LONG res;
        QString idS;

        idS = getXMLAttribute(cNode,"id");
        if(!idS.isEmpty())
        {
            res = ::RegOpenKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
            if(res!=ERROR_SUCCESS)
            {
                res = ::RegCreateKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
                if(res==ERROR_SUCCESS)
                {
                    QString valueS;

                    valueS = getXMLAttribute(cNode,"value");
                    if(!valueS.isEmpty())
                    {
                        res =::RegSetValueW(key,0,REG_SZ,reinterpret_cast<LPCWSTR>(valueS.utf16()),static_cast<DWORD>(valueS.length() * sizeof(tuint16)));
                        if(res!=ERROR_SUCCESS)
                        {
                            printError("processNode","Failed to set value");
                            r = false;
                        }
                    }
                }
                else
                {
                    printError("processNode","Error trying to create required key");
                    return false;
                }
            }
            if(::RegCloseKey(key)!=ERROR_SUCCESS)
            {
                printError("processNode","Failed to close registry key");
            }
        }
        else
        {
            printError("processNode","A NODE entry with an ID attribute is expected");
            r = false;
        }
    }
    else
    {
        printError("processNode","A NODE entry with an ID attribute is expected");
        r = false;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::processDelete(xmlNode *cNode,HKEY parentKey)
{
    bool r = true;

    if(cNode==0)
    {
        printError("processDelete","No XML object given");
        return false;
    }
    if(cNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(cNode->name)).toLower()=="delete")
    {
        HKEY key;
        LONG res;
        QString idS;

        idS = getXMLAttribute(cNode,"id");
        if(!idS.isEmpty())
        {
            res = ::RegOpenKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
            if(res==ERROR_SUCCESS)
            {
                if(!deleteRegistryKey(key))
                {
                    printError("processDelete","Failed to remove item under registry key");
                    return false;
                }
                res = ::RegDeleteKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()));
                if(res!=ERROR_SUCCESS)
                {
                    printError("processDelete","Failed to remove registry key");
                    return false;
                }
            }
        }
        else
        {
            printError("processDelete","ID is a required attribute");
            return false;
        }
    }
    else
    {
        printError("processDelete","Given XML does not represent a deletion of a registry key");
        r = false;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

QString RegistryScript::getXMLAttribute(xmlNode *cNode,const QString& aName)
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

bool RegistryScript::processRegistryXML(xmlNode *pNode)
{
    bool r = true;

    if(pNode==0)
    {
        printError("processRegistryXML","No XML object given");
        return false;
    }
    if(pNode->type==XML_ELEMENT_NODE && QString::fromUtf8(reinterpret_cast<const tchar *>(pNode->name)).toLower()=="root")
    {
        QString idS;

        idS = getXMLAttribute(pNode,"id");
        if(!idS.isEmpty())
        {
            HKEY root;

            if(openRootKey(idS,root))
            {
                if(!runXMLScript(pNode,root))
                {
                    printError("processRegistryXML","Error in processing registry XML script");
                    r = false;
                }
            }
            else
            {
                printError("processRegistryXML","Failed to obtain root registry key for XML script");
                r = false;
            }
        }
        else
        {
            printError("processRegistryXML","ROOT entry requires an ID attribute");
            r = false;
        }
    }
    else
    {
        printError("processRegistryXML","A ROOT entry is expected");
        r = false;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

bool RegistryScript::runXMLScript(xmlNode *pNode,HKEY parentKey)
{
    xmlNode *cNode;
    bool res = true;

    if(pNode==0)
    {
        printError("runXMLScript","No XML object give");
        return false;
    }

    cNode = pNode->xmlChildrenNode;
    while(cNode!=0 && res)
    {
        bool pFlag = true;

        if(cNode->type==XML_ELEMENT_NODE)
        {
            QString cName = QString::fromUtf8(reinterpret_cast<const tchar *>(cNode->name)).toLower();

            if(cName=="string")
            {
                res = processString(cNode,parentKey);
                pFlag = false;
            }
            else if(cName=="node")
            {
                res = processNode(cNode,parentKey);
            }
            else if(cName=="key")
            {
                res = processKey(cNode,parentKey);
            }
            else if(cName=="delete")
            {
                res = processDelete(cNode,parentKey);
                pFlag = false;
            }
            else
            {
                pFlag = false;
            }

            if(pFlag && res)
            {
                HKEY key;
                LONG r;
                QString idS;

                idS = getXMLAttribute(cNode,"id");
                if(!idS.isEmpty())
                {
                    r = RegOpenKeyW(parentKey,reinterpret_cast<LPCWSTR>(idS.utf16()),&key);
                    if(r==ERROR_SUCCESS)
                    {
                        res = runXMLScript(cNode,key);
                        if(!res)
                        {
                            printError("runXMLScript","Failed to process child entries");
                        }

                        if(::RegCloseKey(key)!=ERROR_SUCCESS)
                        {
                            printError("runXMLScript","Failed to close registry key");
                        }
                    }
                    else
                    {
                        printError("runXMLScript","Failed to open expected registry key");
                        res = false;
                    }
                }
                else
                {
                    printError("runXMLScript","ID attribute expected");
                    res = false;
                }
            }
            else if(!res)
            {
                printError("runXMLScript","Error in process registry key");
            }
        }
        cNode = cNode->next;
    }

    return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
