#if defined(OMEGA_WIN32)
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#endif

#include "common/inc/Random.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QMutex Random::m_randomMutex;
QMap<Qt::HANDLE,Random *> Random::m_instanceMap;

//-------------------------------------------------------------------------------------------

Random::Random()
{
    reset();
}

//-------------------------------------------------------------------------------------------

Random::~Random()
{
    QMap<Qt::HANDLE,Random *>::iterator ppI;
    Qt::HANDLE threadHandle = QThread::currentThreadId();

    m_randomMutex.lock();
    ppI = m_instanceMap.find(threadHandle);
    if(ppI!=m_instanceMap.end())
    {
        m_instanceMap.erase(ppI);
    }
    m_randomMutex.unlock();
}

//-------------------------------------------------------------------------------------------

Random *Random::instance()
{
    Random *pInstance;
    QMap<Qt::HANDLE,Random *>::iterator ppI;
    Qt::HANDLE threadHandle = QThread::currentThreadId();

    m_randomMutex.lock();
    ppI = m_instanceMap.find(threadHandle);
    if(ppI!=m_instanceMap.end())
    {
        pInstance = ppI.value();
    }
    else
    {
        pInstance = new Random;
        pInstance->seed((tuint)((ptrdiff_t)(threadHandle)));
        m_instanceMap.insert(threadHandle,pInstance);
    }
    m_randomMutex.unlock();

    return pInstance;
}

//-------------------------------------------------------------------------------------------

void Random::reset()
{
    m_mti = RANDOM_MT19937_LENGTH_N + 1;
    initializePRNG(5489UL);
}

//-------------------------------------------------------------------------------------------

void Random::seed(tint seedValue)
{
    initializePRNG(static_cast<tuint32>(seedValue));
}

//-------------------------------------------------------------------------------------------

void Random::seed(tuint seedValue)
{
    initializePRNG(seedValue);
}

//-------------------------------------------------------------------------------------------

void Random::seed(const tuint32 *key,tint keyLength)
{
    initializePRNGArray(key,keyLength);
}

//-------------------------------------------------------------------------------------------

void Random::initializePRNG(tuint32 s)
{
    m_mt[0] = s & 0xffffffff;
    for(m_mti=1;m_mti < RANDOM_MT19937_LENGTH_N;m_mti++)
    {
        m_mt[m_mti] = (1812433253 * (m_mt[m_mti-1] ^ (m_mt[m_mti-1] >> 30)) + m_mti);
        m_mt[m_mti] &= 0xffffffff;
    }
}

//-------------------------------------------------------------------------------------------

void Random::initializePRNGArray(const tuint32 *key,tint keyLength)
{
    tint i = 1;
    tint j = 0;
    tint k = (RANDOM_MT19937_LENGTH_N > keyLength) ? RANDOM_MT19937_LENGTH_N : keyLength;

    initializePRNG(19650218UL);

    while(k)
    {
        m_mt[i] = (m_mt[i] ^ ((m_mt[i-1] ^ (m_mt[i-1] >> 30)) * 1664525)) + key[j] + j;
        m_mt[i] &= 0xffffffff;
        i++;
        j++;
        if(i>=RANDOM_MT19937_LENGTH_N)
        {
            m_mt[0] = m_mt[RANDOM_MT19937_LENGTH_N-1];
            i=1;
        }
        if(j>=keyLength)
        {
            j=0;
        }
        k--;
    }

    for(k=RANDOM_MT19937_LENGTH_N-1;k;k--)
    {
        m_mt[i] = (m_mt[i] ^ ((m_mt[i-1] ^ (m_mt[i-1] >> 30)) * 1566083941)) - i;
        m_mt[i] &= 0xffffffff;
        i++;
        if(i>=RANDOM_MT19937_LENGTH_N)
        {
            m_mt[0] = m_mt[RANDOM_MT19937_LENGTH_N-1];
            i=1;
        }
    }

    m_mt[0] = 0x80000000;
}
//-------------------------------------------------------------------------------------------

