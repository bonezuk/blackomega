//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_REGISTRYSCRIPT_H
#define __OMEGA_COMMON_REGISTRYSCRIPT_H
//-------------------------------------------------------------------------------------------

#include <QString>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT RegistryScript
{
    public:
        virtual ~RegistryScript();

        static bool exec(const QString& xmlStr);

    protected:

        RegistryScript();

#if defined(OMEGA_WIN32)
        void printError(const tchar *strR,const tchar *strE) const;

        bool processRegistryXML(xmlNode *pNode);
        bool runXMLScript(xmlNode *pNode,HKEY parentKey);

        bool openRootKey(const QString& name,HKEY& key);
        bool deleteRegistryKey(HKEY parent);
        bool processKey(xmlNode *cNode,HKEY parentKey);
        bool processString(xmlNode *node,HKEY key);
        bool processNode(xmlNode *cNode,HKEY parentKey);
        bool processDelete(xmlNode *cNode,HKEY parentKey);
        QString getXMLAttribute(xmlNode *cNode,const QString& aName);
#endif
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
