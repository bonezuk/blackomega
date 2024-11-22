#include "common/inc/XMLReadOps.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

XMLReadOps::XMLReadOps()
{}

//-------------------------------------------------------------------------------------------

XMLReadOps::~XMLReadOps()
{}

//-------------------------------------------------------------------------------------------

QString XMLReadOps::getTextOfNode(xmlNodePtr pNode) const
{
	QString text;
	
	if(pNode!=0 && pNode->type==XML_TEXT_NODE)
	{
		XMLLibIFSPtr api = XMLLibIF::instance();
		
		xmlChar *pText = api->xmlNodeGetContent(pNode);
		if(pText!=0)
		{
			text = QString::fromUtf8(reinterpret_cast<const char *>(pText));
			api->xmlFree(pText);
		}
	}
	return text;
}

//-------------------------------------------------------------------------------------------

QString XMLReadOps::getTextOfElement(xmlNodePtr pElement) const
{
	QString text;
	
	if(pElement!=0 && pElement->type==XML_ELEMENT_NODE)
	{
		XMLLibIFSPtr api = XMLLibIF::instance();
		
		xmlNode *cNode = pElement->children;
		while(cNode!=0 && text.isEmpty())
		{
			if(cNode->type==XML_TEXT_NODE)
			{
				text = getTextOfNode(cNode);
			}
			cNode = cNode->next;
		}
	}
	return text;
}

//-------------------------------------------------------------------------------------------

QString XMLReadOps::getNameOfNode(xmlNodePtr pNode) const
{
	QString name;
	
	if(pNode!=0)
	{
		name = QString::fromUtf8(reinterpret_cast<const char *>(pNode->name));
	}
	return name;
}

//-------------------------------------------------------------------------------------------

bool XMLReadOps::isElementNode(xmlNodePtr pNode) const
{
	return (pNode!=0 && pNode->type==XML_ELEMENT_NODE) ? true : false;
}

//-------------------------------------------------------------------------------------------

QString XMLReadOps::comparisonKey(const tchar *keyTxt) const
{
	return QString::fromUtf8(keyTxt).toLower();
}

//-------------------------------------------------------------------------------------------

void XMLReadOps::removeChildrenFromNode(xmlNodePtr pNode) const
{
	if(pNode!=0)
	{
		XMLLibIFSPtr api = XMLLibIF::instance();
		
		while(pNode->children!=0)
		{
			xmlNodePtr cNode = pNode->children;
			api->xmlUnlinkNode(cNode);
			api->xmlFreeNode(cNode);
		}
	}
}

//-------------------------------------------------------------------------------------------

void XMLReadOps::replaceTextInNode(xmlNodePtr pNode,const QString& txt) const
{
	replaceTextInNode(pNode,txt.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

void XMLReadOps::replaceTextInNode(xmlNodePtr pNode,const tchar *txt) const
{
	if(isElementNode(pNode))
	{
		XMLLibIFSPtr api = XMLLibIF::instance();
		
		removeChildrenFromNode(pNode);
		
		if(txt!=0 && ::strlen(txt)>0)
		{
			xmlNodePtr cNode = api->xmlNewText(reinterpret_cast<const xmlChar *>(txt));
			if(cNode!=0)
			{
				api->xmlAddChild(pNode,cNode);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void XMLReadOps::replaceTextInAllNodes(xmlNodePtr pNode,const QString& nodeName,const QString& text) const
{
	if(!nodeName.isEmpty() && isElementNode(pNode))
	{
		QString nName(nodeName.toLower());
		
		if(getNameOfNode(pNode).toLower()==nName)
		{
			replaceTextInNode(pNode,text);
		}
		else
		{
			xmlNodePtr cNode = pNode->children;
			while(cNode!=0)
			{
				if(isElementNode(cNode))
				{
					if(getNameOfNode(cNode).toLower()==nName)
					{
						replaceTextInNode(cNode,text);
					}
					else
					{
						replaceTextInAllNodes(cNode,nodeName,text);
					}
				}
				cNode = cNode->next;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

bool XMLReadOps::isAttribute(xmlNodePtr pNode,const QString& name) const
{
	bool res = false;

	if(pNode!=0 && !name.isEmpty())
	{
		if(XMLLibIF::instance()->xmlHasProp(pNode,reinterpret_cast<const xmlChar *>(name.toUtf8().constData()))!=0)
		{
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

QString XMLReadOps::getAttribute(xmlNodePtr pNode,const QString& name) const
{
	QString value;

	if(isAttribute(pNode,name))
	{
		xmlChar *pAttr = XMLLibIF::instance()->xmlGetProp(pNode,reinterpret_cast<const xmlChar *>(name.toUtf8().constData()));
		if(pAttr!=0)
		{
			value = QString::fromUtf8(reinterpret_cast<const char *>(pAttr));
			XMLLibIF::instance()->xmlFree(pAttr);
		}
	}
	return value;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

