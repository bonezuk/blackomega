#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/db/inc/TrackDB.h"
#include "engine/blackomega/inc/MPCodec.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/whiteomega/inc/WhiteCodec.h"
#include "track/db/inc/TrackDB.h"
#include "common/inc/DiskOps.h"
#include "common/inc/SBService.h"
#include "common/inc/DiskLayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

TrackDBTestEnviroment *TrackDBTestEnviroment::m_instance = 0;

//-------------------------------------------------------------------------------------------

TrackDBTestEnviroment::TrackDBTestEnviroment()
{
#if defined(OMEGA_WIN32)
        m_blueOmegaDLL = LoadLibraryA("blueomega.dll");
        m_silverOmegaDLL = LoadLibraryA("silveromega.dll");
        m_blackOmegaDLL = LoadLibraryA("blackomega.dll");
        m_whiteOmegaDLL = LoadLibraryA("whiteomega.dll");
        m_redOmegaDLL = LoadLibraryA("redomega.dll");
        m_greenOmegaDLL = LoadLibraryA("greenomega.dll");
        m_RTPsilverOmegaDLL = LoadLibraryA("rtp_silveromega.dll");
#endif

        engine::CodecInitialize::start();
        engine::blackomega::MPCodecInitialize::start();
        engine::silveromega::SilverCodecInitialize::start();
        engine::whiteomega::WhiteCodecInitialize::start();

        m_execPath = "";
        m_spawnICSProcess = true;
}

//-------------------------------------------------------------------------------------------

