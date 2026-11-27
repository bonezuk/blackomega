//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMTODSDTRELLIS_H
#define __OMEGA_ENGINE_PCMTODSDTRELLIS_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/PCMToDSD.h"
#include "engine/inc/SDMTrellisBlock.h"
#if defined(OMEGA_CUDA)
#include "engine/inc/SDMTrellisCUDA.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

/*
PCMtoDSDTrellis employs the new SDMTrellisBlock or its CUDA variation sdmCUDATrellis. The
algorithm is designed to be run as a series of blocks with each its own Viberti SDM Trellis.
Each trellis must be run over a sufficiently large enough number of samples including their
warmup and overlays.

DSD64_32Lane
Blackwell 5090
Complete. Rate=64 NumBlks=64, DSD/sec=40603136.49352057, PCM/sec=634424.00771126
RunTime=2.06600000, NumSamples=8388608, NumPCM=131072
Amphere Orin Nano
Complete. Rate=64 NumBlks=64, DSD/sec=10602386.24872112, PCM/sec=165662.28513627
RunTime=7.91200000, NumSamples=8388608, NumPCM=131072

DSD64_64Lane
Blackwell 5090
Amphere Orin Nano
Complete. Rate=64 NumBlks=32, DSD/sec=5488489.92410899, PCM/sec=85757.65506420
RunTime=7.64200000, NumSamples=4194304, NumPCM=65536

DSD128_32Lane
Blackwell 5090
Complete. Rate=128 NumBlks=64, DSD/sec=38199489.98184986, PCM/sec=298433.51548320
RunTime=2.19600000, NumSamples=8388608, NumPCM=65536
Amphere Orin Nano
Complete. Rate=128 NumBlks=64, DSD/sec=10051052.00107519, PCM/sec=78523.84375840
RunTime=8.34600000, NumSamples=8388608, NumPCM=65536

DSD128_64Lane
Blackwell 5090
Amphere Orin Nano
Complete. Rate=128 NumBlks=32, DSD/sec=5097598.44440681, PCM/sec=39824.98784693
RunTime=8.22800000, NumSamples=4194304, NumPCM=32768

DSD256_32Lane
Blackwell 5090
Amphere Orin Nano
Complete. Rate=256 NumBlks=64, DSD/sec=9112109.49380285, PCM/sec=35594.17771017
RunTime=9.20600000, NumSamples=8388608, NumPCM=32768

DSD256_64Lane
Blackwell 5090
Complete. Rate=256 NumBlks=128, DSD/sec=55151926.36183638, PCM/sec=215437.21235092
RunTime=3.04200000, NumSamples=16777216, NumPCM=65536
Amphere Orin Nano
Complete. Rate=256 NumBlks=32, DSD/sec=4458231.29260174, PCM/sec=17414.96598673
RunTime=9.40800000, NumSamples=4194304, NumPCM=16384
Complete. Rate=256 NumBlks=128, DSD/sec=6222772.15239274, PCM/sec=24307.70372028
RunTime=26.96100000, NumSamples=16777216, NumPCM=65536

DSD512_32Lane
Blackwell 5090
Complete. Rate=512 NumBlks=256, DSD/sec=73007902.52177390, PCM/sec=142593.55961284
RunTime=4.59600000, NumSamples=33554432, NumPCM=65536

DSD512_64Lane
Blackwell 5090
Complete. Rate=1024 NumBlks=1024, DSD/sec=106606614.77489163, PCM/sec=104108.02224111
RunTime=12.59000000, NumSamples=134217728, NumPCM=131072

DSD1024_32Lane
Blackwell 5090
Complete. Rate=1024 NumBlks=256, DSD/sec=58163342.00181147, PCM/sec=56800.13867364
RunTime=5.76900000, NumSamples=33554432, NumPCM=32768

DSD1024_64Lane
Blackwell 5090
Complete. Rate=1024 NumBlks=1024, DSD/sec=70533253.45489350, PCM/sec=68880.13032704
RunTime=19.02900000, NumSamples=134217728, NumPCM=131072


*/

class ENGINE_EXPORT PCMToDSDTrellis : public PCMToDSD
{
    public:
        PCMToDSDTrellis();
        PCMToDSDTrellis(PCMToDSD::ComputeMethod computeMethod);
        virtual ~PCMToDSDTrellis();

        // Finalise is called when all the input has been passed into the instance
        virtual void finalise();

    protected:
        // The number of trellis blocks that are to be processed together
        int m_noBlocks;
        // The number of output 1-bit DSD samples processed by each block
        int m_outSamplesPerBlock;
        // Mutex lock in accessing shared data values
        QMutex m_mutex;
        // Used to signal that input has run to completion and to process the remaining
        // input on the queue.
        volatile bool m_isFinalise;
        // Order of trellis state machine
        int m_order;

#if defined(OMEGA_CUDA)
        // Upscaled sample array, as 32-bit floats.
        // size = m_noBlocks * m_outSamplesPerBlock
        float *m_sSArrayDevice;
        SDMTrellisBlockDataCUDA *m_sdmTrellisGPU;
        SDMTrellisBlockOutputCUDA *m_sdmTOutputGPU;
        bool m_is32Lane;
        uint8_t *m_outputGPU;
#endif
        double *m_sSArrayHost;
        SDMTrellisBlock<float> m_sdmTrellisBlock;
        uint8_t *m_outputCPU;

