//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCM2DSDPROCESSOR_H
#define __OMEGA_ENGINE_PCM2DSDPROCESSOR_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/RData.h"
#include "engine/inc/PCMToDSD.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMToDSDProcessor
{
    public:
        PCMToDSDProcessor();
        virtual ~PCMToDSDProcessor();

        bool init(CodecDataType dataType, int inputFreq, int dsdTimes, int noChannels);

        void push(const RData& data);
        void finalise();
        bool isFinalised() const;

        int available() const;
        void pull(RData& data);

        CodecDataType dataType() const;
        int inputFrequency() const;
        int outputFrequency() const;

    private:
        int m_dsdTimes;

        int m_delayPCMCount;
        // The number of PCM output samples that the current pull position is in with respect
        // to the current output part (m_partInfo.first())
        int m_pcmSampleOffset;

        QVector<QSharedPointer<PCMToDSD> > m_convertors;
        QVector<QPair<int, common::TimeStamp> > m_partInfo;
        QSharedPointer<RData> m_delayData;

        bool m_isFinal;
        bool m_isPartNext;

        void done();
        int delayNoSamples() const;
        int noOutputPCMSamples(int noIn) const;
        double timePerOuputPCMSample() const;
        bool areTwoPartsSequential(int idx) const;
        bool isPartNext(int idx) const;
        int availableFinal() const;
        int availableNotFinal() const;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

