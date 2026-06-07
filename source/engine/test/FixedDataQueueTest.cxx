#include "gtest/gtest.h"
#include <QThread>

#include "common/inc/Random.h"
#include "engine/inc/FixedDataQueue.h"

using namespace omega;

const int c_testArraySize = 1024;

//-------------------------------------------------------------------------------------------

class FixedDataQueueTester : public QThread
{
    public:
        FixedDataQueueTester();
        virtual ~FixedDataQueueTester();

        virtual void push(const double *x);
        virtual void pull(double *x);

    protected:
        volatile bool m_isRunning;
        engine::FixedDataQueue<double> m_inputQueue;
        engine::FixedDataQueue<double> m_outputQueue;

        virtual void run() override;
};

//-------------------------------------------------------------------------------------------

FixedDataQueueTester::FixedDataQueueTester() : QThread(),
    m_isRunning(true),
    m_inputQueue(c_testArraySize),
    m_outputQueue(c_testArraySize)
{}

//-------------------------------------------------------------------------------------------

FixedDataQueueTester::~FixedDataQueueTester()
{
    m_isRunning = false;
    QThread::wait();
}

//-------------------------------------------------------------------------------------------

void FixedDataQueueTester::run()
{
    while(m_isRunning)
    {
        double *in = m_inputQueue.pull(50);
        if(in != NULL)
        {
            double *out = m_outputQueue.get();
            memcpy(out, in, c_testArraySize * sizeof(double));
            m_outputQueue.push(out);
            m_inputQueue.free(in);
        }
    }
}

//-------------------------------------------------------------------------------------------

void FixedDataQueueTester::push(const double *x)
{
    ASSERT_EQ(m_inputQueue.arraySize(), c_testArraySize);
    ASSERT_EQ(m_outputQueue.arraySize(), c_testArraySize);

    double *in = m_inputQueue.get();
    memcpy(in, x, c_testArraySize * sizeof(double));
    m_inputQueue.push(in);
}

//-------------------------------------------------------------------------------------------

void FixedDataQueueTester::pull(double *out)
{
    double *x = m_outputQueue.pull();
    memcpy(out, x, c_testArraySize * sizeof(double));
    m_outputQueue.free(x);
}

//-------------------------------------------------------------------------------------------

TEST(FixedDataQueue, multiThreadPushPull)
{
    const int c_loopCount = 1000;
    common::Random *rand = common::Random::instance();
    FixedDataQueueTester tester;
    double *in[2], *out;

    in[0] = new double [c_testArraySize];
    in[1] = new double [c_testArraySize];
    out = new double [c_testArraySize];

    tester.start();

    for(int idx = 0; idx < c_loopCount; idx++)
    {
        int iIdx = idx & 0x1;
        int oIdx = (idx + 1) & 0x01;
        double *a;

        if(idx < c_loopCount - 1)
        {
            a = in[iIdx];
            for(int j = 0; j < c_testArraySize; j++)
            {
                a[j] = rand->randomReal1();
            }
            tester.push(a);
        }

        if(idx > 0)
        {
            tester.pull(out);
            a = in[oIdx];
            for(int j = 0; j < c_testArraySize; j++)
            {
                EXPECT_NEAR(a[j], out[j], 0.00000001);
            }
        }
    }

    delete [] in[0];
    delete [] in[1];
    delete [] out;
}

//-------------------------------------------------------------------------------------------
