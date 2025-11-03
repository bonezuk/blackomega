//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_IFFFORMCHUNK_H
#define __OMEGA_ENGINE_VIOLETOMEGA_IFFFORMCHUNK_H
//-------------------------------------------------------------------------------------------

#include "engine/violetomega/inc/IFFChunk.h"
#include "engine/violetomega/inc/IFFFile.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

#define IFF_FORM_ID IFF_ID('F','O','R','M')

//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFFormChunk : public IFFChunk
{
    public:
        class Iterator;
    
    public:
        IFFFormChunk();
        virtual ~IFFFormChunk();
        
        virtual bool isEmpty() const;
        virtual int numberOfChunks() const;
        
        virtual IFFChunkSPtr at(int index);
        
        virtual Iterator begin();
        virtual Iterator end();
    
        virtual bool scan();
        
        virtual IFFID formType() const;
        
    protected:
        
        IFFID m_formType;
        QVector<IFFChunkSPtr> m_chunks;
    
};

//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFFormChunk::Iterator
{
    public:
        friend class IFFFormChunk;
    
    public:
        Iterator();
        Iterator(const Iterator& rhs);
        
        virtual const Iterator& operator = (const Iterator& rhs);
        
        virtual bool operator == (const Iterator& b);
        virtual bool operator != (const Iterator& b);
        
        virtual IFFChunkSPtr operator *();
        
        virtual IFFChunkSPtr operator ++();
        virtual IFFChunkSPtr operator ++(int unused);
        
        const int& index() const;
        
    protected:
    
        IFFFormChunk *m_form;
        int m_index;
        
        Iterator(IFFFormChunk *pForm,int index);
        bool isEqual(const Iterator& rhs) const;
};

//-------------------------------------------------------------------------------------------

typedef QSharedPointer<IFFFormChunk> IFFFormChunkSPtr;

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
