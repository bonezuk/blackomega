#include "engine/whiteomega/inc/TNS.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

TNS::TNS() : m_gaConfig(0),
    m_channel(0)
{}

//-------------------------------------------------------------------------------------------

TNS::~TNS()
{}

//-------------------------------------------------------------------------------------------

void TNS::set(GAConfig *cfg,AACDecode *ch)
{
    m_gaConfig = cfg;
    m_channel = ch;
}

//-------------------------------------------------------------------------------------------

void TNS::decode()
{
    doDecode(m_channel->m_spectralCoef);
}

//-------------------------------------------------------------------------------------------

void TNS::encodeLTP(sample_t *spec)
{
    doEncode(spec);
}

//-------------------------------------------------------------------------------------------

void TNS::doDecode(sample_t *spec)
{
    ICSInfo *info = &(m_channel->m_info);
    TNSData *tns = &(info->tns);

    if(info->tnsDataPresent)
    {
        tint w,f,tnsOrder,nshort = m_gaConfig->m_frameLength >> 3;
        tint bottom,top,start,end,size,inc;
        tint maxSfbIdx;
        sample_t lpc[TNS_MAX_ORDER + 1];

        maxSfbIdx = maxSfb();

        for(w=0;w<info->numWindows;++w)
        {
            bottom = info->numSwb;

            for(f=0;f<tns->nFilt[w];++f)
            {
                top = bottom;
                bottom = maxV(top - tns->length[w][f],0);
                tnsOrder = minV(tns->order[w][f],TNS_MAX_ORDER);
                if(tnsOrder)
                {
                    decodeCoef(tnsOrder,tns->coefRes[w],tns->coefCompress[w][f],&(tns->coef[w][f][0]),lpc);

                    start = minV(bottom,maxSfbIdx);
                    start = minV(start,info->maxSfb);
                    start = minV(info->swbOffset[start],info->swbOffsetMax);

                    end = minV(top,maxSfbIdx);
                    end = minV(end,info->maxSfb);
                    end = minV(info->swbOffset[end],info->swbOffsetMax);

                    size = end - start;
                    if(size > 0)
                    {
                        if(tns->direction[w][f])
                        {
                            inc = -1;
                            start = end - 1;
                        }
                        else
                        {
                            inc = 1;
                        }
                        arFilter(&spec[(w * nshort) + start],size,inc,lpc,tnsOrder);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void TNS::doEncode(sample_t *spec)
{
    ICSInfo *info = &(m_channel->m_info);
    TNSData *tns = &(info->tns);

    if(info->tnsDataPresent)
    {
        tint w,f,tnsOrder,nshort = m_gaConfig->m_frameLength >> 3;
        tint bottom,top,start,end,size,inc;
        tint maxSfbIdx;
        sample_t lpc[TNS_MAX_ORDER + 1];

        maxSfbIdx = maxSfb();

        for(w=0;w<info->numWindows;++w)
        {
            bottom = info->numSwb;

            for(f=0;f<tns->nFilt[w];++f)
            {
                top = bottom;
                bottom = maxV(top - tns->length[w][f],0);
                tnsOrder = minV(tns->order[w][f],TNS_MAX_ORDER);
                if(tnsOrder)
                {
                    decodeCoef(tnsOrder,tns->coefRes[w],tns->coefCompress[w][f],&(tns->coef[w][f][0]),lpc);

                    start = minV(bottom,maxSfbIdx);
                    start = minV(start,info->maxSfb);
                    start = minV(info->swbOffset[start],info->swbOffsetMax);

                    end = minV(top,maxSfbIdx);
                    end = minV(end,info->maxSfb);
                    end = minV(info->swbOffset[end],info->swbOffsetMax);

                    size = end - start;
                    if(size > 0)
                    {
                        if(tns->direction[w][f])
                        {
                            inc = -1;
                            start = end - 1;
                        }
                        else
                        {
                            inc = 1;
                        }
                        maFilter(&spec[(w * nshort) + start],size,inc,lpc,tnsOrder);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void TNS::arFilter(sample_t *spec,tint size,tint inc,sample_t *lpc,tint order)
{
    tint i,j,a,stateIdx = 0;
    sample_t y,state[TNS_MAX_ORDER * 2];

    stateIdx = order - 1;
    state[stateIdx] = state[stateIdx + order] = *spec;
    spec += inc;

    for(i=1;i<size;++i)
    {
        sample_t *s = &state[stateIdx - 1];

        a = (i < order) ? i : order;
        y = *spec;

        for(j=1;j<=a;++j)
        {
            y -= s[j] * lpc[j];
        }

        stateIdx--;
        if(stateIdx < 0)
        {
            stateIdx = order - 1;
        }
        state[stateIdx] = state[stateIdx + order] = y;

        *spec = y;
        spec += inc;
    }
}

//-------------------------------------------------------------------------------------------

void TNS::maFilter(sample_t *spec,tint size,tint inc,sample_t *lpc,tint order)
{
    tint i,j,a,stateIdx = 0;
    sample_t y,state[TNS_MAX_ORDER * 2];

    stateIdx = order - 1;
    state[stateIdx] = state[stateIdx + order] = *spec;
    spec += inc;

    for(i=1;i<size;++i)
    {
        sample_t *s = &state[stateIdx - 1];

        a = (i < order) ? i : order;
        y = *spec;

        for(j=1;j<=a;++j)
        {
            y += s[j] * lpc[j];
        }

        stateIdx--;
        if(stateIdx<0)
        {
            stateIdx = order - 1;
        }
        state[stateIdx] = state[stateIdx + order] = *spec;

        *spec = y;
        spec += inc;
    }
}

//-------------------------------------------------------------------------------------------

tint TNS::maxSfb()
{
    static const tint sfbMax[12][4] = {
        { 31,  9, 28,  7}, //  0 - 96000
        { 31,  9, 28,  7}, //  1 - 88200
        { 34, 10, 27,  7}, //  2 - 64000
        { 40, 14, 26,  6}, //  3 - 48000
        { 42, 14, 26,  6}, //  4 - 44100
        { 51, 14, 26,  6}, //  5 - 32000
        { 46, 14, 29,  7}, //  6 - 24000
        { 46, 14, 29,  7}, //  7 - 22050
        { 42, 14, 23,  8}, //  8 - 16000
        { 42, 14, 23,  8}, //  9 - 12000
        { 42, 14, 23,  8}, // 10 - 11025
        { 39, 14, 19,  7}  // 11 -  8000
    };

    tint i = (m_channel->m_info.windowSequence==EIGHT_SHORT_SEQUENCE) ? 1 : 0;

    if(m_gaConfig->m_audioObjectType==GAConfig::e_audioAACSSR)
    {
        i += 2;
    }
    return sfbMax[m_gaConfig->m_samplingFrequencyIndex][i];
}

//-------------------------------------------------------------------------------------------

void TNS::decodeCoef(tint order,tint coefRes,tint coefCompress,tint *coef,sample_t *a)
{
    tint i,m;
    sample_t x[TNS_MAX_ORDER + 1],b[TNS_MAX_ORDER + 1];

    if(!coefRes)
    {
        if(!coefCompress)
        {
            for(i=1;i<=order;++i)
            {
                x[i] = m_winCoef_0_0[ coef[i - 1] ];
            }
        }
        else
        {
            for(i=1;i<=order;++i)
            {
                x[i] = m_winCoef_0_1[ coef[i - 1] ];
            }
        }
    }
    else
    {
        if(!coefCompress)
        {
            for(i=1;i<=order;++i)
            {
                x[i] = m_winCoef_1_0[ coef[i - 1] ];
            }
        }
        else
        {
            for(i=1;i<=order;++i)
            {
                x[i] = m_winCoef_1_1[ coef[i - 1] ];
            }
        }
    }

    a[0] = c_plusOneSample;
    for(m=1;m<=order;++m)
    {
        for(i=1;i<m;++i)
        {
            b[i] = a[i] + (x[m] * a[m - i]);
        }
        for(i=1;i<m;++i)
        {
            a[i] = b[i];
        }
        a[m] = x[m];
    }
}

//-------------------------------------------------------------------------------------------
#if defined(SINGLE_FLOAT_SAMPLE)
//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_0_0[16] = {
     0.00000000000000000000f,  0.43388373911755806489f,  0.78183148246802969261f,  0.97492791218182350832f,
    -0.98480775301220802032f, -0.86602540378443859659f, -0.64278760968653925190f, -0.34202014332566871291f,
    -0.43388373911755762080f, -0.78183148246802969261f, -0.97492791218182350832f, -0.97492791218182384139f,
    -0.98480775301220802032f, -0.86602540378443859659f, -0.64278760968653925190f, -0.34202014332566871291f
};

//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_0_1[16] = {
     0.00000000000000000000f,  0.43388373911755806489f, -0.64278760968653925190f, -0.34202014332566871291f,
     0.97492791218182361934f,  0.78183148246802991466f, -0.64278760968653925190f, -0.34202014332566871291f,
    -0.43388373911755762080f, -0.78183148246802969261f, -0.64278760968653925190f, -0.34202014332566871291f,
    -0.78183148246803058079f, -0.43388373911755911960f, -0.64278760968653925190f, -0.34202014332566871291f
};

//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_1_0[16] = {
     0.00000000000000000000f,  0.20791169081775931482f,  0.40673664307580015276f,  0.58778525229247313710f,
     0.74314482547739413310f,  0.86602540378443859659f,  0.95105651629515353118f,  0.99452189536827328986f,
    -0.99573417629503446769f, -0.96182564317281904476f, -0.89516329135506234049f, -0.79801722728023949394f,
    -0.67369564364655720734f, -0.52643216287735572489f, -0.36124166618715292110f, -0.18374951781657034022f
};

//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_1_1[16] = {
     0.00000000000000000000f,  0.20791169081775931482f,  0.40673664307580015276f,  0.58778525229247313710f,
    -0.67369564364655720734f, -0.52643216287735572489f, -0.36124166618715292110f, -0.18374951781657034022f,
     0.99452189536827340088f,  0.95105651629515364220f,  0.86602540378443881863f,  0.74314482547739446616f,
    -0.67369564364655720734f, -0.52643216287735572489f, -0.36124166618715292110f, -0.18374951781657034022f
};

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_0_0[16] = {
     0.00000000000000000000,  0.43388373911755806489,  0.78183148246802969261,  0.97492791218182350832,
    -0.98480775301220802032, -0.86602540378443859659, -0.64278760968653925190, -0.34202014332566871291,
    -0.43388373911755762080, -0.78183148246802969261, -0.97492791218182350832, -0.97492791218182384139,
    -0.98480775301220802032, -0.86602540378443859659, -0.64278760968653925190, -0.34202014332566871291
};

//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_0_1[16] = {
     0.00000000000000000000,  0.43388373911755806489, -0.64278760968653925190, -0.34202014332566871291,
     0.97492791218182361934,  0.78183148246802991466, -0.64278760968653925190, -0.34202014332566871291,
    -0.43388373911755762080, -0.78183148246802969261, -0.64278760968653925190, -0.34202014332566871291,
    -0.78183148246803058079, -0.43388373911755911960, -0.64278760968653925190, -0.34202014332566871291
};

//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_1_0[16] = {
     0.00000000000000000000,  0.20791169081775931482,  0.40673664307580015276,  0.58778525229247313710,
     0.74314482547739413310,  0.86602540378443859659,  0.95105651629515353118,  0.99452189536827328986,
    -0.99573417629503446769, -0.96182564317281904476, -0.89516329135506234049, -0.79801722728023949394,
    -0.67369564364655720734, -0.52643216287735572489, -0.36124166618715292110, -0.18374951781657034022
};

//-------------------------------------------------------------------------------------------

const sample_t TNS::m_winCoef_1_1[16] = {
     0.00000000000000000000,  0.20791169081775931482,  0.40673664307580015276,  0.58778525229247313710,
    -0.67369564364655720734, -0.52643216287735572489, -0.36124166618715292110, -0.18374951781657034022,
     0.99452189536827340088,  0.95105651629515364220,  0.86602540378443881863,  0.74314482547739446616,
    -0.67369564364655720734, -0.52643216287735572489, -0.36124166618715292110, -0.18374951781657034022
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