        // The number of input samples required to start multi-block processing across the input
        virtual int inputRequired() const;
        virtual int blocksPerInput() const;

        virtual bool initSDMCPU();
        virtual bool freeSDMCPU();
        virtual bool processSDMTrellisCPU(std::vector<uint8_t *>& oPtrVector);

#if defined(OMEGA_CUDA)
        virtual bool initSDMCUDA();
        virtual void freeSDMCUDA();
        virtual bool processSDMTrellisCUDA(std::vector<uint8_t *>& oPtrVector);
#endif

};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "engine/inc/PCMToDSDTrellis.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

PCMToDSDTrellis::PCMToDSDTrellis()
{}

//-------------------------------------------------------------------------------------------

PCMToDSDTrellis::PCMToDSDTrellis(PCMToDSD::ComputeMethod computeMethod)
{}

//-------------------------------------------------------------------------------------------

PCMToDSDTrellis::~PCMToDSDTrellis()
{}

//-------------------------------------------------------------------------------------------

int PCMToDSDTrellis::push(const sample_t *in, int noSamples)
{
    if(in == NULL || noSamples <= 0)
        return 0;

    if(m_inputBuffer == NULL)
    {
        m_inputBuffer = m_inputQueue->get();
        m_inputBufferAmount = 0;
    }

    int pos = 0;
    while(pos < noSamples * m_noChannels)
    {
        m_inputBuffer[m_inputBufferAmount] = in[pos + m_channelIndex];
        m_inputBufferAmount++;
        pos += m_noChannels;

        if(m_inputBufferAmount >= m_noInputSamples)
        {
            m_inputQueue->push(m_inputBuffer);
            m_inputBuffer = m_inputQueue->get();
            m_inputBufferAmount = 0;
        }
    }
    return (pos / m_noChannels);
}

//-------------------------------------------------------------------------------------------

int PCMToDSDTrellis::inputRequired() const
{
    int iSPerBlock = m_outSamplesPerBlock / m_dsdTimes;
    int req = iSPerBlock * m_noBlocks;
    return req;
}

//-------------------------------------------------------------------------------------------

int PCMToDSDTrellis::blocksPerInput() const
{
    return (m_inputQueue->arraySize() * m_dsdTimes) / m_outSamplesPerBlock;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_CUDA)
//-------------------------------------------------------------------------------------------

