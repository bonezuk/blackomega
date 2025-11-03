//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_KAISERFILTER_H
#define __OMEGA_ENGINE_KAISERFILTER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"
#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT KaiserFilter
{
    public:

        typedef enum
        {
            e_filterLarge = 0,
            e_filterSmall
        } Type;

    public:
        ~KaiserFilter();

        static KaiserFilter& instance();

        tint16 *filter(Type t);
        const tint16 *filter(Type t) const;
        tint size(Type t) const;

        tint16 *filterD(Type t);
        const tint16 *filterD(Type t) const;
        tint sizeD(Type t) const;

        tint nmult(Type t) const;
        tint scale(Type t) const;

    protected:

        static KaiserFilter *m_instance;

        static tint16 m_filterL[8192];
        static tint16 m_filterLD[8192];

        static tint16 m_filterS[1536];
        static tint16 m_filterSD[3072];

        KaiserFilter();
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
