#include "engine/silveromega/inc/VSilverCodebook.h"
#include "engine/silveromega/inc/VSilverFunctions.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------
// HuffmanBNode
//-------------------------------------------------------------------------------------------

HuffmanBNode::HuffmanBNode() : parent(0),
    left(0),
    right(0),
    value(-1)
{}

//-------------------------------------------------------------------------------------------

HuffmanBNode::HuffmanBNode(HuffmanBNode *par) : parent(par),
    left(0),
    right(0),
    value(-1)
{}

//-------------------------------------------------------------------------------------------

HuffmanBNode::~HuffmanBNode()
{
    try
    {
        if(left!=0)
        {
            delete left;
            left = 0;
        }
        if(right!=0)
        {
            delete right;
            right = 0;
        }
        parent = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------
// HTEntry
//-------------------------------------------------------------------------------------------

HTEntry::HTEntry() : count(0)
{
    entry.node = 0;
}

//-------------------------------------------------------------------------------------------

HTEntry::~HTEntry()
{
    try
    {
        if(count==-1 && entry.node!=0)
        {
            delete entry.node;
            entry.node = 0;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------
// VSilverCodebook
//-------------------------------------------------------------------------------------------

VSilverCodebook::VSilverCodebook() : VSilverInfoObject(),
    m_dimensions(0),
    m_alloc(),
    m_entries(0),
    m_lengths(0),
    m_type(0),
    m_minimum(0),
    m_delta(0),
    m_valueBits(0),
    m_sequenceP(0),
    m_quantizedLength(0),
    m_quantizedValues(0),
    m_flag(false),
    m_maxEntry(-1),
    m_bookLookupLength(0),
    m_bookLookup(0),
    m_realLookup(0),
    m_t(0)
{}

//-------------------------------------------------------------------------------------------

VSilverCodebook::~VSilverCodebook()
{
    try
    {
        tint i;

        if(m_lengths!=0)
        {
            delete [] m_lengths;
            m_lengths = 0;
        }
        if(m_quantizedValues!=0)
        {
            delete [] m_quantizedValues;
            m_quantizedValues = 0;
        }
        if(m_bookLookup!=0)
        {
            delete [] m_bookLookup;
            m_bookLookup = 0;
        }
        if(m_realLookup!=0)
        {
            for(i=0;i<m_entries;++i)
            {
                delete [] m_realLookup[i];
            }
            m_alloc.Free(m_realLookup);
            m_realLookup = 0;
        }
        m_alloc.FreeAll();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void VSilverCodebook::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverCodebook::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tint VSilverCodebook::mapType1QuantizedLength() const
{
    tuint i,a,b,vals;

    vals = static_cast<tuint>(static_cast<tfloat32>(::floor(static_cast<tfloat32>(::pow(static_cast<tfloat32>(m_entries),1.0f / static_cast<tfloat32>(m_dimensions))))));

    for(;;)
    {
        for(i=0,a=1,b=1;i<static_cast<tuint>(m_dimensions);i++)
        {
            a *= vals;
            b *= vals + 1;
        }
        if(a<=static_cast<tuint>(m_entries) && b>static_cast<tuint>(m_entries))
        {
            return static_cast<tint>(vals);
        }
        else
        {
            if(a>static_cast<tuint>(m_entries))
            {
                vals--;
            }
            else
            {
                vals++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool VSilverCodebook::isValid() const
{
    if(!m_flag)
    {
        printError("isValid","Codebook data has not read in from bitstream");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool VSilverCodebook::read(engine::Sequence *seq)
{
    int i,j,x,len;

    if(seq==NULL)
    {
        printError("read","No sequence instance given");
        return false;
    }
    if(seq->readBits(24)!=0x00564342)
    {
        printError("read","Codebook table boundary not found");
        return false;
    }

    m_dimensions = seq->readBits(16);
    m_entries = seq->readBits(24);
    m_lengths = new tint [static_cast<tuint>(m_entries) + 1];
    ::memset(m_lengths,0,static_cast<tuint>(m_entries + 1) << 2);

    if(!seq->readBit())
    {
        // Unordered
        if(seq->readBit())
        {
            for(i=0;i<m_entries;i++)
            {
                if(seq->readBit())
                {
                    m_lengths[i] = seq->readBits(5) + 1;
                }
                else
                {
                    m_lengths[i] = 0;
                }
                m_maxEntry = (m_maxEntry < m_lengths[i]) ? m_lengths[i] : m_maxEntry;
            }
        }
        else
        {
            for(i=0;i<m_entries;i++)
            {
                m_lengths[i] = seq->readBits(5) + 1;
                m_maxEntry = (m_maxEntry < m_lengths[i]) ? m_lengths[i] : m_maxEntry;
            }
        }
    }
    else
    {
        // Ordered
        len = seq->readBits(5) + 1;
        i=0;
        while(i<m_entries)
        {
            x = seq->readBits(iLog(m_entries - i));
            for(j=0;j<x && i<m_entries;j++,i++)
            {
                m_lengths[i] = len;
                m_maxEntry = (m_maxEntry < m_lengths[i]) ? m_lengths[i] : m_maxEntry;
            }
            len++;
        }
    }

    // Unpack the mapping, if required.
    m_type = seq->readBits(4);
    switch(m_type)
    {
        case 0:
            break;

        case 1:
        case 2:
            m_minimum = float32unpack(seq->readBits(32));
            m_delta = float32unpack(seq->readBits(32));
            m_valueBits = seq->readBits(4) + 1;
            m_sequenceP = seq->readBit();

            if(m_type==1)
            {
                m_quantizedLength = mapType1QuantizedLength();
            }
            else
            {
                m_quantizedLength = m_entries * m_dimensions;
            }
            m_quantizedValues = new tint [static_cast<tuint>(m_quantizedLength + 1)];
            ::memset(m_quantizedValues,0,static_cast<tuint>(m_quantizedLength + 1) << 2);

            for(i=0;i<m_quantizedLength;i++)
            {
                m_quantizedValues[i] = seq->readBits(m_valueBits);
            }
            break;

        default:
            printError("read","Unknown codebook type");
            return false;
    }

    if(seq->isEnd())
    {
        printError("read","End of stream has been detected");
        return false;
    }
    m_flag = true;
    return true;

}

//-------------------------------------------------------------------------------------------

bool VSilverCodebook::buildHuffmanTreeR(HuffmanBNode *item,tint depth,tint len,tint value) const
{
    if(depth==len)
    {
        // Try to place the end node.
        if(item->left==0)
        {
            item->left = new HuffmanBNode(item);
            item->left->value = value;
        }
        else if(item->right==0)
        {
            item->right = new HuffmanBNode(item);
            item->right->value = value;
        }
        else
        {
            return false;
        }
    }
    else
    {
        bool res = false;

        if(item->left==0 || item->left->value<0)
        {
            if(item->left==0)
            {
                item->left = new HuffmanBNode(item);
            }
            if(buildHuffmanTreeR(item->left,depth+1,len,value))
            {
                res = true;
            }
        }

        if(!res)
        {
            if(item->right==0 || item->right->value<0)
            {
                if(item->right==0)
                {
                    item->right = new HuffmanBNode(item);
                }
                if(!buildHuffmanTreeR(item->right,depth+1,len,value))
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    if(item->left!=0 && item->left->value>=0)
    {
        if(item->right!=0 && item->right->value>=0)
        {
            item->value = 1;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

HuffmanBNode *VSilverCodebook::buildHuffmanTree(const tint *codebook,tint len) const
{
    tint i;
    HuffmanBNode *root = new HuffmanBNode;

    for(i=0;i<len;++i)
    {
        if(codebook[i]>0)
        {
            if(!buildHuffmanTreeR(root,1,codebook[i],i))
            {
                printError("buildHuffmanTree","Given codebook has over saturated the Huffman tree");
                delete root;
                return 0;
            }
        }
    }

    if(root->value<-1) // Binary-Tree uncomplete
    {
        printError("buildHuffmanTree","Given codebook gives incomplete Huffman tree");
        delete root;
        return 0;
    }
    return root;
}

//-------------------------------------------------------------------------------------------

const tint c_maxCodebookArrayDepth = 12;

HTEntry *VSilverCodebook::buildCodebookTree()
{
    tint i,j,x,len;
    HTEntry *bookLookup;
    HuffmanBNode *root,*item;
    bool flag = false,res = false;

    m_bookLookupLength = (m_maxEntry < c_maxCodebookArrayDepth) ? m_maxEntry : c_maxCodebookArrayDepth;
    len = 1 << m_bookLookupLength;
    bookLookup = new HTEntry [static_cast<tuint>(len)];

    root = buildHuffmanTree(m_lengths,m_entries);
    if(root!=0)
    {
        for(i=0;i<len;++i)
        {
            x = i;
            item = root;
            for(j=0;j<m_bookLookupLength;++j)
            {
                if(x&1) // branch right
                {
                    item = item->right;
                }
                else
                {
                    item = item->left;
                }
                if(item->left==0 && item->right==0) // no more branches hence end child node.
                {
                    bookLookup[i].entry.value = item->value;
                    bookLookup[i].count = j + 1;
                    break;
                }
                x = static_cast<tint>(static_cast<tuint>(x) >> 1);
            }
            if(j==m_bookLookupLength)
            {
                if(item->parent!=0)
                {
                    if(item->parent->left==item)
                    {
                        item->parent->left = 0;
                    }
                    else
                    {
                        item->parent->right = 0;
                    }
                    item->parent = 0;
                }
                else
                {
                    flag = true;
                }

                bookLookup[i].entry.node = item;
                bookLookup[i].count = -1;
            }
        }

        res = true;
    }
    else
    {
        printError("buildCodebookTree","Failed to build Huffman decode tree from given codebook");
    }

    if(!res)
    {
        delete [] bookLookup;
        bookLookup = 0;
    }
    if(!flag)
    {
        delete root;
    }
    return bookLookup;
}

//-------------------------------------------------------------------------------------------

void VSilverCodebook::buildLatticeVQ()
{
    tint i,j,offset,index;
    tfloat32 last,*y;

    m_realLookup = reinterpret_cast<tfloat32 **>(m_alloc.MemAlloc(static_cast<tuint>(m_entries),sizeof(tfloat32 *)));

    for(i=0;i<m_entries;++i)
    {
        y = m_realLookup[i] = new tfloat32 [static_cast<tuint>(m_dimensions)];

        if(m_quantizedLength && m_quantizedValues!=0)
        {
            for(last=0.0f,index=1,j=0;j<m_dimensions;++j)
            {
                offset = (i / index) % m_quantizedLength;
                y[j] = (static_cast<tfloat32>(m_quantizedValues[offset]) * m_delta) + m_minimum + last;
                if(m_sequenceP)
                {
                    last = y[j];
                }
                index *= m_quantizedLength;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverCodebook::buildScalarVQ()
{
    tint i,j,offset;
    tfloat32 last,*y;

    m_realLookup = reinterpret_cast<tfloat32 **>(m_alloc.MemAlloc(static_cast<tuint>(m_entries),sizeof(tfloat32 *)));

    for(i=0;i<m_entries;++i)
    {
        y = m_realLookup[i] = new tfloat32 [static_cast<tuint>(m_dimensions)];

        offset = i * m_dimensions;

        if(m_quantizedValues!=0)
        {
            for(last=0,j=0;j<m_dimensions;j++)
            {
                y[j] = (static_cast<tfloat32>(m_quantizedValues[offset]) * m_delta) + m_minimum + last;
                if(m_sequenceP)
                {
                    last = y[j];
                }
                offset++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool VSilverCodebook::setup()
{
    m_bookLookup = buildCodebookTree();
    if(m_bookLookup==0)
    {
        printError("setup","Failed to build huffman decoding structure");
        return false;
    }

    switch(m_type)
    {
        case 0:
            break;

        case 1:
            buildLatticeVQ();
            break;

        case 2:
            buildScalarVQ();
            break;

        default:
            printError("setup","Codebook decoding method not supported");
            return false;
    }

    m_t = reinterpret_cast<tfloat32 **>(m_alloc.MemAlloc((256 / m_dimensions) + 1,sizeof(tfloat32 *)));
    return true;
}

//-------------------------------------------------------------------------------------------

tint VSilverCodebook::decode0(engine::Sequence *seq)
{
    tint x;

    if(m_bookLookup==0)
    {
        return 0;
    }

    x = seq->browseBits(m_bookLookupLength);
    if(m_bookLookup[x].count>=0)
    {
        seq->seek(m_bookLookup[x].count);
        return m_bookLookup[x].entry.value;
    }
    else
    {
        HuffmanBNode *item;

        if(seq->seek(m_bookLookupLength))
        {
            item = m_bookLookup[x].entry.node;

            do
            {
                if(seq->readBit())
                {
                    item = item->right;
                }
                else
                {
                    item = item->left;
                }
            } while(item->left!=0 && item->right!=0);

            return item->value;
        }
        else
        {
            return 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

tfloat32 *VSilverCodebook::decode1(engine::Sequence *seq)
{
    if(m_realLookup==0)
    {
        return 0;
    }
    return m_realLookup[decode0(seq)];
}

//-------------------------------------------------------------------------------------------

void VSilverCodebook::decodeResidue0(engine::Sequence *seq,tfloat32 *out,tint n)
{
    tint step = n / m_dimensions;
    tint i,j,o;

    for(i=0;i<step;++i)
    {
        m_t[i] = m_realLookup[decode0(seq)];
    }

    for(i=0,o=0;i<m_dimensions;++i,o+=step)
    {
        for(j=0;j<step;++j)
        {
            out[o+j] = m_t[j][i];
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverCodebook::decodeResidue1(engine::Sequence *seq,tfloat32 *out,tint n)
{
    tfloat32 *t;
    tint i=0,j=0;

    if(m_dimensions>8)
    {
        while(i<n)
        {
            t = m_realLookup[decode0(seq)];
            while(j<m_dimensions)
            {
                out[i++] += t[j++];
            }
        }
    }
    else
    {
        while(i<n)
        {
            t = m_realLookup[decode0(seq)];
            switch(m_dimensions)
            {
                case 8:
                    out[i++] += t[j++];
                case 7:
                    out[i++] += t[j++];
                case 6:
                    out[i++] += t[j++];
                case 5:
                    out[i++] += t[j++];
                case 4:
                    out[i++] += t[j++];
                case 3:
                    out[i++] += t[j++];
                case 2:
                    out[i++] += t[j++];
                case 1:
                    out[i++] += t[j++];
                case 0:
                    break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void VSilverCodebook::decodeResidue2(engine::Sequence *seq,float **out,int offset,int ch,int n)
{
    tint i,j,end,chptr,tmp;
    const tfloat32 *t;

    chptr = 0;

    i = offset / ch;
    end = (offset + n) / ch;

    while(i<end)
    {
        tmp = decode0(seq);

        t = m_realLookup[tmp];

        for(j=0;j<m_dimensions;j++)
        {
            out[chptr][i] += t[j];
            chptr++;
            if(chptr==ch)
            {
                chptr=0;
                i++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
