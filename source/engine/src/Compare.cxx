#include "engine/inc/Compare.h"
#include <stdio.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

Compare g_Compare;

//-------------------------------------------------------------------------------------------

Compare::Compare(QObject *parent) : QThread(parent),
	m_mutex(),
	m_condition(),
	m_semaphore(),
	m_type(e_intCompare),
	m_frameA(0),
	m_frameB(0),
	m_len(0),
	m_memIntA(0),
	m_memIntB(0),
	m_memFloatA(0),
	m_memFloatB(0),
	m_memDoubleA(0),
	m_memDoubleB(0),
	m_threadID(0)
{}

//-------------------------------------------------------------------------------------------

Compare::~Compare()
{}

//-------------------------------------------------------------------------------------------

bool Compare::isThreadA()
{
	return (m_threadID == QThread::currentThreadId()) ? true : false;
}

//-------------------------------------------------------------------------------------------

void Compare::setThreadA()
{
	m_threadID = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

void Compare::compareA(tint *mem,tint len)
{
	m_mutex.lock();
	m_type = e_intCompare;
	m_memIntA = mem;
	m_len = len;
	m_frameA++;
	m_semaphore.release();
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void Compare::compareB(tint *mem,tint len)
{
	m_mutex.lock();
	m_type = e_intCompare;
	m_memIntB = mem;
	m_len = len;
	m_frameB++;
	m_semaphore.release();
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void Compare::compareA(tfloat32 *mem,tint len,bool printF)
{
	m_mutex.lock();
	m_printA = printF;
	m_type = e_floatCompare;
	m_memFloatA = mem;
	m_len = len;
	m_frameA++;
	m_semaphore.release();
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void Compare::compareB(tfloat32 *mem,tint len,bool printF)
{
	m_mutex.lock();
	m_printB = printF;
	m_type = e_floatCompare;
	m_memFloatB = mem;
	m_len = len;
	m_frameB++;
	m_semaphore.release();
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void Compare::compareA(tfloat64 *mem,tint len)
{
	m_mutex.lock();
	m_type = e_doubleCompare;
	m_memDoubleA = mem;
	m_len = len;
	m_frameA++;
	m_semaphore.release();
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void Compare::compareB(tfloat64 *mem,tint len)
{
	m_mutex.lock();
	m_type = e_doubleCompare;
	m_memDoubleB = mem;
	m_len = len;
	m_frameB++;
	m_semaphore.release();
	m_condition.wait(&m_mutex);
	m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void Compare::comp(tint *a,tint *b,tint len)
{
	tint i;
	
	for(i=0;i<len;++i)
	{
		if(a[i]!=b[i])
		{
			Q_ASSERT(false);
		}
	}
}

//-------------------------------------------------------------------------------------------

void Compare::comp(tfloat32 *a,tfloat32 *b,tint len,tfloat64 tolerance)
{
	tint i;

	for(i=0;i<len;++i)
	{
		if(m_printA && m_printB)
		{
			common::Log::g_Log.print("%d - %.8f\t%.8f\t%.8f\n",i,a[i],b[i],a[i] - b[i]);
		}
	}
	if(m_printA && m_printB)
	{
		common::Log::g_Log.print("\n\n");
	}

	tfloat64 xA,xB,yA,yB;
	tfloat64 max = 0.0,k = 1.0;

	for(i=0;i<len;++i)
	{
		yA = static_cast<tfloat64>(a[i]);
		yB = static_cast<tfloat64>(b[i]);
		if(yA < 0.0)
		{
			yA = -yA;
		}
		if(yB < 0.0)
		{
			yB = -yB;
		}
		if(max < yA)
		{
			max = yA;
		}
		if(max < yB)
		{
			max = yB;
		}
	}

	if(max!=0.0)
	{
		k = 1.0 / max;
	
		for(i=0;i<len;++i)
		{		
			xA = static_cast<tfloat64>(a[i]) * k;
			xB = static_cast<tfloat64>(b[i]) * k;
			yA = static_cast<tfloat64>(xB) - tolerance;
			yB = static_cast<tfloat64>(xB) + tolerance;
		
			if(!(yA<=xA && xA<=yB))
			{
				Q_ASSERT(false);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void Compare::comp(tfloat64 *a,tfloat64 *b,tint len,tfloat64 tolerance)
{
	tint i;
	
	for(i=0;i<len;++i)
	{
		tfloat64 x,yA,yB;
		
		x = a[i];
		yA = b[i] - tolerance;
		yB = b[i] + tolerance;
		
		if(!(yA<=x && x<=yB))
		{
			Q_ASSERT(false);
		}
	}	
}

//-------------------------------------------------------------------------------------------

void Compare::run()
{
	while(true)
	{
		m_semaphore.acquire(2);
		m_mutex.lock();
		switch(m_type)
		{
			case e_intCompare:
				{
					int *memA = (tint *)(m_memIntA);
					int *memB = (tint *)(m_memIntB);
					int len = static_cast<int>(m_len);
					comp(memA,memB,len);
					m_memIntA = 0;
					m_memIntB = 0;
				}
				break;
				
			case e_floatCompare:
				{
					tfloat32 *memA = (tfloat32 *)(m_memFloatA);
					tfloat32 *memB = (tfloat32 *)(m_memFloatB);
					int len = static_cast<int>(m_len);
					comp(memA,memB,len,0.001);
					m_memFloatA = 0;
					m_memFloatB = 0;
				}
				break;
				
			case e_doubleCompare:
				{
					tfloat64 *memA = (tfloat64 *)(m_memDoubleA);
					tfloat64 *memB = (tfloat64 *)(m_memDoubleB);
					int len = static_cast<int>(m_len);
					comp(memA,memB,len,0.001);
					m_memDoubleA = 0;
					m_memDoubleB = 0;
				}
				break;
				
			case e_icsCompare:
			default:
				break;
		}
		m_mutex.unlock();
		m_condition.wakeAll();
	}
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

