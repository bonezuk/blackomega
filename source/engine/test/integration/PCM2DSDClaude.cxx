#include "gtest/gtest.h"

#include "common/inc/DiskOps.h"
#include "engine/inc/Codec.h"
#include "engine/inc/RData.h"
#include "engine/inc/PCMToDSD.h"

#include <cuda_runtime.h>

using namespace omega;

//-------------------------------------------------------------------------------------------

typedef struct
{
    engine::FIRFilterType type;
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

        QSharedPointer<double> upscale(const double *in, int noInput, int &noOutput);

    private:
        QVector<QPair<engine::FIRFilterType, FIRConvAddOverlapCuda_Data *> > m_filtersCUDA;
        QSharedPointer<double> m_fbOutput;
        int m_inputFrequency;
        int m_dsdTimes;

        int noSteps(int N) const;
        engine::FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(engine::FIRFilterType type) const;
        engine::FIRFilterType filterTypeAtIndex(int idx) const;
        bool processFilterBankCUDA(const double *in, double *out);
};

//-------------------------------------------------------------------------------------------

PCMUpscale::PCMUpscale()
{}

PCMUpscale::~PCMUpscale()
{}

int PCMUpscale::noInputSamples() const
{
    int num = -1;
    engine::FIRFilterType type;
    
    type = filterTypeAtIndex(0);
    if(type != engine::e_NoFilter)
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
    if(type != engine::e_NoFilter)
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
    engine::FIRFilterType type = engine::e_NoFilter;

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

engine::FIRFilterType PCMUpscale::filterTypeAtIndex(int idx) const
{
    engine::FIRFilterType type = engine::e_NoFilter;

    if(idx >= 0 && idx < m_filtersCUDA.size())
    {
        type = m_filtersCUDA.at(idx).first;
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
    if(c_filterDescriptionsUpscale[idx].times >= dsdTimes)
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
        QSharedPointer<double> pBuffer(new double [c_filterDescriptionsUpscale[idx].blockSize]);
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
    noOutput = c_filterDescriptionsUpscale[idx].blockSize * noBlocks;
    QSharedPointer<double> pBuffer(new double [noOutput]);

    bool res = true;
    double *out = pBuffer.get();
    for(int idx = 0; idx < noBlocks && res; idx++)
    {
        res = processFilterBankCUDA(in, out);
        if(res)
        {
            in += noInputSamples();
            out += c_filterDescriptionsUpscale[idx].blockSize;
        }
    }
    if(!res)
    {
        pBuffer.clear();
    }
    return pBuffer;
}

//-------------------------------------------------------------------------------------------
// DSDModulatorClaude
//-------------------------------------------------------------------------------------------

#define DSD_MAX_ORDER 8

/* Peak TPDF dither amplitude injected at the quantizer, as a fraction of
 * the loop filter's typical internal signal swing. Purpose: decorrelate
 * deterministic limit cycles / idle tones that a noiseless 1-bit quantizer
 * produces on near-silent or highly periodic input (well known in ΔΣ
 * literature, e.g. Norsworthy/Schreier/Temes ch.2 and Reiss 2008 JAES).
 * This value is intentionally small: enough to randomize bit-pattern
 * periodicity, not enough to measurably raise the shaped noise floor.
 * Tune per application if needed (0.0 disables dither entirely). */
#define DSD_DITHER_SCALE 0.02

//-------------------------------------------------------------------------------------------

class DSDModulatorClaude
{
    public:
 
        typedef enum {
            DSD_RATE_64 = 0,   /* 64  x 44.1kHz =  2,822,400 Hz */
            DSD_RATE_128,      /* 128 x 44.1kHz =  5,644,800 Hz */
            DSD_RATE_256,      /* 256 x 44.1kHz = 11,289,600 Hz */
            DSD_RATE_512,      /* 512 x 44.1kHz = 22,579,200 Hz */
            DSD_RATE_1024,     /* 1024x 44.1kHz = 45,158,400 Hz */
            DSD_RATE_COUNT
        } dsd_rate_t;

        typedef struct {
            double         hz;
            int            order;
            const double  *b;
            const double  *a;
            double         max_input_level; /* documented safe peak, incl. margin */
        } dsd_rate_info_t;

        typedef struct {
            dsd_rate_t rate;
            int        order;              /* NTF/loop-filter order for this rate   */
            double     in_hist[DSD_MAX_ORDER]; /* in_hist[0]=in[n-1], in_hist[1]=in[n-2], ... */
            double     y_hist[DSD_MAX_ORDER];  /* y_hist[0]=w[n-1],  y_hist[1]=w[n-2],  ...   */
            uint32_t   rng_state;          /* xorshift32 state, quantizer dither     */
            double     dither_amplitude;   /* peak TPDF dither added at the quantizer */
            unsigned   bit_accum;          /* partial output byte being packed       */
            int        bit_count;          /* number of valid bits in bit_accum (0-7)*/
        } dsd_modulator_t;

    public:
        DSDModulatorClaude();
        virtual ~DSDModulatorClaude();

        bool init(int rate);
        void process(const double *in, int noSamples, uint8_t *out);

    private:

        static constexpr double DSD64_B[8] = {
            0.77768777836567, -5.14225082352806, 14.59910421942007, -23.06722376602455,
            21.90551453146337, -12.50177339809122, 3.97007607918067, -0.54113439186735
        };
        static constexpr double DSD64_A[8] = {
            -7.99603577050550, 27.97621953408770, -55.94055619985439, 69.92074487254449,
        -55.94055619985438, 27.97621953408770, -7.99603577050550, 1.00000000000000
        };

        /* DSD128 (5,644,800 Hz): order 6, OOB gain 1.534, max stable amplitude 0.482 */
        static constexpr double DSD128_B[6] = {
            0.85322292771034, -3.90928328928330, 7.19765860500578, -6.65390648118472,
            3.08746131898673, -0.57507420429147
        };
        static constexpr double DSD128_A[6] = {
            -5.99925663789685, 14.99702668973517, -19.99554010367283, 14.99702668973517,
            -5.99925663789686, 1.00000000000000
        };

        /* DSD256 (11,289,600 Hz): order 6, OOB gain 1.153, max stable amplitude 0.849 */
        static constexpr double DSD256_B[6] = {
            0.28486252984180, -1.38398770093388, 2.69095262458139, -2.61734242282095,
            1.27347307239101, -0.24795795852306
        };
        static constexpr double DSD256_A[6] = {
            -5.99981415515693, 14.99925662926232, -19.99888494821072, 14.99925662926232,
            -5.99981415515693, 1.00000000000000
        };

        /* DSD512 (22,579,200 Hz): order 5, OOB gain 1.178, max stable amplitude 0.801 */
        static constexpr double DSD512_B[5] = {
            0.32768697101076, -1.25768151000048, 1.81216216879685, -1.16171424086315,
            0.27955572445218
        };
        static constexpr double DSD512_A[5] = {
            -4.99997676924971, 9.99993030780908, -9.99993030780908, 4.99997676924971,
            -1.00000000000000
        };

        /* DSD1024 (45,158,400 Hz): order 4, OOB gain 1.129, max stable amplitude 0.904 */
        static constexpr double DSD1024_B[4] = {
            0.24302655137938, -0.69998245745760, 0.67288583150718, -0.21586338031892
        };
        static constexpr double DSD1024_A[4] = {
            -3.99999225640866, 5.99998451282481, -3.99999225640866, 1.00000000000000
        };

        static constexpr dsd_rate_info_t RATE_INFO[DSD_RATE_COUNT] = {
            /* rate         hz            order  b            a            max_input */
            { 2822400.0,     8, DSD64_B,   DSD64_A,   0.454 },
            { 5644800.0,     6, DSD128_B,  DSD128_A,  0.430 },
            {11289600.0,     6, DSD256_B,  DSD256_A,  0.757 },
            {22579200.0,     5, DSD512_B,  DSD512_A,  0.714 },
            {45158400.0,     4, DSD1024_B, DSD1024_A, 0.806 },
        };

        dsd_modulator_t m_mod;

        void dsdModulatorInit(dsd_modulator_t *mod, dsd_rate_t rate, uint32_t dither_seed);
        uint32_t xorshift32(uint32_t *state);
        double dsd_rate_to_hz(dsd_rate_t rate);
        double dsd_max_input_level(dsd_rate_t rate);
        double tpdf_sample(uint32_t *state);
        size_t dsd_output_bytes_for(int bits_pending, size_t num_samples);
        size_t dsd_modulator_process(dsd_modulator_t *mod, const double *pcm_in, size_t num_samples, uint8_t *out);
};

//-------------------------------------------------------------------------------------------

DSDModulatorClaude::DSDModulatorClaude()
{}

//-------------------------------------------------------------------------------------------

DSDModulatorClaude::~DSDModulatorClaude()
{}

//-------------------------------------------------------------------------------------------

bool DSDModulatorClaude::init(int rate)
{
    dsd_rate_t type = DSD_RATE_64;
    bool res = true;

    switch(rate)
    {
        case 64:
            type = DSD_RATE_64;
            break;
        case 128:
            type = DSD_RATE_128;
            break;
        case 256:
            type = DSD_RATE_256;
            break;
        case 512:
            type = DSD_RATE_512;
            break;
        case 1024:
            type = DSD_RATE_1024;
            break;
        default:
            res = false;
            break;
    }
    if(res)
    {
        dsdModulatorInit(&m_mod, type, 0);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void DSDModulatorClaude::process(const double *in, int noSamples, uint8_t *out)
{
    dsd_modulator_process(&m_mod, in, noSamples, out);
}

//-------------------------------------------------------------------------------------------

void DSDModulatorClaude::dsdModulatorInit(dsd_modulator_t *mod, dsd_rate_t rate, uint32_t dither_seed)
{
    memset(mod, 0, sizeof(*mod));
    mod->rate = rate;
    mod->order = RATE_INFO[rate].order;
    mod->rng_state = dither_seed ? dither_seed : 1u; /* xorshift32 needs nonzero state */
    mod->dither_amplitude = dither_seed ? DSD_DITHER_SCALE : 0.0;
    mod->bit_accum = 0;
    mod->bit_count = 0;
}

//-------------------------------------------------------------------------------------------
/* xorshift32 PRNG -- fast, deterministic, no external dependency. */
//-------------------------------------------------------------------------------------------

uint32_t DSDModulatorClaude::xorshift32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

//-------------------------------------------------------------------------------------------

double DSDModulatorClaude::dsd_rate_to_hz(dsd_rate_t rate)
{
    if(rate < 0 || rate >= DSD_RATE_COUNT) 
        return 0.0;
    return RATE_INFO[rate].hz;
}

//-------------------------------------------------------------------------------------------

double DSDModulatorClaude::dsd_max_input_level(dsd_rate_t rate)
{
    if(rate < 0 || rate >= DSD_RATE_COUNT)
        return 0.0;
    return RATE_INFO[rate].max_input_level;
}

//-------------------------------------------------------------------------------------------

double DSDModulatorClaude::tpdf_sample(uint32_t *state)
{
    double u1 = (double)(xorshift32(state) >> 8) * (1.0 / 16777216.0); /* [0,1) */
    double u2 = (double)(xorshift32(state) >> 8) * (1.0 / 16777216.0);
    return (u1 - u2); /* triangular, range (-1,1), zero mean */
}

//-------------------------------------------------------------------------------------------

size_t DSDModulatorClaude::dsd_output_bytes_for(int bits_pending, size_t num_samples)
{
    size_t total_bits = (size_t)bits_pending + num_samples;
    return total_bits / 8; /* whole bytes only; see dsd_modulator_flush for the remainder */
}

//-------------------------------------------------------------------------------------------

size_t DSDModulatorClaude::dsd_modulator_process(dsd_modulator_t *mod, const double *pcm_in, size_t num_samples, uint8_t *out)
{
    const int order = mod->order;
    const dsd_rate_info_t *info = &RATE_INFO[mod->rate];
    const double *b = info->b;
    const double *a = info->a;
    double in_hist[DSD_MAX_ORDER];
    double y_hist[DSD_MAX_ORDER];
    unsigned bit_accum = mod->bit_accum;
    int bit_count = mod->bit_count;
    size_t out_pos = 0;
    size_t n;
    int k;
    const double gain = info->max_input_level;

    memcpy(in_hist, mod->in_hist, sizeof(double) * order);
    memcpy(y_hist, mod->y_hist, sizeof(double) * order);

    for (n = 0; n < num_samples; n++) 
    {
        double x = pcm_in[n] * gain;
        double w = 0.0;
        double dither, v, in_new;
        unsigned bit;

        for(k = 0; k < order; k++) 
        {
            w += b[k] * in_hist[k] - a[k] * y_hist[k];
        }

        dither = mod->dither_amplitude != 0.0 ? mod->dither_amplitude * tpdf_sample(&mod->rng_state) : 0.0;

        bit = (w + dither >= 0.0) ? 1u : 0u;
        v = bit ? 1.0 : -1.0;
        in_new = x - v;

        /* shift delay lines (order is small: 4-8 taps, a loop is simplest
         * and the branch predicts perfectly since `order` is loop-invariant) */
        for(k = order - 1; k > 0; k--) 
        {
            in_hist[k] = in_hist[k - 1];
            y_hist[k] = y_hist[k - 1];
        }
        in_hist[0] = in_new;
        y_hist[0] = w;

        bit_accum = (bit_accum << 1) | bit;
        bit_count++;
        if(bit_count == 8) 
        {
            out[out_pos++] = (uint8_t)bit_accum;
            bit_accum = 0;
            bit_count = 0;
        }
    }

    memcpy(mod->in_hist, in_hist, sizeof(double) * order);
    memcpy(mod->y_hist, y_hist, sizeof(double) * order);
    mod->bit_accum = bit_accum;
    mod->bit_count = bit_count;

    return out_pos;
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

void dsd_write_codec_claude(engine::Codec *inCodec, const QString& outFilename, int DSD_Times)
{
    int devID = initCUDAOmega();
    ASSERT_TRUE(devID >= 0);

    ASSERT_EQ(inCodec->noChannels(), 2);

    if(common::DiskOps::exist(outFilename))
    {
        common::DiskOps::deleteDirectory(outFilename);
    }

    PCMUpscale upscaleL;
    ASSERT_TRUE(upscaleL.init(inCodec->frequency(), DSD_Times));
    PCMUpscale upscaleR;
    ASSERT_TRUE(upscaleR.init(inCodec->frequency(), DSD_Times));
    int inputBlockSize = upscaleL.noInputSamples();

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
    tuint64 noOutputBlocks = OrigDataSize / inputBlockSize;
    if(OrigDataSize % inputBlockSize)
    {
        noOutputBlocks++;
    }
    // No of output bytes in the DSD data.
    tuint64 DSD_DataSize = (noOutputBlocks * inputBlockSize * DSD_Times * inCodec->noChannels()) / 8;

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

    tfloat64 *inL = new tfloat64 [inputBlockSize];
    tfloat64 *inR = new tfloat64 [inputBlockSize];

    int outputLen = upscaleL.noOutputSamples() / 8;
    uint8_t *outL = new uint8_t [outputLen];
    uint8_t *outR = new uint8_t [outputLen];
    uint8_t *out = new uint8_t [outputLen * 2];

    DSDModulatorClaude modL,modR;
    ASSERT_TRUE(modL.init(DSD_Times));
    ASSERT_TRUE(modR.init(DSD_Times));

    engine::RData data(inputBlockSize, inCodec->noChannels(), inCodec->noChannels());

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
            while(idx < inputBlockSize)
            {
                inL[idx] = 0.0;
                inR[idx] = 0.0;
                idx++;
            }

            int outLenL, outLenR;
            QSharedPointer<double> upLPtr = upscaleL.upscale(inL, inputBlockSize, outLenL);
            ASSERT_FALSE(upLPtr.isNull());
            ASSERT_EQ(outLenL / 8, outputLen);
            QSharedPointer<double> upRPtr = upscaleR.upscale(inR, inputBlockSize, outLenR);
            ASSERT_FALSE(upRPtr.isNull());
            ASSERT_EQ(outLenR / 8, outputLen);

            modL.process(upLPtr.get(), outLenL, outL);
            modR.process(upLPtr.get(), outLenR, outR);

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

TEST(PCM2DSDRevClaude, convertDSD128)
{
    QString inFilename = "D:\\Development\\Temp\\dsd\\ironfoot.m4a";
    QString outFilename = "D:\\Development\\Temp\\dsd\\ironfoot_dsd128_2.dff";

    engine::Codec *codec = engine::Codec::get(inFilename);
    ASSERT_FALSE(codec == NULL);
    ASSERT_TRUE(codec->init());

    dsd_write_codec_claude(codec, outFilename, 128);

    delete codec;
}

//-------------------------------------------------------------------------------------------
