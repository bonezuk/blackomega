#include "common/inc/BString.h"
#include "engine/silveromega/inc/VSilverFloorData0.h"
#include "engine/silveromega/inc/VSilverCodecData.h"
#include "engine/silveromega/inc/VSilverFunctions.h"
#include "engine/silveromega/inc/VSilverWindow.h"
#if defined(OMEGA_OGG_COMPARE)
#include "engine/inc/Compare.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverFloorData0::VSilverFloorData0(VSilverCodecInformation *cInfo,VSilverCodecData *cData) : VSilverFloorBase(cInfo,cData,e_floorData0),
    m_order(0),
    m_rate(0),
    m_barkmap(0),
    m_amplitudeBits(0),
    m_amplitudeOffset(0),
    m_numBooks(0),
    m_books(0)
{
    m_linearMap[0] = 0;
    m_linearMap[1] = 0;
    m_linearN[0] = 0;
    m_linearN[1] = 0;
}

//-------------------------------------------------------------------------------------------

VSilverFloorData0::~VSilverFloorData0()
{
    try
    {
        if(m_books!=0)
        {
            delete [] m_books;
            m_books = 0;
        }
        if(m_linearMap[0]!=0)
        {
            delete [] m_linearMap[0];
            m_linearMap[0] = 0;
        }
        if(m_linearMap[1]!=0)
        {
            delete [] m_linearMap[1];
            m_linearMap[1] = 0;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverFloorData0::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverFloorData0::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverFloorData0::isValid() const
{
    tint i;
    common::BString err;
    bool res = true;

    if(m_cData!=0)
    {
        printError("isValid","Codec data container not defined");
        return false;
    }
    if(m_order<1)
    {
        printError("isValid","Order must be greater than zero");
        res = false;
    }
    if(m_rate<1)
    {
        printError("isValid","Rate must be greater than zero");
        res = false;
    }
    if(m_barkmap<1)
    {
        printError("isValid","Bark map must be greater than zero");
        res = false;
    }
    if(m_books!=0)
    {
        for(i=0;i<m_numBooks;++i)
        {
            if(m_books[i]<0 || m_books[i]>=m_cData->m_numCodebooks)
            {
                err << "Book entry " << common::BString::Int(i) << " is out of range";
                printError("isValid",static_cast<const tchar *>(err));
                res = false;
            }
        }
    }
    else
    {
        printError("isValid","No books defined");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverFloorData0::read(engine::Sequence *seq)
{
    tint i;

    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }

    m_order = seq->readBits(8);
    m_rate = seq->readBits(16);
    m_barkmap = seq->readBits(16);
    m_amplitudeBits = seq->readBits(6);
    m_amplitudeOffset = seq->readBits(8);
    m_numBooks = seq->readBits(4) + 1;
    m_books = new tint [static_cast<tuint>(m_numBooks)];
    ::memset(m_books,0,static_cast<tuint>(m_numBooks) << 2);
    for(i=0;i<m_numBooks;++i)
    {
        m_books[i] = seq->readBits(8);
    }
    if(seq->isEnd())
    {
        printError("read","End of stream has been detected");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void *VSilverFloorData0::decode(VSilverCodecData *info,engine::Sequence *seq)
{
    VSilverCodebook *codebook;
    tint i=0,j,amp,book;
    tfloat32 *entry;
    tfloat32 *lsp,last=0.0f;

    amp = seq->readBits(m_amplitudeBits);
    if(amp>0)
    {
        book = seq->readBits(iLog(m_numBooks));
        if(book>=0 && book<m_numBooks)
        {
            codebook = info->m_codebooks[m_books[book]];

            lsp = reinterpret_cast<tfloat32 *>(malloc((m_order + codebook->m_dimensions + 1) * sizeof(tfloat32)));

            while(i<m_order)
            {
                entry = codebook->decode1(seq);
                for(j=0;j<codebook->m_dimensions;++j,++i)
                {
                    lsp[i] = entry[j];
                }
            }

            for(i=0;i<m_order;)
            {
                for(j=0;j<codebook->m_dimensions;++j,++i)
                {
                    lsp[i] += last;
                }
                last = lsp[i - 1];
            }

            lsp[m_order] = static_cast<tfloat32>(amp) / static_cast<tfloat32>((1 << m_amplitudeBits) - 1) * static_cast<tfloat32>(m_amplitudeOffset);

#if defined(OMEGA_OGG_COMPARE)
            if(g_Compare.isThreadA())
            {
                int frame = g_Compare.frameA();
                g_Compare.compareA(lsp,m_order + 1,false);
                frame = g_Compare.frameA();
            }
            else
            {
                int frame = g_Compare.frameB();
                g_Compare.compareB(lsp,m_order + 1,false);
                frame = g_Compare.frameB();
            }
#endif

            return reinterpret_cast<void *>(lsp);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

void VSilverFloorData0::lspToCurve(tfloat32 *curve,tint *map,tint n,tint ln,tfloat32 *lsp,tint m,tfloat32 amp,tfloat32 ampoffset)
{
    tfloat32 p,q,w;
    tint i,k,qexp,c;
    tfloat32 *ftmp,wdel;

    wdel = c_PI / static_cast<tfloat32>(ln);

    for(i=0;i<m;++i)
    {
        lsp[i] = lookCos(lsp[i]);
    }

    i = 0;
    while(i<n)
    {
        k = map[i];
        p = c_PI2_8;
        q = c_PI2_8;
        w = lookCos(wdel * static_cast<tfloat32>(k));
        ftmp = lsp;
        c = m >> 1;

        do
        {
            q *= ftmp[0] - w;
            p *= ftmp[1] - w;
            ftmp += 2;
        } while(--c);

        if(m&1)
        {
            q *= ftmp[0] - w;
            q *= q;
            p *= p * (1.0f - (w * w));
        }
        else
        {
            q *= q * (1.0f + w);
            p *= p * (1.0f - w);
        }

        q = static_cast<tfloat32>(::frexp(p + q,&qexp));
        q = lookFromDB(amp * lookInvSQ(q) * lookInvSQ2Exp(qexp + m) - ampoffset);

        do
        {
            curve[i++] *= q;
        } while(map[i]==k);
    }
}

//-------------------------------------------------------------------------------------------

#define toBARK(n) (static_cast<tfloat32>( 13.1 * ::atan(0.00074 * static_cast<tfloat64>(n)) + 2.24 * ::atan(static_cast<tfloat64>((n)*(n)) * 1.85e-8) + 1e-4 * static_cast<tfloat64>(n) ))

void VSilverFloorData0::initLSPMap(VSilverWindow *win)
{
    tint j,W,n,val;
    tfloat32 scale;

    W = m_cData->m_modes[win->m_mode]->m_blockFlag;

    if(m_linearMap[W]==0)
    {
        if(!W)
        {
            n = m_cInformation->m_blockSize_0 / 2;
        }
        else
        {
            n = m_cInformation->m_blockSize_1 / 2;
        }
        scale = static_cast<tfloat32>(m_barkmap) / toBARK(static_cast<tfloat32>(m_cInformation->m_audioSampleRate) / 2.0f);
        m_linearMap[W] = new tint [n + 1];

        for(j=0;j<n;++j)
        {
            val = static_cast<tint>(::floor(toBARK((static_cast<tfloat32>(m_cInformation->m_audioSampleRate) / 2.0f) / static_cast<tfloat32>(n) * static_cast<tfloat32>(j)) * scale));
            if(val >= m_barkmap)
            {
                val = m_barkmap - 1;
            }
            m_linearMap[W][j] = val;
        }
        m_linearMap[W][j] = -1;
        m_linearN[W] = n;
    }
}

//-------------------------------------------------------------------------------------------

void VSilverFloorData0::product(VSilverWindow *win,void *memo,tfloat32 *out)
{
    tint blockFlag = m_cData->m_modes[win->m_mode]->m_blockFlag;

    initLSPMap(win);

    if(memo!=0)
    {
        tfloat32 *lsp = reinterpret_cast<tfloat32 *>(memo);
        tfloat32 amp = lsp[m_order];

        lspToCurve(out,m_linearMap[blockFlag],m_linearN[blockFlag],m_barkmap,lsp,m_order,amp,static_cast<tfloat32>(m_amplitudeOffset));
    }
    else
    {
        ::memset(out,0,sizeof(tfloat32) * m_linearN[blockFlag]);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
