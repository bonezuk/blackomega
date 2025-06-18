#ifndef __OMEGA_COMMON_BOQUEUETREE_H
#define __OMEGA_COMMON_BOQUEUETREE_H

#include "common/inc/CommonTypes.h"

#define _BOQUEUETREE_DEBUG 1
#ifdef _BOQUEUETREE_DEBUG
#define DBTREE(x) x
#else
#define DBTREE(x)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

const tint g_BOQT_MinQueueLength = 5;
const tint g_BOQT_MaxQueueLength = 15;

typedef struct s_QTItem {
    struct s_QTItem *parent;            //Pointer to item in parent list.
    struct s_QTItem *prev;                //Pointer to previous item in current list.
    struct s_QTItem *next;                //Pointer to next item in current list.
    struct s_QTItem *firstChild;        //Pointer to first item in child list represented by this item.
    struct s_QTItem *lastChild;            //Pointer to last item in child list represented by this item.
    tint N;                                //The number of items in the immediate child list spannded by this item.
    tint index;                            //The number of items in the main list spanned by this item.
} QTItem;

template <class X> class BOQueueTree {
    private:
        
        typedef struct s_RTItem {
            QTItem unit;
            X item;
        } RTItem;
        
        //Reference to the NULLed version of the template's object.
        static X nullObject;
        
        //Root of the queue tree structure.
        QTItem *m_Root,*m_Current;
        
        QTItem *SeekItem(QTItem *item, tint pos) const;
        
        QTItem *AllocateItem() const;
        
        bool SplitRoot(QTItem *item);
        bool SplitAssignToPrevious(QTItem *item,QTItem *cItem) const;
        bool SplitAssignToNext(QTItem *item,QTItem *cItem) const;
        bool SplitInsertItem(QTItem *item,QTItem *cItem);
        bool SplitParent(QTItem *item);
        
        QTItem *AddItemToMain(QTItem *parent,QTItem *pItem) const;
        QTItem *AddItem(tint& pos);
        
        QTItem *DeleteEmptyItem(QTItem *item) const;
        bool MergeDownRoot();
        QTItem *MergeIntoPrevious(QTItem *item) const;
        QTItem *MergeIntoNext(QTItem *item) const;
        bool DeleteItem(tint pos);

        tint SeekPosition(QTItem *item);

    public:
        BOQueueTree();
        ~BOQueueTree();

        tint Size() const;
        
        bool Add(tint& pos,const X& x);
        
        bool Delete(tint pos);
        
        X& operator [] (tint pos);
        
        const X& Find(tint pos) const;
        X& Find(tint pos);

        void Start(tint pos);
        X& Previous();
        X& Next();

        QTItem *Item(tint pos);
        tint Position(QTItem *item);
};

#if defined(OMEGA_MACOSX)
template <typename T> T BOQueueTree<T>::nullObject;
#endif

//-------------------------------------------------------------------------------------
//    Method:
//        BOQueueTree
//
//    Purpose:
//        Object constructer. Setup an initial blank main queue.
//
//    Access:
//        Public
//-------------------------------------------------------------------------------------

template <class X> BOQueueTree<X>::BOQueueTree() : m_Root(NULL), m_Current(NULL) {
    m_Root = AllocateItem();
}

//-------------------------------------------------------------------------------------
//    Method:
//        ~BOQueueTree
//
//    Purpose:
//        Object deconstructer. Deallocate memory    in queue tree structure.
//
//    Access:
//        Public
//-------------------------------------------------------------------------------------

