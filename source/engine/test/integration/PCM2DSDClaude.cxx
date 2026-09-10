#include "gtest/gtest.h"

#include "common/inc/DiskOps.h"
#include "engine/inc/Codec.h"
#include "engine/inc/RData.h"
#include "engine/inc/PCMToDSD.h"
#include "engine/inc/FormatType.h"

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
// DSDModulatorGrok
//-------------------------------------------------------------------------------------------

#define DSD_MOD_MAX_ORDER 8

class DSDModulatorGrok
{
    public:
        typedef enum {
            DSD_RATE_64   = 64,    /*  2.8224 MHz  (64  × 44.1 kHz) */
            DSD_RATE_128  = 128,   /*  5.6448 MHz                  */
            DSD_RATE_256  = 256,   /* 11.2896 MHz                  */
            DSD_RATE_512  = 512,   /* 22.5792 MHz                  */
            DSD_RATE_1024 = 1024   /* 45.1584 MHz                  */
        } dsd_rate_t;

        typedef struct dsd_modulator {
            int      order;
            dsd_rate_t rate;
            double   a[DSD_MOD_MAX_ORDER];   /* distributed feedback (CRFB) */
            double   g[DSD_MOD_MAX_ORDER];   /* resonator gains (NTF zeros) */
            double   state[DSD_MOD_MAX_ORDER];
            double   clip;                   /* integrator clip level       */
            double   in_limit;               /* soft input limiter          */
            uint8_t  bit_acc;                /* pending packed bits         */
            int      bit_count;              /* 0…7 bits already in acc     */
        } dsd_modulator_t;

        typedef struct {
            int         order;
            dsd_rate_t  rate;
            double      a[DSD_MOD_MAX_ORDER];
            double      g[DSD_MOD_MAX_ORDER];
            double      clip;
            double      in_limit;
        } ntf_proto_t;

    public:
        DSDModulatorGrok();
        virtual ~DSDModulatorGrok();

        bool init(int rate);
        void process(const double *in, int noSamples, uint8_t *out);

    private:

        static constexpr ntf_proto_t k_dsd64_clans7 = {
            /* DSD64, 7th-order CLANS.  Best quality that stays stable with
            * integrator clipping on typical music.  ~120 dB+ in-band. */
            7, DSD_RATE_64,
            { 1.30828743581024e+00,
            6.14252690035661e-01,
            1.30284958810903e-01,
            1.31280998331490e-02,
            4.80497172614556e-04,
            1.28747977598542e-07,
            -1.01500259908072e-06 },
            { 0.0,
            3.96825873999969e-04,
            0.0,
            1.32436089566069e-03,
            0.0,
            2.16898568341885e-03 },
            4.0, 0.55
        };

        static constexpr ntf_proto_t k_dsd128_clans7 = {
            7, DSD_RATE_128,
            { 8.98180853333862e-01,
            3.27985497323439e-01,
            6.38803466871112e-02,
            7.18262647412857e-03,
            4.51845004995476e-04,
            1.49685651672331e-05,
            4.22554681245302e-08 },
            { 0.0,
            9.92163123766340e-05,
            0.0,
            3.31199917300393e-04,
            0.0,
            5.42540771343282e-04 },
            4.0, 0.60
        };

        static constexpr ntf_proto_t k_dsd256_clans6 = {
            6, DSD_RATE_256,
            { 9.97000121097967e-01,
            3.46002867430604e-01,
            5.74352078895161e-02,
            4.96197900435677e-03,
            2.16319301330580e-04,
            3.45938007947910e-06 },
            { 8.57500543083848e-06,
            0.0,
            6.58398680532347e-05,
            0.0,
            1.30939362595793e-04,
            0.0 },
            4.0, 0.65
        };

        /* DSD256 5th-order CLANS — prototype for 512 / 1024 (g scaled). */
        static constexpr ntf_proto_t k_dsd256_clans5 = {
            5, DSD_RATE_256,
            { 1.10212073518628e+00,
            4.33447134954244e-01,
            7.17865111532609e-02,
            4.48367825425951e-03,
            8.60861641068938e-05 },
            { 0.0,
            4.36651951230006e-05,
            0.0,
            1.23660417994961e-04 },
            4.0, 0.70
        };

        dsd_modulator_t m_mod;

        void load_proto(dsd_modulator_t *m, const ntf_proto_t *p, double g_scale);
        int dsd_modulator_init(dsd_modulator_t *m, dsd_rate_t rate);
        double sat(double x, double lim);
        double crfb_step(dsd_modulator_t *m, double x);
        size_t dsd_modulator_process(dsd_modulator_t *m, const double *pcm, size_t n_in, uint8_t *dsd_out);
};

DSDModulatorGrok::DSDModulatorGrok()
{}

DSDModulatorGrok::~DSDModulatorGrok()
{}

bool DSDModulatorGrok::init(int rate)
{
    return (dsd_modulator_init(&m_mod, static_cast<dsd_rate_t>(rate)) == 0) ? true : false;
}

void DSDModulatorGrok::process(const double *in, int noSamples, uint8_t *out)
{
    dsd_modulator_process(&m_mod, in, noSamples, out);
}

void DSDModulatorGrok::load_proto(dsd_modulator_t *m, const ntf_proto_t *p, double g_scale)
{
    int i;
    memset(m, 0, sizeof(*m));
    m->order    = p->order;
    m->rate     = p->rate;
    m->clip     = p->clip;
    m->in_limit = p->in_limit;
    for (i = 0; i < p->order; i++) {
        m->a[i] = p->a[i];
        m->g[i] = p->g[i] * g_scale;
    }
}

