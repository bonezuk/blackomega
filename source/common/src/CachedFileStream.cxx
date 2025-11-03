#include "common/inc/CachedFileStream.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

CachedFileStream::CachedFileStream(QObject *parent) : QObject(parent)
    ,m_pFile(0)
    ,m_cache()
#if defined(OMEGA_MAC_STORE)
    ,m_checkOutFlag(true)
#endif
{}

//-------------------------------------------------------------------------------------------

CachedFileStream::~CachedFileStream()
{
    CachedFileStream::close();
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::printError(const tchar *strR,const tchar *strE) const
{
    Log::g_Log << "CachedFileStream::" << strR << " - " << strE << c_endl;
}

//-------------------------------------------------------------------------------------------

bool CachedFileStream::open(const QString& name)
{
    bool res = false;

    close();
    
    m_pFile = new AsynchronousFileReader;

    if(m_pFile->open(name))
    {
        QObject::connect(m_pFile,SIGNAL(completed()),this,SLOT(onCompleted()));
        res = true;
    }
    else
    {
        QString err = "Failed to open file '" + name + "'";
        printError("CachedFileStream",err.toUtf8().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::close()
{
    clear();
    
    if(m_pFile!=0)
    {
        QObject::disconnect(m_pFile,SIGNAL(completed()),this,SLOT(onCompleted()));
        delete m_pFile;
        m_pFile = 0;
    }
}

//-------------------------------------------------------------------------------------------

AsynchronousFileReader *CachedFileStream::getFile()
{
    return m_pFile;
}

//-------------------------------------------------------------------------------------------

const AsynchronousFileReader *CachedFileStream::getFileConst() const
{
    return m_pFile;
}

//-------------------------------------------------------------------------------------------

QMap<tint64,QSharedPointer<QByteArray> >& CachedFileStream::getCache()
{
    return m_cache;
}

//-------------------------------------------------------------------------------------------

const QMap<tint64,QSharedPointer<QByteArray> >& CachedFileStream::getCacheConst() const
{
    return m_cache;
}

//-------------------------------------------------------------------------------------------

QMap<tint64,QMap<tint,tint> >& CachedFileStream::getCacheMask()
{
    return m_cacheMask;
}

//-------------------------------------------------------------------------------------------

const QMap<tint64,QMap<tint,tint> >& CachedFileStream::getCacheMaskConst() const
{
    return m_cacheMask;
}

//-------------------------------------------------------------------------------------------

QMap<tint64,QPair<tint,tint> >& CachedFileStream::readOps()
{
    return m_readOps;
}

//-------------------------------------------------------------------------------------------

const QMap<tint64,QPair<tint,tint> >& CachedFileStream::readOpsConst() const
{
    return m_readOps;
}

//-------------------------------------------------------------------------------------------

bool CachedFileStream::isOffsetIntersection(tint offsetA,tint lengthA,tint offsetB,tint lengthB) const
{
    bool res;
    
    if(lengthA > 0 && lengthB > 0)
    {
        if(offsetA < offsetB)
        {
            res = (offsetA + lengthA > offsetB);
        }
        else
        {
            res = (offsetA < offsetB + lengthB);
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool CachedFileStream::isCached(tint64 fromPosition,tint noBytes) const
{
    bool res = false;

    if(getFileConst()!=0)
    {
        if(validateRequestedLength(fromPosition,noBytes))
        {
            tint64 toPosition = fromPosition + noBytes;
            tint64 pos = fromPosition;
        
            res = true;
            while(res && pos < toPosition)
            {
                tint64 blockIdx = indexFromPosition(pos);
                QMap<tint64,QSharedPointer<QByteArray> >::const_iterator ppI = getCacheConst().constFind(blockIdx);
                if(ppI!=getCacheConst().constEnd())
                {
                    tint offset,blockEnd;
                    const QSharedPointer<QByteArray>& data = ppI.value();
                    
                    offset = offsetInBlock(pos);
                    blockEnd = c_blockSize;
                    if((blockIdx * c_blockSize) + static_cast<tint64>(blockEnd) > toPosition)
                    {
                        blockEnd = static_cast<tint>(toPosition - (blockIdx * c_blockSize));
                    }
                    
                    if(offset<data->size() && blockEnd<=data->size())
                    {
                        QMap<tint64,QMap<tint,tint> >::const_iterator ppJ = getCacheMaskConst().constFind(blockIdx);
                        if(ppJ!=getCacheMaskConst().constEnd())
                        {
                            const QMap<tint,tint>& blockMaskMap = ppJ.value();
                            
                            for(QMap<tint,tint>::const_iterator ppK=blockMaskMap.constBegin();ppK!=blockMaskMap.end() && res;++ppK)
                            {
                                if(isOffsetIntersection(offset,blockEnd-offset,ppK.key(),ppK.value()))
                                {
                                    res = false;
                                }
                            }
                        }
                    }
                    else
                    {
                        res = false;
                    }
                    
                    pos += blockEnd - offset;
                }
                else
                {
                    res = false;
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// Regardless of the exact byte range, the file is read in and placed into the cache as a 
// series of blocks. If a block does not exist or been masked then a read request is generated
// for that given block.
//-------------------------------------------------------------------------------------------

bool CachedFileStream::buildReadRequests(tint64 fromPosition,tint noBytes,QVector<QPair<tint64,tint> >& requests) const
{
    bool res = false;
    
    requests.clear();
    
    if(getFileConst()!=0)
    {
        tint64 toPosition = fromPosition + noBytes;
        
        if(fromPosition>=0 && toPosition<=getFileConst()->size64())
        {
            tint64 pos = fromPosition;
            
            while(pos < toPosition)
            {
                tint amount;
                bool queueReadRequest;
                tint64 blockStart,blockIdx;
                QMap<tint64,QSharedPointer<QByteArray> >::const_iterator ppI;
                
                blockIdx = indexFromPosition(pos);
                blockStart = positionFromIndex(blockIdx);
                
                amount = c_blockSize;
                if(blockStart + amount > getFileConst()->size64())
                {
                    amount = getFileConst()->size64() - blockStart;
                }
                
                ppI = getCacheConst().constFind(blockIdx);
                
                if(ppI!=getCacheConst().constEnd() && amount==ppI.value()->size())
                {
                    QMap<tint64,QMap<tint,tint> >::const_iterator ppJ = getCacheMaskConst().constFind(blockIdx);
                    if(ppJ != getCacheMaskConst().constEnd())
                    {
                        queueReadRequest = (!ppJ.value().isEmpty());
                    }
                    else
                    {
                        queueReadRequest = false;
                    }
                }
                else
                {
                    queueReadRequest = true;
                }
                
                if(queueReadRequest)
                {
                    requests.append(QPair<tint64,tint>(blockStart,amount));
                }
                
                pos = blockStart + amount;
            }
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint64 CachedFileStream::size() const
{
    return (getFileConst()!=0) ? getFileConst()->size64() : 0;
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::clear()
{
    getCache().clear();
    getCacheMask().clear();
    readOps().clear();
}

//-------------------------------------------------------------------------------------------

bool CachedFileStream::validateRequestedLength(tint64 fromPosition,tint& noBytes) const
{
    bool res;
    
    if(fromPosition>=0 && fromPosition<=size())
    {
        tint64 toPosition = fromPosition + noBytes;
        if(toPosition > size())
        {
            noBytes = size() - fromPosition;
        }
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint CachedFileStream::read(tchar *mem,tint64 fromPosition,tint noBytes)
{
    tint amount;

    if(getFile()!=0)
    {
        if(validateRequestedLength(fromPosition,noBytes))
        {
            if(noBytes > 0)
            {            
                if(mem!=0)
                {
                    if(isCached(fromPosition,noBytes))
                    {
                        amount = readCached(mem,fromPosition,noBytes);
                    }
                    else
                    {
                        amount = readUncached(mem,fromPosition,noBytes);
                    }
                }
                else
                {
                    printError("read","No memory buffer given to read into");
                    amount = -1;
                }
            }
            else
            {
                amount = 0;
            }
        }
        else
        {
            QString err = "Given position to read from '" + getFile()->name() + "' is out of range";
            printError("read",err.toUtf8().constData());
            amount = -1;
        }
    }
    else
    {
        printError("read","No file to read from");
        amount = -1;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------
// The 'read' method ensures that the parameters passed into 'readCached' and 'readUncached' are valid
//-------------------------------------------------------------------------------------------

tint CachedFileStream::readCached(tchar *mem,tint64 fromPosition,tint noBytes)
{
    tint outputPosition;
    tint64 pos,toPosition;
    bool res = true;
    
    pos = fromPosition;
    toPosition = fromPosition + noBytes;
    
    outputPosition = 0;
    while(res && pos<toPosition && outputPosition<noBytes)
    {
        tint64 blockIdx = indexFromPosition(pos);
        QMap<tint64,QSharedPointer<QByteArray> >::const_iterator ppI = getCacheConst().constFind(blockIdx);
        if(ppI!=getCacheConst().constEnd())
        {
            tint offset,blockEnd;
            const QSharedPointer<QByteArray>& pData = ppI.value();
            
            offset = offsetInBlock(pos);
            blockEnd = c_blockSize;
            if((blockIdx * c_blockSize) + static_cast<tint64>(blockEnd) > toPosition)
            {
                blockEnd = static_cast<tint>(toPosition - (blockIdx * c_blockSize));
            }
            
            if(offset<=pData->size() && blockEnd<=pData->size())
            {
                tint amount = blockEnd - offset;
                if(amount > 0)
                {
                    const tchar *cacheMem = pData->constData();
                    ::memcpy(&mem[outputPosition],&cacheMem[offset],amount);
                    outputPosition += amount;
                    pos += amount;
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    
    if(!res)
    {
        QString err = "Expected cached data was not found during read of file '" + getFile()->name() + "'";
        printError("readCached",err.toUtf8().constData());
    }
    return (res) ? noBytes : -1;
}

//-------------------------------------------------------------------------------------------

tint CachedFileStream::readUncached(tchar *mem,tint64 fromPosition,tint noBytes)
{
    QVector<QPair<tint64,tint> > waitOnRequests;
    bool res = true;
    
    if(generateCachingRequests(fromPosition,noBytes,waitOnRequests))
    {
        while(!waitOnRequests.isEmpty() && res)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            if(!isErrorInReads(waitOnRequests))
            {
                QVector<QPair<tint64,tint> >::iterator ppI = waitOnRequests.begin();
                while(ppI!=waitOnRequests.end())
                {
                    QPair<tint64,tint>& value = *ppI;
                    
                    if(readOps().find(indexFromPosition(value.first)) == readOps().end())
                    {
                        ppI = waitOnRequests.erase(ppI);
                    }
                    else
                    {
                        ppI++;
                    }
                }
                if(!waitOnRequests.isEmpty())
                {
                    usleepThread(1);
                }
            }
            else
            {
                QString err = "Reads into cache for requested data while reading from '" + getFileConst()->name() + "' have failed";
                printError("readUncached",err.toUtf8().constData());
                res = false;
            }
        }
                
        if(res)
        {
            noBytes = readCached(mem,fromPosition,noBytes);
        }
    }
    else
    {
        printError("readUncached","Error creating caching request");
        res = false;
    }
    return (res) ? noBytes : -1;
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::removePendingReads(QVector<QPair<tint64,tint> >& requests) const
{
    QVector<QPair<tint64,tint> >::iterator ppI;
    QMap<tint64,QPair<tint,tint> >::const_iterator ppJ;
    
    ppI = requests.begin();
    while(ppI!=requests.end())
    {
        QPair<tint64,tint>& v = *ppI;
        tint64 blockIndex = indexFromPosition(v.first);
        
        ppJ = readOpsConst().constFind(blockIndex);
        if(ppJ!=readOpsConst().constEnd() && ppJ.value().second==0)
        {
            ppI = requests.erase(ppI);
        }
        else
        {
            ppI++;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool CachedFileStream::isErrorInReads(const QVector<QPair<tint64,tint> >& requests) const
{
    QMap<tint64,QPair<tint,tint> >::const_iterator ppJ;
    bool errors = false;
    
    for(QVector<QPair<tint64,tint> >::const_iterator ppI=requests.begin();ppI!=requests.end() && !errors;ppI++)
    {
        const QPair<tint64,tint>& v = *ppI;
        tint64 blockIndex = indexFromPosition(v.first);

        ppJ = readOpsConst().constFind(blockIndex);
        if(ppJ!=readOpsConst().constEnd() && ppJ.value().second==1)
        {
            errors = true;
        }        
    }
    return errors;
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::getReadIDToIndexMap(QMap<tint,tint64>& idMap) const
{
    idMap.clear();
    for(QMap<tint64,QPair<tint,tint> >::const_iterator ppI=readOpsConst().constBegin();ppI!=readOpsConst().constEnd();++ppI)
    {
        idMap.insert(ppI.value().first,ppI.key());
    }
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::onCompleted()
{
    if(getFile()!=0)
    {
        QMap<tint,tint64> idMap;
        QMap<tint,tint64>::iterator ppJ;
        
        QList<int> idList;
        
        idList = getFile()->hasCompleted();
        getReadIDToIndexMap(idMap);
        
        for(QList<int>::iterator ppI=idList.begin();ppI!=idList.end();++ppI)
        {
            tint readId = *ppI;
            QSharedPointer<QByteArray> pData = getFile()->result(readId);
            
            ppJ = idMap.find(readId);
            if(ppJ!=idMap.end())
            {
                tint64 blockIndex = ppJ.value();
                QMap<tint64,QPair<tint,tint> >::iterator ppK = readOps().find(blockIndex);
                
                if(ppK!=readOps().end())
                {
                    if(!pData.isNull())
                    {
                        QMap<tint64,QMap<tint,tint> >::iterator ppL = getCacheMask().find(blockIndex);
                        if(ppL != getCacheMask().end())
                        {
                            getCacheMask().erase(ppL);
                        }
                        getCache().insert(blockIndex,pData);
                        readOps().erase(ppK);
                    }
                    else
                    {
                        ppK.value().second = 1;
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool CachedFileStream::generateCachingRequests(tint64 fromPosition,tint noBytes,QVector<QPair<tint64,tint> >& waitOnRequests)
{
    QVector<QPair<tint64,tint> > requests;
    bool res = true;
    
    waitOnRequests.clear();
    
    if(buildReadRequests(fromPosition,noBytes,requests))
    {
        if(!isErrorInReads(requests))
        {
            QVector<QPair<tint64,tint> >::iterator ppI;

            for(ppI=requests.begin();ppI!=requests.end();ppI++)
            {
                waitOnRequests.append(*ppI);
            }
            removePendingReads(requests);
            
            for(ppI=requests.begin();ppI!=requests.end() && res;ppI++)
            {
                tint readId;
                tint64 blockIndex;
                QPair<tint64,tint>& value = *ppI;
                
                blockIndex = indexFromPosition(value.first);
                if(readOps().find(blockIndex) == readOps().end())
                {
                    readId = getFile()->read(value.first,value.second);
                    if(readId > 0)
                    {
                        readOps().insert(blockIndex,QPair<tint,tint>(readId,0));
                    }
                    else
                    {
                        QString err = "Failed to queue asynchronous read on file '" + getFileConst()->name() + "' from position " + QString::number(value.first) + " for length " + QString::number(value.second);
                        printError("generateCachingRequests",err.toUtf8().constData());
                        res = false;                    
                    }
                }
                else
                {
                    printError("generateCachingRequests","Unexpected read operation already exists for block");
                    res = false;
                }
            }            
        }
        else
        {
            QString err = "Reads into cache for requested data while reading from '" + getFileConst()->name() + "' have failed";
            printError("generateCachingRequests",err.toUtf8().constData());
            res = false;
        }
    }
    else
    {
        printError("generateCachingRequests","Error building series of read requests");
        res = false;
    }
    return res;    
}

//-------------------------------------------------------------------------------------------

tint CachedFileStream::cache(tint64 fromPosition,tint noBytes)
{
    bool res = true;

    if(validateRequestedLength(fromPosition,noBytes))
    {
        if(!isCached(fromPosition,noBytes))
        {
            QVector<QPair<tint64,tint> > waitOnRequests;
            res = generateCachingRequests(fromPosition,noBytes,waitOnRequests);
            if(!res)
            {
                printError("cache","Error creating caching request");
            }
        }
    }
    else
    {
        QString err = "Given position to read from '" + getFile()->name() + "' is out of range";
        printError("cache",err.toUtf8().constData());
        res = false;
    }
    return (res) ? noBytes : -1;
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::insertIntoMasks(tint offset,tint length,QMap<tint,tint>& maskMap)
{
    bool intersectLoop,insertFlag;
                
    insertFlag = true;
    do
    {
        intersectLoop = false;
                    
        for(QMap<tint,tint>::iterator ppK=maskMap.begin();!intersectLoop && ppK!=maskMap.end();ppK++)
        {
            if(isOffsetIntersection(offset,length,ppK.key(),ppK.value()))
            {
                if(offset < ppK.key())
                {
                    if((offset + length) < (ppK.key() + ppK.value()))
                    {
                        length = ppK.key() + ppK.value() - offset;
                        ppK = maskMap.erase(ppK);
                    }
                    else
                    {
                        ppK = maskMap.erase(ppK);
                    }
                }
                else
                {
                    if((offset + length) < (ppK.key() + ppK.value()))
                    {
                        insertFlag = false;
                    }
                    else
                    {
                        length = offset + length - ppK.key();
                        offset = ppK.key();
                        ppK = maskMap.erase(ppK);
                    }
                }
                intersectLoop = true;
                if(ppK == maskMap.end())
                {
                    break;
                }
            }
        }
    } while(intersectLoop && insertFlag);
    
    if(insertFlag)
    {
        maskMap.insert(offset,length);
    }
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::mergeAdjacentMasks(QMap<tint,tint>& maskMap)
{
    QMap<tint,tint>::iterator ppI;

    if(maskMap.size() >= 2)
    {
        tint pOffset = -1,pLength = 0;
    
        ppI = maskMap.begin();
                
        while(ppI != maskMap.end())
        {
            if(pOffset>=0 && (pOffset + pLength)==ppI.key())
            {
                pLength += ppI.value();
                maskMap.erase(ppI);
                ppI = maskMap.find(pOffset);
                ppI.value() = pLength;
            }
            else
            {
                pOffset = ppI.key();
                pLength = ppI.value();
            }
            ++ppI;
        }
    }
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::addMaskToBlock(tint64 blockIndex,tint offset,tint length)
{
    QMap<tint64,QSharedPointer<QByteArray> >::iterator ppI;
    QMap<tint64,QMap<tint,tint> >::iterator ppJ;
    bool removeMaskEntry = false;
    
    ppI = getCache().find(blockIndex);
    if(ppI != getCache().end())
    {
        QSharedPointer<QByteArray>& pData = ppI.value();
        
        if(offset<0)
        {
            offset = 0;
        }
        if(offset + length > pData->size())
        {
            length = pData->size() - offset;
        }
        
        if(offset>=0 && length<pData->size())
        {
            ppJ = getCacheMask().find(blockIndex);
            if(ppJ != getCacheMask().end())
            {
                QMap<tint,tint>& maskMap = ppJ.value();
                
                insertIntoMasks(offset,length,maskMap);
                mergeAdjacentMasks(maskMap);
                
                if(maskMap.find(0)!=maskMap.end() && maskMap.find(0).value()==pData->size())
                {
                    removeMaskEntry = true;
                }
                
                if(removeMaskEntry)
                {
                    getCache().erase(ppI);
                }
            }
            else
            {
                QMap<tint,tint> maskMap;
                maskMap.insert(offset,length);
                getCacheMask().insert(blockIndex,maskMap);
            }
        }
        else
        {
            getCache().erase(ppI);
            removeMaskEntry = true;
        }
    }
    else
    {
        removeMaskEntry = true;
    }
    
    if(removeMaskEntry)
    {
        ppJ = getCacheMask().find(blockIndex);
        if(ppJ != getCacheMask().end())
        {
            getCacheMask().erase(ppJ);
        }
    }
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::drop(tint64 fromPosition,tint noBytes)
{
    if(getFileConst()!=0)
    {
        tint64 pos,toPosition;

        if(fromPosition < 0)
        {
            fromPosition = 0;
        }
        
        toPosition = fromPosition + noBytes;
        if(toPosition > getFileConst()->size64())
        {
            toPosition = getFileConst()->size64();
        }
        
        pos = fromPosition;
        while(pos < toPosition)
        {
            tint len;
            tint offset = offsetInBlock(pos);
            tint64 blockIdx = indexFromPosition(pos);
            
            len = c_blockSize - offset;
            if((pos + len) > toPosition)
            {
                len = static_cast<tint>(toPosition - pos);
            }
            
            addMaskToBlock(blockIdx,offset,len);
            
            pos += len;
        }
    }
}

//-------------------------------------------------------------------------------------------

void CachedFileStream::clearWithRetention(tint64 fromPosition,tint noBytes)
{
    if(getFile()!=0 && validateRequestedLength(fromPosition,noBytes))
    {
        tint64 toPosition;
        
        getFile()->cancelAllPending();
        readOps().clear();
        
        if(fromPosition > 0)
        {
            drop(0,static_cast<tint>(fromPosition));
        }
        
        toPosition = fromPosition + noBytes;
        if(toPosition < getFileConst()->size64())
        {
            drop(toPosition,static_cast<tint>(getFileConst()->size64() - toPosition));
        }
    }
}

//-------------------------------------------------------------------------------------------

const AsynchronousFileReader *CachedFileStream::file() const
{
    return getFileConst();
}
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

void CachedFileStream::setCheckOutFlag(bool flag)
{
    m_checkOutFlag = flag;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
