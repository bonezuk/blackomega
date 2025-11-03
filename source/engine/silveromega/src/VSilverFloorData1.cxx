#include "engine/silveromega/inc/VSilverFloorData1.h"
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

VSilverFloorData1::VSilverFloorData1(VSilverCodecInformation *cInfo,VSilverCodecData *cData) : VSilverFloorBase(cInfo,cData,e_floorData1),
    m_partitions(0),
    m_partitionClass(0),
    m_multiplier(0),
    m_maxClass(-1),
    m_count(0),
    m_posts(0),
    m_read(false)
{
    ::memset(m_classDimensions,0,sizeof(tint) * 16);
    ::memset(m_classSub,0,sizeof(tint) * 16);
    ::memset(m_classBook,0,sizeof(tint) * 16);
    ::memset(m_classSubbook,0,sizeof(tint) * 16 * 8);
    ::memset(m_postList,0,sizeof(tint) * 65);
    ::memset(m_loNeighbour,0,sizeof(tint) * 63);
    ::memset(m_hiNeighbour,0,sizeof(tint) * 63);
}

//-------------------------------------------------------------------------------------------

VSilverFloorData1::~VSilverFloorData1()
{
    if(m_partitionClass!=0)
    {
        delete [] m_partitionClass;
        m_partitionClass = 0;
    }
}

//-------------------------------------------------------------------------------------------

