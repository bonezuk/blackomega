#include "track/db/inc/PlaylistAbstractIO.h"
#include "engine/inc/Codec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(PlaylistIOFactory,PlaylistAbstractIO)

//-------------------------------------------------------------------------------------------

PlaylistAbstractIO::PlaylistAbstractIO() : m_parent(0),
    m_useMountedDrives(false)
{
    m_pathParser = new common::BOParse;

    m_pathParserState[0] = m_pathParser->String("/");     /* /usr */
    m_pathParserState[1] = m_pathParser->String("\\");    /* usr\local */
    m_pathParserState[2] = m_pathParser->String(":\\");   /* c:\ */
    m_pathParserState[3] = m_pathParser->String("\\\\");  /* \\machine\drive */
    m_pathParserState[4] = m_pathParser->String("://");   /* file:// or http:// */
    m_pathParserState[5] = m_pathParser->String("file");  /* file:// */
    m_pathParserState[6] = m_pathParser->String("#");     /* #comment */
    m_pathParserState[7] = m_pathParser->String(":/");    /* c:/ */
    m_pathParserState[8] = m_pathParser->String("//");    /* //machine/drive */
}

//-------------------------------------------------------------------------------------------

PlaylistAbstractIO::~PlaylistAbstractIO()
{
    delete m_pathParser;
}

//-------------------------------------------------------------------------------------------

bool PlaylistAbstractIO::load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool PlaylistAbstractIO::save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    return false;
}

//-------------------------------------------------------------------------------------------

void PlaylistAbstractIO::useMountedDrives()
{
    m_useMountedDrives = true;
}

//-------------------------------------------------------------------------------------------

