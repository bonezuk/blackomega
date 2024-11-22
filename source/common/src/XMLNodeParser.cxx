#include "common/inc/XMLNodeParser.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

XMLNodeParser::XMLNodeParser() : XMLReadOps()
{}

//-------------------------------------------------------------------------------------------

XMLNodeParser::~XMLNodeParser()
{}

//-------------------------------------------------------------------------------------------

void XMLNodeParser::parse(xmlNodePtr pNode)
{
	if(isRootNode(pNode))
	{
		xmlNodePtr pCNode = pNode->children;
		
		while(pCNode!=0)
		{
			if(isElementNode(pCNode))
			{
				processNode(pCNode);
			}
			pCNode = pCNode->next;
		}
	}
}

//-------------------------------------------------------------------------------------------

bool XMLNodeParser::isSpecifiedNode(xmlNodePtr pNode,const tchar *name) const
{
	return (isElementNode(pNode) && getNameOfNode(pNode).toLower()==comparisonKey(name)) ? true : false;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
