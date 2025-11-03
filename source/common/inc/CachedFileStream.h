//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_CACHEDFILESTREAM_H
#define __OMEGA_COMMON_CACHEDFILESTREAM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/AsynchronousFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT CachedFileStream : public QObject
{
    public:
        Q_OBJECT

    public:
        static const tint c_blockSize = 1024;

        CachedFileStream(QObject *parent = 0);
        virtual ~CachedFileStream();

        virtual bool open(const QString& name);
        virtual void close();

        virtual tint read(tchar *mem,tint64 fromPosition,tint noBytes);

        virtual bool isCached(tint64 fromPosition,tint noBytes) const;
        virtual tint cache(tint64 fromPosition,tint noBytes);

        virtual tint64 size() const;

        virtual void drop(tint64 fromPosition,tint noBytes);
        virtual void clear();

        // All pending read requests are removed and the cache cleared with the exception of the given range.
        virtual void clearWithRetention(tint64 fromPosition,tint noBytes);

        virtual const AsynchronousFileReader *file() const;

#if defined(OMEGA_MAC_STORE)
        virtual void setCheckOutFlag(bool flag);
#endif

    protected:
        AsynchronousFileReader *m_pFile;

        // Cache is broken into a series of equally sized blocks of length c_blockSize.
        QMap<tint64,QSharedPointer<QByteArray> > m_cache; // < block index , data >

        // In order to delete or invalidated parts of the cache that don't fit exacty into a block
        // the cache mask is used. To each cached data block a list can be associated defining ranges
        // which define those areas in the block that are undefined.
        QMap<tint64,QMap<tint,tint> > m_cacheMask; // < block index, sortedMap(< offset, length >) >

        // On going read operations <block index, QPair<async read ID, read status code 0=pending, 1=error, removed on completion> >
        QMap<tint64,QPair<tint,tint> > m_readOps;

#if defined(OMEGA_MAC_STORE)
        bool m_checkOutFlag;
#endif

        virtual void printError(const tchar *strR,const tchar *strE) const;

        tint64 indexFromPosition(tint64 position) const;
        tint offsetInBlock(tint64 position) const;
        tint64 positionFromIndex(tint64 blockIdx) const;

        virtual AsynchronousFileReader *getFile();
        virtual const AsynchronousFileReader *getFileConst() const;
        virtual QMap<tint64,QSharedPointer<QByteArray> >& getCache();
        virtual const QMap<tint64,QSharedPointer<QByteArray> >& getCacheConst() const;
        virtual QMap<tint64,QMap<tint,tint> >& getCacheMask();
        virtual const QMap<tint64,QMap<tint,tint> >& getCacheMaskConst() const;
        virtual QMap<tint64,QPair<tint,tint> >& readOps();
        virtual const QMap<tint64,QPair<tint,tint> >& readOpsConst() const;

        virtual bool isOffsetIntersection(tint offsetA,tint lengthA,tint offsetB,tint lengthB) const;
        virtual bool buildReadRequests(tint64 fromPosition,tint noBytes,QVector<QPair<tint64,tint> >& requests) const;

        virtual tint readCached(tchar *mem,tint64 fromPosition,tint noBytes);
        virtual tint readUncached(tchar *mem,tint64 fromPosition,tint noBytes);

        virtual void removePendingReads(QVector<QPair<tint64,tint> >& requests) const;
        virtual bool isErrorInReads(const QVector<QPair<tint64,tint> >& requests) const;
        virtual void getReadIDToIndexMap(QMap<tint,tint64>& idMap) const;

        virtual void insertIntoMasks(tint offset,tint length,QMap<tint,tint>& maskMap);
        virtual void mergeAdjacentMasks(QMap<tint,tint>& maskMap);
        virtual void addMaskToBlock(tint64 blockIndex,tint offset,tint length);

        virtual bool generateCachingRequests(tint64 fromPosition,tint noBytes,QVector<QPair<tint64,tint> >& waitOnRequests);

        virtual bool validateRequestedLength(tint64 fromPosition,tint& noBytes) const;

    protected Q_SLOTS:
        void onCompleted();
};

//-------------------------------------------------------------------------------------------

inline tint64 CachedFileStream::indexFromPosition(tint64 position) const
{
    return position / static_cast<tint64>(c_blockSize);
}

//-------------------------------------------------------------------------------------------

inline tint CachedFileStream::offsetInBlock(tint64 position) const
{
    return static_cast<tint>(position % static_cast<tint64>(c_blockSize));
}

//-------------------------------------------------------------------------------------------

inline tint64 CachedFileStream::positionFromIndex(tint64 blockIdx) const
{
    return blockIdx * c_blockSize;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
