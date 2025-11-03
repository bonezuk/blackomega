#include "engine/blackomega/inc/BSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------
// BBitstream
//-------------------------------------------------------------------------------------------

BBitstream::BBitstream() : engine::Bitstream(Bitstream::e_streamMode2)
{}

//-------------------------------------------------------------------------------------------

BBitstream::~BBitstream()
{}

//-------------------------------------------------------------------------------------------

Sequence *BBitstream::newSequence()
{
    BSequence *seq = new BSequence;
    return reinterpret_cast<engine::Sequence *>(seq);
}

//-------------------------------------------------------------------------------------------
// BSequence
//-------------------------------------------------------------------------------------------

BSequence::BSequence() : engine::Sequence(),
    m_count(0),
    m_countOffset(-1),
    m_grainLength(0),
    m_band(Band::instance())
{}

//-------------------------------------------------------------------------------------------

BSequence::~BSequence()
{}

//-------------------------------------------------------------------------------------------

void BSequence::reset()
{
    m_count = 0;
    m_countOffset = -1;
    engine::Sequence::reset();
}

//-------------------------------------------------------------------------------------------

void BSequence::startGrain(tint l)
{
    m_count = 0;
    m_countOffset = static_cast<tint>(m_bitOffset);
    m_grainLength = l;
}

//-------------------------------------------------------------------------------------------

void BSequence::countGrain()
{
    m_count += static_cast<tint>(m_bitOffset) - m_countOffset;
    m_countOffset = static_cast<tint>(m_bitOffset);
}

//-------------------------------------------------------------------------------------------

bool BSequence::next()
{
    bool res;

    if(m_countOffset>=0)
    {
        m_count += static_cast<tint>(m_bitOffset) - m_countOffset;
    }

    res = engine::Sequence::next();

    if(m_countOffset>=0)
    {
        m_countOffset = static_cast<tint>(m_bitOffset);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void BSequence::decode(tint h,tint& x,tint& y)
{
    if(!m_end)
    {
        HuffmanEntry *table = &(m_band->m_ht[h]);
        HuffmanNode *node = table->root;

        if((m_length - m_bitOffset) >= 40)
        {
            while(node->left!=0)
            {
                if(readBitFastI())
                {
                    node = node->left;
                }
                else
                {
                    node = node->right;
                }
            }

            x = node->x;
            y = node->y;

            if(x==15)
            {
                x += readBitsI(table->linbits);
                if(readBitFastI())
                {
                    x = -x;
                }
            }
            else if(x)
            {
                if(readBitFastI())
                {
                    x = -x;
                }
            }

            if(y==15)
            {
                y += readBitsI(table->linbits);
                if(readBitFastI())
                {
                    y = - y;
                }
            }
            else if(y)
            {
                if(readBitFastI())
                {
                    y = -y;
                }
            }
        }
        else
        {
            while(node->left!=0)
            {
                if(readBitI())
                {
                    node = node->left;
                }
                else
                {
                    node = node->right;
                }
            }

            x = node->x;
            y = node->y;

            if(x==15)
            {
                x += readBitsI(table->linbits);
                if(readBitI())
                {
                    x = -x;
                }
            }
            else if(x)
            {
                if(readBitI())
                {
                    x = -x;
                }
            }

            if(y==15)
            {
                y += readBitsI(table->linbits);
                if(readBitI())
                {
                    y = -y;
                }
            }
            else if(y)
            {
                if(readBitI())
                {
                    y = -y;
                }
            }
        }
    }
    else
    {
        x = 0;
        y = 0;
    }
}

//-------------------------------------------------------------------------------------------

void BSequence::decode(tint h,tint& x,tint& y,tint& v,tint& w)
{
    if(!m_end)
    {
        tint a;
        HuffmanEntry *table;
        HuffmanNode *node;

        countGrain();

        if(!h)
        {
            table = &(m_band->m_ht[32]);
        }
        else
        {
            table = &(m_band->m_ht[33]);
        }
        node = table->root;

        if((m_grainLength - m_count)>=40 && (m_length - m_bitOffset)>=40)
        {
            while(node->left!=0)
            {
                if(readBitFastI())
                {
                    node = node->left;
                }
                else
                {
                    node = node->right;
                }
            }
            a = node->y;

            if(a & 0x00000008)
            {
                v = (readBitFastI()) ? -1 : 1;
            }
            else
            {
                v = 0;
            }

            if(a & 0x00000004)
            {
                w = (readBitFastI()) ? -1 : 1;
            }
            else
            {
                w = 0;
            }

            if(a & 0x00000002)
            {
                x = (readBitFastI()) ? -1 : 1;
            }
            else
            {
                x = 0;
            }

            if(a & 0x00000001)
            {
                y = (readBitFastI()) ? -1 : 1;
            }
            else
            {
                y = 0;
            }
        }
        else
        {
            tint remain = m_grainLength - m_count;

            while(node->left!=0 && remain>0)
            {
                if(readBitI())
                {
                    node = node->left;
                }
                else
                {
                    node = node->right;
                }
                remain--;
            }
            a = node->y;

            if((a & 0x00000008) && remain>0)
            {
                v = (readBitI()) ? -1 : 1;
                remain--;
            }
            else
            {
                v = 0;
            }

            if((a & 0x00000004) && remain>0)
            {
                w = (readBitI()) ? -1 : 1;
                remain--;
            }
            else
            {
                w = 0;
            }

            if((a & 0x00000002) && remain>0)
            {
                x = (readBitI()) ? -1 : 1;
                remain--;
            }
            else
            {
                x = 0;
            }

            if((a & 0x00000001) && remain>0)
            {
                y = (readBitI()) ? -1 : 1;
                remain--;
            }
            else
            {
                y = 0;
            }
        }
    }
    else
    {
        v = 0;
        w = 0;
        x = 0;
        y = 0;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
