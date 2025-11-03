//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_ARRAY_H
#define __OMEGA_COMMON_ARRAY_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonDLL.h"
#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------
#include <memory.h>
#include <stdio.h>
#include <string.h>
//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_LINUX)
#include <string.h>
//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX)
#include <string.h>
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

template<class TYPE> inline void arrayConstructElements(TYPE* pElements, tint nCount)
{

    // first do bit-wise zero initialization
    ::memset((void*)pElements, 0, nCount * sizeof(TYPE));
}

//-------------------------------------------------------------------------------------------

template<class TYPE> inline void arrayDestructElements(TYPE* pElements, tint nCount)
{
    // call the destructor(s)
    for (; nCount--; pElements++)
    {
        pElements->~TYPE();
    }
}

//-------------------------------------------------------------------------------------------

template<class TYPE> inline void arrayCopyElements(TYPE* pDest, const TYPE* pSrc, tint nCount)
{
    // default is element-copy using assignment
    while (nCount--)
    {
        *pDest++ = *pSrc++;
    }
}

//-------------------------------------------------------------------------------------------
// Array<TYPE, ARG_TYPE>
//-------------------------------------------------------------------------------------------

template <class TYPE, class ARG_TYPE> class Array
{
    public:

        Array();

        // Attributes
        tint GetSize() const;
        tint GetUpperBound() const;
        void SetSize(tint nNewSize, tint nGrowBy = -1);

        // Operations

        // Clean up
        void FreeExtra();
        void RemoveAll();

        // Accessing elements
        TYPE GetAt(tint nIndex) const;
        void SetAt(tint nIndex, ARG_TYPE newElement);
        TYPE& ElementAt(tint nIndex);

        // Direct Access to the element data (may return NULL)
        const TYPE* GetData() const;
        TYPE* GetData();

        // Potentially growing the array
        void SetAtGrow(tint nIndex, ARG_TYPE newElement);
        tint Add(ARG_TYPE newElement);
        tint Append(const Array& src);
        void AppendRaw(const tchar *mem,tint len);
        void Copy(const Array& src);

        // overloaded operator helpers
        TYPE operator[](tint nIndex) const;
        TYPE& operator[](tint nIndex);

        // Operations that move elements around
        void InsertAt(tint nIndex, ARG_TYPE newElement, tint nCount = 1);
        void RemoveAt(tint nIndex, tint nCount = 1);
        void InsertAt(tint nStartIndex, Array* pNewArray);

    protected:

        TYPE* m_pData;   // the actual array of data
        tint m_nSize;     // # of elements (upperBound - 1)
        tint m_nMaxSize;  // max allocated
        tint m_nGrowBy;   // grow amount

    public:
        ~Array();
};

