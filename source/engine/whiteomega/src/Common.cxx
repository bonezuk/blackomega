#include "engine/whiteomega/inc/Common.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

tint maxPredSfb(tint idx)
{
    static const tint pred[] = {33, 33, 38, 40, 40, 40, 41, 41, 37, 37, 37, 34};

    if(idx<12)
    {
        return pred[idx];
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
