//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FIXEDDATAQUEUE_H
#define __OMEGA_ENGINE_FIXEDDATAQUEUE_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include <QPair>
#include <QSharedPointer>
#include <QMutex>
#include <QSemaphore>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

template <typename X> class FixedDataQueue
{
    public:
        FixedDataQueue(int arrayLen);
        virtual ~FixedDataQueue();

        X *get();
        void free(X *x);

        X *pull();
        X *pull(int timeoutMs);
        void push(X *x);

        int arraySize() const;

    private:
        int m_arraySize;
        QVector<X *> m_queue;
        QVector<X *> m_freeBuffers;
        QMutex m_mutex;
        QSemaphore m_semaphore;

        void freeQueue(QVector<X *>& queue);
};

//-------------------------------------------------------------------------------------------

template <typename X> FixedDataQueue<X>::FixedDataQueue(int arrayLen) : m_arraySize(arrayLen),
    m_queue(),
    m_freeBuffers(),
    m_mutex(),
    m_semaphore()
{}

//-------------------------------------------------------------------------------------------

template <typename X> FixedDataQueue<X>::~FixedDataQueue()
{
    freeQueue(m_queue);
    freeQueue(m_freeBuffers);
}

//-------------------------------------------------------------------------------------------

template <typename X> void FixedDataQueue<X>::freeQueue(QVector<X *>& queue)
{
    auto ppI = queue.begin();
    while(ppI != queue.end())
    {
        X *x = *ppI;
        delete [] x;
        ppI = queue.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

template <typename X> X *FixedDataQueue<X>::get()
{
    X *x;

    m_mutex.lock();
    if(m_freeBuffers.isEmpty())
    {
        x = new X [m_arraySize];
    }
    else
    {
        x = m_freeBuffers.takeFirst();
    }
    m_mutex.unlock();
    return x;
}

//-------------------------------------------------------------------------------------------

template <typename X> void FixedDataQueue<X>::free(X *x)
{
    if(x != NULL)
    {
        m_mutex.lock();
        m_freeBuffers.append(x);
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------

template <typename X> X *FixedDataQueue<X>::pull()
{
    X *x;

    m_semaphore.acquire(1);
    m_mutex.lock();
    x = m_queue.takeFirst();
    m_mutex.unlock();
    return x;
}

//-------------------------------------------------------------------------------------------

template <typename X> X *FixedDataQueue<X>::pull(int timeoutMs)
{
    X *x;

    if(m_semaphore.tryAcquire(1, timeoutMs))
    {
        m_mutex.lock();
        x = m_queue.takeFirst();
        m_mutex.unlock();
    }
    else
    {
        x = NULL;
    }
    return x;
}

//-------------------------------------------------------------------------------------------

template <typename X> void FixedDataQueue<X>::push(X *x)
{
    if(x != NULL)
    {
        m_mutex.lock();
        m_queue.append(x);
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------

template <typename X> int FixedDataQueue<X>::arraySize() const
{
    return m_arraySize;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

