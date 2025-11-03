#include "engine/blackomega/inc/DequantizeStereo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

tint g_pretab_stereo[23] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0, 0
};

//-------------------------------------------------------------------------------------------

DequantizeStereo::DequantizeStereo() : Dequantize(),
    m_stereoL(0),
    m_stereoS0(0),
    m_stereoS1(0),
    m_stereoS2(0)
{}

//-------------------------------------------------------------------------------------------

DequantizeStereo::~DequantizeStereo()
{}

//-------------------------------------------------------------------------------------------

void DequantizeStereo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "DequantizeStereo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool DequantizeStereo::read(engine::Sequence *s)
{
    if((m_hdr->mode==MPG_MD_JOINT_STEREO) && (m_hdr->mode_ext & 0x00000001))
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
                DequantizeStereo::decodeMixed(seq,reg);
            }
            else
            {
                DequantizeStereo::decodeShort(seq,reg);
            }
        }
        else
        {
            DequantizeStereo::decodeLong(seq,reg);
        }
        return true;
    }
    else
    {
        return Dequantize::read(s);
    }
}

//-------------------------------------------------------------------------------------------

tint DequantizeStereo::getStereo(StereoType t)
{
    tint v = 0;

    switch(t)
    {
        case e_stereoLong:
            v = m_stereoL;
            break;

        case e_stereoShort0:
            v = m_stereoS0;
            break;

        case e_stereoShort1:
            v = m_stereoS1;
            break;

        case e_stereoShort2:
            v = m_stereoS2;
            break;
    }
    return v;
}

//-------------------------------------------------------------------------------------------

void DequantizeStereo::decodeLong(BSequence *seq,tint *reg)
{
    Band *bandI = Band::instance();
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i=0, j=0, k=0, h, x, y, v, w;
    tint cbBoundary = 0;
    tint diff = (m_gr->scalefac_scale) ? 2 : 1;
    tint *pre = g_pretab_stereo;
    tint *cb = &(bandI->m_sfBandIndex[m_hdr->sfreq].l[1]);
    tint *sfPtr = &(scaleInfo->l[0]);
    sample_t sc = c_zeroSample;
    sample_t *scale = &(bandI->m_dequantS[m_gr->global_gain + 236]);
    sample_t *quarter = bandI->m_quarter;
    sample_t *xr = &m_ro[0][0];
    tint stereoL = 0,currentL = 0;

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
                    currentL++;
                }
                seq->decode(h,x,y);

                xr[i] = sc * quarter[x];
                if(xr[i]!=c_zeroSample)
                {
                    stereoL = currentL;
                }
                i++;

                xr[i] = sc * quarter[y];
                if(xr[i]!=c_zeroSample)
                {
                    stereoL = currentL;
                }
                i++;
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
                currentL++;
            }

            xr[i] = sc * quarter[v];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;

            xr[i] = sc * quarter[w];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;

            if(i==cbBoundary)
            {
                sc = scale[-((pre[k] + sfPtr[k]) << diff)];
                cbBoundary = cb[k];
                k++;
                currentL++;
            }

            xr[i] = sc * quarter[x];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;

            xr[i] = sc * quarter[y];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;
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
                    currentL++;
                }
                seq->decode(h,x,y);

                xr[i] = sc * quarter[x];
                if(xr[i]!=c_zeroSample)
                {
                    stereoL = currentL;
                }
                i++;

                xr[i] = sc * quarter[y];
                if(xr[i]!=c_zeroSample)
                {
                    stereoL = currentL;
                }
                i++;
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
                currentL++;
            }

            xr[i] = sc * quarter[v];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;

            xr[i] = sc * quarter[w];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;

            if(i==cbBoundary)
            {
                sc = scale[-(sfPtr[k] << diff)];
                cbBoundary = cb[k];
                k++;
                currentL++;
            }

            xr[i] = sc * quarter[x];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;

            xr[i] = sc * quarter[y];
            if(xr[i]!=c_zeroSample)
            {
                stereoL = currentL;
            }
            i++;
        }
    }

    while(i<(SBLIMIT * SSLIMIT))
    {
        xr[i++] = c_zeroSample;
    }

    m_stereoL = stereoL;
}

//-------------------------------------------------------------------------------------------

