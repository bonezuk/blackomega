#include "player/oplayer/inc/OPlayerMain.h"
#include "engine/blackomega/inc/MPCodec.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/whiteomega/inc/WhiteCodec.h"
#include "track/info/inc/Info.h"
#include "common/inc/BIOBufferedStream.h"
#if defined(OMEGA_WIN32)
#include "audioio/inc/WasAPIIF.h"
#endif
#include "common/inc/CommonFunctions.h"

#if !defined(OMEGA_WIN32)
#include <termios.h>
#include <sys/ioctl.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OPlayer::OPlayer(int argc,char **argv) : QCoreApplication(argc,argv),
    m_audio(),
    m_fileNameList(),
    m_playTime(),
    m_playZeroFlag(false),
    m_totalTime(),
    m_printDeviceInfo(false),
    m_forceDacBits(-1)
{
    QTimer::singleShot(100,this,SLOT(onInit()));
    QObject::connect(this,SIGNAL(aboutToQuit()),this,SLOT(onStop()));
    processArguements(argc,argv);
}

//-------------------------------------------------------------------------------------------

OPlayer::~OPlayer()
{
    onStop();
}

//-------------------------------------------------------------------------------------------

void OPlayer::printTrackInfo(const QString& fileName)
{
    printf("Now Playing - %s.\n",fileName.toUtf8().constData());

    if(track::info::Info::isSupported(fileName))
    {
        common::BIOBufferedStream *file = new common::BIOBufferedStream(common::e_BIOStream_FileRead);

        if(file->open(fileName))
        {
            QSharedPointer<track::info::Info> pInfo = track::info::Info::readInfo(file);

            if(!pInfo.isNull())
            {
                if(!pInfo->title().isEmpty())
                {
                    printf("Title - %s\n",pInfo->title().toUtf8().constData());
                }
                if(!pInfo->album().isEmpty())
                {
                    printf("Album - %s\n",pInfo->album().toUtf8().constData());
                }
                if(!pInfo->artist().isEmpty())
                {
                    printf("Artist - %s\n",pInfo->artist().toUtf8().constData());
                }
                if(!pInfo->year().isEmpty())
                {
                    printf("Year - %s\n",pInfo->year().toUtf8().constData());
                }
                if(!pInfo->genre().isEmpty())
                {
                    printf("Genre - %s\n",pInfo->genre().toUtf8().constData());
                }
                printf("Bitrate - %dkbps\n",pInfo->bitrate() / 1000);
                printf("No. of Channels - %d\n",pInfo->noChannels());
                printf("Rate - %dHz\n",pInfo->frequency());
                printf("Length - %s\n",timeToString(pInfo->length()).toUtf8().constData());
                m_totalTime = pInfo->length();
            }
            file->close();
        }
        delete file;
    }
}

//-------------------------------------------------------------------------------------------

QString OPlayer::timeToString(const common::TimeStamp& t) const
{
    QString newT;
    tint min,sec;

    sec = t.secondsTotal();
    min = sec / 60;
    sec = sec % 60;
    newT  = common::BString::Int(min).GetString();
    newT += ":";
    newT += common::BString::Int(sec,2,true).GetString();
    return newT;
}

//-------------------------------------------------------------------------------------------