int DSDModulatorGrok::dsd_modulator_init(dsd_modulator_t *m, dsd_rate_t rate)
{
    if (!m)
        return -1;

    switch (rate) {
    case DSD_RATE_64:
        load_proto(m, &k_dsd64_clans7, 1.0);
        m->rate = DSD_RATE_64;
        return 0;
    case DSD_RATE_128:
        load_proto(m, &k_dsd128_clans7, 1.0);
        m->rate = DSD_RATE_128;
        return 0;
    case DSD_RATE_256:
        load_proto(m, &k_dsd256_clans6, 1.0);
        m->rate = DSD_RATE_256;
        return 0;
    case DSD_RATE_512:
        /* analog zeros held; g ∝ 1/R²  →  scale from R=256 by 1/4 */
        load_proto(m, &k_dsd256_clans5, 0.25);
        m->rate     = DSD_RATE_512;
        m->in_limit = 0.72;
        return 0;
    case DSD_RATE_1024:
        load_proto(m, &k_dsd256_clans5, 0.0625); /* 1/16 */
        m->rate     = DSD_RATE_1024;
        m->in_limit = 0.75;
        return 0;
    default:
        return -1;
    }
}

double DSDModulatorGrok::sat(double x, double lim)
{
    if (x >  lim) return  lim;
    if (x < -lim) return -lim;
    return x;
}

/*
 * One CRFB step.  Returns the 1-bit decision as +1.0 or −1.0.
 * `s` is the integrator state (length = order), updated in place.
 */
double DSDModulatorGrok::crfb_step(dsd_modulator_t *m, double x)
{
    const int     n = m->order;
    const double *a = m->a;
    const double *g = m->g;
    double       *s = m->state;
    double        d[DSD_MOD_MAX_ORDER];
    double        v;
    double        y;
    int           i;

    x = sat(x, m->in_limit);

    /* First integrator + input summing node (includes −y from *previous*
     * decision: the caller has already subtracted y from s[0] via the
     * update of the previous step).  We fold (x − y) into d[0] after
     * the new decision is known; here we first form the pre-quantiser
     * value using a trial with the implicit last y already in the
     * state.  Equivalent SoX form uses explicit y. */

    /* Reconstruct last y from the last update is unnecessary if we
     * apply feedback here.  Keep last_y implicit: we pass y of *this*
     * sample after deciding it.  Compute loop-filter output assuming
     * current y is applied this step (standard delay-free CRFB with
     * NTF leading coefficient 1). */

    /* Pre-feedback filter state advance with y = 0, then correct. */
    d[0] = s[0] - g[0] * ((n > 1) ? s[1] : 0.0) + x;
    v    = x + a[0] * d[0];

    for (i = 1; i < n - 1; i++) {
        d[i] = s[i] + s[i - 1] - g[i] * s[i + 1];
        v   += a[i] * d[i];
    }
    if (n >= 2) {
        d[n - 1] = s[n - 1] + s[n - 2];
        v       += a[n - 1] * d[n - 1];
    }

    y = (v >= 0.0) ? 1.0 : -1.0;

    /* Apply the 1-bit DAC feedback to the first summing node. */
    d[0] -= y;

    for (i = 0; i < n; i++)
        s[i] = sat(d[i], m->clip);

    return y;
}

size_t DSDModulatorGrok::dsd_modulator_process(dsd_modulator_t *m,
                             const double    *pcm,
                             size_t           n_in,
                             uint8_t         *dsd_out)
{
    size_t n_out = 0;
    size_t i;

    if (!m || !pcm || !dsd_out)
        return 0;

    for (i = 0; i < n_in; i++) {
        double y = crfb_step(m, pcm[i]);
        /* MSB-first: first sample occupies bit 7. */
        m->bit_acc = (uint8_t)((m->bit_acc << 1) | (y > 0.0 ? 1u : 0u));
        m->bit_count++;
        if (m->bit_count == 8) {
            dsd_out[n_out++] = m->bit_acc;
            m->bit_acc   = 0;
            m->bit_count = 0;
        }
    }
    return n_out;
}


//-------------------------------------------------------------------------------------------
// DSMModulatorClaudeV2
//-------------------------------------------------------------------------------------------

#define DSM_ORDER 5
#define DSM_STATE_CLAMP 100.0

class DSMModulatorClaudeV2
{
    public:
        typedef struct {
            double x[DSM_ORDER];
        } dsm_state_t;

    public:
        DSMModulatorClaudeV2();

        bool init(int rate);
        void process(const double *in, int noSamples, uint8_t *out);

    private:
        static constexpr double DSM_A[DSM_ORDER][DSM_ORDER] = {
            { 1.0000000000,  0.0000000000,  0.0000000000, 0.0000000000,  0.0000000000 },
            { 1.0000000000,  1.0000000000, -0.0005747563, 0.0000000000,  0.0000000000 },
            { 1.0000000000,  1.0000000000,  0.9994252437, 0.0000000000,  0.0000000000 },
            { 0.0000000000,  0.0000000000,  1.0000000000, 1.0000000000, -0.0016276180 },
            { 0.0000000000,  0.0000000000,  1.0000000000, 1.0000000000,  0.9983723820 },
        };

        static constexpr double DSM_BU[DSM_ORDER] = {
            0.0000078473, 0.0002366273, 0.0043372764, 0.0432892549, 0.3159131899
        };

        static constexpr double DSM_BV[DSM_ORDER] = {
            -0.0000078473, -0.0002366273, -0.0043372764, -0.0432892549, -0.3159131899
        };

        static constexpr double DSM_C[DSM_ORDER] = {
            0.0000000000, 0.0000000000, 0.0000000000, 0.0000000000, 1.0000000000
        };

        static constexpr double DSM_D = 1.0000000000;

        dsm_state_t m_state;

        void dsm_init(dsm_state_t *st);
        void dsm_process(dsm_state_t *st, const double *pcm_in, size_t n_samples, uint8_t *dsd_out);
};

DSMModulatorClaudeV2::DSMModulatorClaudeV2()
{}

void DSMModulatorClaudeV2::dsm_init(dsm_state_t *st)
{
    memset(st->x, 0, sizeof(st->x));
}