tuint Random::generateRandomUInt32()
{
    static tuint mag01[2] = {0x00000000, RANDOM_MT19937_MATRIX_A};

    tuint y;

    if(m_mti>=RANDOM_MT19937_LENGTH_N)
    {
        int kk;

        if(m_mti==RANDOM_MT19937_LENGTH_N+1)
        {
            initializePRNG(5489UL);
        }

        for(kk=0;kk<RANDOM_MT19937_LENGTH_N-RANDOM_MT19937_LENGTH_M;kk++)
        {
            y = (m_mt[kk] & RANDOM_MT19937_UPPER_MASK) | (m_mt[kk+1] & RANDOM_MT19937_LOWER_MASK);
            m_mt[kk] = m_mt[kk+RANDOM_MT19937_LENGTH_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        for(;kk<RANDOM_MT19937_LENGTH_N-1;kk++)
        {
            y = (m_mt[kk] & RANDOM_MT19937_UPPER_MASK) | (m_mt[kk+1] & RANDOM_MT19937_LOWER_MASK);
            m_mt[kk] = m_mt[kk+(RANDOM_MT19937_LENGTH_M-RANDOM_MT19937_LENGTH_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        y = (m_mt[RANDOM_MT19937_LENGTH_N-1] & RANDOM_MT19937_UPPER_MASK) | (m_mt[0] & RANDOM_MT19937_LOWER_MASK);
        m_mt[RANDOM_MT19937_LENGTH_N-1] = m_mt[RANDOM_MT19937_LENGTH_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        m_mti = 0;
    }

    y = m_mt[m_mti++];

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

//-------------------------------------------------------------------------------------------
// generates a random number on [0,0xffffffff]-interval
//-------------------------------------------------------------------------------------------

tuint Random::randomUInt32()
{
    return generateRandomUInt32();
}

//-------------------------------------------------------------------------------------------
// generates a random number on [0,0x7fffffff]-interval
//-------------------------------------------------------------------------------------------

tint Random::randomInt31()
{
    return static_cast<tint>(generateRandomUInt32() >> 1);
}

//-------------------------------------------------------------------------------------------
// generates a random number on [0,0xffffffffffffffff]-interval
//-------------------------------------------------------------------------------------------

tuint64 Random::randomUInt64()
{
    tuint64 a = static_cast<tuint64>(generateRandomUInt32());
    tuint64 b = static_cast<tuint64>(generateRandomUInt32());
    a = ((a << 32) & 0xffffffff00000000ULL) | (b & 0x00000000ffffffffULL);
    return a;
}

//-------------------------------------------------------------------------------------------
// generates a random number on [0,1]-real-interval
//-------------------------------------------------------------------------------------------

tfloat64 Random::randomReal1()
{
    return static_cast<tfloat64>(generateRandomUInt32()) * (1.0/4294967295.0);
}

//-------------------------------------------------------------------------------------------
// generates a random number on [0,1)-real-interval
//-------------------------------------------------------------------------------------------

tfloat64 Random::randomReal2()
{
    return static_cast<tfloat64>(generateRandomUInt32()) * (1.0/4294967296.0);
}

//-------------------------------------------------------------------------------------------
// generates a random number on (0,1)-real-interval
//-------------------------------------------------------------------------------------------

tfloat64 Random::randomReal3()
{
    return (static_cast<tfloat64>(generateRandomUInt32()) + 0.5) * (1.0/4294967296.0);
}

//-------------------------------------------------------------------------------------------
// generates a random number on [0,1) with 53-bit resolution
//-------------------------------------------------------------------------------------------

tfloat64 Random::randomReal53()
{
    tuint a = generateRandomUInt32() >> 5;
    tuint b = generateRandomUInt32() >> 6;
    return ((static_cast<tfloat64>(a) * 67108864.0) + static_cast<tfloat64>(b)) * (1.0/9007199254740992.0);
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
