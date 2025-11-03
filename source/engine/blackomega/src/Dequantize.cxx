#include "engine/blackomega/inc/Dequantize.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

tint g_pretab[23] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0, 0
};

//-------------------------------------------------------------------------------------------

Dequantize::Dequantize() : m_hdr(0),
    m_gr(0),
    m_scaleFactor(0)
{}

//-------------------------------------------------------------------------------------------

Dequantize::~Dequantize()
{}

//-------------------------------------------------------------------------------------------

void Dequantize::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Dequantize::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool Dequantize::read(engine::Sequence *s)
{
    tint reg[3];
    BSequence *seq = dynamic_cast<BSequence *>(s);

    if(seq==0)
    {
        printError("read","Sequence is not specific to black omega decoder");
        return false;
    }

    boundaries(reg);

    if(m_gr->block_type==2)
    {
        if(m_gr->mixed_block_flag)
        {
            Dequantize::decodeMixed(seq,reg);
        }
        else
        {
            Dequantize::decodeShort(seq,reg);
        }
    }
    else
    {
        Dequantize::decodeLong(seq,reg);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void Dequantize::boundaries(tint *reg)
{
    if(m_gr->big_values <= m_gr->region1Start)
    {
        reg[0] = m_gr->big_values;
        reg[1] = 0;
        reg[2] = 0;
    }
    else
    {
        reg[0] = m_gr->region1Start;
        if(m_gr->big_values <= m_gr->region2Start)
        {
            reg[1] = m_gr->big_values;
            reg[2] = 0;
        }
        else
        {
            reg[1] = m_gr->region2Start;
            reg[2] = m_gr->big_values;
        }
    }
}

//-------------------------------------------------------------------------------------------

void Dequantize::decodeLong(BSequence *seq,tint *reg)
{
    Band *bandI = Band::instance();
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i=0, j=0, k=0, h, x, y, v, w;
    tint cbBoundary = 0;
    tint diff = (m_gr->scalefac_scale) ? 2 : 1;
    tint *pre = g_pretab;
    tint *cb = &(bandI->m_sfBandIndex[m_hdr->sfreq].l[1]);
    tint *sfPtr = &(scaleInfo->l[0]);
    sample_t sc = c_zeroSample;
    sample_t *scale = &(bandI->m_dequantS[m_gr->global_gain + 236]);
    sample_t *quarter = bandI->m_quarter;
    sample_t *xr = &m_ro[0][0];

    if(m_gr->preflag)
    {
        while(j < 3)
        {
            h = m_gr->table_select[j];

            while(i < reg[j])
            {
                if(i == cbBoundary)
                {
                    sc = scale[-((pre[k] + sfPtr[k]) << diff)];
                    cbBoundary = cb[k];
                    k++;
                }
                seq->decode(h,x,y);
                xr[i++] = sc * quarter[x];
                xr[i++] = sc * quarter[y];
            }
            j++;
        }

        h = m_gr->count1table_select;

        while(i<(SBLIMIT * SSLIMIT) && seq->remain()>0)
        {
            seq->decode(h,x,y,v,w);

            if(i==cbBoundary)
            {
                sc = scale[-((pre[k] + sfPtr[k]) << diff)];
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[v];
            xr[i++] = sc * quarter[w];

            if(i==cbBoundary)
            {
                sc = scale[-((pre[k] + sfPtr[k]) << diff)];
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[x];
            xr[i++] = sc * quarter[y];
        }
    }
    else
    {
        while(j < 3)
        {
            h = m_gr->table_select[j];

            while(i < reg[j])
            {
                if(i == cbBoundary)
                {
                    sc = scale[-(sfPtr[k] << diff)];
                    cbBoundary = cb[k];
                    k++;
                }
                seq->decode(h,x,y);
                xr[i++] = sc * quarter[x];
                xr[i++] = sc * quarter[y];
            }
            j++;
        }

        h = m_gr->count1table_select;

        while(i<(SBLIMIT * SSLIMIT) && seq->remain()>0)
        {
            seq->decode(h,x,y,v,w);

            if(i==cbBoundary)
            {
                sc = scale[-(sfPtr[k] << diff)];
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[v];
            xr[i++] = sc * quarter[w];

            if(i==cbBoundary)
            {
                sc = scale[-(sfPtr[k] << diff)];
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[x];
            xr[i++] = sc * quarter[y];
        }
    }

    while(i<(SBLIMIT * SSLIMIT))
    {
        xr[i++] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------

void Dequantize::decodeShort(BSequence *seq,tint *reg)
{
    Band *bandI = Band::instance();
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i=0, j=0, k=0, h, x, y, v, w;
    tint win = 0;
    tint cbBoundary = 0;
    tint diff = (m_gr->scalefac_scale) ? 2 : 1;
    tint *cb = &(bandI->m_sfBandIndex[m_hdr->sfreq].s[1]);
    tint *sfSPtr0 = &(scaleInfo->s[0][0]);
    tint *sfSPtr1 = &(scaleInfo->s[1][0]);
    tint *sfSPtr2 = &(scaleInfo->s[2][0]);
    sample_t sc = c_zeroSample;
    sample_t *scale0 = &(bandI->m_dequantS[(m_gr->global_gain + 236) - (m_gr->subblock_gain[0]<<3)]);
    sample_t *scale1 = &(bandI->m_dequantS[(m_gr->global_gain + 236) - (m_gr->subblock_gain[1]<<3)]);
    sample_t *scale2 = &(bandI->m_dequantS[(m_gr->global_gain + 236) - (m_gr->subblock_gain[2]<<3)]);
    sample_t *quarter = bandI->m_quarter;
    sample_t *xr = &m_ro[0][0];

    while(j < 3)
    {
        h = m_gr->table_select[j];

        while(i < reg[j])
        {
            if(i == cbBoundary)
            {
                switch(win)
                {
                    case 0:
                        sc = scale0[-((*sfSPtr0++) << diff)];
                        win = 1;
                        break;

                    case 1:
                        sc = scale1[-((*sfSPtr1++) << diff)];
                        win = 2;
                        break;

                    case 2:
                        sc = scale2[-((*sfSPtr2++) << diff)];
                        win = 0;
                        break;
                }
                cbBoundary = cb[k];
                k++;
            }
            seq->decode(h,x,y);
            xr[i++] = sc * quarter[x];
            xr[i++] = sc * quarter[y];
        }
        j++;
    }

    h = m_gr->count1table_select;

    while(i<(SBLIMIT * SSLIMIT) && seq->remain()>0)
    {
        seq->decode(h,x,y,v,w);

        if(i == cbBoundary)
        {
            switch(win)
            {
                case 0:
                    sc = scale0[-((*sfSPtr0++) << diff)];
                    win = 1;
                    break;

                case 1:
                    sc = scale1[-((*sfSPtr1++) << diff)];
                    win = 2;
                    break;

                case 2:
                    sc = scale2[-((*sfSPtr2++) << diff)];
                    win = 0;
                    break;
            }
            cbBoundary = cb[k];
            k++;
        }
        xr[i++] = sc * quarter[v];
        xr[i++] = sc * quarter[w];

        if(i == cbBoundary)
        {
            switch(win)
            {
                case 0:
                    sc = scale0[-((*sfSPtr0++) << diff)];
                    win = 1;
                    break;

                case 1:
                    sc = scale1[-((*sfSPtr1++) << diff)];
                    win = 2;
                    break;

                case 2:
                    sc = scale2[-((*sfSPtr2++) << diff)];
                    win = 0;
                    break;
            }
            cbBoundary = cb[k];
            k++;
        }
        xr[i++] = sc * quarter[x];
        xr[i++] = sc * quarter[y];
    }

    while(i<(SBLIMIT * SSLIMIT))
    {
        xr[i++] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------

void Dequantize::decodeMixed(BSequence *seq,tint *reg)
{
    Band *bandI = Band::instance();
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i=0, j=0, k=0, h, x, y, v, w;
    tint win = 0;
    tint cbBoundary = 0;
    tint diff = (m_gr->scalefac_scale) ? 2 : 1;
    tint *pre = g_pretab;
    tint *cb = &(bandI->m_sfBandIndex[m_hdr->sfreq].m[1]);
    tint *sfLPtr = &(scaleInfo->l[0]);
    tint *sfSPtr0 = &(scaleInfo->s[0][3]);
    tint *sfSPtr1 = &(scaleInfo->s[1][3]);
    tint *sfSPtr2 = &(scaleInfo->s[2][3]);
    sample_t sc = c_zeroSample;
    sample_t *scaleL  = &(bandI->m_dequantS[ m_gr->global_gain + 236 ]);
    sample_t *scaleS0 = &(bandI->m_dequantS[(m_gr->global_gain + 236) - (m_gr->subblock_gain[0]<<3)]);
    sample_t *scaleS1 = &(bandI->m_dequantS[(m_gr->global_gain + 236) - (m_gr->subblock_gain[1]<<3)]);
    sample_t *scaleS2 = &(bandI->m_dequantS[(m_gr->global_gain + 236) - (m_gr->subblock_gain[2]<<3)]);
    sample_t *quarter = bandI->m_quarter;
    sample_t *xr = &m_ro[0][0];

    if(m_gr->preflag)
    {
        while(j < 3)
        {
            h = m_gr->table_select[j];

            while(i < reg[j])
            {
                if(i==cbBoundary)
                {
                    if(i<36)
                    {
                        sc = scaleL[-((pre[k] + sfLPtr[k]) << diff)];
                    }
                    else
                    {
                        switch(win)
                        {
                            case 0:
                                sc = scaleS0[-((*sfSPtr0++) << diff)];
                                win = 1;
                                break;

                            case 1:
                                sc = scaleS1[-((*sfSPtr1++) << diff)];
                                win = 2;
                                break;

                            case 2:
                                sc = scaleS2[-((*sfSPtr2++) << diff)];
                                win = 0;
                                break;
                        }
                    }
                    cbBoundary = cb[k];
                    k++;
                }

                seq->decode(h,x,y);
                xr[i++] = sc * quarter[x];
                xr[i++] = sc * quarter[y];
            }
            j++;
        }

        while(i<(SBLIMIT * SSLIMIT) && seq->remain()>0)
        {
            seq->decode(h,x,y,v,w);

            if(i==cbBoundary)
            {
                if(i<36)
                {
                    sc = scaleL[-((pre[k] + sfLPtr[k]) << diff)];
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[v];
            xr[i++] = sc * quarter[w];

            if(i==cbBoundary)
            {
                if(i<36)
                {
                    sc = scaleL[-((pre[k] + sfLPtr[k]) << diff)];
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[x];
            xr[i++] = sc * quarter[y];
        }
    }
    else
    {
        while(j < 3)
        {
            h = m_gr->table_select[j];

            while(i < reg[j])
            {
                if(i==cbBoundary)
                {
                    if(i<36)
                    {
                        sc = scaleL[-(sfLPtr[k] << diff)];
                    }
                    else
                    {
                        switch(win)
                        {
                            case 0:
                                sc = scaleS0[-((*sfSPtr0++) << diff)];
                                win = 1;
                                break;

                            case 1:
                                sc = scaleS1[-((*sfSPtr1++) << diff)];
                                win = 2;
                                break;

                            case 2:
                                sc = scaleS2[-((*sfSPtr2++) << diff)];
                                win = 0;
                                break;
                        }
                    }
                    cbBoundary = cb[k];
                    k++;
                }

                seq->decode(h,x,y);
                xr[i++] = sc * quarter[x];
                xr[i++] = sc * quarter[y];
            }
            j++;
        }

        while(i<(SBLIMIT * SSLIMIT) && seq->remain()>0)
        {
            seq->decode(h,x,y,v,w);

            if(i==cbBoundary)
            {
                if(i<36)
                {
                    sc = scaleL[-(sfLPtr[k] << diff)];
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[v];
            xr[i++] = sc * quarter[w];

            if(i==cbBoundary)
            {
                if(i<36)
                {
                    sc = scaleL[-(sfLPtr[k] << diff)];
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }
            xr[i++] = sc * quarter[x];
            xr[i++] = sc * quarter[y];
        }
    }

    while(i<(SBLIMIT * SSLIMIT))
    {
        xr[i++] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