bool DSMModulatorClaudeV2::init(int rate)
{
    dsm_init(&m_state);
    return (rate == 64) ? true : false;
}

void DSMModulatorClaudeV2::process(const double *in, int noSamples, uint8_t *out)
{
    dsm_process(&m_state, in, noSamples, out);
}

/*
 * Run the modulator over `n_samples` of PCM input (already upsampled to the
 * 2.8224 MHz DSD64 rate, one double per output bit). Scale the input to at
 * most 0.75 of full scale before calling this -- see the header note.
 *
 * Writes ceil(n_samples / 8) bytes to `dsd_out`, MSB-first: the first
 * output sample becomes bit 7 of dsd_out[0], the second bit 6, and so on.
 * If n_samples is not a multiple of 8, the unused low bits of the final
 * byte are left as 0.
 *
 * `st` carries the loop-filter state across calls, so a PCM buffer may be
 * streamed through in chunks; pass the same (initialized) dsm_state_t each
 * time to continue the same modulator run.
 */
void DSMModulatorClaudeV2::dsm_process(dsm_state_t *st, const double *pcm_in, size_t n_samples, uint8_t *dsd_out)
{
    double x[DSM_ORDER];
    memcpy(x, st->x, sizeof(x));

    size_t out_bytes = (n_samples + 7) / 8;
    memset(dsd_out, 0, out_bytes);

    for (size_t n = 0; n < n_samples; n++) {
        double u = pcm_in[n] * 0.5;

        double y = DSM_D * u;
        for (int k = 0; k < DSM_ORDER; k++) {
            y += DSM_C[k] * x[k];
        }

        int bit = (y >= 0.0) ? 1 : 0;
        double v = bit ? 1.0 : -1.0;

        if (bit) {
            dsd_out[n >> 3] |= (uint8_t)(0x80u >> (n & 7u));
        }

        double x_next[DSM_ORDER];
        for (int i = 0; i < DSM_ORDER; i++) {
            double acc = DSM_BU[i] * u + DSM_BV[i] * v;
            for (int k = 0; k < DSM_ORDER; k++) {
                acc += DSM_A[i][k] * x[k];
            }
            if (acc > DSM_STATE_CLAMP) acc = DSM_STATE_CLAMP;
            if (acc < -DSM_STATE_CLAMP) acc = -DSM_STATE_CLAMP;
            x_next[i] = acc;
        }
        memcpy(x, x_next, sizeof(x));
    }

    memcpy(st->x, x, sizeof(x));
}


//-------------------------------------------------------------------------------------------
// DSDModulatorSoxTrellis
//-------------------------------------------------------------------------------------------

#define SOX_INT_MIN(bits) (1 <<((bits)-1))
#define SOX_INT_MAX(bits) (((unsigned)-1)>>(33-(bits)))
#define SOX_SAMPLE_MAX (sox_sample_t)SOX_INT_MAX(32)
#define SOX_SAMPLE_MIN (sox_sample_t)SOX_INT_MIN(32)

#define MAX_FILTER_ORDER 8
#define PATH_HASH_SIZE 128
#define PATH_HASH_MASK (PATH_HASH_SIZE - 1)

#define SDM_TRELLIS_MAX_ORDER 32
#define SDM_TRELLIS_MAX_NUM   32
#define SDM_TRELLIS_MAX_LAT   2048

class DSDModulatorSoxTrellis
{
    public:

        typedef int32_t sox_sample_t;

        typedef struct sdm_filter {
            const double  a[MAX_FILTER_ORDER];
            const double  g[MAX_FILTER_ORDER];
            int32_t       order;
            unsigned      freq;
            const char   *name;
            int           trellis_order;
            int           trellis_num;
            int           trellis_lat;
        } sdm_filter_t;

        typedef struct sdm_state {
            double        state[MAX_FILTER_ORDER];
            double        cost;
            uint32_t      path;
            uint8_t       next;
            uint8_t       hist;
            uint8_t       hist_used;
            struct sdm_state *parent;
            struct sdm_state *path_list;
        } sdm_state_t;

        typedef struct {
            sdm_state_t   sdm[2 * SDM_TRELLIS_MAX_NUM];
            sdm_state_t  *act[SDM_TRELLIS_MAX_NUM];
        } sdm_trellis_t;

        typedef struct sdm {
            sdm_trellis_t trellis[2];
            sdm_state_t  *path_hash[PATH_HASH_SIZE];
            uint8_t       hist_free[2 * SDM_TRELLIS_MAX_NUM];
            unsigned      hist_fnum;
            uint32_t      trellis_mask;
            uint32_t      trellis_num;
            uint32_t      trellis_lat;
            unsigned      num_cands;
            unsigned      pos;
            unsigned      pending;
            unsigned      draining;
            unsigned      idx;
            const sdm_filter_t *filter;
            double        prev_y;
            uint64_t      conv_fail;
            uint8_t       hist[2 * SDM_TRELLIS_MAX_NUM][SDM_TRELLIS_MAX_LAT / 8];
        } sdm_t;

        enum sox_error_t {
            SOX_SUCCESS = 0,     /**< Function succeeded = 0 */
            SOX_EOF = -1,        /**< End Of File or other error = -1 */
            SOX_EHDR = 2000,     /**< Invalid Audio Header = 2000 */
            SOX_EFMT,            /**< Unsupported data format = 2001 */
            SOX_ENOMEM,          /**< Can't alloc memory = 2002 */
            SOX_EPERM,           /**< Operation not permitted = 2003 */
            SOX_ENOTSUP,         /**< Operation not supported = 2004 */
            SOX_EINVAL           /**< Invalid argument = 2005 */
        };

    public:
        DSDModulatorSoxTrellis() {}

        bool init(int rate);
        int process(const double *in, int noSamples, uint8_t *out);

    private:
        
        sdm_t *m_mod;

