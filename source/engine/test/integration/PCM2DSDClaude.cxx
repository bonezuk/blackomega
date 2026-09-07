#include "gtest/gtest.h"

#include "common/inc/DiskOps.h"
#include "engine/inc/Codec.h"
#include "engine/inc/RData.h"
#include "engine/inc/PCMToDSD.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

typedef struct
{
    FIRFilterType type;
    int blockSize;
    int times;
} DSDFilterInfoUpscale;

const DSDFilterInfoUpscale c_filterDescriptionsUpscale[12] = {
    { engine::e_lpHalf_DSD0_5, 1024, 0 }, // 0
    { engine::e_lpHalf_DSD1, 2048, 1 },   // 1
    { engine::e_lpQuarter_DSD2, 4096, 2 }, // 2
    { engine::e_lpQuarter_DSD4, 8192, 4 }, // 3
    { engine::e_lpQuarter_DSD8, 16384, 8 }, // 4
    { engine::e_lpQuarter_DSD16, 32768, 16 }, // 5
    { engine::e_lpQuarter_DSD32, 65536, 32 }, // 6
    { engine::e_lpQuarter_DSD64, 131072, 64 }, // 7
    { engine::e_lpQuarter_DSD128, 262144, 128 }, // 8
    { engine::e_lpQuarter_DSD256, 524288, 256 }, // 9
    { engine::e_lpQuarter_DSD512, 1048576, 512 }, // 10
    { engine::e_lpQuarter_DSD1024, 2097152, 1024 } // 11
};

//-------------------------------------------------------------------------------------------

class PCMUpscale
{
    public:
        PCMUpscale();
        virtual ~PCMUpscale();
        
        bool init(int inputFrequency, int dsdTimes);
        int outputFrequency() const;

        int noInputSamples() const;
        int noOutputSamples() const;

        QSharedPointer<double> upscale(double *in, int noInput, int& noOutput);

    private:
        QVector<QPair<FIRFilterType, FIRConvAddOverlapCuda_Data *> > m_filtersCUDA;
        QSharedPointer<double> m_fbOutput;
        int m_inputFrequency;
        int m_dsdTimes;

        int noSteps(int N) const;
        engine::FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(engine::FIRFilterType type) const;
        bool processFilterBankCUDA(const double *in, double *out);
};

//-------------------------------------------------------------------------------------------

