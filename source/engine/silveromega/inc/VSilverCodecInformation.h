//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODECINFORMATION_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODECINFORMATION_H
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

class SILVEROMEGA_EXPORT VSilverCodecInformation : public VSilverInfoObject
{
    public:
        VSilverCodecInformation();
        virtual ~VSilverCodecInformation();

        virtual bool isValid() const;

        virtual bool read(engine::Sequence *seq);

        tint m_version;

        tint m_audioChannels;
        tint m_audioSampleRate;

        tint m_bitrateMaximum;
        tint m_bitrateNominal;
        tint m_bitrateMinimum;

        tuint m_shiftBlockSize_0;
        tuint m_shiftBlockSize_1;

        tint m_blockSize_0;
        tint m_blockSize_1;

    protected:

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
