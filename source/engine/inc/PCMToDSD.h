//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMTODSD_H
#define __OMEGA_ENGINE_PCMTODSD_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include <QPair>
#include <QSharedPointer>
#include <QMutex>
#include <QSemaphore>

#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2_C2R.h"
#include "engine/inc/FIRFilterDB.h"
#include "engine/inc/FIRConvolutionAddOverlap.h"
#include "engine/inc/DeltaSigmaModulator.h"
#include "engine/inc/FixedDataQueue.h"

#if defined(OMEGA_CUDA)
#include "engine/inc/FFTRadix2Cuda.h"
#include "engine/inc/FIRConvolutionAddOverlapCUDA.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMToDSD
{
    public:
        typedef enum
        {
            e_computeMethodCPU = 0,
            e_computeMethodCUDA = 1
        } ComputeMethod;

    public:
        PCMToDSD();
        PCMToDSD(ComputeMethod computeMethod);
        virtual ~PCMToDSD();

        static bool isComputeMethodAvailable(ComputeMethod method);

        bool init(int inputFrequency, int dsdTimes, bool isLSB);

        bool initInterleaved(int inputFrequency, int dsdTimes, bool isLSB, int channelIndex, int noChannels);

        bool process(const double *in, uint8_t *out);

        bool isLSB() const;
        int noInputSamples() const;
        int noOutputSamples() const;
        int noOutputBytes() const;

        int inputFrequency() const;
        int outputFrequency() const;

        int push(const sample_t *in, int noSamples);
        int pull(uint8_t *out, int noBytes);

        int available() const;

    private:
        ComputeMethod m_computeMethod;
        QVector<QPair<FIRFilterType, QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> > > m_filters;
        QVector<QSharedPointer<double> > m_buffers;
        DeltaSigmaModulator m_modulator;
        int m_inputFrequency;
        int m_dsdTimes;

#if defined(OMEGA_CUDA)
        QVector<QPair<FIRFilterType, FIRConvAddOverlapCuda_Data *> > m_filtersCUDA;
        QSharedPointer<double> m_fbOutput;
#endif

        int m_available;
        // Number of channels in multiplexed input passed to push
        int m_noChannels;
        // Channel index of input samples
        int m_channelIndex;
        // number of input samples per block
        int m_noInputSamples;
        // number of samples that the input has been filled up with
        int m_inputBufferAmount;
        // the current input buffer which is filled and then passed onto the input queue
        double *m_inputBuffer;
        int m_outputBufferAmount;
        uint8_t *m_outputBuffer;
        // the input queue. push puts blocks on the input queue. the filter bank thread
        // consumes items from the input queue.
        QSharedPointer<FixedDataQueue<double> > m_inputQueue;
        QSharedPointer<FixedDataQueue<double> > m_dsmQueue;
        QSharedPointer<FixedDataQueue<uint8_t> > m_outputQueue;

        volatile bool m_isRunning;

#if defined(OMEGA_WIN32)
        HANDLE m_hThreads[2];
#else
        pthread_t m_threadIds[2];
#endif

        void done();

        int noSteps(int N) const;
        int baseFrequency(int freq) const;
        FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(FIRFilterType type) const;

        int noFilters() const;
        FIRFilterType filterTypeAtIndex(int idx) const;

        void processFilterBankCPU(const double *in, double *out);
#if defined(OMEGA_CUDA)
        bool processFilterBankCUDA(const double *in, double *out);
#endif

        bool startThreads();
        void stopThreads();

#if defined(OMEGA_WIN32)
        static DWORD WINAPI filterBankThread(LPVOID arg);
        static DWORD WINAPI deltaSigmaThread(LPVOID arg);
#else
        static void filterBankThread(void *arg);
        static void deltaSigmaThread(void *arg);
#endif

        void filterBankMain();
        void deltaSigmaMain();
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
