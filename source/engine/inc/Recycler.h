//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_RECYCLER_H
#define __OMEGA_ENGINE_RECYCLER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"
#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

template <class X> class Recycler
{
    public:
        typedef X* (*FactoryFunc)(void *);
    
    public:
        Recycler();
        Recycler(FactoryFunc fFactory,void *fData);
        ~Recycler();
        
        X *get();
        void put(X *c);
    
    private:
    
        typedef struct s_RecycleItem
        {
            struct s_RecycleItem *next;
            X *item;
        } RecycleItem;
        
        RecycleItem *m_firstItem;
        RecycleItem *m_spareItem;
        
        FactoryFunc m_fFactory;
        void *m_fData;
        
        void getSpare(RecycleItem **pItem);
        void returnSpare(RecycleItem *item);
};

//-------------------------------------------------------------------------------------------

template <class X> Recycler<X>::Recycler() : m_firstItem(0),
    m_spareItem(0),
    m_fFactory(0),
    m_fData(0)
{}

//-------------------------------------------------------------------------------------------

template <class X> Recycler<X>::Recycler(FactoryFunc fFactory,void *fData) : m_firstItem(0),
    m_spareItem(0),
    m_fFactory(fFactory),
    m_fData(fData)
{}

//-------------------------------------------------------------------------------------------

template <class X> Recycler<X>::~Recycler()
{
    try
    {
        RecycleItem *item;
        
        while(m_firstItem!=0)
        {
            item = m_firstItem;
            m_firstItem = m_firstItem->next;
            delete item->item;
            delete item;
        }
        
        while(m_spareItem!=0)
        {
            item = m_spareItem;
            m_spareItem = m_spareItem->next;
            delete item;
        }
        
        m_firstItem = 0;
        m_spareItem = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

template <class X> void Recycler<X>::getSpare(RecycleItem **pItem)
{
    if(m_spareItem==0)
    {
        *pItem = new RecycleItem;
    }
    else
    {
        *pItem = m_spareItem;
        m_spareItem = m_spareItem->next;
    }    
}

//-------------------------------------------------------------------------------------------

template <class X> void Recycler<X>::returnSpare(RecycleItem *item)
{
    item->item =0 ;
    item->next = m_spareItem;
    m_spareItem = item;
}

//-------------------------------------------------------------------------------------------

template <class X> X *Recycler<X>::get()
{
    X *c;
    RecycleItem *item;
    
    if(m_firstItem==0)
    {
        if(m_fFactory!=0)
        {
            c = m_fFactory(m_fData);
        }
        else
        {
            c = new X;
        }
    }
    else
    {
        item = m_firstItem;
        m_firstItem = item->next;
        c = item->item;
        returnSpare(item);
        c->reset();
    }
    return c;
}

//-------------------------------------------------------------------------------------------

template <class X> void Recycler<X>::put(X *c)
{
    RecycleItem *item;
    
    getSpare(&item);
    item->item = c;
    item->next = m_firstItem;
    m_firstItem = item;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