template <class X> BOQueueTree<X>::~BOQueueTree() {
    QTItem *item = m_Root,*cItem,*dItem;
    RTItem *eItem;
    
    try {
        while(item!=NULL) {
            cItem = item;
            item = cItem->firstChild;
            while(cItem!=NULL) {
                dItem = cItem->next;
                eItem = reinterpret_cast<RTItem *>(cItem); //lint !e826 All items are allocate as RTItem structures.
                delete eItem;
                cItem = dItem;
            }
        }
        m_Root = NULL;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------
//    Method:
//        SeekItem
//
//    Purpose:
//        Find the item in the main queue at the initial given position. The method
//        works by looking for the parent item's of the final item in the current
//        list and recursively works down until the main list is found.
//
//    Access:
//        Private
//
//    Parameters:
//        item - The item to begin searching from.
//        pos  - The position within the list the item is to found.
//    
//    Returns:
//        The item of the main list referenced by the initial pos parameter if
//        successful, otherwise NULL is returned.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::SeekItem(QTItem *item, tint pos) const {

    while(item!=NULL) {    
        if(pos>=item->index) {
            pos -= item->index;
        }
        else {
            if(item->firstChild!=NULL) {
                item = SeekItem(item->firstChild,pos);
            }
            return item;
        }
        item = item->next;
    }
    return NULL;
}

//-------------------------------------------------------------------------------------
//    Method:
//        AllocateItem
//
//    Purpose:
//        Allocate and initialize a new queue tree item from the memory heap.
//
//    Access:
//        Private
//
//    Returns:
//        The newly allocated item.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::AllocateItem() const {
    RTItem *item;
    
    item = new RTItem;
    if(item!=NULL) {    //lint !e774 Ensure that structure has been allocated.
        item->unit.parent = NULL;
        item->unit.prev = NULL;
        item->unit.next = NULL;
        item->unit.firstChild = NULL;
        item->unit.lastChild = NULL;
        item->unit.N = 0;
        item->unit.index = 0;
        item->item = nullObject;
    }
    return reinterpret_cast<QTItem *>(item);
}

//-------------------------------------------------------------------------------------
//    Method:
//        SplitRoot
//
//    Purpose:
//        If the root item has to be split then a new list layer has be created
//        and the current root list has to be moved down.
//
//    Access:
//        Private
//
//    Parameters:
//        item - An item in the current root list.
//    
//    Returns:
//        true if successful or false on failure.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::SplitRoot(QTItem *item) {
    QTItem *root;
    
    //Ensure that given item is valid.
    if(item==NULL || item->parent!=NULL) {
        return false;
    }
    
    //Allocate new root item.
    root = AllocateItem();
    if(root==NULL) {
        return false;
    }
    
    //Traverse to first item of current list.
    while(item->prev!=NULL) {
        item = item->prev;
    }
    //Move from first to last item in last and calculate the index and N values of root.
    root->firstChild = item;
    do {
        item->parent = root;    //Assign list to new parent.
        root->index += item->index;
        root->N++;
        if(item->next==NULL) {
            break;
        }
        item = item->next;
    } while(true);
    root->lastChild = item;
    
    //Attach new root into instance.
    m_Root = root;
    return true;
}

//-------------------------------------------------------------------------------------
//    Method:
//        SplitAssignToPrevious
//
//    Purpose:
//        Reassign child items from the current to its previous item. This is used to
//        balance out lists as part of the Split process.
//
//    Access:
//        Private
//
//    Parameters:
//        item  - The item to move the child items from to its previous item.
//        cItem - The child item of item to make the new start item of parent.
//    
//    Returns:
//        true if successful or false on failure.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::SplitAssignToPrevious(QTItem *item,QTItem *cItem) const {
    QTItem *pItem,*aItem;
    tint n=0,t=0;
    
    //Ensure that the given parameters are valid.
    if(item==NULL || item->prev==NULL || cItem==NULL) {
        return false;
    }
    pItem = item->prev;

    //Calculate N and the index values for the child items to be moved upto the previous item.
    aItem = cItem->prev;
    while(aItem!=NULL && aItem!=pItem->lastChild) {
        t += aItem->index;
        n++;
        aItem->parent = pItem;    //Assign the child items to the new parent.
        aItem = aItem->prev;
    }
    if(aItem==NULL) {
        return false;
    }
    
    //Adjust the values of the previous item such that it now covers the split for the previous children.
    pItem->lastChild = cItem->prev;
    pItem->N += n;
    pItem->index += t;
    
    //Adjust the current item to reflect the removal of the child items.
    item->firstChild = cItem;
    item->N -= n;
    item->index -= t;
    return true;
}