int PCMUpscale::noInputSamples() const
{
    int num = -1;
    engine::FIRFilterType type;
    
    type = filterTypeAtIndex(0);
    if(type != e_NoFilter)
    {
        int idx = filterIndexOfType(type);
        if(idx >= 0)
        {
            num = c_filterDescriptionsUpscale[idx].blockSize >> 1;
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::noOutputSamples() const
{
    int num = -1;
    engine::FIRFilterType type;
    
    type = filterTypeAtIndex(m_filtersCUDA.size() - 1);
    if(type != e_NoFilter)
    {
        int idx = filterIndexOfType(type);
        if(idx >= 0)
        {
            num = c_filterDescriptionsUpscale[idx].blockSize;
        }
    }
    return num;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::noSteps(int N) const
{
	int count = 0;

	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

//-------------------------------------------------------------------------------------------

engine::FIRFilterType PCMUpscale::filterForFrequency(int freq) const
{
    engine::FIRFilterType type = e_NoFilter;

    if(freq == 11025 || freq == 12000)
    {
        type = engine::e_lpHalf_DSD0_5;
    }
    else if(freq == 22050 || freq == 24000)
    {
        type = engine::e_lpHalf_DSD1;
    }
    else if(freq == 44100 || freq == 48000)
    {
        type = engine::e_lpHalf_DSD2;
    }
    else if(freq == 88200 || freq == 96000)
    {
        type = engine::e_lpHalf_DSD4;
    }
    else if(freq == 176400 || freq == 192000)
    {
        type = engine::e_lpHalf_DSD8;
    }
    else if(freq == 352800 || freq == 384000)
    {
        type = engine::e_lpQuarter_DSD16;
    }
    else if(freq == 705600 || freq == 768000)
    {
        type = engine::e_lpQuarter_DSD32;
    }
    return type;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::filterIndexOfType(engine::FIRFilterType type) const
{
    int idx;

    switch(type)
    {
        case engine::e_lpHalf_DSD0_5:
            idx = 0;
            break;
        case engine::e_lpHalf_DSD1:
            idx = 1;
            break;
        case engine::e_lpHalf_DSD2:
        case engine::e_lpQuarter_DSD2:
            idx = 2;
            break;
        case engine::e_lpHalf_DSD4:
        case engine::e_lpQuarter_DSD4:
            idx = 3;
            break;
        case engine::e_lpHalf_DSD8:
        case engine::e_lpQuarter_DSD8:
            idx = 4;
            break;
        case engine::e_lpQuarter_DSD16:
            idx = 5;
            break;
        case engine::e_lpQuarter_DSD32:
            idx = 6;
            break;
        case engine::e_lpQuarter_DSD64:
            idx = 7;
            break;
        case engine::e_lpQuarter_DSD128:
            idx = 8;
            break;
        case engine::e_lpQuarter_DSD256:
            idx = 9;
            break;
        case engine::e_lpQuarter_DSD512:
            idx = 10;
            break;
        case engine::e_lpQuarter_DSD1024:
            idx = 11;
            break;
        default:
            idx = -1;
            break;
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

bool PCMUpscale::init(int inputFrequency, int dsdTimes)
{
    bool res;

    int steps = noSteps(dsdTimes);

    if(dsdTimes != (1 << steps))
        return false;
    
    int idx;
    engine::FIRFilterType startType = filterForFrequency(inputFrequency);
    idx = filterIndexOfType(startType);
    if(idx < 0)
        return false;
    if(c_filterDescriptions[idx].times >= dsdTimes)
        return false;
    
    QVector<engine::FIRFilterType> filters;
    filters.append(startType);
    idx++;
    while(idx < 12 && c_filterDescriptionsUpscale[idx].times <= dsdTimes)
    {
        filters.append(c_filterDescriptionsUpscale[idx].type);
        idx++;
    }

    res = true;
    for(auto ppI = filters.begin(); ppI != filters.end() && res; ppI++)
    {
        int coeffLen;
        const engine::FIRFilterType& type = *ppI;

        res = false;
        idx = filterIndexOfType(type);
        if(idx >= 0 && idx <12)
        {            
            double *coeff = getFIRFilterFromDB(type, coeffLen);

            if(coeff != NULL)
            {
                FIRConvAddOverlapCuda_Data *pFilter = FIRConvAddOverlapCUDA_Init(coeff, coeffLen, c_filterDescriptionsUpscale[idx].blockSize);
                if(pFilter != NULL)
                {
                    m_filtersCUDA.append(qMakePair(type, pFilter));
                    res = true;
                }
                delete [] coeff;
            }
        }
    }
    if(filters.isEmpty())
    {
        res = false;
    }

    if(res)
    {
        idx = filterIndexOfType(m_filtersCUDA.at(m_filtersCUDA.size() - 1).first);
        QSharedPointer<double> pBuffer(new double [c_filterDescriptions[idx].blockSize]);
        m_fbOutput = pBuffer;
        m_inputFrequency = inputFrequency;
        m_dsdTimes = dsdTimes;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool PCMUpscale::processFilterBankCUDA(const double *in, double *out)
{
    int idx;
    const double *X;

    X = in;
    for(idx = 0; idx < m_filtersCUDA.size(); idx++)
    {
        X = FIRConvAddOverlapCUDA_OctaveUpscale_Process_Device(X, m_filtersCUDA.at(idx).second, (!idx) ? true : false);
        if(X == NULL)
            return false;
    }
    if(cudaMemcpy(out, X, sizeof(double) * noOutputSamples(), cudaMemcpyDeviceToHost) != cudaSuccess)
        return false;
    return true;
}

//-------------------------------------------------------------------------------------------

int PCMUpscale::outputFrequency() const
{
    return m_inputFrequency * m_dsdTimes;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<double> PCMUpscale::upscale(const double *in, int noInput, int& noOutput)
{
    if(in == NULL || (noInput % noInputSamples()) != 0)
    {
        return QSharedPointer<double>();
    }

    int noBlocks = noInput / noInputSamples();
    int idx = filterIndexOfType(m_filtersCUDA.at(m_filtersCUDA.size() - 1).first);
    noOutput = c_filterDescriptions[idx].blockSize * noBlocks;
    QSharedPointer<double> pBuffer(new double [noOutput]);

    double *out = pBuffer.get()
    for(idx = 0; idx < noBlocks && res; idx++)
    {
        processFilterBankCUDA(in, out);
        in += noInputSamples();
        out += c_filterDescriptions[idx].blockSize;
    }
    if(!res)
    {
        pBuffer.clear();
    }
    return pBuffer;
}

//-------------------------------------------------------------------------------------------

template<class T> T reverse_endian_of_value(T value)
{
	char* first = reinterpret_cast<char*>(&value);
	char* last = first + sizeof(T);
	std::reverse(first, last);
	return value;
}

//-------------------------------------------------------------------------------------------

void dsd_write_codec(engine::Codec *inCodec, const QString& outFilename, int DSD_Times)
{
    const int c_inputBlockSize = 2048;

    ASSERT_EQ(inCodec->noChannels(), 2);

    if(common::DiskOps::exist(outFilename))
    {
        common::DiskOps::deleteDirectory(outFilename);
    }

    FILE *WriteData = fopen(outFilename.toUtf8().constData(), "wb");
    ASSERT_FALSE(WriteData == NULL);

	int OrigSamplingRate = inCodec->frequency();
	int BaseSamplingRate;
	if (OrigSamplingRate % 44100 == 0) {
		BaseSamplingRate = 44100;
	}
	else {
		BaseSamplingRate = 48000;
	}
    int DSD_SamplingRate = BaseSamplingRate * DSD_Times;

    tuint64 OrigDataSize = static_cast<tuint64>(static_cast<tfloat64>(inCodec->length()) * static_cast<tfloat64>(inCodec->frequency()));
    tuint64 noOutputBlocks = OrigDataSize / c_inputBlockSize;
    if(OrigDataSize % c_inputBlockSize)
    {
        noOutputBlocks++;
    }
    // No of output bytes in the DSD data.
    tuint64 DSD_DataSize = (noOutputBlocks * c_inputBlockSize * DSD_Times * inCodec->noChannels()) / 8;

 	fwrite("FRM8", 4, 1, WriteData);//FRM8
    tuint64 binary = 0;
	unsigned short ushort = 0;
	unsigned char uchar = 0;
    tuint64 ulong = 0;
    tuint64 tell = 0;
	binary = reverse_endian_of_value(DSD_DataSize + 152);
	fwrite(&binary, 8, 1, WriteData);
	fwrite("DSD ", 4, 1, WriteData);//DSD

	fwrite("FVER", 4, 1, WriteData);//FVER
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);// Chunk size
	binary = reverse_endian_of_value(4);
	fwrite(&binary, 4, 1, WriteData); //Chunk size

	//Version
	binary = 1;
	fwrite(&binary, 1, 1, WriteData);
	binary = 5;
	fwrite(&binary, 1, 1, WriteData);
	binary = 0;
	fwrite(&binary, 1, 1, WriteData);
	binary = 0;
	fwrite(&binary, 1, 1, WriteData);

	fwrite("PROP", 4, 1, WriteData);//PROP
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian_of_value(108);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	fwrite("SND ", 4, 1, WriteData);//SND

	fwrite("FS  ", 4, 1, WriteData);//FS
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian_of_value(4);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
    tuint32 binary1;
	binary1 = reverse_endian_of_value(DSD_SamplingRate);
	fwrite(&binary1, 4, 1, WriteData);//SamplingRate

	fwrite("CHNL", 4, 1, WriteData);//CHNL
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian_of_value(10);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = 0;//number of channel
	fwrite(&binary, 1, 1, WriteData);
	binary = 2;
	fwrite(&binary, 1, 1, WriteData);
	fwrite("SLFT", 4, 1, WriteData);//SLFT
	fwrite("SRGT", 4, 1, WriteData);//SRGT

	fwrite("CMPR", 4, 1, WriteData);//CMPR
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian_of_value(20);
	fwrite(&binary, 4, 1, WriteData);//Chunk size

	fwrite("DSD ", 4, 1, WriteData);//DSD
	binary = 14;
	fwrite(&binary, 1, 1, WriteData);
	fwrite("not compressed ", 15, 1, WriteData);//not compressed

	fwrite("ABSS", 4, 1, WriteData);//ABSS
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian_of_value(8);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	fwrite(&ushort, 2, 1, WriteData);//Hours
	fwrite(&uchar, 1, 1, WriteData);//Minutes
	fwrite(&uchar, 1, 1, WriteData);//Seconds
	fwrite(&ulong, 4, 1, WriteData);//samples

	fwrite("LSCO", 4, 1, WriteData);//LSCO
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian_of_value(2);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	fwrite(&ushort, 2, 1, WriteData);//IsConfig

	fwrite("DSD ", 4, 1, WriteData);//DSD
	binary = reverse_endian_of_value(DSD_DataSize);
	fwrite(&binary, 8, 1, WriteData);//Chunk size   

    engine::PCMToDSD convertL(engine::PCMToDSD::e_computeMethodCUDA);
    ASSERT_TRUE(convertL.init(inCodec->frequency(), DSD_Times, false));
    engine::PCMToDSD convertR(engine::PCMToDSD::e_computeMethodCUDA);
    ASSERT_TRUE(convertR.init(inCodec->frequency(), DSD_Times, false));

    tfloat64 *inL = new tfloat64 [c_inputBlockSize];
    tfloat64 *inR = new tfloat64 [c_inputBlockSize];

    int outputLen = (c_inputBlockSize * DSD_Times) / 8;
    ASSERT_EQ(outputLen, convertL.noOutputBytes());
    uint8_t *outL = new uint8_t [outputLen];
    ASSERT_EQ(outputLen, convertR.noOutputBytes());
    uint8_t *outR = new uint8_t [outputLen];
    uint8_t *out = new uint8_t [outputLen + 2];

    engine::RData data(c_inputBlockSize, inCodec->noChannels(), inCodec->noChannels());

    tuint64 amount = 0;
    bool loop = true;
    while(loop && amount < DSD_DataSize)
    {
		double progress = static_cast<tfloat64>(amount) / static_cast<tfloat64>(DSD_DataSize);
        printf("%d\r", static_cast<int>(round(progress * 100.0)));

        loop = inCodec->next(data);
        if(data.noParts() > 0)
        {
            int idx;

            EXPECT_EQ(data.noParts(), 1);
            sample_t *x = data.partData(0);
            
            for(idx = 0; idx < data.part(0).length(); idx++)
            {
                inL[idx] = x[(idx << 1) + 0];
                inR[idx] = x[(idx << 1) + 1];
            }
            while(idx < c_inputBlockSize)
            {
                inL[idx] = 0.0;
                inR[idx] = 0.0;
                idx++;
            }

            ASSERT_TRUE(convertL.process(inL, outL));
            ASSERT_TRUE(convertR.process(inR, outR));

            for(idx = 0; idx < outputLen; idx++)
            {
                out[(idx << 1) + 0] = outL[idx];
                out[(idx << 1) + 1] = outR[idx];
            }

            ASSERT_EQ(fwrite(out, 1, 2 * outputLen, WriteData), 2 * outputLen);
            amount += 2 * outputLen;
        }
        data.reset();
    }
    for(int idx = 0; idx < outputLen * 2; idx++)
    {
        out[idx] = 0x69;
    }
    while(amount < DSD_DataSize)
    {
        ASSERT_EQ(fwrite(out, 1, 2 * outputLen, WriteData), 2 * outputLen);
        amount += 2 * outputLen;
    }

    delete [] inL;
    delete [] inR;
    delete [] outL;
    delete [] outR;
    delete [] out;

    fclose(WriteData);
}

//-------------------------------------------------------------------------------------------

TEST(PCM2DSDRevB, convertDSD128)
{
    QString inFilename = "D:\\Development\\Temp\\dsd\\ironfoot.m4a";
    QString outFilename = "D:\\Development\\Temp\\dsd\\ironfoot_dsd128_1.dff";

    engine::Codec *codec = engine::Codec::get(inFilename);
    ASSERT_FALSE(codec == NULL);
    ASSERT_TRUE(codec->init());

    dsd_write_codec(codec, outFilename, 128);

    delete codec;
}

//-------------------------------------------------------------------------------------------
