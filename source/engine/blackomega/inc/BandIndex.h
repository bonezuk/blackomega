//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_BANDINDEX_H
#define __OMEGA_ENGINE_BLACKOMEGA_BANDINDEX_H
//-------------------------------------------------------------------------------------------

#include <math.h>

#include "common/inc/Allocation.h"
#include "common/inc/Log.h"
#include "engine/blackomega/inc/BlackOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

typedef struct s_BandIndex
{
    tint l[25];
    tint s[44];
    tint m[43];
} BandIndex;

//-------------------------------------------------------------------------------------------

typedef struct s_HuffmanNode
{
    struct s_HuffmanNode *left;
    struct s_HuffmanNode *right;
    tint x;
    tint y;
} HuffmanNode;

//-------------------------------------------------------------------------------------------

typedef struct s_HuffmanEntry
{
    tint linbits;
    HuffmanNode *root;
} HuffmanEntry;

//-------------------------------------------------------------------------------------------

struct newhuff
{
    tuint linbits;
    tint16 *table;
};

//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT Band
{
    public:
        ~Band();
        
        static Band *instance();
        
        sample_t *m_quart;
        sample_t *m_quarter;
        
        sample_t *m_dequantS;
        
        HuffmanEntry *m_ht;
        
        struct newhuff *m_ht2;
        
        BandIndex *m_sfBandIndex;
        
    protected:
    
        static Band *m_instance;
        
        common::Allocation m_alloc;
    
        Band();
    
        void printError(const tchar *strR,const tchar *strE) const;
        
        void initDequarter();
        void initBandIndex();
        void initHuffman();
        
        HuffmanNode *makeHuffmanTree(tint16 *tab,bool flag = true);
};

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