//-------------------------------------------------------------------------------------
//    Method:
//        SplitAssignToNext
//
//    Purpose:
//        Reassign child items from the current to its next item. This is used to
//        balance out lists as part of the Split process.
//
//    Access:
//        Private
//
//    Parameters:
//        item  - The item to move the child items from to its next item.
//        cItem - The child item of item to make the new start item of the next parent.
//    
//    Returns:
//        true if successful or false on failure.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::SplitAssignToNext(QTItem *item,QTItem *cItem) const {
    QTItem *nItem,*aItem;
    tint n=0,t=0;
    
    //Ensure that the given parameters are valid.
    if(item==NULL || item->next==NULL || cItem==NULL) {
        return false;
    }
    nItem = item->next;
    
    //Calculate N and the index values for the child items to be moved upto the next item.
    aItem = cItem->prev;
    if(aItem==NULL) {
        return false;
    }
    while(cItem!=NULL && cItem!=nItem->firstChild) {
        t += cItem->index;
        n++;
        cItem->parent = nItem;    //Assign the child items to the parent.
        cItem = cItem->next;
    }
    if(cItem==NULL) {
        return false;
    }
    
    //Adjust the values of the next item such that it now covers the split for next children.
    nItem->firstChild = aItem->next;
    nItem->N += n;
    nItem->index += t;
    
    //Adjust the current item to reflect the removal of the child items.
    item->lastChild = aItem;
    item->N -= n;
    item->index -= t;
    return true;
}

//-------------------------------------------------------------------------------------
//    Method:
//        SplitInsertItem
//
//    Purpose:
//        This method creates a new entry in the current list and the child item passed
//        in is the first child item belonging to the new item.
//
//    Access:
//        Private
//
//    Parameters:
//        item  - The item to previous to the new item and to move the children from.
//        cItem - The child item of item to make the new start item of the new item.
//    
//    Returns:
//        true if successful or false on failure.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::SplitInsertItem(QTItem *item,QTItem *cItem) {
    QTItem *nItem,*aItem,*bItem;
    tint n=0,t=0;
    bool res;
    
    //Ensure that the parameters passed in is valid.
    if(item==NULL || cItem==NULL) {
        return false;
    }
    
    //Allocate the new item to be inserted into the list.
    nItem = AllocateItem();
    if(nItem==NULL) {
        return false;
    }
    
    //Set bItem to the value of the child item to stop on.
    if(item->next!=NULL) {
        bItem = item->next->firstChild;
    }
    else {
        bItem = NULL;
    }

    //For each children items to belong to the new item attach to the new item.
    //Also calculate N and index values for the new items.
    aItem = cItem->prev;
    if(aItem==NULL) {
        return false;
    }
    do {
        cItem->parent = nItem;
        t += cItem->index;
        n++;
        if(cItem->next==bItem) {
            break;
        }
        cItem = cItem->next;
    } while(true);
    
    //Set the values of the new items.
    nItem->firstChild = aItem->next;
    nItem->lastChild = cItem;
    nItem->N = n;
    nItem->index = t;
    
    //Adjust for the removal of the shifted children items.
    item->lastChild = aItem;
    item->N -= n;
    item->index -= t;
    
    //Join up the new item into the list, adjust parent and call approriate methods.
    aItem = item->next;
    item->next = nItem;
    nItem->prev = item;
    nItem->next = aItem;
    nItem->parent = item->parent;
    if(item->parent!=NULL) {
        if(aItem!=NULL) {
            aItem->prev = nItem;
        }
        if(item->parent->lastChild==item)
        {
            item->parent->lastChild = nItem;
        }
        item->parent->N++;
        res = SplitParent(nItem);
        
    }
    else {
        if(aItem!=NULL) {
            aItem->prev = nItem;
        }
        res = SplitRoot(nItem);
    }
    return res;
}