        sdm_filter_t *sdm_find_filter(const char *name, unsigned freq);
        double sdm_filter_calc(const double *s, double *d, const sdm_filter_t *f, double x, double y);
        void sdm_filter_calc2(sdm_state_t *src, sdm_state_t *dst, const sdm_filter_t *f, double x);

        unsigned sdm_histbuf_get(sdm_t *p);
        void sdm_histbuf_put(sdm_t *p, unsigned h);
        unsigned get_bit(uint8_t *p, unsigned i);
        void put_bit(uint8_t *p, unsigned i, unsigned v);
        unsigned sdm_hist_get(sdm_t *p, unsigned h, unsigned i);
        void sdm_hist_put(sdm_t *p, unsigned h, unsigned i, unsigned v);
        void sdm_hist_copy(sdm_t *p, unsigned d, unsigned s);
        int64_t dbl2int64(double a);
        int sdm_cmplt(sdm_state_t *a, sdm_state_t *b);
        int sdm_cmple(sdm_state_t *a, sdm_state_t *b);

        sdm_state_t *sdm_check_path(sdm_t *p, sdm_state_t *s);
        unsigned sdm_sort_cands(sdm_t *p, sdm_trellis_t *st);
        void sdm_step(sdm_t *p, sdm_state_t *cur, sdm_state_t *next, double x);

        sox_sample_t sdm_sample_trellis(sdm_t *p, double x);
        sox_sample_t sdm_sample(sdm_t *p, double x);
        int sdm_process(sdm_t *p, const double *ibuf, sox_sample_t *obuf, size_t *ilen, size_t *olen);
        int sdm_drain(sdm_t *p, sox_sample_t *obuf, size_t *olen);

        sdm_t *sdm_init(const char *filter_name, unsigned freq, unsigned trellis_order, unsigned trellis_num, unsigned trellis_latency);
        void sdm_close(sdm_t *p);

    private:
 
