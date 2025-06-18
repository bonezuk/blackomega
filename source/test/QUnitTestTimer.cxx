#include "test/QUnitTestTimer.h"

//-------------------------------------------------------------------------------------------

QUnitTestTimer::QUnitTestTimer()
{
    m_start = common::TimeStamp::reference();
}

//-------------------------------------------------------------------------------------------

QUnitTestTimer::~QUnitTestTimer()
{
    common::TimeStamp endT = common::TimeStamp::reference();
    common::TimeStamp diffT = endT - m_start;
    qDebug("Time needed: %d ms",static_cast<tint>(static_cast<tfloat64>(diffT) * 1000.0));
}

//-------------------------------------------------------------------------------------------
