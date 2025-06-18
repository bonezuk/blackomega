#ifndef __OMEGA_COMMON_ALLOCATION_H
#define __OMEGA_COMMON_ALLOCATION_H

#include <stdio.h>
#include <stdlib.h>
#include "common/inc/CommonTypes.h"
#include "common/inc/BOTree.h"

#if defined (OMEGA_MSVC)
#pragma warning(disable:4231)
#endif

#define ALLOC_MEMORY 0
#define ALLOC_MEMORY_ALIGN 1

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

typedef struct s_AllocQueueItem 
{
    struct s_AllocQueueItem *prev;
    struct s_AllocQueueItem *next;
    tuint type;
    tbyte *item;
} AllocQueueItem;

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_DEFINE_TEMPLATE_HEADER)
#if !defined(COMP_UNIT_Allocation)
COMMON_EXPORT_TEMPLATE template class COMMON_EXPORT BOTree<tbyte *,AllocQueueItem *>;
#else
template class COMMON_EXPORT BOTree<tbyte *,AllocQueueItem *>;
#endif
#endif

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT Allocation
{
    public:
        Allocation();
        virtual ~Allocation();

        virtual void *MemAlloc(tuint sizeNo,tuint sizeStruct);
        
        virtual void *MemAllocAlign(tuint sizeNo,tuint sizeStruct,tuint align);

        virtual void Free(void *item);
        virtual void FreeAll();
    
    private:
        BOTree<tbyte *,AllocQueueItem *> m_Items;

        AllocQueueItem *firstItem,*lastItem;

        virtual AllocQueueItem *AddItem(tuint type);
        virtual void RemoveItem(AllocQueueItem *item);

        virtual void AlignedFree(void *ptr) const;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