        static constexpr sdm_filter_t sdm_filters[] = {
        {
            {
                1.00323940832478e+00,
                3.54975562370606e-01,
                5.64754047673194e-02,
                3.99067228430322e-03,
            },
            {
                1.74071110561285e-05, 0,
                1.11672812199443e-04, 0,
            },
            4,
            256 * 44100,
            "clans-4",
            0, 0, 0
        },
        {
            {
                8.69746397840960e-01,
                3.58080546314756e-01,
                8.02654082306273e-02,
                8.06528716282692e-03,
            },
            {
                1.74071110561285e-05, 0,
                1.11672812199443e-04, 0,
            },
            4,
            256 * 44100,
            "sdm-4",
            0, 0, 0
        },
        {
            {
                1.10212073518628e+00,
                4.33447134954244e-01,
                7.17865111532609e-02,
                4.48367825425951e-03,
                8.60861641068938e-05,
            },
            {
                0, 4.36651951230006e-05,
                0, 1.23660417994961e-04,
            },
            5,
            256 * 44100,
            "clans-5",
            0, 0, 0
        },
        {
            {
                8.07768375734983e-01,
                3.16440095967511e-01,
                7.38231738259889e-02,
                1.01432044963374e-02,
                6.46658652275506e-04,
            },
            {
                0, 4.36651951230006e-05,
                0, 1.23660417994961e-04,
            },
            5,
            256 * 44100,
            "sdm-5",
            0, 0, 0
        },
        {
            {
                9.97000121097967e-01,
                3.46002867430604e-01,
                5.74352078895161e-02,
                4.96197900435677e-03,
                2.16319301330580e-04,
                3.45938007947910e-06,
            },
            {
                8.57500543083848e-06, 0,
                6.58398680532347e-05, 0,
                1.30939362595793e-04, 0,
            },
            6,
            256 * 44100,
            "clans-6",
            0, 0, 0
        },
        {
            {
                8.08851952379691e-01,
                3.20414766828429e-01,
                7.85858596284593e-02,
                1.24781319607895e-02,
                1.21202847406105e-03,
                5.51622876557856e-05,
            },
            {
                8.57500543083848e-06, 0,
                6.58398680532347e-05, 0,
                1.30939362595793e-04, 0,
            },
            6,
            256 * 44100,
            "sdm-6",
            0, 0, 0
        },
        {
            {
                1.10629931445134e+00,
                4.22135693734657e-01,
                7.54595882135669e-02,
                7.07164815703843e-03,
                3.53092575577382e-04,
                8.89662856104825e-06,
                5.79674109824069e-08,
            },
            {
                0, 2.48046933669715e-05,
                0, 8.28068362972358e-05,
                0, 1.35653594733585e-04,
            },
            7,
            256 * 44100,
            "clans-7",
            0, 0, 0
        },
        {
            {
                7.82785077952658e-01,
                3.01888671316811e-01,
                7.36594376027782e-02,
                1.22068270909817e-02,
                1.36572694403914e-03,
                9.57806082134936e-05,
                3.13239368043838e-06,
            },
            {
                0, 2.48046933669715e-05,
                0, 8.28068362972358e-05,
                0, 1.35653594733585e-04,
            },
            7,
            256 * 44100,
            "sdm-7",
            0, 0, 0
        },
        {
            {
                1.15188624720851e+00,
                5.45054196257555e-01,
                1.38703640845632e-01,
                2.07076444822072e-02,
                1.85506614417771e-03,
                9.63403135615390e-05,
                2.69174565706992e-06,
                2.22594461751768e-08,
            },
            {
                5.06749566262594e-06, 0,
                4.15924517416912e-05, 0,
                9.55783346944871e-05, 0,
                1.38868728742641e-04, 0,
            },
            8,
            256 * 44100,
            "clans-8",
            0, 0, 0
        },
        {
            {
                7.42329617949054e-01,
                2.72509195471757e-01,
                6.41424039739473e-02,
                1.05299412132258e-02,
                1.23178223428228e-03,
                9.94985029720342e-05,
                5.13169547054423e-06,
                1.20466411041020e-07,
            },
            {
                5.06749566262594e-06, 0,
                4.15924517416912e-05, 0,
                9.55783346944871e-05, 0,
                1.38868728742641e-04, 0,
            },
            8,
            256 * 44100,
            "sdm-8",
            0, 0, 0
        },
        {
            {
                1.19985242167687e+00,
                5.39366678861047e-01,
                1.07433710905069e-01,
                7.85649993434925e-03,
            },
            {
                6.96272321944526e-05, 0,
                4.46641365529834e-04, 0,
            },
            4,
            128 * 44100,
            "clans-4",
            0, 0, 0
        },
        {
            {
                8.69935494013007e-01,
                3.57844753369190e-01,
                8.00232187246903e-02,
                7.95176796646842e-03,
            },
            {
                6.96272321944526e-05, 0,
                4.46641365529834e-04, 0,
            },
            4,
            128 * 44100,
            "sdm-4",
            0, 0, 0
        },
        {
            {
                1.12849522129362e+00,
                5.02128177800632e-01,
                1.10084368682902e-01,
                1.18635667860902e-02,
                4.71059243536326e-04,
            },
            {
                0, 1.74653153894942e-04,
                0, 4.94580504383930e-04,
            },
            5,
            128 * 44100,
            "clans-5",
            0, 0, 0
        },
        {
            {
                8.08016362125685e-01,
                3.16129639744972e-01,
                7.34835047943110e-02,
                1.00377576971692e-02,
                6.20309683440734e-04,
            },
            {
                0, 1.74653153894942e-04,
                0, 4.94580504383930e-04,
            },
            5,
            128 * 44100,
            "sdm-5",
            0, 0, 0
        },
        {
            {
                1.13839804508630e+00,
                5.16338264778321e-01,
                1.20760874713903e-01,
                1.53496744585395e-02,
                1.00733946588732e-03,
                2.18223963130981e-05,
            },
            {
                3.42997276004814e-05, 0,
                2.63342132660038e-04, 0,
                5.23688869916463e-04, 0,
            },
            6,
            128 * 44100,
            "clans-6",
            0, 0, 0
        },
        {
            {
                8.09157514480151e-01,
                3.20038611545599e-01,
                7.81955723892726e-02,
                1.23074728674017e-02,
                1.18346416730106e-03,
                5.04301224894810e-05,
            },
            {
                3.42997276004814e-05, 0,
                2.63342132660038e-04, 0,
                5.23688869916463e-04, 0,
            },
            6,
            128 * 44100,
            "sdm-6",
            0, 0, 0
        },
        {
            {
                8.98180853333862e-01,
                3.27985497323439e-01,
                6.38803466871112e-02,
                7.18262647412857e-03,
                4.51845004995476e-04,
                1.49685651672331e-05,
                4.22554681245302e-08,
            },
            {
                0, 9.92163123766340e-05,
                0, 3.31199917300393e-04,
                0, 5.42540771343282e-04,
            },
            7,
            128 * 44100,
            "clans-7",
            0, 0, 0
        },
        {
            {
                7.83148010097334e-01,
                3.01437231238902e-01,
                7.31891646224574e-02,
                1.20314098366875e-02,
                1.32077937193861e-03,
                9.11181979169687e-05,
                2.59895240306562e-06,
            },
            {
                0, 9.92163123766340e-05,
                0, 3.31199917300393e-04,
                0, 5.42540771343282e-04,
            },
            7,
            128 * 44100,
            "sdm-7",
            0, 0, 0
        },
        {
            {
                1.04472698053970e+00,
                4.62088167600438e-01,
                1.13484722685479e-01,
                1.68939738398161e-02,
                1.55891676875336e-03,
                8.23864822188133e-05,
                2.39690238375972e-06,
                -1.75063180618551e-09,
            },
            {
                2.02698799324546e-05, 0,
                1.66362887238597e-04, 0,
                3.82276797905696e-04, 0,
                5.55397776875272e-04, 0,
            },
            8,
            128 * 44100,
            "clans-8",
            0, 0, 0
        },
        {
            {
                7.42763211426562e-01,
                2.71983157679393e-01,
                6.36389361390464e-02,
                1.03289230528372e-02,
                1.19045645863092e-03,
                9.25357160397986e-05,
                4.64982367004083e-06,
                8.14280266547840e-08,
            },
            {
                2.02698799324546e-05, 0,
                1.66362887238597e-04, 0,
                3.82276797905696e-04, 0,
                5.55397776875272e-04, 0,
            },
            8,
            128 * 44100,
            "sdm-8",
            0, 0, 0
        },
        {
            {
                1.27879853057675e+00,
                6.11303913722028e-01,
                1.28497083869344e-01,
                9.36669621421730e-03,
            },
            {
                2.78489536971958e-04, 0,
                1.78576750808173e-03, 0,
            },
            4,
            64 * 44100,
            "clans-4",
            0, 0, 0
        },
        {
            {
                8.70691905361989e-01,
                3.56902669565715e-01,
                7.90540396115068e-02,
                7.49922172520510e-03,
            },
            {
                2.78489536971958e-04, 0,
                1.78576750808173e-03, 0,
            },
            4,
            64 * 44100,
            "sdm-4",
            0, 0, 0
        },
        {
            {
                1.09979653514762e+00,
                4.81149952106030e-01,
                1.03481231987752e-01,
                1.07520561970131e-02,
                3.08801118488355e-04,
            },
            {
                0, 6.98490600683106e-04,
                0, 1.97734357803445e-03,
            },
            5,
            64 * 44100,
            "clans-5",
            0, 0, 0
        },
        {
            {
                8.09008352716413e-01,
                3.14889441429587e-01,
                7.21235639855639e-02,
                9.61769014140330e-03,
                5.16726747047100e-04,
            },
            {
                0, 6.98490600683106e-04,
                0, 1.97734357803445e-03,
            },
            5,
            64 * 44100,
            "sdm-5",
            0, 0, 0
        },
        {
            {
                1.07903996429881e+00,
                4.81889508657128e-01,
                1.12960470418260e-01,
                1.41786764681378e-02,
                8.90696638761455e-04,
                3.12209321540191e-06,
            },
            {
                1.37194204516672e-04, 0,
                1.05309113432481e-03, 0,
                2.09365847953595e-03, 0,
            },
            6,
            64 * 44100,
            "clans-6",
            0, 0, 0
        },
        {
            {
                8.10379824203071e-01,
                3.18536209193388e-01,
                7.66325098232035e-02,
                1.16280270347611e-02,
                1.07113013239551e-03,
                3.23564051386283e-05,
            },
            {
                1.37194204516672e-04, 0,
                1.05309113432481e-03, 0,
                2.09365847953595e-03, 0,
            },
            6,
            64 * 44100,
            "sdm-6",
            0, 0, 0
        },
        {
            {
                1.30828743581024e+00,
                6.14252690035661e-01,
                1.30284958810903e-01,
                1.31280998331490e-02,
                4.80497172614556e-04,
                1.28747977598542e-07,
                -1.01500259908072e-06,
            },
            {
                0, 3.96825873999969e-04,
                0, 1.32436089566069e-03,
                0, 2.16898568341885e-03,
            },
            7,
            64 * 44100,
            "clans-7",
            0, 0, 0
        },
        {
            {
                7.84599817960974e-01,
                2.99634346028983e-01,
                7.13049276218066e-02,
                1.13334107086916e-02,
                1.14497642818158e-03,
                7.33018502803093e-05,
                6.80633400002018e-07,
            },
            {
                0, 3.96825873999969e-04,
                0, 1.32436089566069e-03,
                0, 2.16898568341885e-03,
            },
            7,
            64 * 44100,
            "sdm-7",
            0, 0, 0
        },
        {
            {
                1.18730059129261e+00,
                5.66733317291325e-01,
                1.40117339676942e-01,
                1.87599862200771e-02,
                1.27685506908071e-03,
                8.76397405988154e-06,
                -1.90294986721073e-06,
                -7.39020160622772e-08,
            },
            {
                8.10778762576884e-05, 0,
                6.65340842513387e-04, 0,
                1.52852264942192e-03, 0,
                2.22035724073886e-03, 0,
            },
            8,
            64 * 44100,
            "clans-8",
            0, 0, 0
        },
        {
            {
                7.44453769826547e-01,
                2.69850507860307e-01,
                6.16093616071757e-02,
                9.52771711245796e-03,
                1.02903114196526e-03,
                6.63758229311911e-05,
                2.91124056073927e-06,
                -4.29323230577427e-08,
            },
            {
                8.10778762576884e-05, 0,
                6.65340842513387e-04, 0,
                1.52852264942192e-03, 0,
                2.22035724073886e-03, 0,
            },
            8,
            64 * 44100,
            "sdm-8",
            0, 0, 0
        }, };

    
};

