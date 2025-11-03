#include "engine/whiteomega/inc/AACRoot.h"
#include "engine/whiteomega/inc/FillDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

AACRoot::AACRoot() : m_gaConfig(0),
    m_decoderCPE(0)
{}

//-------------------------------------------------------------------------------------------

AACRoot::~AACRoot()
{
    if(m_decoderCPE!=0)
    {
        delete m_decoderCPE;
    }
}

//-------------------------------------------------------------------------------------------

void AACRoot::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "AACRoot::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AACRoot::setGAConfig(GAConfig *cfg)
{
    m_gaConfig = cfg;
}

//-------------------------------------------------------------------------------------------

void AACRoot::reset()
{
    if(m_decoderCPE!=0)
    {
        m_decoderCPE->reset();
    }
}

//-------------------------------------------------------------------------------------------

tint AACRoot::read(Sequence *seq,sample_t *out,tint& len)
{
    // state : 0 = data, 1 = end of packet, -1 = error
    GAPayloadType payloadType;

    len = 0;

    while(payloadType = static_cast<GAPayloadType>(seq->readBitsI(3)),payloadType!=e_idEND)
    {
        switch(payloadType)
        {
            // single channel element
            case e_idSCE:
                if(!readSingleChannel(seq))
                {
                    return -1;
                }
                break;

            // channel pair element
            case e_idCPE:
                if(!readChannelPair(seq,out,len))
                {
                    return -1;
                }
                break;

            // coupling channel element
            case e_idCCE:
                if(!readCouplingChannel(seq))
                {
                    return -1;
                }
                break;

            // lfe channel element
            case e_idLFE:
                if(!readLFEChannel(seq))
                {
                    return -1;
                }
                break;

            // data stream element
            case e_idDSE:
                if(!readDataStream(seq))
                {
                    return -1;
                }
                break;

            // program config element
            case e_idPCE:
                if(!readProgramConfig(seq))
                {
                    return -1;
                }
                break;

            // fill element
            case e_idFIL:
                if(!readFillElement(seq))
                {
                    return -1;
                }
                break;

            default:
                break;
        }

        if(len>0)
        {
            return 0;
        }
    }
    return 1;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readSingleChannel(Sequence *seq)
{
    // TODO - implement
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readChannelPair(Sequence *seq,sample_t *out,tint& len)
{
    if(m_decoderCPE==0)
    {
        m_decoderCPE = new CPEDecode;
    }
    m_decoderCPE->reset();
    m_decoderCPE->setGAConfig(m_gaConfig);

    if(!m_decoderCPE->decode(seq,out,len))
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readCouplingChannel(Sequence *seq)
{
    // TODO - implement
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readLFEChannel(Sequence *seq)
{
    // TODO - implement
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readDataStream(Sequence *seq)
{
    // TODO - implement
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readProgramConfig(Sequence *seq)
{
    // TODO - implement
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACRoot::readFillElement(Sequence *seq)
{
    FillDecode fill;
    return fill.decode(seq);
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