bool PCMToDSDTrellis::initSDMCUDA()
{
    SDMTrellisBlockDataCUDA trellisCPU;
    bool res = false;
    if(cudaMalloc(&m_sSArrayDevice, m_noBlocks * m_outSamplesPerBlock * sizeof(float)) == cudaSuccess)
    {
        if(initSDMTrellisBlockCUDA(m_dsdTimes, m_order, false, &trellisCPU, baseFrequency(m_inputFrequency)))
        {
            trellisCPU.prefBlockSamples = m_outSamplesPerBlock;
            sdmTrellisCUDACalcBlockSize(trellisCPU, m_outSamplesPerBlock * m_noBlocks);
            m_sdmTrellisGPU = engine::allocateSDMTrellisCUDABlockData();
            if(m_sdmTrellisGPU != nullptr)
            {
                if(copySDMTrellisCUDABlockOutput(&sdmTrellis, m_sdmTrellisGPU))
                {
                    res = true;
                }
            }
        }
        if(res)
        {
            res = false;
            m_sdmTOutputGPU = allocateSDMTrellisCUDABlockOuput(&sdmTrellis, true);
            if(m_sdmTOutputGPU != nullptr)
            {
                if(cudaMalloc(&m_outputGPU, (m_noBlocks * m_outSamplesPerBlock) / 8) == cudaSuccess)
                {
                    res = true;
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PCMToDSDTrellis::freeSDMCUDA()
{
    if(m_sSArrayDevice != nullptr)
    {
        cudaFree(m_sSArrayDevice);
        m_sSArrayDevice = nullptr;
    }
    if(m_sdmTrellisGPU != nullptr)
    {
        freeSDMTrellisCUDABlockData(m_sdmTrellisGPU);
        m_sdmTrellisGPU = nullptr;
    }
    if(m_sdmTOutputGPU != nullptr)
    {
        freeSDMTrellisCUDABlockOutput(m_sdmTOutputGPU);
        m_sdmTOutputGPU = nullptr;
    }
    if(m_outputGPU != nullptr)
    {
        cudaFree(m_outputGPU);
        m_outputGPU = nullptr;
    }
}

//-------------------------------------------------------------------------------------------

bool PCMToDSDTrellis::processSDMTrellisCUDA(std::vector<uint8_t *>& oPtrVector)
{
    int req = inputRequired();
    int outCount = 0;
    int pos = 0;
    int nBlocks = 0;
    while(m_inputQueue->lengthOfQueue() > 0 && pos < req && nBlocks < m_noBlocks)
    {
        const double *in = m_inputQueue->pull();
        const double *X = in;
        for(int idx = 0; idx < m_filtersCUDA.size(); idx++)
        {
            X = FIRConvAddOverlapCUDA_OctaveUpscale_Process_Device(X, m_filtersCUDA.at(idx).second, (!idx) ? true : false);
            if(X == nullptr)
                return false;
        }
        int noSamples = m_outSamplesPerBlock * blocksPerInput();
        float *oBlk = &m_sSArrayDevice[m_outSamplesPerBlock * nBlocks];
        convertCUDADoubleToFloatNoSync(X, oBlk, noSamples);
        m_inputQueue->free(in);
        pos += m_inputQueue->arraySize();
        nBlocks += blocksPerInput();
        outCount++;
    }

    int outNoSamples = pos * m_outSamplesPerBlock * blocksPerInput();
    nBlocks = sdmTrellisCUDACalcBlockSize(m_sdmTrellisGPU, outNoSamples);
    if(nBlocks < 0)
        return false;
    if(!reallocIfRequiredSDMTrellisCUDABlockOuput(m_sdmTrellisGPU, m_sdmTOutputGPU))
        return false;
    
    if(!runSDMTrellis(m_sdmTrellisGPU, m_sSArrayDevice, m_sdmTOutputGPU, nBlocks, m_is32Lane))
        return false;
    if(!sdmTrellisCUDAFindSplices(m_sdmTrellisGPU, m_sdmTOutputGPU, nBlocks))
        return false;

    ;
    for(int idx = 0; idx < outCount; idx++)
    {
        uint8_t *oBuffer = m_outputQueue->get();
        if(oBuffer != nullptr)
            return false;
        oPtrVector.push_back(oBuffer);
    }

    if(!sdmTrellisCUDAPackByBlock(m_sdmTrellisGPU, m_sdmTOutputGPU, m_outputGPU, outNoSamples, 
        isLSB(), oPtrVector, m_outputQueue->arraySize()))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool PCMToDSDTrellis::initSDMCPU()
{
    bool res = false;
    if(m_sdmTrellisBlock.init(m_dsdTimes, m_order, 31, m_noBlocks))
    {
        const int noOutSamples = m_noBlocks * m_outSamplesPerBlock;
        m_sSArrayHost = new double [noOutSamples];
        m_outputCPU = new uint8_t [noOutSamples >> 3];
        if(m_sSArrayHost != nullptr)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool PCMToDSDTrellis::freeSDMCPU()
{
    if(m_sSArrayHost != nullptr)
    {
        delete [] m_sSArrayHost;
        m_sSArrayHost = nullptr;
    }
}

//-------------------------------------------------------------------------------------------

bool PCMToDSDTrellis::processSDMTrellisCPU(std::vector<uint8_t *>& oPtrVector)
{
    int req = inputRequired();
    int outCount = 0;
    int pos = 0;
    int nBlocks = 0;
    while(m_inputQueue->lengthOfQueue() > 0 && pos < req && nBlocks < m_noBlocks)
    {
        const double *in = m_inputQueue->pull();
        const double *X = in;
        int noSamples = m_outSamplesPerBlock * blocksPerInput();
        double *oBlk = &m_sSArrayHost[m_outSamplesPerBlock * nBlocks];
        processFilterBankCPU(in, oBlk);
        m_inputQueue->free(in);
        pos += m_inputQueue->arraySize();
        nBlocks += blocksPerInput();
        outCount++;
    }

    int outNoSamples = pos * m_outSamplesPerBlock * blocksPerInput();
    int outDSDBytes = m_sdmTrellisBlock.process(m_sSArrayHost, outNoSamples, m_outputCPU, outNoSamples >> 3, isLSB());

    for(int idx = 0; idx < outCount; idx++)
    {
        uint8_t *oBuffer = m_outputQueue->get();
        if(oBuffer != nullptr)
            return false;
        memcpy(oBuffer, &m_outputCPU[idx * m_outputQueue->arraySize()], m_outputQueue->arraySize());
        oPtrVector.push_back(oBuffer);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void PCMToDSDTrellis::sdmTrellisMain()
{
    while(m_isRunning)
    {
        int req = inputRequired();
        while(m_inputQueue->size() < req && !m_isFinalise)
        {
            QThread::msleep(10);
        }
        
        std::vector<uint8_t *> oPtrVector;
#if defined(OMEGA_CUDA)
        if(m_computeMethod == e_computeMethodCUDA)
        {
            processSDMTrellisCUDA(oPtrVector);
        }
        else
#endif
        {
            processSDMTrellisCPU(oPtrVector);
        }

        for(int idx = 0; oPtrVector.size(); idx++)
        {
            m_outputQueue->push(oPtrVector[idx]);
        }
        oPtrVector.clear();
    }
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
