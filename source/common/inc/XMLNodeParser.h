//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_XMLNODEPARSER_H
#define __OMEGA_COMMON_XMLNODEPARSER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/XMLReadOps.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT XMLNodeParser : public XMLReadOps
{
	public:
		XMLNodeParser();
		virtual ~XMLNodeParser();
		
		virtual void parse(xmlNodePtr pNode);
		
	protected:
	
		virtual bool isRootNode(xmlNodePtr pNode) const = 0;
		virtual void processNode(xmlNodePtr pNode) = 0;

		virtual bool isSpecifiedNode(xmlNodePtr pNode,const tchar *name) const;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

