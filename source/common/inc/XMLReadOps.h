//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_XMLREADOPS_H
#define __OMEGA_COMMON_XMLREADOPS_H
//-------------------------------------------------------------------------------------------

#include "common/inc/XMLLibIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT XMLReadOps
{
    public:
        XMLReadOps();
        virtual ~XMLReadOps();

        virtual QString getTextOfNode(xmlNodePtr pNode) const;
        virtual QString getTextOfElement(xmlNodePtr pElement) const;
        virtual QString getNameOfNode(xmlNodePtr pNode) const;

        virtual bool isElementNode(xmlNodePtr pNode) const;

        virtual QString comparisonKey(const tchar *keyTxt) const;

        virtual void removeChildrenFromNode(xmlNodePtr pNode) const;

        virtual void replaceTextInNode(xmlNodePtr pNode,const QString& text) const;
        virtual void replaceTextInNode(xmlNodePtr pNode,const tchar *text) const;

        virtual void replaceTextInAllNodes(xmlNodePtr pNode,const QString& nodeName,const QString& text) const;

        virtual bool isAttribute(xmlNodePtr pNode,const QString& name) const;
        virtual QString getAttribute(xmlNodePtr pNode,const QString& name) const;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

