//-------------------------------------------------------------------------------------------
#ifndef __ORCUS_PLAYERAPP_AUDIODAEMON_AUDIODAEMONMAIN_H
#define __ORCUS_PLAYERAPP_AUDIODAEMON_AUDIODAEMONMAIN_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/OmegaAudioDaemon.h"
#include "playerapp/audiodaemon/inc/OmegaPLBusInterface.h"
#include "playerapp/audiodaemon/inc/OmegaAudioDBusAdaptor.h"

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace orcus
{
//-------------------------------------------------------------------------------------------

class AudioDaemonMain : public QCoreApplication
{
	Q_OBJECT
	
	public:
		AudioDaemonMain(int argc, char **argv);
		virtual ~AudioDaemonMain();
		
		virtual void shutdownDaemon();
		
	private:
		OmegaAudioDaemon *m_pAudio;
		OmegaPLBusInterface *m_pPLInterface;
		OmegaAudioDBusAdaptor *m_pDBusAudio;
		
		virtual void printError(const char *strR, const char *strE) const;
		
		virtual bool initAudioDBus();
		
	private slots:
		void onInit();
		void onQuitDaemon();
};

//-------------------------------------------------------------------------------------------
} // namespace orcus
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
