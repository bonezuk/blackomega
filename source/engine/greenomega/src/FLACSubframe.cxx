#include "engine/greenomega/inc/FLACSubframe.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACSubframe::FLACSubframe(FLACMetaStreamInfo *info,FLACHeader *hdr,tint ch) : m_alloc(),
    m_streamInfo(info),
    m_header(hdr),
    m_channel(ch),
    m_outputSize(0),
    m_output(0),
    m_residual(0),
    m_resCapacityByOrder(0),
    m_resParameters(0),
    m_resRawBits(0)
{}

//-------------------------------------------------------------------------------------------

FLACSubframe::~FLACSubframe()
{
    m_alloc.FreeAll();
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::allocate()
{
    tint size = m_header->blockSize();
    
    if(size<=m_outputSize)
    {
        return true;
    }
    
    if(m_output!=0)
    {
        m_alloc.Free(m_output);
    }
    m_output = reinterpret_cast<tint *>(m_alloc.MemAllocAlign(size + 4,sizeof(tint),16));
    if(m_output==0)
    {
        return false;
    }
    ::memset(m_output,0,(size + 4) * sizeof(tint));
    
    if(m_residual!=0)
    {
        m_alloc.Free(m_residual);
    }
    m_residual = reinterpret_cast<tint *>(m_alloc.MemAllocAlign(size,sizeof(tint),16));
    if(m_residual==0)
    {
        return false;
    }
    ::memset(m_residual,0,size * sizeof(tint));
    
    m_outputSize = size;
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::read(Sequence *seq,tint bps)
{
    tint i,type,wastedBits;
    
    if(seq->readBitI())
    {
        return false;
    }
    type = seq->readBitsI(6);
    
    if(seq->readBitI())
    {
        wastedBits = 1;
        while(!seq->readBitI())
        {
            wastedBits++;
        }
        bps -= wastedBits;
    }
    else
    {
        wastedBits = 0;
    }
    
    if(type==0)
    {
        // constant
        if(!readConstant(seq,bps))
        {
            return false;
        }
    }
    else if(type==1)
    {
        // verbatim
        if(!readVerbatim(seq,bps))
        {
            return false;
        }
    }
    else if((type&0x00000008) && !(type & 0x00000030))
    {
        // fixed
        tint order = type & 0x00000007;
        if(order<=4)
        {
            if(!readFixed(seq,order,bps))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if(type & 0x00000020)
    {
        // lpc
        tint order = (type & 0x0000001f) + 1;
        if(!readLPC(seq,order,bps))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    
    if(wastedBits)
    {
        for(i=0;i<m_header->blockSize();i++)
        {
            m_output[i] <<= wastedBits;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::readConstant(Sequence *seq,tint bps)
{
    tint i,x,size = m_header->blockSize();
    
    x = seq->readBitsSignI(bps);
    for(i=0;i<size;i++)
    {
        m_output[i] = x;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::readVerbatim(Sequence *seq,tint bps)
{
    tint i,size = m_header->blockSize();
    for(i=0;i<size;i++)
    {
        m_output[i] = seq->readBitsSignI(bps);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::readFixed(Sequence *seq,tint order,tint bps)
{
    tint i,riceEncoding,riceOrder;
    tint warmup[4];
    
    for(i=0;i<order;i++)
    {
        warmup[i] = seq->readBitsSignI(bps);
    }
    
    riceEncoding = seq->readBitsI(2);
    if(riceEncoding>=2)
    {
        return false;
    }
    riceOrder = seq->readBitsI(4);
    if(!readResidualRice(seq,order,riceOrder,(riceEncoding) ? true : false))
    {
        return false;
    }
    
    if(order>0)
    {
        ::memcpy(m_output,warmup,order * sizeof(tint));
    }
    restoreResidual(m_residual,m_header->blockSize()-order,order,m_output+order);

    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::allocateResidualRice(Sequence *seq,tint maxOrder)
{
    if(static_cast<tuint>(maxOrder) > m_resCapacityByOrder)
    {
        if(m_resParameters!=0)
        {
            m_alloc.Free(m_resParameters);
        }
        m_resParameters = reinterpret_cast<tuint32 *>(m_alloc.MemAlloc(1 << maxOrder,sizeof(tuint)));
        if(m_resRawBits!=0)
        {
            m_alloc.Free(m_resRawBits);
        }
        m_resRawBits = reinterpret_cast<tuint32 *>(m_alloc.MemAlloc(1 << maxOrder,sizeof(tuint)));
        if(m_resParameters==0 || m_resRawBits==0)
        {
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::readResidualRice(Sequence *seq,tint predOrder,tint partOrder,bool extended)
{
    tint i,sample,parts = 1 << partOrder;
    tint partSamples = (partOrder>0) ? (m_header->blockSize() >> partOrder) : (m_header->blockSize() - predOrder);
    tint pLen = (extended) ? 5 : 4;
    tint pEsc = (extended) ? 0x0000001f : 0x0000000f;
    
    if(partOrder==0)
    {
        if(m_header->blockSize()<predOrder)
        {
            return false;
        }
    }
    else
    {
        if(partSamples<predOrder)
        {
            return false;
        }
    }
    if(!allocateResidualRice(seq,(partOrder<6) ? 6 : partOrder))
    {
        return false;
    }
    
    sample = 0;
    for(i=0;i<parts;i++)
    {
        tint u,p;
        
        p = seq->readBitsI(pLen);
        m_resParameters[i] = p;
        if(p < pEsc)
        {
            m_resRawBits[i] = 0;
            if(partOrder==0 || i>0)
            {
                u = partSamples;
            }
            else
            {
                u = partSamples - predOrder;
            }
            readRiceBlock(seq,m_residual + sample,u,p);
            sample += u;
        }
        else
        {
            m_resRawBits[i] = p;
            if(partOrder==0 || i>0)
            {
                u = 0;
            }
            else
            {
                u = predOrder;
            }
            for(;u<partSamples;u++,sample++)
            {
                m_residual[sample] = seq->readBitsSignI(p);
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void FLACSubframe::readRiceBlock(Sequence *seq,tint *sample,tint len,tint n)
{
    int i;
    
    for(i=0;i<len;i++)
    {
        tuint a = 0,b;
        
        while(!seq->readBitI())
        {
            a++;
        }
        b = seq->readBitsI(n);
        
        a = (a << n) | b;
        if(a & 1)
        {
            sample[i] = static_cast<int>((a >> 1) ^ 0xffffffff);
        }
        else
        {
            sample[i] = static_cast<int>(a >> 1);
        }
    }
}

//-------------------------------------------------------------------------------------------

void FLACSubframe::restoreResidual(const tint *residual,tint len,tint order,tint *data)
{
    tint i;
    
    switch(order)
    {
        case 0:
            ::memcpy(data,residual,sizeof(tint) * len);
            break;
            
        case 1:
            for(i=0;i<len;i++)
            {
                data[i] = residual[i] + data[i-1];
            }
            break;
            
        case 2:
            for(i=0;i<len;i++)
            {
                data[i] = residual[i] + (data[i-1]<<1) - data[i-2];
            }
            break;
            
        case 3:
            for(i=0;i<len;i++)
            {
                data[i] = residual[i] + (((data[i-1]-data[i-2])<<1) + (data[i-1]-data[i-2])) + data[i-3];
            }
            break;
            
        case 4:
            for(i=0;i<len;i++)
            {
                data[i] = residual[i] + ((data[i-1]+data[i-3])<<2) - ((data[i-2]<<2) + (data[i-2]<<1)) - data[i-4];
            }
            break;
            
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------

bool FLACSubframe::readLPC(Sequence *seq,tint order,tint bps)
{
    tint i;
    tint coeffPrecision;
    tint quantizationLevel;
    tint riceEncoding,riceOrder;
    tint qlfCoeff[32],warmup[32];
    
    for(i=0;i<order;i++)
    {
        warmup[i] = seq->readBitsSignI(bps);
    }
    
    coeffPrecision = seq->readBitsI(4) + 1;
    if(coeffPrecision==16)
    {
        return false;
    }
    quantizationLevel = seq->readBitsSignI(5);
    for(i=0;i<order;i++)
    {
        qlfCoeff[i] = seq->readBitsSignI(coeffPrecision);    
    }

    riceEncoding = seq->readBitsI(2);
    if(riceEncoding>=2)
    {
        return false;
    }
    riceOrder = seq->readBitsI(4);
    if(!readResidualRice(seq,order,riceOrder,(riceEncoding) ? true : false))
    {
        return false;
    }

    if(order>0)
    {
        ::memcpy(m_output,warmup,order * sizeof(tint));
    }
    if((bps+coeffPrecision+iLog2(order)) <= 32)
    {
        restoreLPCSignal(m_residual,m_header->blockSize()-order,qlfCoeff,order,quantizationLevel,m_output+order);
    }
    else
    {
        restoreLPCSignalWide(m_residual,m_header->blockSize()-order,qlfCoeff,order,quantizationLevel,m_output+order);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void FLACSubframe::restoreLPCSignal(const tint *residual,tint len,const tint *qCoeff,tint order,tint lpQuant,tint *data)
{
    tint i,sum;
    
    if(order<=12)
    {
        if(order > 8)
        {
            if(order > 10)
            {
                if(order==12)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[11] * data[i-12];
                        sum += qCoeff[10] * data[i-11];
                        sum += qCoeff[ 9] * data[i-10];
                        sum += qCoeff[ 8] * data[i- 9];
                        sum += qCoeff[ 7] * data[i- 8];
                        sum += qCoeff[ 6] * data[i- 7];
                        sum += qCoeff[ 5] * data[i- 6];
                        sum += qCoeff[ 4] * data[i- 5];
                        sum += qCoeff[ 3] * data[i- 4];
                        sum += qCoeff[ 2] * data[i- 3];
                        sum += qCoeff[ 1] * data[i- 2];
                        sum += qCoeff[ 0] * data[i- 1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
                else // order==11
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[10] * data[i-11];
                        sum += qCoeff[ 9] * data[i-10];
                        sum += qCoeff[ 8] * data[i- 9];
                        sum += qCoeff[ 7] * data[i- 8];
                        sum += qCoeff[ 6] * data[i- 7];
                        sum += qCoeff[ 5] * data[i- 6];
                        sum += qCoeff[ 4] * data[i- 5];
                        sum += qCoeff[ 3] * data[i- 4];
                        sum += qCoeff[ 2] * data[i- 3];
                        sum += qCoeff[ 1] * data[i- 2];
                        sum += qCoeff[ 0] * data[i- 1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
            }
            else
            {
                if(order==10)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[9] * data[i-10];
                        sum += qCoeff[8] * data[i- 9];
                        sum += qCoeff[7] * data[i- 8];
                        sum += qCoeff[6] * data[i- 7];
                        sum += qCoeff[5] * data[i- 6];
                        sum += qCoeff[4] * data[i- 5];
                        sum += qCoeff[3] * data[i- 4];
                        sum += qCoeff[2] * data[i- 3];
                        sum += qCoeff[1] * data[i- 2];
                        sum += qCoeff[0] * data[i- 1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
                else // order==9
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[8] * data[i-9];
                        sum += qCoeff[7] * data[i-8];
                        sum += qCoeff[6] * data[i-7];
                        sum += qCoeff[5] * data[i-6];
                        sum += qCoeff[4] * data[i-5];
                        sum += qCoeff[3] * data[i-4];
                        sum += qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
            }
        }
        else if(order > 4)
        {
            if(order > 6)
            {
                if(order==8)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[7] * data[i-8];
                        sum += qCoeff[6] * data[i-7];
                        sum += qCoeff[5] * data[i-6];
                        sum += qCoeff[4] * data[i-5];
                        sum += qCoeff[3] * data[i-4];
                        sum += qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
                else // order==7
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[6] * data[i-7];
                        sum += qCoeff[5] * data[i-6];
                        sum += qCoeff[4] * data[i-5];
                        sum += qCoeff[3] * data[i-4];
                        sum += qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
            }
            else
            {
                if(order==6)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[5] * data[i-6];
                        sum += qCoeff[4] * data[i-5];
                        sum += qCoeff[3] * data[i-4];
                        sum += qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
                else // order==5
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[4] * data[i-5];
                        sum += qCoeff[3] * data[i-4];
                        sum += qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
            }
        }
        else
        {
            if(order > 2)
            {
                if(order==4)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[3] * data[i-4];
                        sum += qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
                else // order==3
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[2] * data[i-3];
                        sum += qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
            }
            else
            {
                if(order==2)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = qCoeff[1] * data[i-2];
                        sum += qCoeff[0] * data[i-1];
                        data[i] = residual[i] + (sum >> lpQuant);
                    }
                }
                else // order == 1
                {
                    for(i=0;i<len;i++)
                    {
                        data[i] = residual[i] + ((qCoeff[0] * data[i-1]) >> lpQuant);
                    }
                }
            }
        }
    }
    else // order > 12
    {
        for(i=0;i<len;i++)
        {
            sum = 0;
            switch(order) {
                case 32: sum += qCoeff[31] * data[i-32];
                case 31: sum += qCoeff[30] * data[i-31];
                case 30: sum += qCoeff[29] * data[i-30];
                case 29: sum += qCoeff[28] * data[i-29];
                case 28: sum += qCoeff[27] * data[i-28];
                case 27: sum += qCoeff[26] * data[i-27];
                case 26: sum += qCoeff[25] * data[i-26];
                case 25: sum += qCoeff[24] * data[i-25];
                case 24: sum += qCoeff[23] * data[i-24];
                case 23: sum += qCoeff[22] * data[i-23];
                case 22: sum += qCoeff[21] * data[i-22];
                case 21: sum += qCoeff[20] * data[i-21];
                case 20: sum += qCoeff[19] * data[i-20];
                case 19: sum += qCoeff[18] * data[i-19];
                case 18: sum += qCoeff[17] * data[i-18];
                case 17: sum += qCoeff[16] * data[i-17];
                case 16: sum += qCoeff[15] * data[i-16];
                case 15: sum += qCoeff[14] * data[i-15];
                case 14: sum += qCoeff[13] * data[i-14];
                case 13: sum += qCoeff[12] * data[i-13];
                         sum += qCoeff[11] * data[i-12];
                         sum += qCoeff[10] * data[i-11];
                         sum += qCoeff[ 9] * data[i-10];
                         sum += qCoeff[ 8] * data[i- 9];
                         sum += qCoeff[ 7] * data[i- 8];
                         sum += qCoeff[ 6] * data[i- 7];
                         sum += qCoeff[ 5] * data[i- 6];
                         sum += qCoeff[ 4] * data[i- 5];
                         sum += qCoeff[ 3] * data[i- 4];
                         sum += qCoeff[ 2] * data[i- 3];
                         sum += qCoeff[ 1] * data[i- 2];
                         sum += qCoeff[ 0] * data[i- 1];
            }
            data[i] = residual[i] + (sum >> lpQuant);
        }
    }
}

//-------------------------------------------------------------------------------------------

void FLACSubframe::restoreLPCSignalWide(const tint *residual,tint len,const tint *qCoeff,tint order,tint lpQuant,tint *data)
{
    tint i;
    tint64 sum;
    
    if(order<=12)
    {
        if(order > 8)
        {
            if(order > 10)
            {
                if(order==12)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[11]) * static_cast<tint64>(data[i-12]);
                        sum += static_cast<tint64>(qCoeff[10]) * static_cast<tint64>(data[i-11]);
                        sum += static_cast<tint64>(qCoeff[ 9]) * static_cast<tint64>(data[i-10]);
                        sum += static_cast<tint64>(qCoeff[ 8]) * static_cast<tint64>(data[i- 9]);
                        sum += static_cast<tint64>(qCoeff[ 7]) * static_cast<tint64>(data[i- 8]);
                        sum += static_cast<tint64>(qCoeff[ 6]) * static_cast<tint64>(data[i- 7]);
                        sum += static_cast<tint64>(qCoeff[ 5]) * static_cast<tint64>(data[i- 6]);
                        sum += static_cast<tint64>(qCoeff[ 4]) * static_cast<tint64>(data[i- 5]);
                        sum += static_cast<tint64>(qCoeff[ 3]) * static_cast<tint64>(data[i- 4]);
                        sum += static_cast<tint64>(qCoeff[ 2]) * static_cast<tint64>(data[i- 3]);
                        sum += static_cast<tint64>(qCoeff[ 1]) * static_cast<tint64>(data[i- 2]);
                        sum += static_cast<tint64>(qCoeff[ 0]) * static_cast<tint64>(data[i- 1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
                else // order==11
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[10]) * static_cast<tint64>(data[i-11]);
                        sum += static_cast<tint64>(qCoeff[ 9]) * static_cast<tint64>(data[i-10]);
                        sum += static_cast<tint64>(qCoeff[ 8]) * static_cast<tint64>(data[i- 9]);
                        sum += static_cast<tint64>(qCoeff[ 7]) * static_cast<tint64>(data[i- 8]);
                        sum += static_cast<tint64>(qCoeff[ 6]) * static_cast<tint64>(data[i- 7]);
                        sum += static_cast<tint64>(qCoeff[ 5]) * static_cast<tint64>(data[i- 6]);
                        sum += static_cast<tint64>(qCoeff[ 4]) * static_cast<tint64>(data[i- 5]);
                        sum += static_cast<tint64>(qCoeff[ 3]) * static_cast<tint64>(data[i- 4]);
                        sum += static_cast<tint64>(qCoeff[ 2]) * static_cast<tint64>(data[i- 3]);
                        sum += static_cast<tint64>(qCoeff[ 1]) * static_cast<tint64>(data[i- 2]);
                        sum += static_cast<tint64>(qCoeff[ 0]) * static_cast<tint64>(data[i- 1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
            }
            else
            {
                if(order==10)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[9]) * static_cast<tint64>(data[i-10]);
                        sum += static_cast<tint64>(qCoeff[8]) * static_cast<tint64>(data[i- 9]);
                        sum += static_cast<tint64>(qCoeff[7]) * static_cast<tint64>(data[i- 8]);
                        sum += static_cast<tint64>(qCoeff[6]) * static_cast<tint64>(data[i- 7]);
                        sum += static_cast<tint64>(qCoeff[5]) * static_cast<tint64>(data[i- 6]);
                        sum += static_cast<tint64>(qCoeff[4]) * static_cast<tint64>(data[i- 5]);
                        sum += static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i- 4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i- 3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i- 2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i- 1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
                else // order==9
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[8]) * static_cast<tint64>(data[i-9]);
                        sum += static_cast<tint64>(qCoeff[7]) * static_cast<tint64>(data[i-8]);
                        sum += static_cast<tint64>(qCoeff[6]) * static_cast<tint64>(data[i-7]);
                        sum += static_cast<tint64>(qCoeff[5]) * static_cast<tint64>(data[i-6]);
                        sum += static_cast<tint64>(qCoeff[4]) * static_cast<tint64>(data[i-5]);
                        sum += static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i-4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
            }
        }
        else if(order > 4)
        {
            if(order > 6)
            {
                if(order==8)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[7]) * static_cast<tint64>(data[i-8]);
                        sum += static_cast<tint64>(qCoeff[6]) * static_cast<tint64>(data[i-7]);
                        sum += static_cast<tint64>(qCoeff[5]) * static_cast<tint64>(data[i-6]);
                        sum += static_cast<tint64>(qCoeff[4]) * static_cast<tint64>(data[i-5]);
                        sum += static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i-4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
                else // order==7
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[6]) * static_cast<tint64>(data[i-7]);
                        sum += static_cast<tint64>(qCoeff[5]) * static_cast<tint64>(data[i-6]);
                        sum += static_cast<tint64>(qCoeff[4]) * static_cast<tint64>(data[i-5]);
                        sum += static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i-4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
            }
            else
            {
                if(order==6)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[5]) * static_cast<tint64>(data[i-6]);
                        sum += static_cast<tint64>(qCoeff[4]) * static_cast<tint64>(data[i-5]);
                        sum += static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i-4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
                else // order==5
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[4]) * static_cast<tint64>(data[i-5]);
                        sum += static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i-4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
            }
        }
        else
        {
            if(order > 2)
            {
                if(order==4)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[3]) * static_cast<tint64>(data[i-4]);
                        sum += static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
                else // order==3
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[2]) * static_cast<tint64>(data[i-3]);
                        sum += static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
            }
            else
            {
                if(order==2)
                {
                    for(i=0;i<len;i++)
                    {
                        sum  = static_cast<tint64>(qCoeff[1]) * static_cast<tint64>(data[i-2]);
                        sum += static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1]);
                        data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
                    }
                }
                else // order == 1
                {
                    for(i=0;i<len;i++)
                    {
                        data[i] = residual[i] + static_cast<tint>((static_cast<tint64>(qCoeff[0]) * static_cast<tint64>(data[i-1])) >> lpQuant);
                    }
                }
            }
        }
    }
    else // order > 12
    {
        for(i=0;i<len;i++)
        {
            sum = 0;
            switch(order) {
                case 32: sum += static_cast<tint64>(qCoeff[31]) * static_cast<tint64>(data[i-32]);
                case 31: sum += static_cast<tint64>(qCoeff[30]) * static_cast<tint64>(data[i-31]);
                case 30: sum += static_cast<tint64>(qCoeff[29]) * static_cast<tint64>(data[i-30]);
                case 29: sum += static_cast<tint64>(qCoeff[28]) * static_cast<tint64>(data[i-29]);
                case 28: sum += static_cast<tint64>(qCoeff[27]) * static_cast<tint64>(data[i-28]);
                case 27: sum += static_cast<tint64>(qCoeff[26]) * static_cast<tint64>(data[i-27]);
                case 26: sum += static_cast<tint64>(qCoeff[25]) * static_cast<tint64>(data[i-26]);
                case 25: sum += static_cast<tint64>(qCoeff[24]) * static_cast<tint64>(data[i-25]);
                case 24: sum += static_cast<tint64>(qCoeff[23]) * static_cast<tint64>(data[i-24]);
                case 23: sum += static_cast<tint64>(qCoeff[22]) * static_cast<tint64>(data[i-23]);
                case 22: sum += static_cast<tint64>(qCoeff[21]) * static_cast<tint64>(data[i-22]);
                case 21: sum += static_cast<tint64>(qCoeff[20]) * static_cast<tint64>(data[i-21]);
                case 20: sum += static_cast<tint64>(qCoeff[19]) * static_cast<tint64>(data[i-20]);
                case 19: sum += static_cast<tint64>(qCoeff[18]) * static_cast<tint64>(data[i-19]);
                case 18: sum += static_cast<tint64>(qCoeff[17]) * static_cast<tint64>(data[i-18]);
                case 17: sum += static_cast<tint64>(qCoeff[16]) * static_cast<tint64>(data[i-17]);
                case 16: sum += static_cast<tint64>(qCoeff[15]) * static_cast<tint64>(data[i-16]);
                case 15: sum += static_cast<tint64>(qCoeff[14]) * static_cast<tint64>(data[i-15]);
                case 14: sum += static_cast<tint64>(qCoeff[13]) * static_cast<tint64>(data[i-14]);
                case 13: sum += static_cast<tint64>(qCoeff[12]) * static_cast<tint64>(data[i-13]);
                         sum += static_cast<tint64>(qCoeff[11]) * static_cast<tint64>(data[i-12]);
                         sum += static_cast<tint64>(qCoeff[10]) * static_cast<tint64>(data[i-11]);
                         sum += static_cast<tint64>(qCoeff[ 9]) * static_cast<tint64>(data[i-10]);
                         sum += static_cast<tint64>(qCoeff[ 8]) * static_cast<tint64>(data[i- 9]);
                         sum += static_cast<tint64>(qCoeff[ 7]) * static_cast<tint64>(data[i- 8]);
                         sum += static_cast<tint64>(qCoeff[ 6]) * static_cast<tint64>(data[i- 7]);
                         sum += static_cast<tint64>(qCoeff[ 5]) * static_cast<tint64>(data[i- 6]);
                         sum += static_cast<tint64>(qCoeff[ 4]) * static_cast<tint64>(data[i- 5]);
                         sum += static_cast<tint64>(qCoeff[ 3]) * static_cast<tint64>(data[i- 4]);
                         sum += static_cast<tint64>(qCoeff[ 2]) * static_cast<tint64>(data[i- 3]);
                         sum += static_cast<tint64>(qCoeff[ 1]) * static_cast<tint64>(data[i- 2]);
                         sum += static_cast<tint64>(qCoeff[ 0]) * static_cast<tint64>(data[i- 1]);
            }
            data[i] = residual[i] + static_cast<tint>(sum >> lpQuant);
        }
    }
}

//-------------------------------------------------------------------------------------------

tint FLACSubframe::iLog2(tint v)
{
    tint l=0;
    
    while(v>>=1)
    {
        l++;
    }
    return l;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
