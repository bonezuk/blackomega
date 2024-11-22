//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_XMLLIBLAYERIF_H
#define __OMEGA_COMMON_XMLLIBLAYERIF_H
//-------------------------------------------------------------------------------------------

#include "common/inc/XMLLibIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT XMLLibLayerIF : public XMLLibIF
{
	public:
		XMLLibLayerIF();
		virtual ~XMLLibLayerIF();
		
		virtual xmlChar *xmlNodeGetContent(xmlNodePtr cur);
		virtual void xmlFree(void *mem);

		virtual xmlNodePtr xmlNewText(const xmlChar *content);
		virtual void xmlFreeNode(xmlNodePtr cur);
		virtual xmlNodePtr xmlReplaceNode(xmlNodePtr old,xmlNodePtr cur);
		virtual xmlNodePtr xmlAddChild(xmlNodePtr parent,xmlNodePtr cur);
		virtual void xmlUnlinkNode(xmlNodePtr cur);

		virtual xmlAttrPtr xmlHasProp(xmlNodePtr node,const xmlChar *name);
		virtual xmlChar *xmlGetProp(xmlNodePtr node,const xmlChar *name);
		
		virtual xmlDocPtr xmlParseMemory(const char *buffer,int size);
		virtual xmlDocPtr xmlParseFile(const char *filename);
		virtual xmlNodePtr xmlDocGetRootElement(xmlDocPtr doc);
		virtual void xmlFreeDoc(xmlDocPtr cur);
		
		virtual xmlTextWriterPtr xmlNewTextWriterDoc(xmlDocPtr *doc,int compression);
		virtual int xmlTextWriterStartDocument(xmlTextWriterPtr writer,const char *version,const char *encoding,const char *standalone);
		virtual int xmlTextWriterStartElement(xmlTextWriterPtr writer,const char *name);
		virtual int xmlTextWriterWriteAttribute(xmlTextWriterPtr writer,const char *name,const char *content);
		virtual int xmlTextWriterWriteElement(xmlTextWriterPtr writer,const char *name,const char *content);
		virtual int xmlTextWriterEndElement(xmlTextWriterPtr writer);
		virtual void xmlFreeTextWriter(xmlTextWriterPtr writer);
		virtual int xmlSaveFileEnc(const char *filename,xmlDocPtr doc,const char *encoding);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
