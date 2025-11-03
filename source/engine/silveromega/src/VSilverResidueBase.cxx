#include "engine/silveromega/inc/VSilverResidueBase.h"
#include "engine/silveromega/inc/VSilverCodecData.h"
#include "engine/silveromega/inc/VSilverFunctions.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverResidueBase::VSilverResidueBase(VSilverCodecData *cData) : VSilverInfoObject(),
    m_type(0),
    m_begin(0),
    m_end(0),
    m_grouping(0),
    m_partitions(0),
    m_groupbook(0),
    m_secondstages(0),
    m_numBooklist(0),
    m_booklist(0),
    m_cData(cData),
    m_alloc(),
    m_stages(0),
    m_phrasebook(0),
    m_partbooks(0),
    m_partvals(0),
    m_decodeMap(0),
    m_n(0),
    m_resSamplesPerPartition(0),
    m_resPartitionsPerWord(0),
    m_resN(0),
    m_resPartvals(0),
    m_resPartwords(0),
    m_resPartword(0),
    m_res2Partword(0)
{}

//-------------------------------------------------------------------------------------------

VSilverResidueBase::~VSilverResidueBase()
{
    try
    {
        m_alloc.FreeAll();
        if(m_secondstages!=0)
        {
            delete [] m_secondstages;
            m_secondstages = 0;
        }
        if(m_booklist!=0)
        {
            delete [] m_booklist;
            m_booklist = 0;
        }
        m_cData = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverResidueBase::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverResidueBase::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverResidueBase::isValid() const
{
    tint i;
    bool res = true;

    if(m_cData==0)
    {
        printError("isValid","No data container given to instance");
        return false;
    }
    if(m_groupbook>=m_cData->m_numCodebooks)
    {
        printError("isValid","Class book out of range");
        res = false;
    }
    if(m_secondstages==0)
    {
        printError("isValid","Partition bitmap not defined");
        res = false;
    }

    for(i=0;i<m_numBooklist;++i)
    {
        if(m_booklist==0)
        {
            printError("isValid","No booklist has been allocated");
            res = false;
        }
        else
        {
            if(m_booklist[i]>=m_cData->m_numCodebooks)
            {
                printError("isValid","Book list entry out of range");
                res = false;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool VSilverResidueBase::read(engine::Sequence *seq)
{
    tint i,cascade,acc=0;

    if(seq==0)
    {
        printError("read","No sequence instance given");
        return false;
    }

    m_type = seq->readBits(16);
    if(m_type<0 || m_type>2)
    {
        printError("read","Unknown residue type");
        return false;
    }

    m_begin = seq->readBits(24);
    m_end = seq->readBits(24);
    m_grouping = seq->readBits(24) + 1;
    m_partitions = seq->readBits(6) + 1;
    m_groupbook = seq->readBits(8);

    m_secondstages = new tint [static_cast<tuint>(m_partitions)];
    ::memset(m_secondstages,0,static_cast<tuint>(m_partitions) << 2);
    for(i=0;i<m_partitions;++i)
    {
        cascade = seq->readBits(3);
        if(seq->readBit())
        {
            cascade |= static_cast<tint>(static_cast<tuint>(seq->readBits(5)) << 3);
        }
        m_secondstages[i] = cascade;
        acc += iCount(cascade);
    }
    m_numBooklist = acc;

    m_booklist = new tint [static_cast<tuint>(acc) + 1];
    ::memset(m_booklist,0,static_cast<tint>(static_cast<tuint>(acc + 1) << 2));
    for(i=0;i<acc;++i)
    {
        m_booklist[i] = seq->readBits(8);
    }

    if(seq->isEnd())
    {
        printError("read","End of stream has been detected");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverResidueBase::setup()
{
    tint i,j,dim,stages,maxStage=0,acc=0;
    long val,mult,deco;

    m_phrasebook = m_cData->m_codebooks[m_groupbook];
    dim = m_phrasebook->m_dimensions;

    m_partbooks = reinterpret_cast<VSilverCodebook ***>(m_alloc.MemAlloc(m_partitions,sizeof(VSilverCodebook **)));

    for(i=0;i<m_partitions;++i)
    {
        stages = iLog(m_secondstages[i]);
        if(stages)
        {
            if(stages>maxStage)
            {
                maxStage = stages;
            }
            m_partbooks[i] = reinterpret_cast<VSilverCodebook **>(m_alloc.MemAlloc(stages,sizeof(VSilverCodebook *)));
            for(j=0;j<stages;++j)
            {
                if(m_secondstages[i] & (1<<j))
                {
                    m_partbooks[i][j] = m_cData->m_codebooks[m_booklist[acc++]];
                }
            }
        }
    }
    m_stages = maxStage;

    m_partvals = rint(pow(static_cast<tfloat32>(m_partitions),static_cast<tfloat32>(dim)));

    m_decodeMap = reinterpret_cast<tint **>(m_alloc.MemAlloc(m_partvals,sizeof(tint *)));

    for(i=0;i<m_partvals;++i)
    {
        m_decodeMap[i] = reinterpret_cast<tint *>(m_alloc.MemAlloc(dim,sizeof(tint)));

        val = i;
        mult = m_partvals / m_partitions;

        for(j=0;j<dim;++j)
        {
            deco = val / mult;
            val -= deco * mult;
            mult /= m_partitions;
            m_decodeMap[i][j] = deco;
        }
    }

    m_resSamplesPerPartition = m_grouping;
    m_resPartitionsPerWord = m_phrasebook->m_dimensions;
    m_resN = m_end - m_begin;
    m_resPartvals = m_resN / m_resSamplesPerPartition;
    m_resPartwords = (m_resPartvals + m_resPartitionsPerWord - 1) / m_resPartitionsPerWord;

    if(m_type==2)
    {
        m_res2Partword = reinterpret_cast<tint **>(m_alloc.MemAlloc(m_resPartwords,sizeof(tint *)));
    }
    else
    {
        m_resPartword = reinterpret_cast<tint ***>(m_alloc.MemAlloc(256,sizeof(tint **)));
        for(i=0;i<256;++i)
        {
            m_resPartword[i] = reinterpret_cast<tint **>(m_alloc.MemAlloc(m_resPartwords,sizeof(tint *)));
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------

void VSilverResidueBase::decode0(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch)
{
    tint i,j,k,m,s,t,tmp,offset,lookup;
    VSilverCodebook *stagebook;
    tint r,used=0;

    for(r=0;r<ch;++r)
    {
        if(nonzero[r])
        {
            out[used++] = out[r];
        }
    }
    if(!used)
    {
        return;
    }

    for(s=0;s<m_stages;++s)
    {
        t = 1 << s;

        for(i=0,j=0;i<m_resPartvals;++j)
        {
            if(s==0)
            {
                for(k=0;k<ch;++k)
                {
                    tmp = m_phrasebook->decode0(seq);
                    if(tmp>=0 && tmp<m_partvals)
                    {
                        m_resPartword[k][j] = m_decodeMap[tmp];
                    }
                    else
                    {
                        return;
                    }
                }
            }

            for(k=0;k<m_resPartitionsPerWord && i<m_resPartvals;k++,i++)
            {
                offset = m_begin + (i * m_resSamplesPerPartition);
                for(m=0;m<ch;++m)
                {
                    lookup = m_resPartword[m][j][k];

                    if(m_secondstages[lookup] & t)
                    {
                        stagebook = m_partbooks[lookup][s];
                        if(stagebook!=0)
                        {
                            stagebook->decodeResidue0(seq,out[m] + offset,m_resSamplesPerPartition);
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverResidueBase::decode1(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch)
{
    tint i,j,k,m,s,t,tmp,offset,lookup;
    VSilverCodebook *stagebook;
    tint r,used=0;

    for(r=0;r<ch;++r)
    {
        if(nonzero[r])
        {
            out[used++] = out[r];
        }
    }
    if(!used)
    {
        return;
    }

    for(s=0;s<m_stages;++s)
    {
        t = 1 << s;

        for(i=0,j=0;i<m_resPartvals;++j)
        {
            if(s==0)
            {
                for(k=0;k<ch;++k)
                {
                    tmp = m_phrasebook->decode0(seq);
                    if(tmp>=0 && tmp<m_partvals)
                    {
                        m_resPartword[k][j] = m_decodeMap[tmp];
                    }
                    else
                    {
                        return;
                    }
                }
            }

            for(k=0;k<m_resPartitionsPerWord && i<m_resPartvals;++k,++i)
            {
                offset = m_begin + (i * m_resSamplesPerPartition);
                for(m=0;m<ch;++m)
                {
                    lookup = m_resPartword[m][j][k];

                    if(m_secondstages[lookup] & t)
                    {
                        stagebook = m_partbooks[lookup][s];
                        if(stagebook!=0)
                        {
                            stagebook->decodeResidue1(seq,out[m] + offset,m_resSamplesPerPartition);
                        }
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverResidueBase::decode2(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch)
{
    tint i,j,k,s,t,tmp;
    VSilverCodebook *stagebook;

    for(i=0;i<ch;++i)
    {
        if(nonzero[i])
        {
            break;
        }
    }
    if(i==ch)
    {
        return;
    }

    for(s=0;s<m_stages;++s)
    {
        t = 1 << s;

        for(i=0,j=0;i<m_resPartvals;++j)
        {
            if(s==0)
            {
                tmp = m_phrasebook->decode0(seq);

                if(tmp>=0 && tmp<m_partvals)
                {
                    m_res2Partword[j] = m_decodeMap[tmp];
                }
                else
                {
                    return;
                }
            }

            for(k=0;k<m_resPartitionsPerWord && i<m_resPartvals;++k,++i)
            {
                if(m_secondstages[m_res2Partword[j][k]] & t)
                {
                    stagebook = m_partbooks[m_res2Partword[j][k]][s];

                    if(stagebook!=0)
                    {
                        stagebook->decodeResidue2(seq,out,m_begin + (i * m_resSamplesPerPartition),ch,m_resSamplesPerPartition);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverResidueBase::decode(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch)
{
    switch(m_type)
    {
        case 0:
            decode0(seq,out,nonzero,ch);
            break;

        case 1:
            decode1(seq,out,nonzero,ch);
            break;

        case 2:
            decode2(seq,out,nonzero,ch);
            break;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