void OPlayer::onInit()
{
    if(!m_fileNameList.isEmpty() || m_printDeviceInfo)
    {
#if defined(OMEGA_WIN32)
        m_audio = audioio::AOBase::get("win32");
#elif defined(OMEGA_MACOSX)
        m_audio = audioio::AOBase::get("coreaudio");
#elif defined(OMEGA_LINUX)
        m_audio = audioio::AOBase::get("alsa");
#endif

        if(!m_audio.isNull())
        {
            connect(m_audio.data(),SIGNAL(onStart(const QString&)),this,SLOT(onAudioStart(const QString&)));
            connect(m_audio.data(),SIGNAL(onStop()),this,SLOT(onStop()));
            connect(m_audio.data(),SIGNAL(onPlay()),this,SLOT(onAudioPlay()));
            connect(m_audio.data(),SIGNAL(onPause()),this,SLOT(onAudioPause()));
            connect(m_audio.data(),SIGNAL(onTime(quint64)),this,SLOT(onAudioTime(quint64)));
            connect(m_audio.data(),SIGNAL(onBuffer(tfloat32)),this,SLOT(onAudioBuffer(tfloat32)));
            connect(m_audio.data(),SIGNAL(onReadyForNext()),this,SLOT(onAudioReadyForNext()));
            connect(m_audio.data(),SIGNAL(onNoNext()),this,SLOT(onAudioNoNext()));
            connect(m_audio.data(),SIGNAL(onCrossfade()),this,SLOT(onAudioCrossfade()));

            if(m_forceDacBits==16 || m_forceDacBits==24 || m_forceDacBits==32)
            {
                m_audio->forceBitsPerSample(m_forceDacBits);
            }
            m_audio->setCrossfade(0.0);

            if(!m_printDeviceInfo)
            {
                QString fileName = m_fileNameList.at(0);
                m_audio->open(fileName);
            }
            else
            {
                common::Log::g_Log.print("Number of Audio Devices %d\n",m_audio->noDevices());
                for(int deviceIdx=0;deviceIdx<m_audio->noDevices();deviceIdx++)
                {
                    m_audio->device(deviceIdx)->print();
                    common::Log::g_Log.print("\n");
                }
                onStop();
            }
        }
        else
        {
            common::Log::g_Log.print("Failed to initialise audio engine\n");
            onStop();
        }
    }
    else
    {
        onStop();
    }
}

//-------------------------------------------------------------------------------------------

void OPlayer::onStop()
{
    if(!m_audio.isNull())
    {
        m_audio.clear();
        common::Log::g_Log.print("\n");
    }
    quit();
}

//-------------------------------------------------------------------------------------------

void OPlayer::processArguements(int argc,char **argv)
{
    printf("Black Omega CLI Player v2.3.0 (c) 2021 Stuart A. MacLean\n");

    for(int idx=1;idx<argc;idx++)
    {
        QString name = QString::fromUtf8(argv[idx]);

        if(name.toLower().trimmed()=="--device_list")
        {
            m_printDeviceInfo = true;
        }
        else if(name.toLower().trimmed()=="--dac_bits")
        {
            idx++;
            if(idx < argc)
            {
                m_forceDacBits = QString::fromUtf8(argv[idx]).toInt();
            }
        }
        else
        {
            if(engine::Codec::getFileExtension(name)!="tone")
            {
                if(common::DiskOps::exist(name) && track::info::Info::isSupported(name))
                {
                    m_fileNameList.append(name);
                }
            }
            else
            {
                m_fileNameList.append(name);
            }
        }
    }

    if(m_fileNameList.isEmpty() && !m_printDeviceInfo)
    {
        printf("Usage: oplayer <audio files to play>\n");
        printf("       oplayer --device_list : To list audio devices\n");
        printf("       oplayer --dac_bits <16,24,32> <audio files to play>\n");
    }
}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioStart(const QString& name)
{
    printTrackInfo(name);
    m_playTime = 0;
    m_playZeroFlag = false;
}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioPlay()
{}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioPause()
{}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioTime(quint64 t)
{
    common::TimeStamp cT(t);

    if(cT.secondsTotal()!=m_playTime.secondsTotal() || (cT==0 && !m_playZeroFlag))
    {
        tint cWidth;

#if defined(OMEGA_WIN32)
        cWidth = 80;
#else
        struct winsize ws;
        ioctl(0, TIOCGWINSZ, &ws);
        cWidth = static_cast<tint>(ws.ws_col);
#endif

        QString newT = timeToString(cT);
        QString totT = timeToString(m_totalTime);
        QString timeP = newT + " / " + totT;
        QString line;

        if(timeP.length() + 9 < cWidth)
        {
            tint i;
            tint sbLen = cWidth - (timeP.length() + 6);
            tint sbPos = static_cast<tint>((static_cast<tfloat64>(cT) / static_cast<tfloat64>(m_totalTime)) * static_cast<tfloat64>(sbLen));
            if(sbPos<0)
            {
                sbPos = 0;
            }
            else if(sbPos>sbLen)
            {
                sbPos = sbLen;
            }

            line = "[";
            for(i=0;i<sbPos;i++)
            {
                line += "#";
            }
            line += "*";
            for(;i<sbLen;i++)
            {
                line += "-";
            }
            line += "] " + timeP;
        }
        else
        {
            line = timeP;
        }

#if defined(OMEGA_WIN32)
        printf("%s\r",line.toLatin1().constData());
#else
        printf("\33[2K%s\r",line.toLatin1().constData());
#endif
        fflush(stdout);

        if(cT==0)
        {
            m_playZeroFlag = true;
        }
    }
    m_playTime = cT;
}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioBuffer(tfloat32)
{}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioReadyForNext()
{}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioNoNext()
{}

