//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_RTP_SILVEROMEGA_VTIMECALC_H
#define __OMEGA_NETWORK_RTP_SILVEROMEGA_VTIMECALC_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Array.h"
#include "common/inc/TimeStamp.h"
#include "engine/inc/File.h"
#include "engine/silveromega/inc/VSilverContainer.h"
#include "network/rtp/silveromega/inc/RTPSilverOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

typedef common::Array<tubyte,tubyte> RTPArray;

//-------------------------------------------------------------------------------------------

class RTPSILVEROMEGA_EXPORT VTimeCalc
{
    public:
        VTimeCalc();
        VTimeCalc(QSharedPointer<engine::silveromega::VSilverContainer> container);
        virtual ~VTimeCalc();

        virtual bool setConfiguration(RTPArray& info,RTPArray& comments,RTPArray& data);
        virtual bool setConfiguration(QSharedPointer<engine::silveromega::VSilverContainer> container);

        virtual void initLength();

        virtual tint calcLength(RTPArray& array);

        virtual tint frequency() const;

    protected:

        QSharedPointer<engine::silveromega::VSilverContainer> m_container;

        tint m_prevBlockMode;
        tint m_currentBlockMode;
        tint m_centerW;
        tint m_outCurrent;
        tint m_outReturn;

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