#define sqr(a) ((a) * (a))
#define array_length(a) (sizeof(a)/sizeof(a[0]))
#define min(a, b) ((a) <= (b) ? (a) : (b))

DSDModulatorSoxTrellis::sdm_filter_t *DSDModulatorSoxTrellis::sdm_find_filter(const char *name, unsigned freq)
{
  unsigned i;

  for (i = 0; i < array_length(sdm_filters); i++)
    if (!name || !strcmp(name, sdm_filters[i].name))
      if (sdm_filters[i].freq <= freq)
        return (sdm_filter_t *)&sdm_filters[i];

  return NULL;
}

double DSDModulatorSoxTrellis::sdm_filter_calc(const double *s, double *d, const sdm_filter_t *f, double x, double y)
{
  const double *a = f->a;
  const double *g = f->g;
  double v;
  int i;

  d[0] = s[0] - g[0] * s[1] + x - y;
  v = x + a[0] * d[0];

  for (i = 1; i < f->order - 1; i++) {
    d[i] = s[i] + s[i - 1] - g[i] * s[i + 1];
    v += a[i] * d[i];
  }

  d[i] = s[i] + s[i - 1];
  v += a[i] * d[i];

  return v;
}

void DSDModulatorSoxTrellis::sdm_filter_calc2(sdm_state_t *src, sdm_state_t *dst, const sdm_filter_t *f, double x)
{
  const double *a = f->a;
  double v;
  int i;

  v = sdm_filter_calc(src->state, dst[0].state, f, x, 0.0);

  for (i = 0; i < f->order; i++)
    dst[1].state[i] = dst[0].state[i];

  dst[0].state[0] += 1.0;
  dst[1].state[0] -= 1.0;

  dst[0].cost = src->cost + sqr(v + a[0]);
  dst[1].cost = src->cost + sqr(v - a[0]);
}


inline unsigned DSDModulatorSoxTrellis::sdm_histbuf_get(sdm_t *p)
{
  return p->hist_free[--p->hist_fnum];
}

inline void DSDModulatorSoxTrellis::sdm_histbuf_put(sdm_t *p, unsigned h)
{
  p->hist_free[p->hist_fnum++] = h;
}

inline unsigned DSDModulatorSoxTrellis::get_bit(uint8_t *p, unsigned i)
{
  return (p[i >> 3] >> (i & 7)) & 1;
}

inline void DSDModulatorSoxTrellis::put_bit(uint8_t *p, unsigned i, unsigned v)
{
  int b = p[i >> 3];
  int s = i & 7;
  b &= ~(1 << s);
  b |= v << s;
  p[i >> 3] = b;
}

inline unsigned DSDModulatorSoxTrellis::sdm_hist_get(sdm_t *p, unsigned h, unsigned i)
{
  return get_bit(p->hist[h], i);
}

