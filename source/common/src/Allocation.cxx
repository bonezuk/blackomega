#include "common/inc/Allocation.h"

#if defined (OMEGA_MSVC)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER tbyte *BOTree<tbyte *,AllocQueueItem *>::nullIndex=0;
COMMON_TEMPLATE_MEMBER AllocQueueItem *BOTree<tbyte *,AllocQueueItem *>::nullObject=0;

//-------------------------------------------------------------------------------------------

Allocation::Allocation() : m_Items(), firstItem(0), lastItem(0)
{}

//-------------------------------------------------------------------------------------------

Allocation::~Allocation()
{
    try
    {
        Allocation::FreeAll();
        firstItem=0;
        lastItem=0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

AllocQueueItem *Allocation::AddItem(tuint type)
{
    AllocQueueItem *item;

    try
    {
        item=new AllocQueueItem;
        item->prev=lastItem;
        item->next=0;
        if(lastItem!=0)
        {
            lastItem->next=item;
        }
        else
        {
            firstItem=item;
        }
        lastItem=item;
        item->type=type;
    }
    catch(...)
    {
        return 0;
    }
    return item;
}

//-------------------------------------------------------------------------------------------

void Allocation::RemoveItem(AllocQueueItem *item)
{
    AllocQueueItem *pItem=item->prev,*nItem=item->next;

    if(pItem!=0)
    {
        pItem->next=nItem;
    }
    else
    {
        firstItem=nItem;
    }
    if(nItem!=0)
    {
        nItem->prev=pItem;
    }
    else
    {
        lastItem=pItem;
    }
    delete item;
}

//-------------------------------------------------------------------------------------------

void *Allocation::MemAlloc(tuint sizeNo,tuint sizeStruct)
{
    tbyte *mem;
    AllocQueueItem *item;

    mem=new tbyte [sizeNo*sizeStruct];
    item=AddItem(ALLOC_MEMORY);
    item->item = mem;
    m_Items[mem]=item;
    return reinterpret_cast<void *>(mem);
}

//-------------------------------------------------------------------------------------------

void *Allocation::MemAllocAlign(tuint sizeNo,tuint sizeStruct,tuint align)
{
    tbyte *ptr,*ptr2,*aPtr;
    tint32 *iPtr;
    size_t mask;
    AllocQueueItem *item;

    mask = static_cast<size_t>(align) - 1;

    ptr = new tbyte [(sizeNo * sizeStruct) + align + sizeof(size_t)];

    item = AddItem(ALLOC_MEMORY_ALIGN);

    ptr2 = ptr + sizeof(size_t);
    aPtr = ptr2 + (align - (reinterpret_cast<size_t>(ptr2) & mask));
    iPtr = reinterpret_cast<tint32 *>(aPtr); //lint !e826 Memory allocated with conversion in mind.
    size_t *bPtr = (size_t *)iPtr;
    bPtr--;
    *bPtr = (size_t)ptr;
    item->item = aPtr;
    m_Items[aPtr]=item;

    return reinterpret_cast<void *>(aPtr);
}

//-------------------------------------------------------------------------------------------

void Allocation::AlignedFree(void *ptr) const
{
    tbyte *aPtr;
    size_t *iPtr = reinterpret_cast<size_t *>(ptr);

    iPtr--;
    aPtr = (tbyte *)(*iPtr); //lint !e1924 C casting used for bit level manipulation.
    delete [] aPtr;
}

//-------------------------------------------------------------------------------------------

void Allocation::Free(void *item)
{
    tbyte *pItem = reinterpret_cast<tbyte *>(item);
    AllocQueueItem *nItem;

    if(m_Items.Exist(pItem))
    {
        nItem=m_Items[pItem];
        m_Items.Remove(pItem);
        switch(nItem->type)
        {
            case ALLOC_MEMORY:
                {
                    delete [] pItem;
                }
                break;

            case ALLOC_MEMORY_ALIGN:
                {
                    AlignedFree(item);
                }
                break;

            default:
                break;
        }
        RemoveItem(nItem);
    }
}

//-------------------------------------------------------------------------------------------

void Allocation::FreeAll()
{
    while(lastItem!=0)
    {
        Free(lastItem->item);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
