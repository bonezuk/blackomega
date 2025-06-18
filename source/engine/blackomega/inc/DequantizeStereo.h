//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_DEQUANTIZESTEREO_H
#define __OMEGA_ENGINE_BLACKOMEGA_DEQUANTIZESTEREO_H
//-------------------------------------------------------------------------------------------

#include "engine/blackomega/inc/Dequantize.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT DequantizeStereo : public Dequantize
{
    public:
    
        typedef enum
        {
            e_stereoLong = 0,
            e_stereoShort0,
            e_stereoShort1,
            e_stereoShort2
        } StereoType;
    
    public:
        DequantizeStereo();
        virtual ~DequantizeStereo();
        
        virtual bool read(engine::Sequence *s);
        
        tint getStereo(StereoType t);
        
    protected:
    
        tint m_stereoL;
        tint m_stereoS0;
        tint m_stereoS1;
        tint m_stereoS2;
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void decodeLong(BSequence *seq,tint *reg);
        virtual void decodeShort(BSequence *seq,tint *reg);
        virtual void decodeMixed(BSequence *seq,tint *reg);        
};

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

