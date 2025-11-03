#ifndef __OMEGA_COMMON_BSTRINGCOLLECTION_H
#define __OMEGA_COMMON_BSTRINGCOLLECTION_H

#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BStringCollection 
{
    public:
        BStringCollection();
        ~BStringCollection();

        bool Group(BString& str,bool reverse=false);

        friend COMMON_EXPORT BStringCollection& operator <<(BStringCollection& in,const BString& s);
        friend COMMON_EXPORT BStringCollection& operator <<(BStringCollection& in,const tchar *s);
        friend COMMON_EXPORT BStringCollection& operator <<(BStringCollection& in,const BStringCollection& s);
        friend COMMON_EXPORT BStringCollection& operator <<(BStringCollection& in,const tchar s);
    
    protected:

        typedef struct s_BSCItem 
        {
            struct s_BSCItem *prev;
            struct s_BSCItem *next;
            tint amount;
            tint len;
            tchar *str;
        } BSCItem;

        BSCItem *firstItem,*lastItem;
        tint total;

        char *AddString(const tchar *s,tint& len) const;
        void Add(const tchar *s,tint len);
};


//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

