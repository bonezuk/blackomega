#include "engine/redomega/test/comparison/RedOmegaComparison.h"
#include "common/inc/CommonFunctions.h"

#include "ALACDecoderRef.h"
#include "ALACBitUtilities.h"

#include <QTimer>
#include <QFileInfo>
#include <QDir>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// RedOmegaCodecThread
//-------------------------------------------------------------------------------------------

RedOmegaCodecThread::RedOmegaCodecThread(const QString& fileName,QObject *parent) : QThread(parent),
	m_fileName(fileName)
{}

//-------------------------------------------------------------------------------------------

RedOmegaCodecThread::~RedOmegaCodecThread()
{}

//-------------------------------------------------------------------------------------------

void RedOmegaCodecThread::decodeUsingOmega(const QString& fileName)
{
	common::BIOBufferedStream *fileStream = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
	if(fileStream->open(fileName))
	{
		whiteomega::WhiteCodec *codec = new whiteomega::WhiteCodec;
		if(codec->open(fileName) && codec->init())
		{
			engine::RData data(2048, codec->noChannels(), codec->noChannels());
			
			while(codec->next(data))
			{
				data.reset();
			}
		}
		delete codec;
	}
	delete fileStream;
}

//-------------------------------------------------------------------------------------------

void RedOmegaCodecThread::run()
{
	decodeUsingOmega(m_fileName);
}

//-------------------------------------------------------------------------------------------
// RedOmegaComparisonApp
//-------------------------------------------------------------------------------------------

RedOmegaComparisonApp::RedOmegaComparisonApp(int& argc, char **argv) : QCoreApplication(argc, argv)
{
	QTimer::singleShot(1, this, SLOT(onRun()));
}

//-------------------------------------------------------------------------------------------

RedOmegaComparisonApp::~RedOmegaComparisonApp()
{}

//-------------------------------------------------------------------------------------------

void RedOmegaComparisonApp::decodeUsingReference(const QString& fileName)
{
	common::BIOBufferedStream *fileStream = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
	if(fileStream->open(fileName))
	{
		IOFile *fileIO = new IOFile(fileStream);
		whiteomega::Atom *atom = new whiteomega::Atom;
		if(atom->readMP4File(fileIO))
		{
            whiteomega::Atom::Track *track = atom->m_tracks.first();
            if(track->m_type==whiteomega::Atom::Track::e_track_alac)
			{
				ALACDecoderRef *refDecoder = new ALACDecoderRef;
				
				if(refDecoder->Init(track) == ALAC_noErr)
				{
					uint32_t numFrames = 0;
					tint outPacketSize = track->m_alacNumChannels * (track->m_alacBitDepth >> 3) * track->m_alacFrameLength;
					tuint8 *outBuffer = new tuint8 [outPacketSize];
				
					atom->buildSampleTable(0);
					for(tint dSampleNo = 0; dSampleNo < atom->noSamples(0); dSampleNo++)
					{
						common::Array<tubyte,tubyte> mem;
						if(atom->readSample(fileIO, 0, dSampleNo, mem))
						{
							BitBuffer theInputBuffer;
							BitBufferInit(&theInputBuffer, mem.GetData(), mem.GetSize());
                            refDecoder->Decode(&theInputBuffer, outBuffer, track->m_alacFrameLength, track->m_alacNumChannels, &numFrames);
						}
						else
						{
							break;
						}
					}
					delete [] outBuffer;
				}
				delete refDecoder;
			}
		}
		delete atom;
		delete fileIO;
	}
	delete fileStream;
}

//-------------------------------------------------------------------------------------------

void RedOmegaComparisonApp::onRun()
{
	QString testFilename = "D:\\Music\\Temp\\Fading.m4a";
	
	/*
	engine::g_Compare.setThreadA();
	engine::g_Compare.run();
	*/
	RedOmegaCodecThread *omegaThread = new RedOmegaCodecThread(testFilename, this);
	omegaThread->start();

	//decodeUsingReference(testFilename);
	
	omegaThread->wait();
	delete omegaThread;
	
	quit();
}

//-------------------------------------------------------------------------------------------
} // namespace engine
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
	::omega::common::loadSharedLibrary("redomega");
	::omega::common::loadSharedLibrary("whiteomega");
#if defined(OMEGA_WIN32)
	CoInitialize(NULL);
#endif
}

//-------------------------------------------------------------------------------------------

void setupSettingsPath()
{
	QCoreApplication::setOrganizationName("Tiger-Eye");
	QCoreApplication::setOrganizationDomain("www.blackomega.co.uk");
	QCoreApplication::setApplicationName("RedOmegaComparison");
}

//-------------------------------------------------------------------------------------------

void setupEnviroment(const char *appPath)
{
	setPluginLocation(appPath);
	setupPlatform();
	setupSettingsPath();    
}

//-------------------------------------------------------------------------------------------

int main(int argc,char **argv)
{
	setupEnviroment(argv[0]);

    omega::engine::CodecInitialize::start();
    omega::engine::whiteomega::WhiteCodecInitialize::start();

    omega::engine::RedOmegaComparisonApp app(argc, argv);
	app.exec();
	
    omega::engine::whiteomega::WhiteCodecInitialize::end();
    omega::engine::CodecInitialize::end();

	return 0;
}