//-------------------------------------------------------------------------------------------
// Array<TYPE, ARG_TYPE> inline functions
//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline int Array<TYPE, ARG_TYPE>::GetSize() const
{
    return m_nSize;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline int Array<TYPE, ARG_TYPE>::GetUpperBound() const
{
    return m_nSize-1;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline void Array<TYPE, ARG_TYPE>::RemoveAll()
{
    SetSize(0, -1);
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline TYPE Array<TYPE, ARG_TYPE>::GetAt(tint nIndex) const
{
    return m_pData[nIndex];
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline void Array<TYPE, ARG_TYPE>::SetAt(tint nIndex, ARG_TYPE newElement)
{
    m_pData[nIndex] = newElement;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline TYPE& Array<TYPE, ARG_TYPE>::ElementAt(tint nIndex)
{
    return m_pData[nIndex];
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline const TYPE* Array<TYPE, ARG_TYPE>::GetData() const
{
    return (const TYPE*)m_pData;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline TYPE* Array<TYPE, ARG_TYPE>::GetData()
{
    return (TYPE*)m_pData;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline tint Array<TYPE, ARG_TYPE>::Add(ARG_TYPE newElement)
{
    tint nIndex = m_nSize;
    SetAtGrow(nIndex, newElement);
    return nIndex;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline TYPE Array<TYPE, ARG_TYPE>::operator[](tint nIndex) const
{
    return GetAt(nIndex);
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> inline TYPE& Array<TYPE, ARG_TYPE>::operator[](tint nIndex)
{
    return ElementAt(nIndex);
}

//-------------------------------------------------------------------------------------------
// Array<TYPE, ARG_TYPE> out-of-line functions
//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> Array<TYPE, ARG_TYPE>::Array()
{
    m_pData = NULL;
    m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> Array<TYPE, ARG_TYPE>::~Array()
{
    if (m_pData != NULL)
    {
        arrayDestructElements<TYPE>(m_pData, m_nSize);
        delete [] (tbyte*)m_pData;
    }
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::SetSize(tint nNewSize, tint nGrowBy)
{
    if (nGrowBy != -1)
    {
        m_nGrowBy = nGrowBy;  // set new size
    }

    if (nNewSize == 0)
    {
        // shrink to nothing
        if (m_pData != NULL)
        {
            arrayDestructElements<TYPE>(m_pData, m_nSize);
            delete[] (tbyte*)m_pData;
            m_pData = NULL;
        }
        m_nSize = m_nMaxSize = 0;
    }
    else if (m_pData == NULL)
    {
        // create one with exact size
        m_pData = (TYPE*) new tbyte[nNewSize * sizeof(TYPE)];
        arrayConstructElements<TYPE>(m_pData, nNewSize);
        m_nSize = m_nMaxSize = nNewSize;
    }
    else if (nNewSize <= m_nMaxSize)
    {
        // it fits
        if (nNewSize > m_nSize)
        {
            // initialize the new elements
            arrayConstructElements<TYPE>(&m_pData[m_nSize], nNewSize-m_nSize);
        }
        else if (m_nSize > nNewSize)
        {
            // destroy the old elements
            arrayDestructElements<TYPE>(&m_pData[nNewSize], m_nSize-nNewSize);
        }
        m_nSize = nNewSize;
    }
    else
    {
        // otherwise, grow array
        tint nGrowBy = m_nGrowBy;

        if (nGrowBy == 0)
        {
            // heuristically determine growth when nGrowBy == 0
            //  (this avoids heap fragmentation in many situations)
            nGrowBy = m_nSize / 8;
            nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
        }

        tint nNewMax;

        if (nNewSize < m_nMaxSize + nGrowBy)
        {
            nNewMax = m_nMaxSize + nGrowBy;  // granularity
        }
        else
        {
            nNewMax = nNewSize;  // no slush
        }

        TYPE* pNewData = (TYPE*) new tbyte[nNewMax * sizeof(TYPE)];

        // copy new data from old
        ::memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));

        // construct remaining elements
        arrayConstructElements<TYPE>(&pNewData[m_nSize], nNewSize-m_nSize);

        // get rid of old stuff (note: no destructors called)
        delete[] (tbyte*)m_pData;
        m_pData = pNewData;
        m_nSize = nNewSize;
        m_nMaxSize = nNewMax;
    }
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> tint Array<TYPE, ARG_TYPE>::Append(const Array& src)
{
    tint nOldSize = m_nSize;
    SetSize(m_nSize + src.m_nSize);
    arrayCopyElements<TYPE>(m_pData + nOldSize, src.m_pData, src.m_nSize);
    return nOldSize;
}

//-------------------------------------------------------------------------------------------

template<class TYPE,class ARG_TYPE> void Array<TYPE, ARG_TYPE>::AppendRaw(const tchar *mem,tint len)
{
    if(mem!=NULL && len>0)
    {
        tint nOldSize = m_nSize;
        SetSize(m_nSize + len);
        arrayCopyElements<TYPE>(m_pData + nOldSize,reinterpret_cast<const TYPE *>(mem),len);
    }
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::Copy(const Array& src)
{
    SetSize(src.m_nSize);
    arrayCopyElements<TYPE>(m_pData, src.m_pData, src.m_nSize);
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::FreeExtra()
{
    if (m_nSize != m_nMaxSize)
    {
        // shrink to desired size
        TYPE* pNewData = NULL;
        if (m_nSize != 0)
        {
            pNewData = (TYPE*) new tbyte[m_nSize * sizeof(TYPE)];
            // copy new data from old
            ::memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
        }

        // get rid of old stuff (note: no destructors called)
        delete[] (tbyte*)m_pData;
        m_pData = pNewData;
        m_nMaxSize = m_nSize;
    }
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::SetAtGrow(tint nIndex, ARG_TYPE newElement)
{
    if (nIndex >= m_nSize)
    {
        SetSize(nIndex+1, -1);
    }
    m_pData[nIndex] = newElement;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::InsertAt(tint nIndex, ARG_TYPE newElement, tint nCount /*=1*/)
{
    if (nIndex >= m_nSize)
    {
        // adding after the end of the array
        SetSize(nIndex + nCount, -1);   // grow so nIndex is valid
    }
    else
    {
        // inserting in the middle of the array
        tint nOldSize = m_nSize;
        SetSize(m_nSize + nCount, -1);  // grow it to new size
        // destroy intial data before copying over it
        arrayDestructElements<TYPE>(&m_pData[nOldSize], nCount);
        // shift old data up to fill gap
        ::memmove(&m_pData[nIndex+nCount], &m_pData[nIndex],(nOldSize-nIndex) * sizeof(TYPE));

        // re-init slots we copied from
        arrayConstructElements<TYPE>(&m_pData[nIndex], nCount);
    }

    // insert new value in the gap
    while (nCount--)
    {
        m_pData[nIndex++] = newElement;
    }
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::RemoveAt(tint nIndex, tint nCount)
{
    // just remove a range
    tint nMoveCount = m_nSize - (nIndex + nCount);
    arrayDestructElements<TYPE>(&m_pData[nIndex], nCount);
    if (nMoveCount)
    {
        ::memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],nMoveCount * sizeof(TYPE));
    }
    m_nSize -= nCount;
}

//-------------------------------------------------------------------------------------------

template<class TYPE, class ARG_TYPE> void Array<TYPE, ARG_TYPE>::InsertAt(tint nStartIndex, Array* pNewArray)
{
    if (pNewArray->GetSize() > 0)
    {
        InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
        for (tint i = 0; i < pNewArray->GetSize(); i++)
        {
            SetAt(nStartIndex + i, pNewArray->GetAt(i));
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
