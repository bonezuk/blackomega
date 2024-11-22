//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_XMLLIBIF_H
#define __OMEGA_COMMON_XMLLIBIF_H
//-------------------------------------------------------------------------------------------

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "common/inc/CommonTypes.h"
#include "common/inc/Factory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT XMLLibIF
{
	public:
		XMLLibIF();
		virtual ~XMLLibIF();
		
		static QSharedPointer<XMLLibIF> instance(const QString& factoryKey);
		static QSharedPointer<XMLLibIF> instance();
		static void release();
		
		virtual xmlChar *xmlNodeGetContent(xmlNodePtr cur) = 0;
		virtual void xmlFree(void *mem) = 0;
		
		virtual xmlNodePtr xmlNewText(const xmlChar *content) = 0;
		virtual void xmlFreeNode(xmlNodePtr cur) = 0;
		virtual xmlNodePtr xmlReplaceNode(xmlNodePtr old,xmlNodePtr cur) = 0;
		virtual xmlNodePtr xmlAddChild(xmlNodePtr parent,xmlNodePtr cur) = 0;
		virtual void xmlUnlinkNode(xmlNodePtr cur) = 0;
		
		virtual xmlAttrPtr xmlHasProp(xmlNodePtr node,const xmlChar *name) = 0;
		virtual xmlChar *xmlGetProp(xmlNodePtr node,const xmlChar *name) = 0;

		virtual xmlDocPtr xmlParseMemory(const char *buffer,int size) = 0;
		virtual xmlDocPtr xmlParseFile(const char *filename) = 0;
		virtual xmlNodePtr xmlDocGetRootElement(xmlDocPtr doc) = 0;
		virtual void xmlFreeDoc(xmlDocPtr cur) = 0;

		virtual xmlTextWriterPtr xmlNewTextWriterDoc(xmlDocPtr *doc,int compression) = 0;
		virtual int xmlTextWriterStartDocument(xmlTextWriterPtr writer,const char *version,const char *encoding,const char *standalone) = 0;
		virtual int xmlTextWriterStartElement(xmlTextWriterPtr writer,const char *name) = 0;
		virtual int xmlTextWriterWriteAttribute(xmlTextWriterPtr writer,const char *name,const char *content) = 0;
		virtual int xmlTextWriterWriteElement(xmlTextWriterPtr writer,const char *name,const char *content) = 0;
		virtual int xmlTextWriterEndElement(xmlTextWriterPtr writer) = 0;
		virtual void xmlFreeTextWriter(xmlTextWriterPtr writer) = 0;
		virtual int xmlSaveFileEnc(const char *filename,xmlDocPtr doc,const char *encoding) = 0;

	protected:
	
		static QSharedPointer<XMLLibIF> m_instance;
};

typedef QSharedPointer<XMLLibIF> XMLLibIFSPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(COMMON_EXPORT,XMLLibIFFactory,XMLLibIF)

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