//-------------------------------------------------------------------------------------
//    Method:
//        SplitParent
//
//    Purpose:
//        Checks the parent items, after insertion of new item from the main list,
//        to see if new parent items need to be inserted to reduce the search index.
//
//    Access:
//        Private
//
//    Parameters:
//        item  - The child item that has newly been inserted.
//    
//    Returns:
//        true if successful or false on failure.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::SplitParent(QTItem *item) {
    tint i;
    QTItem *cItem;
    bool res;
    
    //Ensure the item passed is not blank.
    if(item==NULL) 
        return false;
    
    //Move up to the parent and check to see if the addition of the new item requires a split.
    item = item->parent;
    if(item==NULL) 
        return true;
    if(item->N <= g_BOQT_MaxQueueLength)
        return true;
    
    //Find the middle position of the child list to perform the split at.
    for(i=0,cItem=item->firstChild ; cItem!=NULL && i<=(g_BOQT_MaxQueueLength>>1) ; cItem=cItem->next,i++) ;
    if(cItem==NULL)
        return false;    
    
    if(item->prev!=NULL && (item->prev->N + i) <= g_BOQT_MaxQueueLength) {
        //Move excess to previous item if possible.
        res = SplitAssignToPrevious(item,cItem);
    }
    else if(item->next!=NULL && (item->next->N + (item->N-i)) <= g_BOQT_MaxQueueLength) {
        //Otherwise try and move to next item if possible.
        res = SplitAssignToNext(item,cItem);
    }
    else {
        //Allocate the new item.
        res = SplitInsertItem(item,cItem);
    }
    return res;
}

//-------------------------------------------------------------------------------------
//    Method:
//        AddItemToMain
//
//    Purpose:
//        Adds a new item into the main list either above or below the given item. The
//        parent item line is altered accordingly to reflect the addition of the new item
//        into the main list.
//
//    Access:
//        Private
//
//    Parameters:
//        parent - The parent item against which the child is attached to.
//        pItem  - The main list item to which the new item is to added. If the item is
//                 NULL then it implies place the item on the top of the queue.
//    
//    Returns:
//        Pointer to the newly added item.
//
//    Rules for method:
//        1. If pItem!=NULL then it must be a child of 'parent'.
//        2. If pItem==NULL then 'parent' must be the top item in its list.
//        3. pItem preceeds the new item and nItem follows the new item.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::AddItemToMain(QTItem *parent,QTItem *pItem) const {
    QTItem *nItem,*item;

    //Ensure that a parent item to the new child item is given.
    if(parent==NULL) {
        return NULL;
    }
    
    //Determine the previous and next items for the new child.
    if(pItem!=NULL) {
        nItem = pItem->next;
    }
    else {
        if(parent->prev!=NULL) {
            return NULL;
        }
        nItem = parent->firstChild;
    }
    
    //Allocate new item from heap memory.
    item = AllocateItem();
    if(item==NULL) {
        return NULL;
    }
    
    //Attach new item into main list and parent.
    if(pItem!=NULL) {
        pItem->next = item;
    }
    else {
        parent->firstChild = item;
    }
    if(nItem!=NULL) {
        nItem->prev = item;
    }
    else {
        parent->lastChild = item;
    }
    if(pItem==parent->lastChild) {
        parent->lastChild = item;
    }
    item->parent = parent;
    item->prev = pItem;
    item->next = nItem;
    
    //Set the weights for node accordingly as a member of the main list.
    item->N = 0;
    item->index = 1;
    
    //Adjust the weight accordingly up the line of succession back to the root.
    parent->N++;
    do {
        parent->index++;
        parent = parent->parent;
    } while(parent!=NULL);
    
    return item;
}

