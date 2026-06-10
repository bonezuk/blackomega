//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMTODSD_H
#define __OMEGA_ENGINE_PCMTODSD_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include <QPair>
#include <QSharedPointer>
#include <QMutex>
#include <QSemaphore>

#include "engine/inc/RData.h"
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

        bool initInterleaved(int inputFrequency, int dsdTimes, CodecDataType dataType, int channelIndex, int noChannels);

        bool process(const double *in, uint8_t *out);

        bool isLSB() const;
        int noInputSamples() const;
        int noOutputSamples() const;
        int noOutputBytes() const;

        int inputFrequency() const;
        int outputFrequency() const;

        int push(const sample_t *in, int noSamples);
        int pull(uint8_t *out, int noBytes);

        // Each RData stores samples in sample_t (double) format with a size of 8 bytes
        // per sample. The number of samples for a given channel is reported by
        // RData.length() and RData::Part.length() for a given RData instance.
        // DSD data, dependant on dataType, is packed differently.
        // The noPCMSamples is the no of sample_t 8 byte long samples for a given channel.
        // e_SampleDSD8LSB & e_SampleDSD8MSB
        // Each byte contains 8 1-bit samples. Channel interleaving is done on a byte
        // per byte basis. For every 1 PCM samples then 8 DSD bytes (64 1-bit samples).
        // e_SampleInt24 & e_SampleInt32
        // Each 4-bytes is a single PCM encoded DSD packet containing 2-DSD byte samples (16-bits).
        // Uses MSB DSD byte encoding/
        // The Int24 sample is a 24-bit sample inside a 32-bit container.
        // The Int32 sample differs only by a logic shift << of 8 bytes.
        // For every 1 PCM sample_t of (8 bytes) there are 2 32-bit PCM samples each containing
        // 2 DSD-bytes making for a total of 4 DSD bytes per 1 PCM sample_t.
        // The return of pullInterleaved is out number of PCM sample_t samples filled with DSD data.
        int pullInterleaved(sample_t *out, int noPCMSamples);

        // Reports the number DSD bytes that are available to pull
        int available() const;
        // Reports the number of PCM sample_t that are available.
        // Conversion ratio dependant on the dataType.
        int availablePCMSamples() const;

        CodecDataType dataType() const;

    private:
        ComputeMethod m_computeMethod;
        CodecDataType m_dataType;
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
        int m_markerInc;

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
        int numberOfDSDBytesInAPCMSample() const;

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
