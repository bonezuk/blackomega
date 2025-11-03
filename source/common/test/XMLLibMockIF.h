//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_XMLLIBMOCKIF_H
#define __OMEGA_COMMON_XMLLIBMOCKIF_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/XMLLibIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class XMLLibMockIF : public XMLLibIF
{
    public:
        XMLLibMockIF();
        virtual ~XMLLibMockIF();

        MOCK_METHOD1(xmlNodeGetContent,xmlChar *(xmlNodePtr cur));
        MOCK_METHOD1(xmlFree,void(void *));

        MOCK_METHOD1(xmlNewText,xmlNodePtr(const xmlChar *content));
        MOCK_METHOD1(xmlFreeNode,void(xmlNodePtr cur));
        MOCK_METHOD2(xmlReplaceNode,xmlNodePtr(xmlNodePtr old,xmlNodePtr cur));
        MOCK_METHOD2(xmlAddChild,xmlNodePtr(xmlNodePtr parent,xmlNodePtr cur));
        MOCK_METHOD1(xmlUnlinkNode,void(xmlNodePtr cur));

        MOCK_METHOD2(xmlHasProp,xmlAttrPtr(xmlNodePtr node,const xmlChar *name));
        MOCK_METHOD2(xmlGetProp,xmlChar *(xmlNodePtr node,const xmlChar *name));

        MOCK_METHOD2(xmlParseMemory,xmlDocPtr(const char *buffer,int size));
        MOCK_METHOD1(xmlParseFile,xmlDocPtr(const char *filename));
        MOCK_METHOD1(xmlDocGetRootElement,xmlNodePtr(xmlDocPtr doc));
        MOCK_METHOD1(xmlFreeDoc,void(xmlDocPtr cur));

        MOCK_METHOD2(xmlNewTextWriterDoc,xmlTextWriterPtr(xmlDocPtr *doc,int compression));
        MOCK_METHOD4(xmlTextWriterStartDocument,int(xmlTextWriterPtr writer,const char *version,const char *encoding,const char *standalone));
        MOCK_METHOD2(xmlTextWriterStartElement,int(xmlTextWriterPtr writer,const char *name));
        MOCK_METHOD3(xmlTextWriterWriteAttribute,int(xmlTextWriterPtr writer,const char *name,const char *content));
        MOCK_METHOD3(xmlTextWriterWriteElement,int(xmlTextWriterPtr writer,const char *name,const char *content));
        MOCK_METHOD1(xmlTextWriterEndElement,int(xmlTextWriterPtr writer));
        MOCK_METHOD1(xmlFreeTextWriter,void(xmlTextWriterPtr writer));
        MOCK_METHOD3(xmlSaveFileEnc,int(const char *filename,xmlDocPtr doc,const char *encoding));
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
