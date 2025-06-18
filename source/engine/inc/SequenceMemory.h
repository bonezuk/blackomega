//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SEQUENCEMEMORY_H
#define __OMEGA_ENGINE_SEQUENCEMEMORY_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BString.h"
#include "common/inc/UString.h"
#include "common/inc/Log.h"

#include <QMap>

#include <QSharedPointer>

#include "common/inc/Array.h"
#include "common/inc/Mutex.h"
#include "engine/inc/Sequence.h"



//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT SequenceMemory : public Sequence
{
    public:
        friend class Bitstream;
        
    public:
        SequenceMemory(common::Array<tubyte,tubyte>& array);
        SequenceMemory(common::Array<tubyte,tubyte>& array,tint bitLength);
        SequenceMemory(QSharedPointer<common::Array<tubyte,tubyte> >& array);
        SequenceMemory(QSharedPointer<common::Array<tubyte,tubyte> >& array,tint bitLength);

        virtual ~SequenceMemory(); //
        
        static void generateArray(const tubyte *mem,tint len,QSharedPointer<common::Array<tubyte,tubyte> >& array);
        
        virtual bool move(tint bkMark); //
                
        virtual void reset(); //
                
        virtual tint bookmark(); //
        
        virtual tint copy(tbyte *mem,tint len); //
        
        virtual bool seek(tint offset);

    protected:
        
        static common::Mutex m_mutex;
        static tint m_nextBookmark;
        
        QSharedPointer<common::Array<tubyte,tubyte> > m_sArray;
        QMap<tint,tint> m_bookmarks;
        tint m_originalLen;
        
        virtual void printError(const tchar *strR,const tchar *strE) const; //
        
        virtual bool previous(); //
        virtual bool next(); //
        virtual bool checkNext(); //
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
