#include "engine/whiteomega/inc/HCR.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

const tint HCR::m_codebookPriority[32] = {
    -1, 21, 21, 20, 20, 19, 19, 18,
    18, 17, 17,  0, -1, -1, -1, -1,
    16, 15, 14, 13, 12, 11, 10,  9,
     8,  7,  6,  5,  4,  3,  2,  1
};

//-------------------------------------------------------------------------------------------

const tint HCR::m_maxCWLen[32] = {
     0, 11,  9, 20, 16, 13, 11, 14,
    12, 17, 14, 49,  0,  0,  0,  0,
    14, 17, 21, 21, 25, 25, 29, 29,
    29, 29, 33, 33, 33, 37, 37, 41
};

//-------------------------------------------------------------------------------------------

HCR::HCR() : m_lengthOfReorderedSpectralData(0),
    m_lengthOfLongestCodeword(0),
    m_noCW(0),
    m_noSegments(0)
{}

//-------------------------------------------------------------------------------------------

bool HCR::read(WSequence *seq,ICSInfo *info,GAConfig *config,tint *spectralData)
{
    tint cwIdx = 0;
    bool direction = false;

    m_lengthOfReorderedSpectralData = seq->readBitsI(14);
    m_lengthOfLongestCodeword = seq->readBitsI(6);

    preSort(info,config);
    calcCodewords();
    calcSegments(seq);

    while(cwIdx < m_noCW)
    {
        seq->readHCRSet(m_codewords,cwIdx,m_noCW,m_segments,m_noSegments,spectralData,direction);
        direction = (direction) ? false : true;
    }

    return true;
}

//-------------------------------------------------------------------------------------------

void HCR::quickSort(UInfo *units,tint len)
{
    quickSort(units,0,len);
}

//-------------------------------------------------------------------------------------------

void HCR::quickSort(UInfo *units,tint lo,tint hi)
{
    tint i = lo,j = hi;
    tint x = units[(lo + hi) >> 1].priority;

    do
    {
        while(units[i].priority < x)
        {
            i++;
        }
        while(units[j].priority > x)
        {
            j--;
        }
        if(i <= j)
        {
            qSswap(&units[i],&units[j]);
            i++;
            j--;
        }
    } while(i <= j);

    if(lo < j)
    {
        quickSort(units,lo,j);
    }
    if(i < hi)
    {
        quickSort(units,i,hi);
    }
}

//-------------------------------------------------------------------------------------------

void HCR::qSswap(UInfo *a,UInfo *b)
{
    UInfo tmp = *a;
    *a = *b;
    *b = tmp;
}

//-------------------------------------------------------------------------------------------

void HCR::preSort(ICSInfo *info,GAConfig *gaConfig)
{
    tint g,i,j,p,pEnd,cb;
    tint winStart = 0,winEnd,window;
    tint nShorts = (gaConfig->m_frameLength >> 3);
    tint nrOfFirstLineInUnit;
    tint maxNrOfWindows = (info->windowSequence==EIGHT_SHORT_SEQUENCE) ? 8 : 1;
    tint maxNrOfLinesInWindow = (info->windowSequence==EIGHT_SHORT_SEQUENCE) ? 128 : 1024;

    p = 0;
    for(g=0;g<info->numWindowGroups;++g)
    {
        if(info->windowSequence==EIGHT_SHORT_SEQUENCE)
        {
            pEnd = p + ((info->windowGroupLength[g] * nShorts) >> 2);
        }
        else
        {
            pEnd = gaConfig->m_frameLength >> 2;
        }

        winEnd = winStart + info->windowGroupLength[g];
        nrOfFirstLineInUnit = 0;

        for(i=0;i<info->maxSfb;++i)
        {
            tint len,width,cbPriority;

            cb = info->sfbCb[g][i];
            len = info->sectSfbOffset[g][i+1] - info->sectSfbOffset[g][i];

            if(m_codebookPriority[cb] >= 0)
            {
                cbPriority = m_codebookPriority[cb] * maxNrOfLinesInWindow;
                width = len / info->windowGroupLength[g];
                window = winStart;
                for(window=winStart;window<winEnd;++window)
                {
                    for(j=0;j<width;j+=4)
                    {
                        m_units[p].priority = ((cbPriority + nrOfFirstLineInUnit) * maxNrOfWindows) + window;
                        m_units[p].codebook = cb;
                        m_units[p].position = p;
                        nrOfFirstLineInUnit += 4;
                        p++;
                    }
                }
            }
            else
            {
                for(j=0;j<len;j+=4)
                {
                    m_units[p].priority = 999999;
                    m_units[p].codebook = cb;
                    m_units[p].position = p;
                    nrOfFirstLineInUnit += 4;
                    p++;
                }
            }
        }

        while(p < pEnd)
        {
            m_units[p].priority = 999999;
            m_units[p].codebook = -1;
            m_units[p].position = p;
            p++;
        }
        p = pEnd;
    }

    m_units[p].priority = 999999;
    m_units[p].codebook = -1;
    m_units[p].position = -1;

    quickSort(m_units,pEnd);
}

//-------------------------------------------------------------------------------------------

void HCR::calcCodewords()
{
    tint i,j,cb,ps;

    for(i=0,j=0;m_units[i].priority!=999999;++i)
    {
        cb = m_units[i].codebook;
        ps = m_units[i].position;

        m_codewords[j].codebook = cb;
        m_codewords[j].position = ps;
        j++;

        if(cb >= 5)
        {
            m_codewords[j].codebook = cb;
            m_codewords[j].position = ps + 2;
            j++;
        }
    }
    m_noCW = j;
}

//-------------------------------------------------------------------------------------------

void HCR::calcSegments(WSequence *seq)
{
    tint len, i = 0, k = 0, offset = 0,end, bsOffset = seq->offset();
    UInfo *u = m_units;

    m_segments[i].start = 0;
    while(offset < m_lengthOfReorderedSpectralData)
    {
        if(u->priority != 999999)
        {
            len = minV(m_maxCWLen[u->codebook],m_lengthOfLongestCodeword);
            end = offset + len;
            if(end < m_lengthOfReorderedSpectralData)
            {
                m_segments[i].start = bsOffset + offset;
                m_segments[i].end   = bsOffset + end - 1;
                i++;
            }
            else
            {
                m_segments[i].end = bsOffset + m_lengthOfReorderedSpectralData - 1;
            }
            offset = end;

            k += (u->codebook < 5) ? 4 : 2;
            if(k>=4)
            {
                k = 0;
                u++;
            }
        }
        else
        {
            break;
        }
    }
    m_noSegments = i + 1;
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