bool PlaylistAbstractIO::isSupported(const QString& name)
{
    QString ext = engine::Codec::getFileExtension(name);

    if(ext=="m3u" || ext=="m3u8")
    {
        return true;
    }
    if(ext=="pls")
    {
        return true;
    }
    if(ext=="xspf")
    {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------

QString PlaylistAbstractIO::factoryName(const QString& name)
{
    QString fName,ext = engine::Codec::getFileExtension(name);

    if(ext=="m3u" || ext=="m3u8")
    {
        fName = "m3u";
    }
    if(ext=="pls")
    {
        fName = "pls";
    }
    if(ext=="xspf")
    {
        fName = "xspf";
    }
    return fName;
}

//-------------------------------------------------------------------------------------------

bool PlaylistAbstractIO::writeLine(common::BIOStream& pFile,const QString& line)
{
    QString nLine = line + "\r\n";
    QByteArray lArray = nLine.toUtf8();
    return (pFile.write(lArray.data(),lArray.length())==lArray.length()) ? true : false;
}

//-------------------------------------------------------------------------------------------

QByteArray PlaylistAbstractIO::readLine(common::BIOStream& pFile)
{
    tchar t[1] = {'\0'};
    tint state = 0;
    QByteArray lArray;
    bool loop = true;

    while(loop)
    {
        if(pFile.read(t,1)==1)
        {
            switch(state)
            {
                case 0:
                    if(t[0]=='\r')
                    {
                        state = 1;
                    }
                    else if(t[0]=='\n')
                    {
                        loop = false;
                    }
                    else
                    {
                        lArray.append(t,1);
                    }
                    break;

                case 1:
                    if(t[0]=='\n')
                    {
                        loop = false;
                    }
                    else
                    {
                        lArray.append('\r');
                        lArray.append(t,1);
                        state = 0;
                    }
                    break;
            }
        }
        else
        {
            loop = false;
        }
    }
    return lArray;
}

//-------------------------------------------------------------------------------------------

QString PlaylistAbstractIO::findFileFromMounts(const QString& fileName)
{
    int i, startIndex;
    QString oName(fileName);
    QStringList mounts = TrackDB::instance()->mountPoints()->mountPoints();
    QString actualFileName;

    oName = oName.replace(QChar('\\'), QChar('/'));
#if QT_VERSION >= 0x050000
    if(oName.startsWith(QStringLiteral("//")))
#else
    if(oName.startsWith(QString("//")))
#endif
    {
        // Windows share //machine/drive/dir1/dir2
        startIndex = oName.indexOf(QChar('/'), 2);
        if(startIndex > 2)
        {
            startIndex = oName.indexOf(QChar('/'), startIndex);
        }
        else
        {
            startIndex = -1;
        }
    }
    else if(oName.at(1)==QChar(':') && oName.at(2)==QChar('/'))
    {
        // Windows drive letter g:/dir1/dir2
        startIndex = 3;
    }
    else if(oName.at(0)==QChar('/'))
    {
        // UNIX absolute /dir1/dir2
        startIndex = 1;
    }
    // Cannot find from relative path but from absolute path

    if(startIndex > 0 && mounts.size() > 0)
    {
        for(QStringList::iterator ppI = mounts.begin(); actualFileName.isEmpty() && ppI != mounts.end(); ppI++)
        {
            QString mountName = (*ppI).replace(QChar('\\'), QChar('/'));

            if(mountName.at(mountName.size() - 1) != QChar('/'))
            {
#if QT_VERSION >= 0x050000
                mountName += QStringLiteral("/");
#else
                mountName += QString("/");
#endif
            }
            for(i = startIndex; i > 0 && i < oName.length() && actualFileName.isEmpty(); i = oName.indexOf(QChar('/'), i) + 1)
            {
                QString fName = mountName + oName.mid(i);
                QFileInfo fInfo(fName);
                if(fInfo.exists())
                {
                    actualFileName = fInfo.absoluteFilePath();
                }
            }
        }
    }
    return actualFileName;
}

//-------------------------------------------------------------------------------------------

track::info::InfoSPtr PlaylistAbstractIO::getTrack(const QString& fileName)
{
    if(!fileName.isEmpty() && track::info::Info::isSupported(fileName))
    {
        return track::db::DBInfo::readInfo(fileName);
    }
    else
    {
        return track::info::InfoSPtr();
    }
}

//-------------------------------------------------------------------------------------------

QString PlaylistAbstractIO::getURLFilename(const QString& uPath)
{
    QString fPath;

    if(!uPath.isEmpty())
    {
        xmlURIPtr rURI = xmlCreateURI();
        if(rURI!=0)
        {
            if(xmlParseURIReference(rURI,uPath.toUtf8().constData())==0)
            {
                xmlChar *refS = xmlSaveUri(rURI);
                if(refS!=0)
                {
                    QString n;
                    QUrl fUrl(QString::fromUtf8(reinterpret_cast<const char *>(refS)));
                    fUrl.setHost("");
                    n = fUrl.toLocalFile();
                    n = QDir::toNativeSeparators(n);
                    if(!n.isEmpty())
                    {
                        if(n.at(n.size()-1)==QChar('/') || n.at(n.size()-1)==QChar('\\'))
                        {
                            n = n.mid(0,n.size()-1);
                        }
                        if(common::DiskOps::exist(n))
                        {
                            fPath = n;
                        }
                    }
                    xmlFree(refS);
                }
            }
            xmlFreeURI(rURI);
        }
    }
    return fPath;
}

//-------------------------------------------------------------------------------------------
// Alternative\Band - Song.mp3
// Classical\Other Band - New Song.mp3
// Stuff.mp3
// D:\More Music\Foo.mp3
// ..\Other Music\Bar.mp3
// http://emp.cx:8000/Listen.pls
// http://www.example.com/~user/Mine.mp3
//-------------------------------------------------------------------------------------------

QString PlaylistAbstractIO::getFilePath(const QString& inName,const QDir& homeDir,bool commentFlag)
{
    QString oName;
    common::BO_Parse_Unit *item;
    QByteArray iArray = inName.trimmed().toUtf8();
    const tchar *x = iArray.data();

    item = m_pathParser->Lexical(x);
    if(item!=0)
    {
        tint i,cPos = 0;
        QDir cDir(homeDir);
        bool urlFlag = false;
        bool pNextFlag = false;
        bool absolutePathFlag = false;

        if(item->state==m_pathParserState[6] && item->start==0 && commentFlag)
        {
            urlFlag = true;
        }
        else if((item->state==m_pathParserState[2] || item->state==m_pathParserState[7]) && item->start==1)
        {
            QDir hDir(QString::fromUtf8(x,item->start + item->length));
            cDir = hDir;
            pNextFlag = true;
            absolutePathFlag = true;
        }
        else if((item->state==m_pathParserState[0] || item->state==m_pathParserState[1]) && item->start==0)
        {
            QDir hDir("/");
            cDir = hDir;
            pNextFlag = true;
            absolutePathFlag = true;
        }
        else if((item->state==m_pathParserState[3] || item->state==m_pathParserState[8]) && item->start==0)
        {
            i = 0;
            while(i<2 && item!=0)
            {
                item = item->next;
                if(item->state==m_pathParserState[0] || item->state==m_pathParserState[1])
                {
                    i++;
                }
            }
            cPos = item->start;

            if(item!=0)
            {
                QDir hDir(QString::fromUtf8(x,item->start));
                cDir = hDir;
                absolutePathFlag = true;
            }
            else
            {
                return oName;
            }
        }
        else if(item->state==m_pathParserState[5] && item->start==0)
        {
            if(item->next!=0 && item->next->state==m_pathParserState[4])
            {
                oName = getURLFilename(inName);
                urlFlag = true;
            }
        }
        else if(item->state==m_pathParserState[4])
        {
            urlFlag = true;
        }

        if(!urlFlag)
        {
#if defined(OMEGA_LINUX)
            if(absolutePathFlag && m_useMountedDrives)
            {
                oName = findFileFromMounts(inName);
            }
            else
#endif
            {
                if(pNextFlag)
                {
                    cPos = item->start + item->length;
                    item = item->next;
                }

                while(item!=0)
                {
                    if(item->state==m_pathParserState[0] || item->state==m_pathParserState[1])
                    {
                        tint diff = item->start - cPos;
                        if(diff>0)
                        {
                            QString dName = QString::fromUtf8(&x[cPos],diff);
                            if(dName=="..")
                            {
                                if(!cDir.cdUp())
                                {
                                    return oName;
                                }
                            }
                            else if(dName!=".")
                            {
                                if(!cDir.cd(dName))
                                {
                                    return oName;
                                }
                            }
                        }
                        cPos = item->start + item->length;
                    }
                    item = item->next;
                }
                oName = cDir.absolutePath();
                if(oName.at(oName.length()-1)!=QChar('/') && oName.at(oName.length()-1)!=QChar('\\'))
                {
                    oName += "/";
                }
                oName += QString::fromUtf8(&x[cPos]);
            }
        }
    }
    else
    {
        oName = homeDir.absoluteFilePath(inName);
    }
    oName = QDir::toNativeSeparators(oName);
    if(!oName.isEmpty())
    {
        if(!common::DiskOps::exist(oName))
        {
            oName = "";
        }
    }
    return oName;
}

//-------------------------------------------------------------------------------------------

void PlaylistAbstractIO::appendToList(const QString& lPath,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    track::info::InfoSPtr info = getTrack(lPath);

    if(info.data()==0)
    {
        if(isSupported(lPath))
        {
            QString lFacName = factoryName(lPath);
            if(!lFacName.isEmpty())
            {
                QSharedPointer<PlaylistAbstractIO> pLoader = PlaylistIOFactory::createShared(lFacName);
                if(pLoader.data()!=0)
                {
                    QVector<track::info::InfoSPtr> cList;

                    if(pLoader->load(lPath,cList,progress))
                    {
                        pList += cList;
                    }
                }
            }
        }
    }
    else
    {
        pList.append(info);
    }
}

//-------------------------------------------------------------------------------------------

void PlaylistAbstractIO::setParent(QObject *w)
{
    m_parent = w;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