//-------------------------------------------------------------------------------------
//    Method:
//        AddItem
//
//    Purpose:
//        Add a new item to main list at the given position and reindex the parent lists
//        accordingly.
//
//    Access:
//        Private
//
//    Parameters:
//        pos - [in]  The desired position of for the new item.
//              [out] The actual position of the new item.
//    
//    Returns:
//        Pointer to the newly added item.
//
//    Rules for method:
//        1. The actual position is based on sizing the position to the given list range.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::AddItem(tint& pos) {
    QTItem *parent,*pItem,*item;
    tint prev;
    
    //Ensure that object instance is properly setup.
    if(m_Root==NULL) {
        return NULL;
    }
    
    //Set position of new item to within the current range of the array.
    if(pos<0) {
        pos = 0;
    }
    else if(pos>Size()) {
        pos = Size();
    }
    
    //Get the previous and parent item positions for insertion of the new item.
    prev = pos;
    if(Size()==0) {
        parent = m_Root;
        pItem = NULL;
    }
    else {
        if(prev>0) {
            prev--;
        }
        pItem = SeekItem(m_Root,prev);
        if(pItem==NULL) {
            return NULL;
        }
        parent = pItem->parent;
        if(parent==NULL) {
            return NULL;
        }        
        if(pos==0) {
            pItem = NULL;
        }
    }
    
    //Add the item to the main list.
    item = AddItemToMain(parent,pItem);
    if(item==NULL) {
        return NULL;
    }
    
    //Split and reindex the parent lists accordingly.
    if(!SplitParent(item)) {
        return NULL;
    }
    return item;
}

//-------------------------------------------------------------------------------------
//    Method:
//        DeleteEmptyItem
//
//    Purpose:
//        This method deletes an empty item either from the main list or an index list.
//        The given must be empty to removed and its parent item is adjusted accordingly
//        and returned back by the method.
//
//    Access:
//        Private
//
//    Parameters:
//        item - The empty item to delete.
//    
//    Returns:
//        Pointer to the parent of the deleted item or NULL if the given item is invalid
//        or has children.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::DeleteEmptyItem(QTItem *item) const {
    tint index;
    QTItem *parent,*pItem,*nItem;
    
    //Check the validity of the parameters.
    if(item==NULL || item->parent==NULL || item->N>0) {
        return NULL;
    }
    
    //Obtain the surrounding items from the given item.
    parent = item->parent;
    pItem = item->prev;
    nItem = item->next;

    //Adjust the range of the child items from the parent.
    if(parent->firstChild==item) {
        if(parent->lastChild==item) {
            parent->firstChild = NULL;
            parent->lastChild = NULL;
        }
        else {
            parent->firstChild = nItem;
        }
    }
    else if(parent->lastChild==item) {
        parent->lastChild = pItem;
    }
    
    //Adjust the main list to delete the item and delete the item.
    if(pItem!=NULL) {
        pItem->next = nItem;
    }
    if(nItem!=NULL) {
        nItem->prev = pItem;
    }
    index = item->index;

    {
        RTItem *itemA = reinterpret_cast<RTItem *>(item);
        delete itemA;
    }
    
    //Adjust the weight accordingly up the line of succession back to the root.
    nItem = parent;
    nItem->N--;
    do {
        nItem->index -= index;
        nItem = nItem->parent;
    } while(nItem!=NULL);
    
    return parent;
}

