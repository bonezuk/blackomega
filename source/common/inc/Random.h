//-------------------------------------------------------------------------------------------
// PRNG is Mersenne Twister implementation MT19937
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_RANDOM_H
#define __OMEGA_COMMON_RANDOM_H
//-------------------------------------------------------------------------------------------

#include <QMap>
#include <QMutex>
#include <QThread>

#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

#define RANDOM_MT19937_LENGTH_N 624
#define RANDOM_MT19937_LENGTH_M 397
#define RANDOM_MT19937_MATRIX_A 0x9908B0DF
#define RANDOM_MT19937_UPPER_MASK 0x80000000
#define RANDOM_MT19937_LOWER_MASK 0x7fffffff

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT Random
{
    public:
        ~Random();

        static Random *instance();

        // generates a random number on [0,0xffffffff]-interval
        tuint randomUInt32();
        // generates a random number on [0,0x7fffffff]-interval
        tint randomInt31();
        // generates a random number on [0,0xffffffffffffffff]-interval
        tuint64 randomUInt64();
        
        // generates a random number on [0,1]-real-interval
        tfloat64 randomReal1();
        // generates a random number on [0,1)-real-interval
        tfloat64 randomReal2();
        // generates a random number on (0,1)-real-interval
        tfloat64 randomReal3();
        // generates a random number on [0,1) with 53-bit resolution
        tfloat64 randomReal53();

        void reset();
        
        void seed(tint seedValue);
        void seed(tuint seedValue);
        void seed(const tuint32 *key,tint keyLength);

    private:

        static QMutex m_randomMutex;
        static QMap<Qt::HANDLE,Random *> m_instanceMap;
        
        tuint32 m_mt[RANDOM_MT19937_LENGTH_N];
        tint m_mti;
        
        Random();

        void initializePRNG(tuint32 s);
        void initializePRNGArray(const tuint32 *key,tint keyLength);
        
        tuint generateRandomUInt32();
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