inline void DSDModulatorSoxTrellis::sdm_hist_put(sdm_t *p, unsigned h, unsigned i, unsigned v)
{
  put_bit(p->hist[h], i, v);
}

inline void DSDModulatorSoxTrellis::sdm_hist_copy(sdm_t *p, unsigned d, unsigned s)
{
  memcpy(p->hist[d], p->hist[s], (size_t)(p->trellis_lat + 7) / 8);
}

inline int64_t DSDModulatorSoxTrellis::dbl2int64(double a)
{
  union { double d; int64_t i; } v;
  v.d = a;
  return v.i;
}

inline int DSDModulatorSoxTrellis::sdm_cmplt(sdm_state_t *a, sdm_state_t *b)
{
  return dbl2int64(a->cost) < dbl2int64(b->cost);
}

inline int DSDModulatorSoxTrellis::sdm_cmple(sdm_state_t *a, sdm_state_t *b)
{
  return dbl2int64(a->cost) <= dbl2int64(b->cost);
}

DSDModulatorSoxTrellis::sdm_state_t *DSDModulatorSoxTrellis::sdm_check_path(sdm_t *p, sdm_state_t *s)
{
  unsigned index = s->path & PATH_HASH_MASK;
  sdm_state_t **hash = p->path_hash;
  sdm_state_t *t = hash[index];

  while (t) {
    if (t->path == s->path)
      return t;
    t = t->path_list;
  }

  s->path_list = hash[index];
  hash[index] = s;

  return NULL;
}

unsigned DSDModulatorSoxTrellis::sdm_sort_cands(sdm_t *p, sdm_trellis_t *st)
{
  sdm_state_t *r, *s, *t;
  sdm_state_t *min;
  unsigned i, j, n;

  for (i = 0; i < 2 * p->num_cands; i++) {
    s = &st->sdm[i];
    p->path_hash[s->path & PATH_HASH_MASK] = NULL;
    if (!i || sdm_cmplt(s, min))
      min = s;
  }

  for (i = 0, n = 0; i < 2 * p->num_cands; i++) {
    s = &st->sdm[i];

    if (s->next != min->next)
      continue;

    if (n == p->trellis_num && sdm_cmple(st->act[n - 1], s))
      continue;

    t = sdm_check_path(p, s);

    if (!t) {
      for (j = n; j > 0; j--) {
        t = st->act[j - 1];
        if (sdm_cmple(t, s))
          break;
        st->act[j] = t;
      }
      if (j < p->trellis_num)
        st->act[j] = s;
      if (n < p->trellis_num)
        n++;
      continue;
    }

    if (sdm_cmple(t, s))
      continue;

    for (j = 0; j < n; j++) {
      r = st->act[j];
      if (sdm_cmple(s, r))
        break;
    }

    st->act[j++] = s;

    while (r != t && j < n) {
      sdm_state_t *u = st->act[j];
      st->act[j] = r;
      r = u;
      j++;
    }
  }

  return n;
}

inline void DSDModulatorSoxTrellis::sdm_step(sdm_t *p, sdm_state_t *cur, sdm_state_t *next, double x)
{
  const sdm_filter_t *f = p->filter;
  int i;

  sdm_filter_calc2(cur, next, f, x);

  for (i = 0; i < 2; i++) {
    next[i].path = (cur->path << 1 | i) & p->trellis_mask;
    next[i].hist = cur->hist;
    next[i].next = cur->next;
    next[i].parent = cur;
  }
}

DSDModulatorSoxTrellis::sox_sample_t DSDModulatorSoxTrellis::sdm_sample_trellis(sdm_t *p, double x)
{
  sdm_trellis_t *st_cur = &p->trellis[p->idx];
  sdm_trellis_t *st_next = &p->trellis[p->idx ^ 1];
  double min_cost;
  unsigned new_cands;
  unsigned next_pos;
  unsigned output;
  unsigned i;

  next_pos = p->pos + 1;
  if (next_pos == p->trellis_lat)
    next_pos = 0;

  for (i = 0; i < p->num_cands; i++) {
    sdm_state_t *cur = st_cur->act[i];
    sdm_state_t *next = &st_next->sdm[2 * i];
    sdm_step(p, cur, next, x);
    cur->next = sdm_hist_get(p, cur->hist, next_pos);
    cur->hist_used = 0;
  }

  new_cands = sdm_sort_cands(p, st_next);
  min_cost = st_next->act[0]->cost;
  output = st_next->act[0]->next;

  for (i = 0; i < new_cands; i++) {
    sdm_state_t *s = st_next->act[i];
    if (s->parent->hist_used) {
      unsigned h = sdm_histbuf_get(p);
      sdm_hist_copy(p, h, s->hist);
      s->hist = h;
    } else {
      s->parent->hist_used = 1;
    }

    s->cost -= min_cost;
    s->next = s->parent->next;
    sdm_hist_put(p, s->hist, p->pos, s->path & 1);
  }

  for (i = 0; i < p->num_cands; i++) {
    sdm_state_t *s = st_cur->act[i];
    if (!s->hist_used)
      sdm_histbuf_put(p, s->hist);
  }

  if (new_cands < p->num_cands)
    p->conv_fail++;

  p->num_cands = new_cands;
  p->pos = next_pos;
  p->idx ^= 1;

  return output ? SOX_SAMPLE_MAX : -SOX_SAMPLE_MAX;
}

DSDModulatorSoxTrellis::sox_sample_t DSDModulatorSoxTrellis::sdm_sample(sdm_t *p, double x)
{
  const sdm_filter_t *f = p->filter;
  double *s0 = p->trellis[0].sdm[p->idx].state;
  double *s1 = p->trellis[0].sdm[p->idx ^ 1].state;
  double y, v;

  v = sdm_filter_calc(s0, s1, f, x, p->prev_y);
  y = signbit(v) ? -1.0 : 1.0;

  p->idx ^= 1;
  p->prev_y = y;

  return y * SOX_SAMPLE_MAX;
}

