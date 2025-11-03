//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACFRAMEINDEX_H
#define __OMEGA_ENGINE_REDOMEGA_ALACFRAMEINDEX_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include "engine/redomega/inc/ALACSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACFrameIndex
{
    public:
        ALACFrameIndex();
        virtual ~ALACFrameIndex();

        bool read(ALACSequence *seq);

        const QVector<QPair<tuint32,tuint32> >& list() const;

    protected:

        QVector<QPair<tuint32,tuint32> > m_frameList; // <offset,length>

        void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

inline const QVector<QPair<tuint32,tuint32> >& ALACFrameIndex::list() const
{
    return m_frameList;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

