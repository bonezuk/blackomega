//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_WCOMPARE_H
#define __OMEGA_WHITEOMEGA_WCOMPARE_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Compare.h"
#include "engine/whiteomega/inc/WhiteOmegaDLL.h"
#include "engine/whiteomega/inc/ICS.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT WCompare : public engine::Compare
{
    public:
        WCompare(QObject *parent = 0);
        virtual ~WCompare();
        
        virtual void compareICSA(engine::whiteomega::ICSInfo *icsA,tint *specDataA,tint frameLenA) = 0;
};

//-------------------------------------------------------------------------------------------

extern WHITEOMEGA_EXPORT WCompare *g_WCompare;

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
