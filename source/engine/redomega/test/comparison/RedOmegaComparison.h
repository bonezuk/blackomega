//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TEST_COMPARISON_REDOMEGACOMPARISON_H
#define __OMEGA_TEST_COMPARISON_REDOMEGACOMPARISON_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/WhiteCodec.h"
#include "engine/redomega/inc/RedCodec.h"
#include "engine/inc/Compare.h"

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class RedOmegaCodecThread : public QThread
{
	Q_OBJECT
	public:
		RedOmegaCodecThread(const QString& fileName,QObject *parent);
		virtual ~RedOmegaCodecThread();
		
	protected:
		QString m_fileName;
		void run() override;
        void decodeUsingOmega(const QString& fileName);
};

class RedOmegaComparisonApp : public QCoreApplication
{
	Q_OBJECT
	public:
		RedOmegaComparisonApp(int& argc, char **argv);
		virtual ~RedOmegaComparisonApp();
	
	private:
		virtual void decodeUsingReference(const QString& fileName);
		
	private slots:
		virtual void onRun();
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
