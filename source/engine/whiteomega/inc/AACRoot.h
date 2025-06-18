//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_AACROOT_H
#define __OMEGA_WHITEOMEGA_AACROOT_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Sequence.h"
#include "engine/whiteomega/inc/Atom.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/AACDecode.h"
#include "engine/whiteomega/inc/CPEDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT AACRoot
{
    public:
    
        typedef enum
        {
            e_idSCE = 0,    // single channel element
            e_idCPE = 1,    // channel pair element
            e_idCCE = 2,    // coupling channel element
            e_idLFE = 3,    // lfe channel element
            e_idDSE = 4,    // data stream element
            e_idPCE = 5,    // program config element
            e_idFIL = 6,    // fill element
            e_idEND = 7        // end element
        } GAPayloadType;
    
    public:
        AACRoot();
        virtual ~AACRoot();
        
        void setGAConfig(GAConfig *cfg);
        
        void reset();
        
        tint read(Sequence *seq,sample_t *out,tint& len);
    
    protected:
        
        GAConfig *m_gaConfig;

        CPEDecode *m_decoderCPE;
        
        void printError(const tchar *strR,const tchar *strE) const;
        
        bool readSingleChannel(Sequence *seq);
        bool readChannelPair(Sequence *seq,sample_t *out,tint& len);
        bool readCouplingChannel(Sequence *seq);
        bool readLFEChannel(Sequence *seq);
        bool readDataStream(Sequence *seq);
        bool readProgramConfig(Sequence *seq);
        bool readFillElement(Sequence *seq);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
