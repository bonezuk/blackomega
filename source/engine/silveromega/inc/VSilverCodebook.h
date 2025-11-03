//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODEBOOK_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODEBOOK_H
//-------------------------------------------------------------------------------------------

#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverInfoObject.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT HuffmanBNode
{
    public:
        HuffmanBNode();
        HuffmanBNode(HuffmanBNode *par);
        ~HuffmanBNode();

        HuffmanBNode *parent;
        HuffmanBNode *left;
        HuffmanBNode *right;
        tint value;
};

//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT HTEntry
{
    public:
        HTEntry();
        ~HTEntry();

        union HTValue
        {
            tint value;
            HuffmanBNode *node;
        } entry;

        tint count;
};

//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverCodebook : public VSilverInfoObject
{
    public:
        VSilverCodebook();
        virtual ~VSilverCodebook();

        virtual bool isValid() const;

        virtual bool read(engine::Sequence *seq);

        virtual bool setup();

        tint decode0(engine::Sequence *seq);
        tfloat32 *decode1(engine::Sequence *seq);

        void decodeResidue0(engine::Sequence *seq,tfloat32 *out,tint n);
        void decodeResidue1(engine::Sequence *seq,tfloat32 *out,tint n);
        void decodeResidue2(engine::Sequence *seq,tfloat32 **out,tint offset,tint ch,tint n);

        tint m_dimensions;

    protected:

        common::Allocation m_alloc;

        tint m_entries;
        tint *m_lengths;
        tint m_type;
        tfloat32 m_minimum;
        tfloat32 m_delta;
        tint m_valueBits;
        tint m_sequenceP;
        tint m_quantizedLength;
        tint *m_quantizedValues;
        bool m_flag;
        tint m_maxEntry;

        tint m_bookLookupLength;
        HTEntry *m_bookLookup;

        tfloat32 **m_realLookup;
        tfloat32 **m_t;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        tint mapType1QuantizedLength() const;

        bool buildHuffmanTreeR(HuffmanBNode *item,tint depth,tint len,tint value) const;
        HuffmanBNode *buildHuffmanTree(const tint *codebook,tint len) const;

        HTEntry *buildCodebookTree();

        void buildLatticeVQ();
        void buildScalarVQ();

        tint decodeNextHuffman(engine::Sequence *seq);

        tint *decodeType0(engine::Sequence *seq);
        tint *deocdeType1(engine::Sequence *seq);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

