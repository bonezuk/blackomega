#include "engine/blackomega/inc/SideInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

SideInfo::SideInfo() : m_hdr(0),
    m_info(0)
{}

//-------------------------------------------------------------------------------------------

void *SideInfo::get(tint val)
{
    switch(val)
    {
        case 0:
            return reinterpret_cast<void *>(m_info);
            
        case 1:
            return reinterpret_cast<void *>(&(m_info->ch[0].gr[0]));
            
        case 2:
            return reinterpret_cast<void *>(&(m_info->ch[0].gr[1]));
            
        case 3:
            return reinterpret_cast<void *>(&(m_info->ch[1].gr[0]));
            
        case 4:
            return reinterpret_cast<void *>(&(m_info->ch[1].gr[1]));
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

bool SideInfo::readInfoMPEG1(Sequence *seq)
{
    tint i,j,k;
    tint region0Count,region1Count;
    GrInfoS *gr;
    tuint *s;
    Band *band = Band::instance();
    
    m_info->main_data_begin = seq->readBitsI(9);
    if(m_hdr->stereo==1)
    {
        m_info->private_bits = seq->readBitsI(5);
    }
    else
    {
        m_info->private_bits = seq->readBitsI(3);
    }
    
    for(i=0;i<static_cast<tint>(m_hdr->stereo);++i)
    {
        s = reinterpret_cast<tuint *>(&(m_info->ch[i].scfsi[0]));
        j = seq->readBitsI(4);
        s[0] = (j >> 3) & 0x00000001;
        s[1] = (j >> 2) & 0x00000001;
        s[2] = (j >> 1) & 0x00000001;
        s[3] = j & 0x00000001;
    }
    
    for(i=0;i<2;++i)
    {
        for(j=0;j<static_cast<tint>(m_hdr->stereo);++j)
        {
            gr = reinterpret_cast<GrInfoS *>(&(m_info->ch[j].gr[i]));
            gr->part2_3_length = seq->readBitsI(12);
            gr->big_values = seq->readBitsI(9) << 1;
            if(gr->big_values > 576)
            {
                return false;
            }
            gr->global_gain = seq->readBitsI(8);
            gr->scalefac_compress = seq->readBitsI(4);
            if(seq->readBitI())
            {
                gr->block_type = seq->readBitsI(2);
                gr->mixed_block_flag = seq->readBitI();
                for(k=0;k<2;++k)
                {
                    gr->table_select[k] = seq->readBitsI(5);
                }
                gr->table_select[2] = 0;
                for(k=0;k<3;++k)
                {
                    gr->subblock_gain[k] = seq->readBitsI(3);
                }
                gr->region1Start = 36;
                gr->region2Start = 576;
            }
            else
            {
                for(k=0;k<3;++k)
                {
                    gr->table_select[k] = seq->readBitsI(5);
                }
                region0Count = seq->readBitsI(4);
                region1Count = seq->readBitsI(3);
                gr->region1Start = band->m_sfBandIndex[m_hdr->sfreq].l[region0Count + 1];
                gr->region2Start = band->m_sfBandIndex[m_hdr->sfreq].l[region0Count + region1Count + 2];
                gr->block_type = 0;
                gr->mixed_block_flag = 0;
            }
            gr->preflag = seq->readBitI();
            gr->scalefac_scale = seq->readBitI();
            gr->count1table_select = seq->readBitI();
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool SideInfo::readInfoMPEG2(Sequence *seq)
{
    tuint i,j;
    tint region0Count,region1Count;
    GrInfoS *gr;
    Band *band = Band::instance();
    
    m_info->main_data_begin = seq->readBitsI(8);
    if(m_hdr->stereo==1)
    {
        m_info->private_bits = seq->readBitI();
    }
    else
    {
        m_info->private_bits = seq->readBitsI(2);
    }
    
    for(i=0;i<m_hdr->stereo;++i)
    {
        gr = reinterpret_cast<GrInfoS *>(&(m_info->ch[i].gr[0]));
        gr->part2_3_length = seq->readBitsI(12);
        gr->big_values = seq->readBitsI(9) << 1;
        if(gr->big_values > 576)
        {
            return false;
        }
        gr->global_gain = seq->readBitsI(8);
        gr->scalefac_compress = seq->readBitsI(9);
        if(seq->readBitI())
        {
            gr->block_type = seq->readBitsI(2);
            gr->mixed_block_flag = seq->readBitI();
            for(j=0;j<2;++j)
            {
                gr->table_select[j] = seq->readBitsI(5);
            }
            for(j=0;j<3;++j)
            {
                gr->subblock_gain[j] = seq->readBitsI(3);
            }
            if(gr->block_type==2)
            {
                gr->region1Start = 36;
            }
            else
            {
                gr->region1Start = 54;
            }
            gr->region2Start = 576;
        }
        else
        {
            for(j=0;j<3;++j)
            {
                gr->table_select[j] = seq->readBitsI(5);
            }
            region0Count = seq->readBitsI(4);
            region1Count = seq->readBitsI(3);
            gr->region1Start = band->m_sfBandIndex[m_hdr->sfreq].l[region0Count + 1];
            gr->region2Start = band->m_sfBandIndex[m_hdr->sfreq].l[region0Count + region1Count + 2];
            gr->block_type = 0;
            gr->mixed_block_flag = 0;
        }
        gr->preflag = 0;
        gr->scalefac_scale = seq->readBitI();
        gr->count1table_select = seq->readBitI();
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool SideInfo::read(Sequence *seq)
{
    if(!m_hdr->lsf)
    {
        return readInfoMPEG1(seq);
    }
    else
    {
        return readInfoMPEG2(seq);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