TrackDBTestEnviroment::~TrackDBTestEnviroment()
{
    unload();

    engine::whiteomega::WhiteCodecInitialize::end();
    engine::silveromega::SilverCodecInitialize::end();
    engine::blackomega::MPCodecInitialize::end();
    engine::CodecInitialize::end();

    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

TrackDBTestEnviroment *TrackDBTestEnviroment::instance()
{
    if(m_instance==0)
    {
        m_instance = new TrackDBTestEnviroment;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void TrackDBTestEnviroment::setExecPath(const QString &path)
{
    m_execPath = path;
}

//-------------------------------------------------------------------------------------------

const QString& TrackDBTestEnviroment::execPath() const
{
    return m_execPath;
}

//-------------------------------------------------------------------------------------------

void TrackDBTestEnviroment::setSpawnICSProcess(bool flag)
{
    m_spawnICSProcess = flag;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::spawnICSProcess() const
{
    return m_spawnICSProcess;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::buildTestDB(const QString& dbFileName,const QString& dirName)
{
    bool res = false;
    db::TrackDB *trackDB = db::TrackDB::instance(dbFileName);

    if(trackDB!=0)
    {
        common::DiskIFSPtr diskIF = common::DiskIF::instance("disk");

        if(!diskIF.isNull())
        {
            res = scanDBDirectory(dirName);
            common::DiskIF::release();
        }
        delete trackDB;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::scanDBDirectory(const QString& dirName)
{
    bool res = false;
    db::TrackDB *trackDB = db::TrackDB::instance();

    if(trackDB!=0)
    {
        common::DiskIF::DirHandle h;
        common::DiskIFSPtr diskIF = common::DiskIF::instance();

        if(diskIF.data()!=0)
        {
            h = diskIF->openDirectory(dirName);
            if(h!=common::DiskIF::invalidDirectory())
            {
                QString name;

                res = true;
                while(name=diskIF->nextDirectoryEntry(h),!name.isEmpty() && res)
                {
                    QString fullName = common::DiskOps::mergeName(dirName,name);

                    if(diskIF->isDirectory(fullName))
                    {
                        if(name!=".AppleDouble")
                        {
                            res = scanDBDirectory(fullName);
                        }
                    }
                    else if(diskIF->isFile(fullName))
                    {
                        if(info::Info::isSupported(fullName))
                        {
                            common::BIOBufferedStream *trackFile = new common::BIOBufferedStream(common::e_BIOStream_FileRead);

                            if(trackFile->open(fullName))
                            {
                                info::InfoSPtr pInfo = info::Info::readInfo(trackFile);
                                if(pInfo.data()!=0)
                                {
                                    if(!trackDB->add(pInfo.data()))
                                    {
                                        res = false;
                                    }
                                }
                                else
                                {
                                    res = false;
                                }
                                trackFile->close();
                            }
                            else
                            {
                                res = false;
                            }
                            delete trackFile;
                        }
                    }
                }
                diskIF->closeDirectory(h);
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString TrackDBTestEnviroment::getTempDirectory()
{
    return common::SBService::tempDirectory();
}

//-------------------------------------------------------------------------------------------

QString TrackDBTestEnviroment::getDBDirectory()
{
    QString dbDirPath = common::DiskOps::mergeName(getTempDirectory(),"trackdbtest");
    dbDirPath = common::DiskOps::toNativeSeparators(dbDirPath);
    return dbDirPath;
}

//-------------------------------------------------------------------------------------------

QString TrackDBTestEnviroment::getDBFilename()
{
    QString dbFilename = common::DiskOps::mergeName(getDBDirectory(),"track.db");
    return dbFilename;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::load()
{
    bool res = false;
    QString dbDirPath = getDBDirectory();

    common::Log::g_Log.print("%s\n",dbDirPath.toUtf8().constData());

    common::DiskOps::deleteDirectory(dbDirPath);

    if(common::DiskOps::path(dbDirPath,true))
    {
        QString resourceName = ":/model/Resources/trackdb/track.db";
        QString dbFileName = getDBFilename();

        QVector<QPair<QString,QString> > fileList;

        fileList.append(QPair<QString,QString>(resourceName,"track.db"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel1.csv","albumModel1.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel2.csv","albumModel2.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel3.csv","albumModel3.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel4.csv","albumModel4.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel5.csv","albumModel5.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel6.csv","albumModel6.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel7.csv","albumModel7.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel8.csv","albumModel8.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel9.csv","albumModel9.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel10.csv","albumModel10.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel11.csv","albumModel11.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModel12.csv","albumModel12.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities1.csv","albumModelUtilities1.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities2.csv","albumModelUtilities2.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities3.csv","albumModelUtilities3.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities4.csv","albumModelUtilities4.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities5.csv","albumModelUtilities5.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities6.csv","albumModelUtilities6.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities7.csv","albumModelUtilities7.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities8.csv","albumModelUtilities8.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/albumModelUtilities9.csv","albumModelUtilities9.csv"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/iTunes.xml","iTunes.xml"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/iTunesNon.xml","iTunesNon.xml"));
        fileList.append(QPair<QString,QString>(":/model/Resources/trackdb/biostreamBookmark11Byte.dat","biostreamBookmark11Byte.dat"));
        fileList.append(QPair<QString,QString>(":/aiff/Resources/aiff/sample1.aiff","sample1.aiff"));
        fileList.append(QPair<QString,QString>(":/aiff/Resources/aiff/sample2.aiff","sample2.aiff"));
        fileList.append(QPair<QString,QString>(":/samples/Resources/samples/gather1.ape","gather1.ape"));
        fileList.append(QPair<QString,QString>(":/samples/Resources/samples/mpcchapter.mpc","mpcchapter.mpc"));
        fileList.append(QPair<QString,QString>(":/samples/Resources/samples/mpcchapteritem.mpc","mpcchapteritem.mpc"));
        fileList.append(QPair<QString,QString>(":/samples/Resources/samples/hobbit.cue","hobbit.cue"));
        fileList.append(QPair<QString,QString>(":/samples/Resources/samples/info1.wav","info1.wav"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/cue/album.cue","cue/album.cue"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/cue/album.flac","cue/album.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/none/track1.flac","none/track1.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/none/track2.flac","none/track2.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/none/track3.flac","none/track3.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/none/unrelated.cue","none/unrelated.cue"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/pic/cover.jpg","pic/cover.jpg"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/pic/track1.flac","pic/track1.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/pic/track2.flac","pic/track2.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/pic/track3.flac","pic/track3.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/piccue/album.cue","piccue/album.cue"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/piccue/album.flac","piccue/album.flac"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/piccue/cover.jpg","piccue/cover.jpg"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/wavpack/track1.wv","wavpack/track1.wv"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/wavpack/track1.wvc","wavpack/track1.wvc"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/wavpack/track2.wv","wavpack/track2.wv"));
        fileList.append(QPair<QString,QString>(":/depend/Resources/depend/wavpack/track2.wvc","wavpack/track2.wvc"));

        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/aiff/bps_16bit_2ch.aiff","aiff/bps_16bit_2ch.aiff"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/caf/bps_16bit_2ch.caf","caf/bps_16bit_2ch.caf"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/flac/bps_16bit_2ch_comp0.flac","flac/bps_16bit_2ch_comp0.flac"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/flac/bps_16bit_2ch_comp5.flac","flac/bps_16bit_2ch_comp5.flac"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/flac/bps_16bit_2ch_comp8.flac","flac/bps_16bit_2ch_comp8.flac"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/flac/bps_16bit_2ch_comp8_noseek.flac","flac/bps_16bit_2ch_comp8_noseek.flac"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/m4a_aac/bps_16bit_2ch_96kbps.m4a","m4a_aac/bps_16bit_2ch_96kbps.m4a"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/m4a_aac/bps_16bit_2ch_128kbps.m4a","m4a_aac/bps_16bit_2ch_128kbps.m4a"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/m4a_aac/bps_16bit_2ch_256kbps.m4a","m4a_aac/bps_16bit_2ch_256kbps.m4a"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/m4a_alac/bps_16bit_2ch.m4a","m4a_alac/bps_16bit_2ch.m4a"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/mp3/bps_16bit_2ch_96kbps.mp3","mp3/bps_16bit_2ch_96kbps.mp3"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/mp3/bps_16bit_2ch_128kbps.mp3","mp3/bps_16bit_2ch_128kbps.mp3"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/mp3/bps_16bit_2ch_256kbps.mp3","mp3/bps_16bit_2ch_256kbps.mp3"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/mp3/bps_16bit_2ch_vbr.mp3","mp3/bps_16bit_2ch_vbr.mp3"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/mpc/bps_16bit_2ch_q3_129.2kbps.mpc","mpc/bps_16bit_2ch_q3_129.2kbps.mpc"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/mpc/bps_16bit_2ch_q7_291.8kbps.mpc","mpc/bps_16bit_2ch_q7_291.8kbps.mpc"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/ogg/bps_16bit_2ch_96kpbs.ogg","ogg/bps_16bit_2ch_96kpbs.ogg"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/ogg/bps_16bit_2ch_128kpbs.ogg","ogg/bps_16bit_2ch_128kpbs.ogg"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/ogg/bps_16bit_2ch_256kpbs.ogg","ogg/bps_16bit_2ch_256kpbs.ogg"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wav/bps_16bit_2ch.wav","wav/bps_16bit_2ch.wav"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wv_full/bps_16bit_2ch_96kbps.wv","wv_full/bps_16bit_2ch_96kbps.wv"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wv_full/bps_16bit_2ch_96kbps.wvc","wv_full/bps_16bit_2ch_96kbps.wvc"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wv_full/bps_16bit_2ch_256kbps.wv","wv_full/bps_16bit_2ch_256kbps.wv"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wv_full/bps_16bit_2ch_256kbps.wvc","wv_full/bps_16bit_2ch_256kbps.wvc"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wv_lossy/bps_16bit_2ch_96kbps.wv","wv_lossy/bps_16bit_2ch_96kbps.wv"));
        fileList.append(QPair<QString,QString>(":/bps/Resources/bps/wv_lossy/bps_16bit_2ch_256kbps.wv","wv_lossy/bps_16bit_2ch_256kbps.wv"));

        if(setupDirectory(fileList))
        {
            db::TrackDB *db = db::TrackDB::instance(dbFileName);
            if(db!=0)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::setupDirectory(const QVector<QPair<QString,QString> >& fileList)
{
    int i;
    QString dbDirPath = getDBDirectory();
    bool res = true;

    for(i=0;i<fileList.size() && res;i++)
    {
        const QPair<QString,QString>& p = fileList.at(i);
        QString dbFileName = common::DiskOps::mergeName(dbDirPath,p.second);
        QFile file(p.first);

        if(!common::DiskOps::path(dbFileName))
        {
            res = false;
        }
        else
        {
            if(file.open(QIODevice::ReadOnly))
            {
                {
                    common::BIOStream oFile(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
                    if(oFile.open(dbFileName))
                    {
                        QByteArray mem = file.readAll();
                        if(oFile.write(mem.data(),mem.size())!=mem.size())
                        {
                            res = false;
                        }
                        oFile.close();
                    }
                    else
                    {
                        res = false;
                    }
                }
                file.close();

                if(res)
                {
                    if (!QFile::setPermissions(dbFileName, QFile::ReadOwner | QFile::WriteOwner)) {
                        res = false;
                    }
                }
            }
            else
            {
                res = false;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void TrackDBTestEnviroment::unload()
{
    db::TrackDB *db = db::TrackDB::instance();

    if(db!=0)
    {
        delete db;
    }
    if(common::DiskOps::exist(getDBDirectory()))
    {
        common::DiskOps::deleteDirectory(getDBDirectory());
    }
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::compareResults(const QVector<QVector<QVariant> >& results,const QString& resultPrefix,int resultID)
{
    bool res = true;
    QString resultFileName = resultPrefix + QString::number(resultID) + ".csv";
    QVector<QVector<QVariant> > expectResults;

    resultFileName = common::DiskOps::mergeName(getDBDirectory(),resultFileName);
    if(loadCVS(resultFileName,expectResults))
    {
        QVector<QVector<QVariant> >::iterator ppI;
        QVector<QVector<QVariant> >::const_iterator ppJ;

        for(ppJ=results.begin();ppJ!=results.end() && res;ppJ++)
        {
            ppI = indexOfResult(*ppJ,expectResults);
            if(ppI!=expectResults.end())
            {
                expectResults.erase(ppI);
            }
            else
            {
                res = false;
            }
        }

        if(!expectResults.empty())
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

QVector<QVector<QVariant> >::iterator TrackDBTestEnviroment::indexOfResult(const QVector<QVariant>& results,QVector<QVector<QVariant> >& list)
{
    QVector<QVector<QVariant> >::iterator ppI;

    for(ppI=list.begin();ppI!=list.end();ppI++)
    {
        QVector<QVariant>& eResults = *ppI;

        if(results.size()==eResults.size())
        {
            bool res = true;

            for(int i=0;i<static_cast<int>(results.size()) && res;i++)
            {
                if(!isAnyEqual(results.at(i),eResults.at(i)))
                {
                    res = false;
                }
            }
            if(res)
            {
                break;
            }
        }
    }
    return ppI;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::isAnyEqual(const QVariant& a,const QVariant& b)
{
    bool res = false;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if(!res || (a.canConvert(QVariant::Double) && b.canConvert(QVariant::Double)))
    {
        res = isEqual(a.toDouble(),b.toDouble());
    }
    if(!res || (a.canConvert(QVariant::Int) && b.canConvert(QVariant::Int)))
    {
        res = (a.toInt()==b.toInt());
    }
    if(!res || (a.canConvert(QVariant::String) && b.canConvert(QVariant::String)))
    {
        res = (a.toString()==b.toString());
    }
#elif QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if(!res || (a.canConvert(QMetaType::Double) && b.canConvert(QMetaType::Double)))
    {
        res = isEqual(a.toDouble(),b.toDouble());
    }
    if(!res || (a.canConvert(QMetaType::Int) && b.canConvert(QMetaType::Int)))
    {
        res = (a.toInt()==b.toInt()) ? true : false;
    }
    if(!res || (a.canConvert(QMetaType::QString) && b.canConvert(QMetaType::QString)))
    {
        res = (a.toString()==b.toString()) ? true : false;
    }
#else
    if(!res || (a.canConvert(QMetaType(QMetaType::Double)) && b.canConvert(QMetaType(QMetaType::Double))))
    {
        res = isEqual(a.toDouble(),b.toDouble());
    }
    if(!res || (a.canConvert(QMetaType(QMetaType::Int)) && b.canConvert(QMetaType(QMetaType::Int))))
    {
        res = (a.toInt()==b.toInt()) ? true : false;
    }
    if(!res || (a.canConvert(QMetaType(QMetaType::QString)) && b.canConvert(QMetaType(QMetaType::QString))))
    {
        res = (a.toString()==b.toString()) ? true : false;
    }
#endif
    return res;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::loadCVS(const QString& cvsFileName,QVector<QVector<QVariant> >& entries)
{
    common::BIOBufferedStream cvsFile(common::e_BIOStream_FileRead);
    bool res = false;

    if(cvsFile.open(cvsFileName))
    {
        tint len = cvsFile.size();
        tchar *mem = new tchar [len + 1];

        if(cvsFile.read(mem,len)==len)
        {
            int i,s,j,state = 0;
            QString iStr;
            QVector<QVariant> entry;

            for(i=0,s=0,j=0;i<=len;i++)
            {
                bool entryFlag = false, newlineFlag = false;

                if(i < len)
                {
                    switch(state)
                    {
                        case 0:
                            if(mem[i]=='\"')
                            {
                                s = i + 1;
                                state = 1;
                            }
                            else if(mem[i]=='\r')
                            {
                                state = 3;
                            }
                            else if(mem[i]=='\n')
                            {
                                newlineFlag = true;
                            }
                            break;

                        case 1:
                            if(mem[i]=='\"')
                            {
                                j = i;
                                state = 2;
                            }
                            break;

                        case 2:
                            if(mem[i]==',')
                            {
                                entryFlag = true;
                                state = 0;
                            }
                            else if(mem[i]=='\r')
                            {
                                entryFlag = true;
                                state = 3;
                            }
                            else if(mem[i]=='\n')
                            {
                                entryFlag = true;
                                newlineFlag = true;
                                state = 0;
                            }
                            break;

                        case 3:
                            if(mem[i]=='\n')
                            {
                                newlineFlag = true;
                            }
                            state = 0;
                            break;
                    }
                }
                else if(state==2)
                {
                    entryFlag = true;
                    newlineFlag = true;
                }

                if(entryFlag)
                {
                    if(i >= s)
                    {
                        mem[j] = '\0';
                        iStr = &mem[s];
                        if(isInteger(iStr))
                        {
                            int v = getIntegerFromString(iStr);
                            entry.push_back(v);
                        }
                        else if(isDouble(iStr))
                        {
                            tfloat64 v = getDoubleFromString(iStr);
                            entry.push_back(v);
                        }
                        else
                        {
                            iStr = removeWhitespace(iStr);
                            entry.push_back(iStr);
                        }
                    }
                    s = i + 1;
                }
                if(newlineFlag)
                {
                    if(entries.size() > 0)
                    {
                        if(entries.at(0).size()==entry.size())
                        {
                            entries.push_back(entry);
                        }
                    }
                    else
                    {
                        entries.push_back(entry);
                    }
                    entry.clear();
                }
            }
            res = true;
        }
        cvsFile.close();
        delete [] mem;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString TrackDBTestEnviroment::removeWhitespace(const QString& str)
{
    int i = 0,j = str.size()-1;
    QString sTxt;

    while(i<static_cast<int>(str.size()) && (str.at(i)==QChar(' ') || str.at(i)==QChar('\t')))
    {
        i++;
    }
    while(j>=0 && (str.at(j)==QChar(' ') || str.at(j)==QChar('\t')))
    {
        j--;
    }
    if(i<=j)
    {
        sTxt = str.mid(i,j+1);
    }
    return sTxt;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::isInteger(const QString& str)
{
    QString txt = removeWhitespace(str);
    bool res = true;

    if(txt.size() > 0)
    {
        for(int i=0;res && i<static_cast<int>(txt.size());i++)
        {
            if(!(txt.at(i)>=QChar('0') && txt.at(i)<=QChar('9')))
            {
                res = false;
            }
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint TrackDBTestEnviroment::getIntegerFromString(const QString& str)
{
    QString txt = removeWhitespace(str);
    int num = 0;

    for(int i=0;i<static_cast<int>(txt.size());i++)
    {
        if(txt.at(i)>=QChar('0') && txt.at(i)<=QChar('9'))
        {
            num = (num * 10) + (static_cast<int>(txt.at(i).toLatin1() - '0'));
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

bool TrackDBTestEnviroment::isDouble(const QString& str)
{
    QString txt = removeWhitespace(str);
    bool res = true;

    if(txt.size() > 0)
    {
        int fStop = 0;

        for(int i=0;res && i<static_cast<int>(txt.size());i++)
        {
            if(txt.at(i)==QChar('.'))
            {
                fStop++;
            }
            else if(!(txt.at(i)>=QChar('0') && txt.at(i)<=QChar('9')))
            {
                res = false;
            }
        }

        if(fStop>1)
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

tfloat64 TrackDBTestEnviroment::getDoubleFromString(const QString& str)
{
    int i;
    int numA = 0,numB = 0,digitsB = 0;
    QString txt = removeWhitespace(str);
    tfloat64 n,d;

    for(i=0;i<static_cast<int>(txt.size()) && txt.at(i)!=QChar('.');i++)
    {
        if(txt.at(i)>='0' && txt.at(i)<='9')
        {
            numA = (numA * 10) + (static_cast<int>(txt.at(i).toLatin1() - '0'));
        }
    }
    for(;i<static_cast<int>(txt.size());i++)
    {
        if(txt.at(i)>=QChar('0') && txt.at(i)<=QChar('9'))
        {
            numB = (numB * 10) + (static_cast<int>(txt.at(i).toLatin1() - '0'));
            digitsB++;
        }
    }

    if(digitsB>0)
    {
        d = static_cast<tfloat64>(numB) / pow(10.0,static_cast<tfloat64>(digitsB));
    }
    else
    {
        d = 0.0;
    }
    n = static_cast<tfloat64>(numA) + d;
    return n;
}

//-------------------------------------------------------------------------------------------

QString TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(const QString& iSql)
{
    int i,state = 0;
    QStringList x;
    QString oSql,t;

    for(i=0;i<iSql.length();i++)
    {
        QChar c = iSql.at(i);

        switch(state)
        {
        case 0:
            if(c==QChar('\''))
            {
                state = 2;
                break;
            }

        case 1:
            if(!c.isSpace())
            {
                t += c;
            }
            else
            {
                x.append(t);
                t = "";
                state = 3;
            }
            break;

        case 2:
            if(c==QChar('\''))
            {
                t = "'" + t + "'";
                x.append(t);
                t = "";
                state = 0;
            }
            else
            {
                t += c;
            }
            break;

        case 3:
            if(!c.isSpace())
            {
                state = 0;
                i--;
            }
            break;
        }
    }
    if(!t.isEmpty())
    {
        x.append(t);
    }
    oSql = x.join(" ").trimmed();
    return oSql;
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
