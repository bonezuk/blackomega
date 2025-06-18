#include "track/db/inc/PlaylistM3UIO.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/CommonDirectoriesForFiles.h"
#include "track/db/inc/TrackFileDependencies.h"
#include "widget/inc/ImportPlaylistDialog.h"
#endif

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(PlaylistIOFactory,PlaylistAbstractIO, \
                            PlaylistM3UIOFactory,PlaylistM3UIO, \
                            "m3u",false)

//-------------------------------------------------------------------------------------------

PlaylistM3UIO::PlaylistM3UIO() : PlaylistAbstractIO()
{}

//-------------------------------------------------------------------------------------------

PlaylistM3UIO::~PlaylistM3UIO()
{}

//-------------------------------------------------------------------------------------------

bool PlaylistM3UIO::load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    common::BIOBufferedStream pFile(common::e_BIOStream_FileRead);
    bool res = true;
    
    pList.clear();

#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    bool canRead = false;

    if(!sbBookmark->has(fileName,true))
    {
        sbBookmark->add(fileName,true);
    }

    {
        track::db::TrackFileDependencies dependency;
        QStringList accessFileList;
        common::BIOBufferedStream tFile(common::e_BIOStream_FileRead);
        if(tFile.open(fileName))
        {
            QDir homeDir = QFileInfo(fileName).dir();
            tint fCount = 0;
            
            while(!tFile.eof())
            {
                QString lPath;
                QByteArray lArray = readLine(tFile);
                
                lPath = QString::fromLatin1(lArray.data(),lArray.length());
                lPath = getFilePath(lPath,homeDir,true);
                if(lPath.isEmpty())
                {
                    lPath = QString::fromUtf8(lArray.data(),lArray.length());
                    lPath = getFilePath(lPath,homeDir,true);
                }
            
                if(!lPath.isEmpty())
                {
                    if(sbBookmark->has(lPath,true))
                    {
                        fCount++;
                    }
                    else
                    {
                        accessFileList << lPath;
                        dependency.add(lPath);
                    }
                }
            }
            tFile.close();

            QSet<QString> allDependencies = dependency.allDependencies();
            for(QSet<QString>::iterator ppI=allDependencies.begin();ppI!=allDependencies.end();++ppI)
            {
                const QString& lPath = *ppI;
                if(!sbBookmark->has(lPath,true))
                {
                    accessFileList << *ppI;
                }
            }
            
            if(accessFileList.size() > 0)
            {
                QFileInfo fInfo(fileName);
                QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
                QStringList pathList(pathSet.begin(), pathSet.end());
                
                widget::ImportPlaylistDialog importDialog(dynamic_cast<QWidget *>(m_parent));
                importDialog.setPlaylistFileName(fInfo.fileName());
                importDialog.setDirectories(pathList);
                importDialog.setModal(true);
                importDialog.exec();
                
                if(importDialog.result()==QDialog::Accepted)
                {
                    for(QStringList::iterator ppI=pathList.begin();ppI!=pathList.end();ppI++)
                    {
                        if(sbBookmark->has(*ppI,true))
                        {
                            fCount++;
                        }
                    }
                }
            }
            
            if(fCount>0)
            {
                canRead = true;
            }
        }
    }
    if(!canRead)
    {
        return false;
    }
#endif

    if(pFile.open(fileName))
    {
        tint size = pFile.size();
        QDir homeDir = QFileInfo(fileName).dir();
        
        while(!pFile.eof() && !progress->isCancelled())
        {
            QString lPath;
            QByteArray lArray = readLine(pFile);
            
            progress->setProgress(static_cast<tfloat32>(pFile.offset() * 100) / static_cast<tfloat32>(size));
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            
            lPath = QString::fromLatin1(lArray.data(),lArray.length());
            lPath = getFilePath(lPath,homeDir,true);
            if(lPath.isEmpty())
            {
                lPath = QString::fromUtf8(lArray.data(),lArray.length());
                lPath = getFilePath(lPath,homeDir,true);
            }
            
            if(!lPath.isEmpty())
            {
#if defined(OMEGA_MAC_STORE)
                if(sbBookmark->has(lPath,true))
                {
                    appendToList(lPath,pList,progress);
                }
#else
                appendToList(lPath,pList,progress);
#endif
            }
        }
        pFile.close();
        
        if(progress->isCancelled())
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool PlaylistM3UIO::save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    common::BIOBufferedStream pFile(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
    bool res;

#if defined(OMEGA_MAC_STORE)
    bool addBkFlag = false;
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    if(!sbBookmark->has(fileName,false))
    {
        addBkFlag = true;
    }
    pFile.setCheckOutFlag((addBkFlag) ? false : true);
#endif

    progress->activate(true);
    if(pFile.open(fileName))
    {
        tint i;
        QString line;
        QVector<track::info::InfoSPtr>::const_iterator ppI;
        
        res = writeLine(pFile,"#EXTM3U");
        for(i=0,ppI=pList.begin();ppI!=pList.end() && res && !progress->isCancelled();ppI++,i++)
        {
            QString infLine("#EXTINF:");
            track::info::InfoSPtr pTrack(*ppI);
            if(pTrack.data()!=0)
            {
                progress->setProgress(static_cast<tfloat32>(i * 100) / static_cast<tfloat32>(pList.size()));
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                
                infLine += QString::number(pTrack->length().secondsTotal()) + "," + pTrack->artist() + " - " + pTrack->title();
                res = writeLine(pFile,infLine);
                if(res)
                {
                    res = writeLine(pFile,pTrack->getFilename());
                }
            }
        }
        pFile.close();

#if defined(OMEGA_MAC_STORE)
        if(addBkFlag)
        {
            sbBookmark->add(fileName,false);
        }
#endif
        
        if(progress->isCancelled())
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    progress->deactivate(true);
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