//-------------------------------------------------------------------------------------
//    Method:
//        MergeDownRoot
//
//    Purpose:
//        Remove the top root item if the tree can be brought down a level
//
//    Access:
//        Private
//
//    Returns:
//        True if successful otherwise false.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::MergeDownRoot() {
    //Ensure item is not invalid.
    if(m_Root==NULL) {
        return false;
    }
    
    //Query root to see if requires removal.
    if(m_Root->N>1) {
        return true;
    }
    
    if(m_Root->firstChild!=NULL) {
        QTItem *item = m_Root->firstChild;
        
        //Ensure that the item is not a single main list item.
        if(item->N>0) {
            delete m_Root;
            m_Root = item;
            m_Root->parent = NULL;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------
//    Method:
//        MergeIntoPrevious
//
//    Purpose:
//        Merge the given item's child items into the previous item and remove the
//        given item.
//
//    Access:
//        Private
//
//    Parameters:
//        item - The item to be removed.
//    
//    Returns:
//        Pointer to the parent of the deleted item or NULL if the given item invalid
//        or could not be moved.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::MergeIntoPrevious(QTItem *item) const {
    tint n,t;
    QTItem *parentA,*parentB,*pItem,*aItem;
    
    //Check the validity of the parameters.
    if(item==NULL || item->prev==NULL) {
        return NULL;
    }
    
    //Get pointers to related items and check them.
    pItem = item->prev;
    parentA = item->parent;
    parentB = pItem->parent;
    if(parentA==NULL || parentB==NULL) {
        return NULL;
    }
    
    //Assign the children to the new parent.
    for(aItem=item->firstChild ; aItem!=NULL ; aItem=aItem->next) {
        aItem->parent = pItem;
        if(aItem==item->lastChild) {
            break;
        }
    }

    //Move the child items into the previous item.
    if(pItem->firstChild==NULL) {
        pItem->firstChild = item->firstChild;
    }
    pItem->lastChild = item->lastChild;
    item->firstChild = NULL;
    item->lastChild = NULL;
    
    //Remove N and index from the items parent and tranfer it to previous item's parent.
    n = item->N;
    t = item->index;
    pItem->N += n;
    pItem->index += t;
    parentA->index -= t;
    parentB->index += t;
    item->N = 0;
    item->index = 0;
    
    //Now the entry has been emptied remove it.
    return DeleteEmptyItem(item);
}

//-------------------------------------------------------------------------------------
//    Method:
//        MergeIntoNext
//
//    Purpose:
//        Merge the given item's child items into the next item and remove the
//        given item.
//
//    Access:
//        Private
//
//    Parameters:
//        item - The item to be removed.
//    
//    Returns:
//        Pointer to the parent of the deleted item or NULL if the given item invalid
//        or could not be moved.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::MergeIntoNext(QTItem *item) const {
    tint n,t;
    QTItem *parentA,*parentB,*nItem,*aItem;
    
    //Check the validity of the parameters.
    if(item==NULL || item->next==NULL) {
        return NULL;
    }
    
    //Get pointers to related items and check them.
    nItem = item->next;
    parentA = item->parent;
    parentB = nItem->parent;
    if(parentA==NULL || parentB==NULL) {
        return NULL;
    }

    //Assign the children to the new parent.
    for(aItem=item->firstChild ; aItem!=NULL ; aItem=aItem->next) {
        aItem->parent = nItem;
        if(aItem==item->lastChild) {
            break;
        }
    }

    //Move the child items into the next item.
    nItem->firstChild = item->firstChild;
    if(nItem->lastChild == NULL) {
        nItem->lastChild = item->lastChild;
    }
    item->firstChild = NULL;
    item->lastChild = NULL;
    
    //Remove N and index from the items parent and tranfer it to previous item's parent.
    n = item->N;
    t = item->index;
    nItem->N += n;
    nItem->index += t;
    parentA->index -= t;
    parentB->index += t;
    item->N = 0;
    item->index = 0;

    //Now the entry has been emptied remove it.
    return DeleteEmptyItem(item);    
}

//-------------------------------------------------------------------------------------
//    Method:
//        DeleteItem
//
//    Purpose:
//        This method is used delete an item at the specified position in the main list.
//        Afterwards the path to the root is traced from the deleted item back and the
//        parent lists reorganized accordingly.
//
//    Access:
//        Private
//
//    Parameters:
//        pos - The position of the item in the main list to be deleted.
//    
//    Returns:
//        True if successful and false on failure.
//-------------------------------------------------------------------------------------