void DequantizeStereo::decodeShort(BSequence *seq,tint *reg)
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
    tint stereoS0 = 0, stereoS1 = 0, stereoS2 = 0, currentS0 = 0, currentS1 = 0, currentS2 = 0;
    tint *stereoS = &stereoS0, *currentS = &currentS0;

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
                        stereoS = &stereoS0;
                        currentS = &currentS0;
                        currentS0++;
                        win = 1;
                        break;

                    case 1:
                        sc = scale1[-((*sfSPtr1++) << diff)];
                        stereoS = &stereoS1;
                        currentS = &currentS1;
                        currentS1++;
                        win = 2;
                        break;

                    case 2:
                        sc = scale2[-((*sfSPtr2++) << diff)];
                        stereoS = &stereoS2;
                        currentS = &currentS2;
                        currentS2++;
                        win = 0;
                        break;
                }
                cbBoundary = cb[k];
                k++;
            }
            seq->decode(h,x,y);

            xr[i] = sc * quarter[x];
            if(xr[i]!=c_zeroSample)
            {
                *stereoS = *currentS;
            }
            i++;

            xr[i++] = sc * quarter[y];
            if(xr[i]!=c_zeroSample)
            {
                *stereoS = *currentS;
            }
            i++;
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
                    stereoS = &stereoS0;
                    currentS = &currentS0;
                    currentS0++;
                    win = 1;
                    break;

                case 1:
                    sc = scale1[-((*sfSPtr1++) << diff)];
                    stereoS = &stereoS1;
                    currentS = &currentS1;
                    currentS1++;
                    win = 2;
                    break;

                case 2:
                    sc = scale2[-((*sfSPtr2++) << diff)];
                    stereoS = &stereoS2;
                    currentS = &currentS2;
                    currentS2++;
                    win = 0;
                    break;
            }
            cbBoundary = cb[k];
            k++;
        }

        xr[i] = sc * quarter[v];
        if(xr[i]!=c_zeroSample)
        {
            *stereoS = *currentS;
        }
        i++;

        xr[i] = sc * quarter[w];
        if(xr[i]!=c_zeroSample)
        {
            *stereoS = *currentS;
        }
        i++;

        if(i == cbBoundary)
        {
            switch(win)
            {
                case 0:
                    sc = scale0[-((*sfSPtr0++) << diff)];
                    stereoS = &stereoS0;
                    currentS = &currentS0;
                    currentS0++;
                    win = 1;
                    break;

                case 1:
                    sc = scale1[-((*sfSPtr1++) << diff)];
                    stereoS = &stereoS1;
                    currentS = &currentS1;
                    currentS1++;
                    win = 2;
                    break;

                case 2:
                    sc = scale2[-((*sfSPtr2++) << diff)];
                    stereoS = &stereoS2;
                    currentS = &currentS2;
                    currentS2++;
                    win = 0;
                    break;
            }
            cbBoundary = cb[k];
            k++;
        }

        xr[i] = sc * quarter[x];
        if(xr[i]!=c_zeroSample)
        {
            *stereoS = *currentS;
        }
        i++;

        xr[i] = sc * quarter[y];
        if(xr[i]!=c_zeroSample)
        {
            *stereoS = *currentS;
        }
        i++;
    }

    while(i<(SBLIMIT * SSLIMIT))
    {
        xr[i++] = c_zeroSample;
    }

    m_stereoS0 = stereoS0;
    m_stereoS1 = stereoS1;
    m_stereoS2 = stereoS2;
}

//-------------------------------------------------------------------------------------------

