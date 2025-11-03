#include "track/db/inc/PlaylistPLSIO.h"

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
                            PlaylistPLSIOFactory,PlaylistPLSIO, \
                            "pls",false)

//-------------------------------------------------------------------------------------------

PlaylistPLSIO::PlaylistPLSIO() : PlaylistAbstractIO()
{
    m_plsLang = new common::BOParse;

    m_plsState[0] = m_plsLang->String("[");
    m_plsState[1] = m_plsLang->String("]");
    m_plsState[2] = m_plsLang->String("=");
    m_plsState[3] = m_plsLang->String("playlist");
    m_plsState[4] = m_plsLang->String("File");
    m_plsState[5] = m_plsLang->String("Title");
    m_plsState[6] = m_plsLang->String("Length");
    m_plsState[7] = m_plsLang->String("NumberOfEntries");
    m_plsState[8] = m_plsLang->String("Version");
}

//-------------------------------------------------------------------------------------------

PlaylistPLSIO::~PlaylistPLSIO()
{
    delete m_plsLang;
}

//-------------------------------------------------------------------------------------------

PlaylistPLSIO::LineType PlaylistPLSIO::parseLineInfo(const QString& line,QString& txtData,tint& noData)
{
    LineType type = e_lineUnknown;
    common::BO_Parse_Unit *item;
    QByteArray lArray(line.toUtf8());
    const tchar *x = lArray.data();

    txtData = "";
    noData = 0;

    item = m_plsLang->Lexical(x);
    if(item!=0)
    {
        if(item->state==m_plsState[0])
        {
            item = item->next;
            if(item!=0 && item->state==m_plsState[3])
            {
                item = item->next;
                if(item!=0 && item->state==m_plsState[1])
                {
                    type = e_linePlaylist;
                }
            }
        }
        else
        {
            if(item->state==m_plsState[4])
            {
                type = e_lineFile;
            }
            else if(item->state==m_plsState[5])
            {
                type = e_lineTitle;
            }
            else if(item->state==m_plsState[6])
            {
                type = e_lineLength;
            }
            else if(item->state==m_plsState[7])
            {
                type = e_lineNoOfEntries;
            }
            else if(item->state==m_plsState[8])
            {
                type = e_lineVersion;
            }

            if(type!=e_lineUnknown)
            {
                tint cPos = item->start + item->length;

                item = item->next;
                if(item!=0 && item->state==m_plsState[2])
                {
                    tint diff = item->start - cPos;

                    if((type==e_lineFile || type==e_lineLength || type==e_lineTitle) && diff>0)
                    {
                        QString noS = QString::fromUtf8(&x[cPos],diff);
                        noData = noS.toInt();
                    }
                    cPos = item->start + item->length;
                    txtData = QString::fromUtf8(&x[cPos]);
                }
                else
                {
                    type = e_lineUnknown;
                }
            }
        }
    }
    return type;
}

//-------------------------------------------------------------------------------------------

bool PlaylistPLSIO::load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
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
        QStringList accessFileList;
        common::BIOBufferedStream tFile(common::e_BIOStream_FileRead);
        if(tFile.open(fileName))
        {
            tint noData;
            QString lPath,lText;
            LineType type;
            bool playlistFlag = false;
            QDir homeDir = QFileInfo(fileName).dir();
            track::db::TrackFileDependencies dependency;
            tint fCount = 0;

            while(!tFile.eof() && !progress->isCancelled())
            {
                QByteArray lArray = readLine(tFile);

                lText = QString::fromLatin1(lArray.data(),lArray.length());
                type = parseLineInfo(lText,lPath,noData);
                if(type!=e_lineUnknown)
                {
                    if(type==e_linePlaylist)
                    {
                        playlistFlag = true;
                    }
                    else if(playlistFlag)
                    {
                        if(type==e_lineFile)
                        {
                            lPath = getFilePath(lPath,homeDir,true);
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
        tint noData;
        QString lPath,lText;
        LineType type;
        bool playlistFlag = false;
        QDir homeDir = QFileInfo(fileName).dir();
        tint size = pFile.size();

        while(!pFile.eof() && !progress->isCancelled())
        {
            QByteArray lArray = readLine(pFile);

            progress->setProgress(static_cast<tfloat32>(pFile.offset() * 100) / static_cast<tfloat32>(size));
            QCoreApplication::processEvents(QEventLoop::AllEvents);

            lText = QString::fromLatin1(lArray.data(),lArray.length());
            type = parseLineInfo(lText,lPath,noData);
            if(type!=e_lineUnknown)
            {
                if(type==e_linePlaylist)
                {
                    playlistFlag = true;
                }
                else if(playlistFlag)
                {
                    if(type==e_lineFile)
                    {
                        lPath = getFilePath(lPath,homeDir,true);
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
                }
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

bool PlaylistPLSIO::save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    common::BIOBufferedStream pFile(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
    bool res = true;

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
        int i;
        QString line;
        QVector<track::info::InfoSPtr>::const_iterator ppI;

        res = writeLine(pFile,"[playlist]");
        for(i=1,ppI=pList.begin();ppI!=pList.end() && res && !progress->isCancelled();ppI++,i++)
        {
            QString trackStr,lengthStr,fileStr;
            track::info::InfoSPtr pTrack(*ppI);

            progress->setProgress(static_cast<tfloat32>(i * 100) / static_cast<tfloat32>(pList.size()));
            QCoreApplication::processEvents(QEventLoop::AllEvents);

            fileStr = "File" + QString::number(i) + "=" + pTrack->getFilename();
            trackStr = "Title" + QString::number(i) + "=" + pTrack->artist() + " - " + pTrack->title();
            lengthStr = "Length" + QString::number(i) + "=" + QString::number(pTrack->length().secondsTotal());
            res = writeLine(pFile,fileStr);
            if(res)
            {
                res = writeLine(pFile,trackStr);
                if(res)
                {
                    res = writeLine(pFile,lengthStr);
                }
            }
        }
        if(res && !progress->isCancelled())
        {
            QString noEntries = "NumberOfEntries=" + QString::number(i-1);
            res = writeLine(pFile,noEntries);
            if(res)
            {
                res = writeLine(pFile,"Version=2");
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
    progress->deactivate();
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
