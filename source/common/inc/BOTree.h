#ifndef __OMEGA_COMMON_BOTREE_H
#define __OMEGA_COMMON_BOTREE_H

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

template <class T,class S> class BOTree 
{
    private:

        typedef enum {
            BLACK    =    0,
            RED        =    1
        } NodeColour;

        typedef struct s_RBNode {
            struct s_RBNode *parent;
            struct s_RBNode *left;
            struct s_RBNode *right;
            NodeColour colour;
            T key;
            S entry;
        } RBNode;

        RBNode *root;
        mutable RBNode *parseItem;
        mutable RBNode *walkItem;

        tint amount;

        static S nullObject;
        static T nullIndex;

        RBNode rbNullNode,*RBNULL;

        tint Cmp(const T& a,const T& b) const;

        void RotateLeft(RBNode *x);
        void RotateRight(RBNode *y);

        bool Insert(RBNode *z);

        void Successor(const RBNode *x,RBNode **w) const;
        void DeleteFix(RBNode *x);
        void Delete(RBNode *z);

        void ParseToLeft() const;
        void ParseToRight() const;

        void FindItem(const T& t,RBNode **itemAddr) const;

        void DeleteTree(RBNode *item);

    public:
        BOTree();
        ~BOTree();

        bool Start() const;
        bool Start(const T& t) const;

        bool End() const;

        S& Find(const T& t);
        const S& Find(const T& t) const;
        
        bool IsStart() const;
        bool IsEnd() const;

        S& First();
        const S& First() const;

        S& Last();
        const S& Last() const;

        const T& FirstIndex() const;

        const T& LastIndex() const;

        S& Previous();
        const S& Previous() const;

        const T& PreviousIndex() const;

        S& Next();
        const S& Next() const;

        const T& NextIndex() const;

        S& Current();
        const S& Current() const;

        const T& CurrentIndex() const;

        tint Count() const;
        
        void Remove(const T& t);

        S& operator[] (const T& t);
        bool Exist(const T& t) const;

        S& Walk();
        const S& Walk() const;

        S& Left();
        const S& Left() const;

        S& Right();
        const S& Right() const;

        const T& WalkIndex() const;
        
        const S& Get(const T& t) const;
        void Set(const T& index,const S& item);
};

#if defined(OMEGA_MACOSX)
template <typename T, typename S> S BOTree<T,S>::nullObject;
template <typename T, typename S> T BOTree<T,S>::nullIndex;
#endif

//-------------------------------------------------------------------------------------------

