#include "common/inc/XMLLibLayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(XMLLibIFFactory,XMLLibIF, \
                            XMLLibIFLayerFactory,XMLLibLayerIF, \
                            "xml",false)

//-------------------------------------------------------------------------------------------

XMLLibLayerIF::XMLLibLayerIF()
{}

//-------------------------------------------------------------------------------------------

XMLLibLayerIF::~XMLLibLayerIF()
{}

//-------------------------------------------------------------------------------------------

xmlChar *XMLLibLayerIF::xmlNodeGetContent(xmlNodePtr cur)
{
    return ::xmlNodeGetContent(cur);
}

//-------------------------------------------------------------------------------------------

void XMLLibLayerIF::xmlFree(void *mem)
{
    ::xmlFree(mem);
}

//-------------------------------------------------------------------------------------------

xmlNodePtr XMLLibLayerIF::xmlNewText(const xmlChar *content)
{
    return ::xmlNewText(content);
}

//-------------------------------------------------------------------------------------------

void XMLLibLayerIF::xmlFreeNode(xmlNodePtr cur)
{
    ::xmlFreeNode(cur);
}

//-------------------------------------------------------------------------------------------

xmlNodePtr XMLLibLayerIF::xmlReplaceNode(xmlNodePtr old,xmlNodePtr cur)
{
    return ::xmlReplaceNode(old,cur);
}

//-------------------------------------------------------------------------------------------

xmlNodePtr XMLLibLayerIF::xmlAddChild(xmlNodePtr parent,xmlNodePtr cur)
{
    return ::xmlAddChild(parent,cur);
}

//-------------------------------------------------------------------------------------------

void XMLLibLayerIF::xmlUnlinkNode(xmlNodePtr cur)
{
    ::xmlUnlinkNode(cur);
}

//-------------------------------------------------------------------------------------------

xmlAttrPtr XMLLibLayerIF::xmlHasProp(xmlNodePtr node,const xmlChar *name)
{
    return ::xmlHasProp(node,name);
}

//-------------------------------------------------------------------------------------------

xmlChar *XMLLibLayerIF::xmlGetProp(xmlNodePtr node,const xmlChar *name)
{
    return ::xmlGetProp(node,name);
}

//-------------------------------------------------------------------------------------------

xmlDocPtr XMLLibLayerIF::xmlParseMemory(const char *buffer,int size)
{
    return ::xmlParseMemory(buffer,size);
}

//-------------------------------------------------------------------------------------------

xmlDocPtr XMLLibLayerIF::xmlParseFile(const char *filename)
{
    return ::xmlParseFile(filename);
}

//-------------------------------------------------------------------------------------------

xmlNodePtr XMLLibLayerIF::xmlDocGetRootElement(xmlDocPtr doc)
{
    return ::xmlDocGetRootElement(doc);
}

//-------------------------------------------------------------------------------------------

void XMLLibLayerIF::xmlFreeDoc(xmlDocPtr cur)
{
    return ::xmlFreeDoc(cur);
}

//-------------------------------------------------------------------------------------------

xmlTextWriterPtr XMLLibLayerIF::xmlNewTextWriterDoc(xmlDocPtr *doc,int compression)
{
    return ::xmlNewTextWriterDoc(doc,compression);
}

//-------------------------------------------------------------------------------------------

int XMLLibLayerIF::xmlTextWriterStartDocument(xmlTextWriterPtr writer,const char *version,const char *encoding,const char *standalone)
{
    return ::xmlTextWriterStartDocument(writer,version,encoding,standalone);
}

//-------------------------------------------------------------------------------------------

int XMLLibLayerIF::xmlTextWriterStartElement(xmlTextWriterPtr writer,const char *name)
{
    return ::xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>(name));
}

//-------------------------------------------------------------------------------------------

int XMLLibLayerIF::xmlTextWriterWriteAttribute(xmlTextWriterPtr writer,const char *name,const char *content)
{
    return ::xmlTextWriterWriteAttribute(writer,reinterpret_cast<const xmlChar *>(name),reinterpret_cast<const xmlChar *>(content));
}

//-------------------------------------------------------------------------------------------

int XMLLibLayerIF::xmlTextWriterWriteElement(xmlTextWriterPtr writer,const char *name,const char *content)
{
    return ::xmlTextWriterWriteElement(writer,reinterpret_cast<const xmlChar *>(name),reinterpret_cast<const xmlChar *>(content));
}

//-------------------------------------------------------------------------------------------

int XMLLibLayerIF::xmlTextWriterEndElement(xmlTextWriterPtr writer)
{
    return ::xmlTextWriterEndElement(writer);
}

//-------------------------------------------------------------------------------------------

void XMLLibLayerIF::xmlFreeTextWriter(xmlTextWriterPtr writer)
{
    ::xmlFreeTextWriter(writer);
}

//-------------------------------------------------------------------------------------------

int XMLLibLayerIF::xmlSaveFileEnc(const char *filename,xmlDocPtr doc,const char *encoding)
{
    return ::xmlSaveFileEnc(filename,doc,encoding);
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