int DSDModulatorSoxTrellis::sdm_process(sdm_t *p, const double *ibuf, sox_sample_t *obuf, size_t *ilen, size_t *olen)
{
  const double c_gain = 0.5;
  sox_sample_t *out = obuf;
  size_t len = *ilen = min(*ilen, *olen);
  double x;

  if (p->trellis_mask) {
    if (p->pending < p->trellis_lat) {
      size_t pre = min(p->trellis_lat - p->pending, len);
      p->pending += pre;
      len -= pre;
      while (pre--) {
        x = *ibuf++ * c_gain;
        sdm_sample_trellis(p, x);
      }
    }
    while (len--) {
      x = *ibuf++ * c_gain;
      *out++ = sdm_sample_trellis(p, x);
    }
  } else {
    while (len--) {
      x = *ibuf++ * c_gain;
      *out++ = sdm_sample(p, x);
    }
  }

  *olen = out - obuf;

  return SOX_SUCCESS;
}

int DSDModulatorSoxTrellis::sdm_drain(sdm_t *p, sox_sample_t *obuf, size_t *olen)
{
  if (p->trellis_mask) {
    size_t len = *olen = min(p->pending, *olen);

    if (!p->draining && p->pending < p->trellis_lat) {
      unsigned flush = p->trellis_lat - p->pending;
      while (flush--)
        sdm_sample_trellis(p, 0.0);
    }

    p->draining = 1;
    p->pending -= len;

    while (len--)
      *obuf++ = sdm_sample_trellis(p, 0.0);
  } else {
    *olen = 0;
  }

  return SOX_SUCCESS;
}

DSDModulatorSoxTrellis::sdm_t *DSDModulatorSoxTrellis::sdm_init(const char *filter_name,
                unsigned freq,
                unsigned trellis_order,
                unsigned trellis_num,
                unsigned trellis_latency)
{
  sdm_t *p;
  const sdm_filter_t *f;
  sdm_trellis_t *st;
  unsigned i;

  if (trellis_order > SDM_TRELLIS_MAX_ORDER) {
    return NULL;
  }

  if (trellis_num > SDM_TRELLIS_MAX_NUM) {
    return NULL;
  }

  if (trellis_latency > SDM_TRELLIS_MAX_LAT) {
    return NULL;
  }

  p = (sdm_t *)malloc(sizeof(*p));
  if (!p)
    return NULL;

  memset(p, 0, sizeof(*p));

  p->filter = sdm_find_filter(filter_name, freq);
  if (!p->filter) {
    return NULL;
  }

  f = p->filter;
  st = &p->trellis[0];

  if (trellis_order || f->trellis_order) {
    if (trellis_order < 1)
      trellis_order = f->trellis_order ? f->trellis_order : 13;

    if (trellis_num)
      p->trellis_num = trellis_num;
    else
      p->trellis_num = f->trellis_num ? f->trellis_num : 8;

    if (trellis_latency)
      p->trellis_lat = trellis_latency;
    else
      p->trellis_lat = f->trellis_lat ? f->trellis_lat : 1024;

    p->trellis_mask = ((uint64_t)1 << trellis_order) - 1;

    for (i = 0; i < 2 * p->trellis_num; i++)
      sdm_histbuf_put(p, i);

    p->num_cands = 1;

    st->sdm[0].hist = sdm_histbuf_get(p);
    st->sdm[0].path = 0;
    st->act[0] = &st->sdm[0];
  }

  return p;
}

void DSDModulatorSoxTrellis::sdm_close(sdm_t *p)
{
  free(p);
}

bool DSDModulatorSoxTrellis::init(int rate)
{
    const char *c_filterName = "sdm-7";
    unsigned int freq = rate * 44100;
    unsigned int order = 8;
    unsigned int nopaths = 16;
    unsigned int latency = 50;

    m_mod = sdm_init(c_filterName, freq, order, nopaths, latency);
    return (m_mod != NULL) ? true : false;
}

int DSDModulatorSoxTrellis::process(const double *in, int noSamples, uint8_t *out)
{
    static const uint8_t msb[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
    size_t noInSamples = noSamples;
    size_t noOutSamples = noSamples;
    sox_sample_t *obuf = new sox_sample_t [noSamples];

    sdm_process(m_mod, in, obuf, &noInSamples, &noOutSamples);

    memset(out, 0, noSamples / 8);
    int dIdx = 0;
    for(int sIdx = 0; sIdx < noOutSamples; sIdx++)
    {
        int bit = sIdx & 0x7;

        if(obuf[sIdx] > 0)
        {
            out[dIdx] |= msb[bit];
        }
        if(bit == 7)
        {
            dIdx++;
        }
    }
    delete [] obuf;
    return (int)noOutSamples / 8;
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

    DSDModulatorSoxTrellis modL,modR;
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

            outLenL = modL.process(upLPtr.get(), outLenL, outL);
            outLenR = modR.process(upRPtr.get(), outLenR, outR);

            int outMin = (outLenL < outLenR) ? outLenL : outLenR;
            for(idx = 0; idx < outMin; idx++)
            {
                out[(idx << 1) + 0] = outL[idx];
                out[(idx << 1) + 1] = outR[idx];
            }

            ASSERT_EQ(fwrite(out, 1, 2 * outMin, WriteData), 2 * outMin);
            amount += 2 * outMin;
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
    QString inFilename = "D:\\Development\\Temp\\dsd\\isla.wav";
    QString outFilename = "D:\\Development\\Temp\\dsd\\isla_64_trellis_1.dff";

    engine::Codec *codec = engine::Codec::get(inFilename);
    ASSERT_FALSE(codec == NULL);
    ASSERT_TRUE(codec->init());

    dsd_write_codec_claude(codec, outFilename, 64);

    delete codec;
}

//-------------------------------------------------------------------------------------------