template <class T,class S> BOTree<T,S>::BOTree() : amount(0)
{ //lint !e1926 !e1927 Attributes are setup in the constructor.
    rbNullNode.parent=&rbNullNode;
    rbNullNode.left=&rbNullNode;
    rbNullNode.right=&rbNullNode;
    rbNullNode.colour=BLACK;
    rbNullNode.key=nullIndex;
    rbNullNode.entry=nullObject;
    RBNULL=&rbNullNode;
    root=RBNULL;
    parseItem=RBNULL;
    walkItem=RBNULL;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> BOTree<T,S>::~BOTree() 
{
    try
    {
        if(root!=RBNULL)
        {
            DeleteTree(root);
        }
    }
    catch(...) {}
} //lint !e1740

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::DeleteTree(RBNode *item) 
{
    if(item->left!=RBNULL)
    {
        DeleteTree(item->left);
    }
    if(item->right!=RBNULL)
    {
        DeleteTree(item->right);
    }
    delete item;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> tint BOTree<T,S>::Cmp(const T& a,const T& b) const
{
    if(a<b)
    {
        return -1;
    }
    if(a>b)
    {
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::RotateLeft(RBNode *x) 
{
    RBNode *y;

    y=x->right;
    x->right=y->left;
    if(y->left!=RBNULL)
    {
        y->left->parent=x;
    }
    y->parent=x->parent;
    if(x->parent==RBNULL) 
    {
        root=y;
    }
    else 
    {
        if(x==x->parent->left)
        {
            x->parent->left=y;
        }
        else
        {
            x->parent->right=y;
        }
    }
    y->left=x;
    x->parent=y;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::RotateRight(RBNode *y) 
{
    RBNode *x;

    x=y->left;
    y->left=x->right;
    if(x->right!=RBNULL)
    {
        x->right->parent=y;
    }
    x->parent=y->parent;
    if(y->parent==RBNULL) 
    {
        root=x;
    }
    else 
    {
        if(y==y->parent->left)
        {
            y->parent->left=x;
        }
        else
        {
            y->parent->right=x;
        }
    }
    x->right=y;
    y->parent=x;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::Insert(RBNode *z) 
{
    tint cmp;
    RBNode *x=root,*y=RBNULL;

    while(x!=RBNULL) 
    {
        y=x;
        cmp=Cmp(z->key,x->key);
        if(cmp<0)
        {
            x=x->left;
        }
        else if(cmp>0)
        {
            x=x->right;
        }
        else
        {
            return false;
        }
    }

    z->parent=y;
    if(y==RBNULL) 
    {
        root=z;
    }
    else 
    {
        cmp=Cmp(z->key,y->key);
        if(cmp<0)
        {
            y->left=z;
        }
        else
        {
            y->right=z;
        }
    }

    z->left=RBNULL;
    z->right=RBNULL;
    z->colour=RED;

    x=z;

    while(x!=root && x->parent->colour==RED) 
    {
        if(x->parent==x->parent->parent->left) 
        {
            y=x->parent->parent->right;
            if(y->colour==RED) 
            {
                x->parent->colour=BLACK;
                y->colour=BLACK;
                x->parent->parent->colour=RED;
                x=x->parent->parent;
            }
            else 
            {
                if(x==x->parent->right) 
                {
                    x=x->parent;
                    RotateLeft(x);
                }
                x->parent->colour=BLACK;
                x->parent->parent->colour=RED;
                RotateRight(x->parent->parent);
            }
        }
        else 
        {
            y=x->parent->parent->left;
            if(y->colour==RED) 
            {
                x->parent->colour=BLACK;
                y->colour=BLACK;
                x->parent->parent->colour=RED;
                x=x->parent->parent;
            }
            else 
            {
                if(x==x->parent->left) 
                {
                    x=x->parent;
                    RotateRight(x);
                }
                x->parent->colour=BLACK;
                x->parent->parent->colour=RED;
                RotateLeft(x->parent->parent);
            }
        }
    }

    root->colour=BLACK;
    amount++;

    return true;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::Successor(const RBNode *x,RBNode **w) const
{
    RBNode *y;

    if(x->right!=RBNULL) 
    {
        for(y=x->right;y->left!=RBNULL;y=y->left) ;
    }
    else 
    {
        y=x->parent;
        while(y!=RBNULL && x==y->right) 
        {
            x=y;
            y=y->parent;
        }
    }
    *w=y;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::DeleteFix(RBNode *x) 
{
    RBNode *w;

    while(x!=root && x->colour==BLACK) 
    {
        if(x==x->parent->left) 
        {
            w=x->parent->right;
            if (w->colour==RED) 
            {
                w->colour=BLACK;
                x->parent->colour=RED;
                RotateLeft(x->parent);
                w=x->parent->right;
            }
            if(w->left->colour==BLACK && w->right->colour==BLACK) 
            {
                w->colour=RED;
                x=x->parent;
            }
            else 
            {
                if(w->right->colour == BLACK) 
                {
                    w->left->colour=BLACK;
                    w->colour=RED;
                    RotateRight(w);
                    w=x->parent->right;
                }
                w->colour=x->parent->colour;
                x->parent->colour = BLACK;
                w->right->colour = BLACK;
                RotateLeft(x->parent);
                x=root;
            }
        }
        else 
        {
            w=x->parent->left;
            if(w->colour==RED) 
            {
                w->colour=BLACK;
                x->parent->colour=RED;
                RotateRight(x->parent);
                w=x->parent->left;
            }
            if(w->right->colour==BLACK && w->left->colour==BLACK) 
            {
                w->colour=RED;
                x=x->parent;
            }
            else 
            {
                if(w->left->colour==BLACK) 
                {
                    w->right->colour=BLACK;
                    w->colour=RED;
                    RotateLeft(w);
                    w=x->parent->left;
                }
                w->colour=x->parent->colour;
                x->parent->colour=BLACK;
                w->left->colour=BLACK;
                RotateRight(x->parent);
                x=root;
            }
        }
    }
    x->colour=BLACK;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::Delete(RBNode *z) 
{
    RBNode *x,*y;

    if(z->left==RBNULL || z->right==RBNULL)
    {
        y=z;
    }
    else
    {
        Successor(z,&y);
    }

    if(y->left!=RBNULL)
    {
        x=y->left;
    }
    else
    {
        x=y->right;
    }

    x->parent=y->parent;

    if(y->parent==RBNULL) 
    {
        root=x;
    }
    else 
    {
        if(y==y->parent->left)
        {
            y->parent->left=x;
        }
        else
        {
            y->parent->right=x;
        }
    }

    if(y!=z) 
    {
        z->key=y->key;
        z->entry=y->entry;
    }

    if(y->colour==BLACK)
    {
        DeleteFix(x);
    }

    delete y;
    amount--;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::ParseToLeft() const {
    if(parseItem->left!=RBNULL) 
    { 
        for(parseItem=parseItem->left;parseItem->right!=RBNULL;parseItem=parseItem->right) ;
    }
    else 
    {
        RBNode *tItem;
        do 
        {
            tItem=parseItem;
            parseItem=parseItem->parent;
        } while(parseItem!=RBNULL && parseItem->left==tItem);
    }
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::ParseToRight() const {
    if(parseItem->right!=RBNULL) 
    { 
        for(parseItem=parseItem->right;parseItem->left!=RBNULL;parseItem=parseItem->left) ;
    }
    else 
    {
        RBNode *tItem;
        do 
        {
            tItem=parseItem;
            parseItem=parseItem->parent;
        } while(parseItem!=RBNULL && parseItem->right==tItem);
    }
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::FindItem(const T& t,RBNode **itemAddr) const
{
    tint cmp;
    RBNode *item=root;

    while(item!=RBNULL) 
    {
        cmp=Cmp(t,item->key);
        if(cmp==0) 
        {
            *itemAddr=item;
            return;
        }
        else if(cmp<0)
        {
            item=item->left;
        }
        else
        {
            item=item->right;
        }
    }
    *itemAddr=RBNULL;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::Start() const
{
    parseItem=root;
    if(parseItem!=RBNULL) 
    {
        while(parseItem->left!=RBNULL)
        {
            parseItem=parseItem->left;
        }
    }
    else
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::End() const 
{
    parseItem=root;
    if(parseItem!=RBNULL) 
    {
        while(parseItem->right!=RBNULL)
        {
            parseItem=parseItem->right;
        }
    }
    else
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::Start(const T& t) const
{
    tint cmp;

    parseItem=root;
    while(parseItem!=RBNULL) 
    {
        cmp=Cmp(t,parseItem->key);
        if(cmp==0)
        {
            return true;
        }
        else if(cmp<0)
        {
            parseItem=parseItem->left;
        }
        else
        {
            parseItem=parseItem->right;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Find(const T& t)
{
    RBNode *pItem;
    tint cmp;

    pItem=root;
    while(pItem!=RBNULL) 
    {
        cmp=Cmp(t,pItem->key);
        if(cmp==0)
        {
            return pItem->entry;
        }
        else if(cmp<0)
        {
            pItem=pItem->left;
        }
        else
        {
            pItem=pItem->right;
        }
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Find(const T& t) const
{
    RBNode *pItem;
    tint cmp;

    pItem=root;
    while(pItem!=RBNULL) 
    {
        cmp=Cmp(t,pItem->key);
        if(cmp==0)
        {
            return pItem->entry;
        }
        else if(cmp<0)
        {
            pItem=pItem->left;
        }
        else
        {
            pItem=pItem->right;
        }
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::IsStart() const
{
    return (parseItem==RBNULL) ? true : false;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::IsEnd() const
{
    return (parseItem==RBNULL) ? true : false;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::First()
{
    RBNode *item=root;

    if(root==RBNULL)
    {
        return nullObject;
    }
    while(item->left!=RBNULL)
    {
        item=item->left;
    }
    return item->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::First() const
{
    RBNode *item=root;

    if(root==RBNULL)
    {
        return nullObject;
    }
    while(item->left!=RBNULL)
    {
        item=item->left;
    }
    return item->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Last()
{
    RBNode *item=root;

    if(root==RBNULL)
    {
        return nullObject;
    }
    while(item->right!=RBNULL) 
    {
        item=item->right;
    }
    return item->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Last() const 
{
    RBNode *item=root;

    if(root==RBNULL)
    {
        return nullObject;
    }
    while(item->right!=RBNULL) 
    {
        item=item->right;
    }
    return item->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const T& BOTree<T,S>::FirstIndex() const
{
    RBNode *item=root;

    if(root==RBNULL)
    {
        return nullIndex;
    }
    while(item->left!=RBNULL)
    {
        item=item->left;
    }
    return item->key;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const T& BOTree<T,S>::LastIndex() const
{
    RBNode *item=root;

    if(root==RBNULL)
    {
        return nullIndex;
    }
    while(item->right!=RBNULL) 
    {
        item=item->right;
    }
    return item->key;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Previous()
{
    if(parseItem!=RBNULL) 
    {
        RBNode *item=parseItem;
        ParseToLeft();
        return item->entry;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Previous() const
{
    if(parseItem!=RBNULL) 
    {
        RBNode *item=parseItem;
        ParseToLeft();
        return item->entry;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const T& BOTree<T,S>::PreviousIndex() const
{
    if(parseItem!=RBNULL) 
    {
        RBNode *item=parseItem;
        ParseToLeft();
        return item->key;
    }
    return nullIndex;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Next()
{
    if(parseItem!=RBNULL) 
    {
        RBNode *item=parseItem;
        ParseToRight();
        return item->entry;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Next() const
{
    if(parseItem!=RBNULL) 
    {
        RBNode *item=parseItem;
        ParseToRight();
        return item->entry;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const T& BOTree<T,S>::NextIndex() const
{
    if(parseItem!=RBNULL) 
    {
        RBNode *item=parseItem;
        ParseToRight();
        return item->key;
    }
    return nullIndex;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Current()
{
    if(parseItem!=RBNULL) 
    {
        return parseItem->entry;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Current() const
{
    if(parseItem!=RBNULL) 
    {
        return parseItem->entry;
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const T& BOTree<T,S>::CurrentIndex() const
{
    if(parseItem!=RBNULL) 
    {
        return parseItem->key;
    }
    return nullIndex;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> tint BOTree<T,S>::Count() const
{
    return amount;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::Remove(const T& t) 
{
    RBNode *item;

    FindItem(t,&item);
    if(item!=RBNULL)
    {
        Delete(item);
    }
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::operator[] (const T& t) 
{
    RBNode *item;

    FindItem(t,&item);
    if(item==RBNULL) 
    {
        item=new RBNode;
        item->key=t;
        if(!Insert(item))
        {
            return RBNULL->entry;
        }
    }
    return item->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> bool BOTree<T,S>::Exist(const T& t) const
{
    RBNode *item;
    FindItem(t,&item);
    return (item!=RBNULL) ? true : false;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Walk()
{
    walkItem=root;
    if(walkItem==RBNULL)
    {
        return nullObject;
    }
    return walkItem->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Walk() const
{
    walkItem=root;
    if(walkItem==RBNULL)
    {
        return nullObject;
    }
    return walkItem->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Left()
{
    walkItem=walkItem->left;
    if(walkItem==RBNULL)
    {
        return nullObject;
    }
    return walkItem->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Left() const
{
    walkItem=walkItem->left;
    if(walkItem==RBNULL)
    {
        return nullObject;
    }
    return walkItem->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> S& BOTree<T,S>::Right()
{
    walkItem=walkItem->right;
    if(walkItem==RBNULL)
    {
        return nullObject;
    }
    return walkItem->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const S& BOTree<T,S>::Right() const
{
    walkItem=walkItem->right;
    if(walkItem==RBNULL)
    {
        return nullObject;
    }
    return walkItem->entry;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> const T& BOTree<T,S>::WalkIndex() const
{
    if(walkItem!=RBNULL)
    {
        return walkItem->key;
    }
    else
    {
        return nullIndex;
    }
}

//-------------------------------------------------------------------------------------------

template <class T,class S> inline const S& BOTree<T,S>::Get(const T& t) const
{
    RBNode *pItem;
    tint cmp;

    pItem=root;
    while(pItem!=RBNULL) 
    {
        cmp=Cmp(t,pItem->key);
        if(cmp==0)
        {
            return pItem->entry;
        }
        else if(cmp<0)
        {
            pItem=pItem->left;
        }
        else
        {
            pItem=pItem->right;
        }
    }
    return nullObject;
}

//-------------------------------------------------------------------------------------------

template <class T,class S> void BOTree<T,S>::Set(const T& index,const S& entry)
{
    RBNode *item;

    FindItem(index,&item);
    if(item==RBNULL) 
    {
        item = new RBNode;
        item->key = index;
        item->entry = entry;
        Insert(item);
    }
    else
    {
        item->entry = entry;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