template <class X> bool BOQueueTree<X>::DeleteItem(tint pos) {
    QTItem *item;
    
    //Ensure that the position is in range.
    if(pos<0) {
        pos = 0;
    }
    else if(pos>=Size()) {
        pos = Size()-1;
    }
    
    //Seek out item that is to be deleted.
    item = SeekItem(m_Root,pos);
    if(item==NULL) {
        return false;
    }
    
    //Process backwards to the root from the deleted item.
    do {
        if(item==m_Root) {
            //If the root item is reached check to see if it an unnessary layer the can be removed.
            if(item->N==1) {
                if(MergeDownRoot()) {
                    return true;
                }
                item = NULL;
            }
            else {
                return true;
            }
        }
        else if(item->N==0) {
            //The item can be removed from the list as it has no children.
            //This automatically applies to main list items as they cannot have children.
            item = DeleteEmptyItem(item);
        }
        else if(item->N<g_BOQT_MinQueueLength) {
            //Check to see if the current item children falls below the minimum queue length and if there
            //exists any queues into which they can be merged.
            if(item->prev!=NULL && (item->prev->N + item->N)<g_BOQT_MaxQueueLength) {
                item = MergeIntoPrevious(item);
            }
            else if(item->next!=NULL && (item->next->N + item->N)<g_BOQT_MaxQueueLength) {
                item = MergeIntoNext(item);
            }
            else {
                //If merge not possible before or afterwards then do nothing.
                return true;
            }
        }
        else {
            //Nothing else to do on this path.
            return true;
        }
    } while(item!=NULL);
    
    //Failure from fact that loop has exited without a successful return path.
    return false;
}


//-------------------------------------------------------------------------------------
//    Method:
//        Size
//
//    Purpose:
//        Return the number of items of the main list.
//
//    Access:
//        Public
//    
//    Returns:
//        The number of items in the main list.
//-------------------------------------------------------------------------------------

template <class X> tint BOQueueTree<X>::Size() const {
    if(m_Root==NULL) {
        return 0;
    }
    return m_Root->index;
}

//-------------------------------------------------------------------------------------
//    Method:
//        Add
//
//    Purpose:
//        Add a new item a the specified position.
//
//    Access:
//        Public
//    
//    Parameters:
//        pos - The position to add the new entry to.
//        x   - The item to attach to the position
//
//    Returns:
//        True if successful otherwise false.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::Add(tint& pos,const X& x) {
    RTItem *item;
    
    item = reinterpret_cast<RTItem *>(AddItem(pos)); //lint !e826 All items are allocate as RTItem structures.
    if(item==NULL) {
        return false;
    }
    item->item = x;
    return true;
}

//-------------------------------------------------------------------------------------
//    Method:
//        Item
//
//    Purpose:
//        Gets access to the direct main list item via its position.
//
//    Access:
//        Public
//    
//    Parameters:
//        pos - The position entry to retrieve.
//
//    Returns:
//        The item held at that position or NULL if the position doesn't exist.
//-------------------------------------------------------------------------------------

template <class X> QTItem *BOQueueTree<X>::Item(tint pos) {
    QTItem *item;
    
    if(pos<0 || pos>=Size())
        return NULL;
    item = SeekItem(m_Root,pos);
    return item;
}

//-------------------------------------------------------------------------------------
//    Method:
//        SeekPosition
//
//    Purpose:
//        Calculate the position of a given item by recursively working back from
//        the main list through to the root.
//
//    Access:
//        Private
//    
//    Parameters:
//        item - The item to find the position of.
//
//    Returns:
//        The position of the item in the main list.
//-------------------------------------------------------------------------------------

template <class X> tint BOQueueTree<X>::SeekPosition(QTItem *item) {
    QTItem *parent;
    tint pos = 0;
    
    if(item!=NULL) {
        if(item->parent!=NULL) {
            parent = item->parent;

            if(item!=parent->firstChild) {
                do {
                    item=item->prev;
                    if(item!=NULL) {
                        pos += item->index;
                    }
                } while(item!=parent->firstChild);
            }

            pos += SeekPosition(parent);
            return pos;
        }
        else {
            return 0;
        }
    }
    return -1;
}

