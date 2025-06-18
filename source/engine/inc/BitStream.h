//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BITSTREAM_H
#define __OMEGA_ENGINE_BITSTREAM_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"
#include "engine/inc/File.h"
#include "engine/inc/Recycler.h"
#include "common/inc/BOTree.h"
#include "common/inc/Allocation.h"

#include <QSet>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class Sequence;
class Associate;

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Bookmark
{
    public:
        Bookmark();
        
        void reset();
        
        Associate *m_item;
        tuint m_offset;
};

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Associate
{
    public:
        Associate();
        
        void reset();
        
        tint m_stream;
        
        Associate *m_prev;
        Associate *m_next;
        
        Sequence *m_sequence;
        
        common::BOTree<tint,Bookmark *> m_bookmarks;
        
        tubyte *m_buffer;
        
        tuint m_offset;
        tuint m_length;
};

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Bitstream
{
    public:
        friend class Sequence;

        typedef struct
        {
            tuint rShiftA;
            tuint rShiftB;
            tuint rMaskA;
            tuint rMaskB;
            tuint wShiftA;
            tuint wShiftB;
            tuint wMask1A;
            tuint wMask1B;
            tuint wMask2A;
            tuint wMask2B;
            tint width;
            tint required;
            tint newOffset;
        } BSMask;
        
        typedef enum
        {
            e_streamMode1 = 0,
            e_streamMode2
        } Mode;

    public:
        Bitstream(Mode m = e_streamMode1);
        virtual ~Bitstream();
        
        static void start();
        static void stop();
        
        bool open(File *file);
        void close();
        
        bool move(tint bkID);
        bool move(tint bkID,tint offset,bool toStream=false);
        
        bool moveTo(tint bkID,tint offset,const QSet<tint>& exSeqSet);

        tint bookmark();
        tint bookmark(tint bkID,tint offset,bool toStream=false);
        
        bool mark(tint stream);
        bool mark(tint stream,tint bkMark,tint offset=0);
        
        Sequence *getSequence(tint stream);
        
        void complete(tint bkMark);
        
        bool seek(tint offset,File::Position position);
        
        const File *file() const;

        static bool isLittleEndian();
        
        static BSMask *m_maskI[33];
        static tuint m_w1maskI[32];
        
    protected:
    
        static common::Allocation m_maskAlloc;
        static tuint *m_mask[33][8];
        
        Mode m_mode;
        
        File *m_file;
        common::BOTree<tint,Sequence *> m_sequences;
        common::BOTree<tint,Bookmark *> m_bookmarks;
        Associate *m_firstItem;
        Associate *m_lastItem;
        
        Recycler<Associate> m_recycleAssociate;
        Recycler<Bookmark> m_recycleBookmark;
        Recycler<Sequence> m_recycleSequence;
        
        tubyte *m_recycleBuffer;
        
        Associate *m_currentItem;
        tuint m_bitOffset;
        
        bool m_fileEnd;
        tint m_nextBookID;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        void reset();
        
        tubyte *getFileBuffer();
        void returnFileBuffer(tubyte *buffer);
        
        void associateWithSequence(Associate *item,Sequence *seq);
        
        bool readInNextBuffer();
        
        tint newBookmark(Associate *item,tint offset);
        
        static Sequence *getNewSequence(void *p);
        virtual Sequence *newSequence();
};

//-------------------------------------------------------------------------------------------

const tint c_bitstreamBufferSize = 1024;

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