void DequantizeStereo::decodeMixed(BSequence *seq,tint *reg)
{
    Band *bandI = Band::instance();
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i=0, j=0, k=0, h, x, y, v, w;
    tint win = 0;
    tint cbBoundary = 0;
    tint diff = (m_gr->scalefac_scale) ? 2 : 1;
    tint *pre = g_pretab_stereo;
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
    tint stereoL = 0, stereoS0 = 3, stereoS1 = 3, stereoS2 = 3;
    tint currentL = 0, currentS0 = 3, currentS1 = 3, currentS2 = 3;
    tint *stereo = &stereoL, *current = &currentL;

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
                        stereo = &stereoL;
                        current = &currentL;
                        currentL++;
                    }
                    else
                    {
                        switch(win)
                        {
                            case 0:
                                sc = scaleS0[-((*sfSPtr0++) << diff)];
                                stereo = &stereoS0;
                                current = &currentS0;
                                currentS0++;
                                win = 1;
                                break;

                            case 1:
                                sc = scaleS1[-((*sfSPtr1++) << diff)];
                                stereo = &stereoS1;
                                current = &currentS1;
                                currentS1++;
                                win = 2;
                                break;

                            case 2:
                                sc = scaleS2[-((*sfSPtr2++) << diff)];
                                stereo = &stereoS2;
                                current = &currentS2;
                                currentS2++;
                                win = 0;
                                break;
                        }
                    }
                    cbBoundary = cb[k];
                    k++;
                }

                seq->decode(h,x,y);

                xr[i] = sc * quarter[x];
                if(xr[i]!=c_zeroSample)
                {
                    *stereo = *current;
                }
                i++;

                xr[i] = sc * quarter[y];
                if(xr[i]!=c_zeroSample)
                {
                    *stereo = *current;
                }
                i++;
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
                    stereo = &stereoL;
                    current = &currentL;
                    currentL++;
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            stereo = &stereoS0;
                            current = &currentS0;
                            currentS0++;
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            stereo = &stereoS1;
                            current = &currentS1;
                            currentS1++;
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            stereo = &stereoS2;
                            current = &currentS2;
                            currentS2++;
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }

            xr[i] = sc * quarter[v];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;

            xr[i] = sc * quarter[w];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;

            if(i==cbBoundary)
            {
                if(i<36)
                {
                    sc = scaleL[-((pre[k] + sfLPtr[k]) << diff)];
                    stereo = &stereoL;
                    current = &currentL;
                    currentL++;
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            stereo = &stereoS0;
                            current = &currentS0;
                            currentS0++;
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            stereo = &stereoS1;
                            current = &currentS1;
                            currentS1++;
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            stereo = &stereoS2;
                            current = &currentS2;
                            currentS2++;
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }

            xr[i] = sc * quarter[x];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;

            xr[i] = sc * quarter[y];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;
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
                        stereo = &stereoL;
                        current = &currentL;
                        currentL++;
                    }
                    else
                    {
                        switch(win)
                        {
                            case 0:
                                sc = scaleS0[-((*sfSPtr0++) << diff)];
                                stereo = &stereoS0;
                                current = &currentS0;
                                currentS0++;
                                win = 1;
                                break;

                            case 1:
                                sc = scaleS1[-((*sfSPtr1++) << diff)];
                                stereo = &stereoS1;
                                current = &currentS1;
                                currentS1++;
                                win = 2;
                                break;

                            case 2:
                                sc = scaleS2[-((*sfSPtr2++) << diff)];
                                stereo = &stereoS2;
                                current = &currentS2;
                                currentS2++;
                                win = 0;
                                break;
                        }
                    }
                    cbBoundary = cb[k];
                    k++;
                }

                seq->decode(h,x,y);

                xr[i] = sc * quarter[x];
                if(xr[i]!=c_zeroSample)
                {
                    *stereo = *current;
                }
                i++;

                xr[i] = sc * quarter[y];
                if(xr[i]!=c_zeroSample)
                {
                    *stereo = *current;
                }
                i++;
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
                    stereo = &stereoL;
                    current = &currentL;
                    currentL++;
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            stereo = &stereoS0;
                            current = &currentS0;
                            currentS0++;
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            stereo = &stereoS1;
                            current = &currentS1;
                            currentS1++;
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            stereo = &stereoS2;
                            current = &currentS2;
                            currentS2++;
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }

            xr[i] = sc * quarter[v];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;

            xr[i] = sc * quarter[w];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;

            if(i==cbBoundary)
            {
                if(i<36)
                {
                    sc = scaleL[-(sfLPtr[k] << diff)];
                    stereo = &stereoL;
                    current = &currentL;
                    currentL++;
                }
                else
                {
                    switch(win)
                    {
                        case 0:
                            sc = scaleS0[-((*sfSPtr0++) << diff)];
                            stereo = &stereoS0;
                            current = &currentS0;
                            currentS0++;
                            win = 1;
                            break;

                        case 1:
                            sc = scaleS1[-((*sfSPtr1++) << diff)];
                            stereo = &stereoS1;
                            current = &currentS1;
                            currentS1++;
                            win = 2;
                            break;

                        case 2:
                            sc = scaleS2[-((*sfSPtr2++) << diff)];
                            stereo = &stereoS2;
                            current = &currentS2;
                            currentS2++;
                            win = 0;
                            break;
                    }
                }
                cbBoundary = cb[k];
                k++;
            }

            xr[i] = sc * quarter[x];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;

            xr[i] = sc * quarter[y];
            if(xr[i]!=c_zeroSample)
            {
                *stereo = *current;
            }
            i++;
        }
    }

    while(i<(SBLIMIT * SSLIMIT))
    {
        xr[i++] = c_zeroSample;
    }

    m_stereoL = stereoL;
    m_stereoS0 = stereoS0;
    m_stereoS1 = stereoS1;
    m_stereoS2 = stereoS2;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