//-------------------------------------------------------------------------------------
//    Method:
//        Position
//
//    Purpose:
//        Obtain the list position of the given item.
//
//    Access:
//        Public
//    
//    Parameters:
//        item - The item to find the position of.
//
//    Returns:
//        The position of the item in the main list.
//-------------------------------------------------------------------------------------

template <class X> int BOQueueTree<X>::Position(QTItem *item) {
    return SeekPosition(item);
}

//-------------------------------------------------------------------------------------
//    Method:
//        Delete
//
//    Purpose:
//        Delete the item at the given position.
//
//    Access:
//        Public
//    
//    Parameters:
//        pos - The position of the entry to delete.
//
//    Returns:
//        True if successful otherwise false.
//-------------------------------------------------------------------------------------

template <class X> bool BOQueueTree<X>::Delete(tint pos) {
    bool res = DeleteItem(pos);
    return res;
}

//-------------------------------------------------------------------------------------
//    Method:
//        operator []
//
//    Purpose:
//        Array operator for the structure.
//
//    Access:
//        Public
//    
//    Parameters:
//        pos - The position of the entry to delete.
//
//    Returns:
//        The item at the given position.
//-------------------------------------------------------------------------------------
        
template <class X> X& BOQueueTree<X>::operator [] (tint pos) {
    RTItem *item;
    
    if(pos<0 || pos>=Size()) {
        return nullObject;
    }
    item = reinterpret_cast<RTItem *>(SeekItem(m_Root,pos)); //lint !e826 All items are allocated as RTItem structures.
    if(item!=NULL) {
        return item->item;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------

template <class X> const X& BOQueueTree<X>::Find(tint pos) const
{
    RTItem *item;

    if(pos<0 || pos>=Size())
    {
        return nullObject;
    }
    item = reinterpret_cast<RTItem *>(SeekItem(m_Root,pos)); //lint !e826 All items are allocated as RTItem structures.
    if(item!=NULL) 
    {
        return item->item;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------

template <class X> X& BOQueueTree<X>::Find(tint pos)
{
    RTItem *item;

    if(pos<0 || pos>=Size())
    {
        return nullObject;
    }
    item = reinterpret_cast<RTItem *>(SeekItem(m_Root,pos)); //lint !e826 All items are allocated as RTItem structures.
    if(item!=NULL) 
    {
        return item->item;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------
//    Method:
//        Start
//
//    Purpose:
//        To provide a starting point for moving through the main list either backwards
//        or forwards from a given position.
//
//    Access:
//        Public
//    
//    Parameters:
//        pos - The position to start parsing from.
//-------------------------------------------------------------------------------------

template <class X> void BOQueueTree<X>::Start(tint pos) {
    m_Current = SeekItem(m_Root,pos);
}

//-------------------------------------------------------------------------------------
//    Method:
//        Previous
//
//    Purpose:
//        Parse up the main list from the current position.
//
//    Access:
//        Public
//    
//    Returns:
//        The current item
//-------------------------------------------------------------------------------------

template <class X> X& BOQueueTree<X>::Previous() {
    RTItem *item;
    
    if(m_Current==NULL) {
        return nullObject;
    }
    item = reinterpret_cast<RTItem *>(m_Current); //lint !e826 All items are allocated as RTItem structures.
    m_Current = m_Current->prev;
    return item->item;
}

//-------------------------------------------------------------------------------------
//    Method:
//        Next
//
//    Purpose:
//        Parse up the main list from the current position.
//
//    Access:
//        Public
//    
//    Returns:
//        The current item
//-------------------------------------------------------------------------------------

template <class X> X& BOQueueTree<X>::Next() {
    RTItem *item;
    
    if(m_Current==NULL) {
        return nullObject;
    }
    item = reinterpret_cast<RTItem *>(m_Current); //lint !e826 All items are allocated as RTItem structures.
    m_Current = m_Current->next;
    return item->item;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