//-------------------------------------------------------------------------------------------

void OPlayer::onAudioCrossfade()
{}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------

void setPluginLocation(const char *appPath)
{
#if defined(Q_OS_MAC)
    QFileInfo appFile(appPath);
    QDir d = appFile.absolutePath();
    QString pluginDir;
#if defined(BUNDLE_PLUGINS)
    d.cdUp();
#endif
    d.cd("Plugins");
    pluginDir = d.absolutePath();
    QCoreApplication::setLibraryPaths(QStringList(pluginDir));
#elif defined(Q_OS_WIN)
    QFileInfo appFile(appPath);
    QDir d = appFile.absolutePath();
    QString pluginDir;
    d.cdUp();
    d.cd("plugins");
    pluginDir = d.absolutePath();
    QCoreApplication::setLibraryPaths(QStringList(pluginDir));
#endif
}

//-------------------------------------------------------------------------------------------

void setupPlatform()
{
    ::omega::common::loadSharedLibrary("blackomega");
    ::omega::common::loadSharedLibrary("blueomega");
    ::omega::common::loadSharedLibrary("cyanomega");
    ::omega::common::loadSharedLibrary("greenomega");
    ::omega::common::loadSharedLibrary("redomega");
    ::omega::common::loadSharedLibrary("silveromega");
    ::omega::common::loadSharedLibrary("toneomega");
    ::omega::common::loadSharedLibrary("violetomega");
    ::omega::common::loadSharedLibrary("wavpackomega");
    ::omega::common::loadSharedLibrary("whiteomega");
    ::omega::common::loadSharedLibrary("widget");
#if defined(OMEGA_WIN32)
    CoInitialize(NULL);
#endif
}

//-------------------------------------------------------------------------------------------

void setupSettingsPath()
{
    QCoreApplication::setOrganizationName("Tiger-Eye");
    QCoreApplication::setOrganizationDomain("www.blackomega.co.uk");
    QCoreApplication::setApplicationName("BlackOmega2");
}

//-------------------------------------------------------------------------------------------

void setupEnviroment(const char *appPath)
{
    ::omega::network::Resource::instance();
    setPluginLocation(appPath);
    setupPlatform();
    setupSettingsPath();
}

//-------------------------------------------------------------------------------------------

omega::OPlayer *g_OPlayer = 0;

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

BOOL WINAPI ctrlHandler(DWORD)
{
    if(g_OPlayer!=0)
    {
        g_OPlayer->quit();
    }
    return TRUE;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

int main(int argc,char **argv)
{
    int res = 0;

    setupEnviroment(argv[0]);

#if defined(OMEGA_WIN32)
    ::SetConsoleCtrlHandler(ctrlHandler,TRUE);
    omega::audioio::WasAPIIF::instance("wasapi");
#endif

    omega::engine::CodecInitialize::start();
    omega::engine::blackomega::MPCodecInitialize::start();
    omega::engine::silveromega::SilverCodecInitialize::start();
    omega::engine::whiteomega::WhiteCodecInitialize::start();

    g_OPlayer = new omega::OPlayer(argc,argv);
    g_OPlayer->exec();
    delete g_OPlayer;
    g_OPlayer = 0;

    omega::engine::whiteomega::WhiteCodecInitialize::end();
    omega::engine::silveromega::SilverCodecInitialize::end();
    omega::engine::blackomega::MPCodecInitialize::end();
    omega::engine::CodecInitialize::end();

#if defined(OMEGA_WIN32)
    omega::audioio::WasAPIIF::release();
#endif

    return res;
}

//-------------------------------------------------------------------------------------------