void VSilverFloorData1::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverFloorData1::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverFloorData1::isValid() const
{
    if(!m_read)
    {
        printError("isValid","Data has not been read from bitstream");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverFloorData1::read(engine::Sequence *seq)
{
    tint i,j,rangebits,count = 0;

    if(seq==0)
    {
        printError("read","No stream instance given");
        return false;
    }

    m_partitions = seq->readBits(5);
    if(!m_partitions)
    {
        m_multiplier = seq->readBits(2) + 1;
        rangebits = seq->readBits(4);
    }
    else
    {
        m_partitionClass = new tint [static_cast<tuint>(m_partitions)];
        ::memset(m_partitionClass,0,static_cast<tuint>(m_partitions) << 2);

        for(i=0;i<m_partitions;++i)
        {
            m_partitionClass[i] = seq->readBits(4);
            if(m_maxClass < m_partitionClass[i])
            {
                m_maxClass = m_partitionClass[i];
            }
        }

        for(i=0;i<m_maxClass+1;++i)
        {
            m_classDimensions[i] = seq->readBits(3) + 1;
            m_classSub[i] = seq->readBits(2);
            if(m_classSub[i])
            {
                m_classBook[i] = seq->readBits(8);
                if(m_classBook[i]>=m_cData->m_numCodebooks)
                {
                    printError("read","Class book out of range");
                    return false;
                }
            }
            for(j=0;j<(1<<m_classSub[i]);++j)
            {
                m_classSubbook[i][j] = seq->readBits(8) - 1;
                if(m_classSubbook[i][j]>=m_cData->m_numCodebooks)
                {
                    printError("read","Class subbook out of range return false");
                }
            }
        }

        m_multiplier = seq->readBits(2) + 1;
        rangebits = seq->readBits(4);

        for(i=0,j=0;i<m_partitions;++i)
        {
            count += m_classDimensions[m_partitionClass[i]];
            while(j<count)
            {
                m_postList[j+2] = seq->readBits(rangebits);
                j++;
            }
        }
        m_postList[0] = 0;
        m_postList[1] = 1 << rangebits;
        m_count = count + 2;
    }

    if(seq->isEnd())
    {
        printError("read","End of stream has been detected");
        return false;
    }

    m_read = true;
    return true;
}

//-------------------------------------------------------------------------------------------

static tint setupIComp(const void *a,const void *b)
{
    return static_cast<tint>(**(size_t **)a-**(size_t **)b);
}

//-------------------------------------------------------------------------------------------

bool VSilverFloorData1::setup()
{
    tint j,x;
    tint i,lo,hi,lx,hx,n,currentx;
    tint *sortIndex[65];

    for(i=0,m_posts=0;i<m_partitions;++i)
    {
        m_posts += m_classDimensions[m_partitionClass[i]];
    }
    m_posts += 2;

    for(i=0;i<m_posts;i++)
    {
        sortIndex[i] = m_postList + i;
    }
    ::qsort(sortIndex,m_posts,sizeof(size_t),setupIComp);

    for(i=0;i<m_posts;++i)
    {
        m_forwardIndex[i] = sortIndex[i] - m_postList;
    }

    n = m_count - 2;
    for(i=0;i<n;++i)
    {
        lo = 0;
        hi = 1;
        lx = 0;
        hx = m_postList[1];
        currentx = m_postList[i + 2];

        for(j=0;j<(i+2);++j)
        {
            x = m_postList[j];
            if(x>lx && x<currentx)
            {
                lo = j;
                lx = x;
            }
            if(x<hx && x>currentx)
            {
                hi = j;
                hx = x;
            }
        }
        m_loNeighbour[i] = lo;
        m_hiNeighbour[i] = hi;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void *VSilverFloorData1::decode(VSilverCodecData *info,engine::Sequence *seq)
{
    static const tint vectorRange[4] = {256, 128, 86, 64};

    tint i,j,book,range,len,offset;
    tint ccla,cdim,cbit,csub,cval;
    tint *fit = 0;

    if(seq->readBit())
    {
        fit = reinterpret_cast<tint *>(malloc(m_posts * sizeof(tint)));

        range = vectorRange[m_multiplier - 1];

        len = iLog(range - 1);
        fit[0] = seq->readBits(len);
        fit[1] = seq->readBits(len);

        offset = 2;

        for(i=0;i<m_partitions;++i)
        {
            ccla = m_partitionClass[i];
            cdim = m_classDimensions[ccla];
            cbit = m_classSub[ccla];
            csub = 1 << cbit;
            cval = 0;

            if(cbit)
            {
                cval = info->m_codebooks[m_classBook[ccla]]->decode0(seq);
            }

            for(j=0;j<cdim;++j)
            {
                book = m_classSubbook[ccla][cval & (csub-1)];
                cval >>= cbit;
                if(book>=0)
                {
                    fit[j + offset] = info->m_codebooks[book]->decode0(seq);
                }
                else
                {
                    fit[j + offset] = 0;
                }
            }
            offset += cdim;
        }

        {
            tint val;
            tint predicted,hiroom,loroom,room,loOffset,hiOffset;

            for(i=2;i<m_count;i++)
            {
                loOffset = m_loNeighbour[i - 2];
                hiOffset = m_hiNeighbour[i - 2];

                predicted = renderPoint(m_postList[loOffset],m_postList[hiOffset],fit[loOffset],fit[hiOffset],m_postList[i]);

                hiroom = range - predicted;
                loroom = predicted;
                room = ((hiroom < loroom) ? hiroom : loroom) << 1;
                val = fit[i];

                if(val)
                {
                    if(val >= room)
                    {
                        if(hiroom > loroom)
                        {
                            val -= loroom;
                        }
                        else
                        {
                            val = -1-(val-hiroom);
                        }
                    }
                    else
                    {
                        if(val & 1)
                        {
                            val = -((val + 1) >> 1);
                        }
                        else
                        {
                            val >>= 1;
                        }
                    }

                    fit[i] = val + predicted;
                    fit[m_loNeighbour[i-2]] &= 0x00007fff;
                    fit[m_hiNeighbour[i-2]] &= 0x00007fff;
                }
                else
                {
                    fit[i] = 0x00008000 | predicted;
                }
            }
        }

#if defined(OMEGA_OGG_COMPARE)
        if(g_Compare.isThreadA())
        {
            tint frame = g_Compare.frameA();
            g_Compare.compareA(fit,m_posts);
            frame = g_Compare.frameA();
        }
        else
        {
            tint frame = g_Compare.frameB();
            g_Compare.compareB(fit,m_posts);
            frame = g_Compare.frameB();
        }
#endif
    }
    return fit;
}

//-------------------------------------------------------------------------------------------

const tfloat32 c_FLOOR1_fromdB_LOOKUP[256]={
    1.0649863e-07F, 1.1341951e-07F, 1.2079015e-07F, 1.2863978e-07F,
    1.3699951e-07F, 1.4590251e-07F, 1.5538408e-07F, 1.6548181e-07F,
    1.7623575e-07F, 1.8768855e-07F, 1.9988561e-07F, 2.128753e-07F,
    2.2670913e-07F, 2.4144197e-07F, 2.5713223e-07F, 2.7384213e-07F,
    2.9163793e-07F, 3.1059021e-07F, 3.3077411e-07F, 3.5226968e-07F,
    3.7516214e-07F, 3.9954229e-07F, 4.2550680e-07F, 4.5315863e-07F,
    4.8260743e-07F, 5.1396998e-07F, 5.4737065e-07F, 5.8294187e-07F,
    6.2082472e-07F, 6.6116941e-07F, 7.0413592e-07F, 7.4989464e-07F,
    7.9862701e-07F, 8.5052630e-07F, 9.0579828e-07F, 9.6466216e-07F,
    1.0273513e-06F, 1.0941144e-06F, 1.1652161e-06F, 1.2409384e-06F,
    1.3215816e-06F, 1.4074654e-06F, 1.4989305e-06F, 1.5963394e-06F,
    1.7000785e-06F, 1.8105592e-06F, 1.9282195e-06F, 2.0535261e-06F,
    2.1869758e-06F, 2.3290978e-06F, 2.4804557e-06F, 2.6416497e-06F,
    2.8133190e-06F, 2.9961443e-06F, 3.1908506e-06F, 3.3982101e-06F,
    3.6190449e-06F, 3.8542308e-06F, 4.1047004e-06F, 4.3714470e-06F,
    4.6555282e-06F, 4.9580707e-06F, 5.2802740e-06F, 5.6234160e-06F,
    5.9888572e-06F, 6.3780469e-06F, 6.7925283e-06F, 7.2339451e-06F,
    7.7040476e-06F, 8.2047000e-06F, 8.7378876e-06F, 9.3057248e-06F,
    9.9104632e-06F, 1.0554501e-05F, 1.1240392e-05F, 1.1970856e-05F,
    1.2748789e-05F, 1.3577278e-05F, 1.4459606e-05F, 1.5399272e-05F,
    1.6400004e-05F, 1.7465768e-05F, 1.8600792e-05F, 1.9809576e-05F,
    2.1096914e-05F, 2.2467911e-05F, 2.3928002e-05F, 2.5482978e-05F,
    2.7139006e-05F, 2.8902651e-05F, 3.0780908e-05F, 3.2781225e-05F,
    3.4911534e-05F, 3.7180282e-05F, 3.9596466e-05F, 4.2169667e-05F,
    4.4910090e-05F, 4.7828601e-05F, 5.0936773e-05F, 5.4246931e-05F,
    5.7772202e-05F, 6.1526565e-05F, 6.5524908e-05F, 6.9783085e-05F,
    7.4317983e-05F, 7.9147585e-05F, 8.4291040e-05F, 8.9768747e-05F,
    9.5602426e-05F, 0.00010181521F, 0.00010843174F, 0.00011547824F,
    0.00012298267F, 0.00013097477F, 0.00013948625F, 0.00014855085F,
    0.00015820453F, 0.00016848555F, 0.00017943469F, 0.00019109536F,
    0.00020351382F, 0.00021673929F, 0.00023082423F, 0.00024582449F,
    0.00026179955F, 0.00027881276F, 0.00029693158F, 0.00031622787F,
    0.00033677814F, 0.00035866388F, 0.00038197188F, 0.00040679456F,
    0.00043323036F, 0.00046138411F, 0.00049136745F, 0.00052329927F,
    0.00055730621F, 0.00059352311F, 0.00063209358F, 0.00067317058F,
    0.00071691700F, 0.00076350630F, 0.00081312324F, 0.00086596457F,
    0.00092223983F, 0.00098217216F, 0.0010459992F, 0.0011139742F,
    0.0011863665F, 0.0012634633F, 0.0013455702F, 0.0014330129F,
    0.0015261382F, 0.0016253153F, 0.0017309374F, 0.0018434235F,
    0.0019632195F, 0.0020908006F, 0.0022266726F, 0.0023713743F,
    0.0025254795F, 0.0026895994F, 0.0028643847F, 0.0030505286F,
    0.0032487691F, 0.0034598925F, 0.0036847358F, 0.0039241906F,
    0.0041792066F, 0.0044507950F, 0.0047400328F, 0.0050480668F,
    0.0053761186F, 0.0057254891F, 0.0060975636F, 0.0064938176F,
    0.0069158225F, 0.0073652516F, 0.0078438871F, 0.0083536271F,
    0.0088964928F, 0.009474637F, 0.010090352F, 0.010746080F,
    0.011444421F, 0.012188144F, 0.012980198F, 0.013823725F,
    0.014722068F, 0.015678791F, 0.016697687F, 0.017782797F,
    0.018938423F, 0.020169149F, 0.021479854F, 0.022875735F,
    0.024362330F, 0.025945531F, 0.027631618F, 0.029427276F,
    0.031339626F, 0.033376252F, 0.035545228F, 0.037855157F,
    0.040315199F, 0.042935108F, 0.045725273F, 0.048696758F,
    0.051861348F, 0.055231591F, 0.058820850F, 0.062643361F,
    0.066714279F, 0.071049749F, 0.075666962F, 0.080584227F,
    0.085821044F, 0.091398179F, 0.097337747F, 0.10366330F,
    0.11039993F, 0.11757434F, 0.12521498F, 0.13335215F,
    0.14201813F, 0.15124727F, 0.16107617F, 0.17154380F,
    0.18269168F, 0.19456402F, 0.20720788F, 0.22067342F,
    0.23501402F, 0.25028656F, 0.26655159F, 0.28387361F,
    0.30232132F, 0.32196786F, 0.34289114F, 0.36517414F,
    0.38890521F, 0.41417847F, 0.44109412F, 0.46975890F,
    0.50028648F, 0.53279791F, 0.56742212F, 0.60429640F,
    0.64356699F, 0.68538959F, 0.72993007F, 0.77736504F,
    0.82788260F, 0.88168307F, 0.9389798F, 1.F,
};

//-------------------------------------------------------------------------------------------

void VSilverFloorData1::renderLine(tint x0,tint x1,tint y0,tint y1,tfloat32 *d)
{
    tint dy = y1 - y0;
    tint adx = x1 - x0;
    tint ady = static_cast<tint>(::abs(dy));
    tint base = dy / adx;
    tint sy = (dy<0) ? base - 1 : base + 1;
    tint x = x0;
    tint y = y0;
    tint err = 0;

    ady -= static_cast<tint>(::abs(base * adx));

    d[x] *= c_FLOOR1_fromdB_LOOKUP[y];
    while(++x < x1)
    {
        err = err + ady;
        if(err>=adx)
        {
            err -= adx;
            y += sy;
        }
        else
        {
            y += base;
        }
        d[x] *= c_FLOOR1_fromdB_LOOKUP[y];
    }
}

//-------------------------------------------------------------------------------------------

void VSilverFloorData1::product(VSilverWindow *win,void *memo,tfloat32 *out)
{
    tint j,n;

    if(!m_cData->m_modes[win->m_mode]->m_blockFlag)
    {
        n = m_cInformation->m_blockSize_0 / 2;
    }
    else
    {
        n = m_cInformation->m_blockSize_1 / 2;
    }

    if(memo!=0)
    {
        tint *fitValue = reinterpret_cast<tint *>(memo);
        tint hx = 0;
        tint lx = 0;
        tint ly = fitValue[0] * m_multiplier;

        for(j=1;j<m_posts;++j)
        {
            tint current = m_forwardIndex[j];
            tint hy = fitValue[current] & 0x00007fff;

            if(hy==fitValue[current])
            {
                hy *= m_multiplier;
                hx = m_postList[current];

                renderLine(lx,hx,ly,hy,out);

                lx = hx;
                ly = hy;
            }
        }

        for(j=hx;j<n;++j)
        {
            out[j] *= c_FLOOR1_fromdB_LOOKUP[ly];
        }
    }
    else
    {
        ::memset(out,0,sizeof(tfloat32) * n);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

