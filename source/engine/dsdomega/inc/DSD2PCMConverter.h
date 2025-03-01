//-------------------------------------------------------------------------------------------
/*
 * 
 * Acknowledgments
 * 
 * Many thanks to the following authors and projects whose work has greatly
 * helped the development of this tool.
 * 
 * 
 * Sebastian Gesemann - dsd2pcm (http://code.google.com/p/dsd2pcm/)
 * SACD Ripper (http://code.google.com/p/sacd-ripper/)
 * Maxim V.Anisiutkin - foo_input_sacd (http://sourceforge.net/projects/sacddecoder/files/)
 * Vladislav Goncharov - foo_input_sacd_hq (http://vladgsound.wordpress.com)
 * Jesus R - www.sonore.us
 * 
 */
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSD2PCMCONVERTER_H
#define __OMEGA_ENGINE_DSD_DSD2PCMCONVERTER_H
//-------------------------------------------------------------------------------------------

#include "engine/dsdomega/inc/DSFFileReader.h"
#include <QThreadPool>
#include <QSemaphore>

#define DSD2PCMCONVERTER_MULTITHREADED 1

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------
#if defined(DSD2PCMCONVERTER_MULTITHREADED)
//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSD2PCMConverterWorker : public QRunnable
{
	public:
		DSD2PCMConverterWorker(tfloat64 **lookupTable, int noLookupTable, int nStep);
		virtual ~DSD2PCMConverterWorker();
		
		virtual void setup(const QByteArray& dsdInArray, int offset, int len, QSemaphore *sema);
		virtual void run() override;
		virtual const QList<sample_t>& pcmOutput() const;
		
	private:
		tfloat64 **m_lookupTable;
		int m_noLookupTable;
		int m_nStep;
		
		QByteArray m_dsdInArray;
		int m_tzPos;
		int m_inEndPos;
		QList<sample_t> m_pcmOutput;

		QSemaphore *m_semaphore;
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSD2PCMConverter
{
	public:
		DSD2PCMConverter();
		virtual ~DSD2PCMConverter();
		
		virtual bool setup(int dsdSampleFreq, int pcmSampleFreq, bool isLSB);
		
		virtual void push(const QByteArray& dsdInArray);
		virtual void pull(QByteArray& pcmOutArray, bool isFinal);
		
		virtual void reset();
		
	private:
		tint m_outputSampleRate;
		tint m_noLookupTable;
		// filter t=0 position
		tint m_tZero;
		tint m_nCoefficients;
		tfloat64 **m_lookupTable;
		// inFs/outFs
		tint m_ratio;
		tint m_nStep;
		// Input DSD samples
		QList<QByteArray> m_dsdInList;
		// The offset, in bytes, from start of m_dsdInList.at(0) array.
		int m_dsdTZPosition;
		// Flag set to true at the start of conversion.
		bool m_isStart;

#if defined(DSD2PCMCONVERTER_MULTITHREADED)
		QList<DSD2PCMConverterWorker *> m_workers;
		void setupWorkers();
#endif
		
		virtual void printError(const tchar *strR, const tchar *strE) const;
		virtual void initLookupTable(const tfloat64 *coefs, const int nCoefs, const int tz, bool isLSB);
		virtual void release();
		virtual void concatInput(QByteArray& dsdInArray);
		virtual void pcmListToOutput(QList<tfloat64> pcmList, QByteArray& pcmOutArray);
		virtual void convert(const QByteArray& dsdInArray, QByteArray& pcmOutArray);
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

