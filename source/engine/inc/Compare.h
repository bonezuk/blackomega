//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMPARE_H
#define __OMEGA_COMPARE_H
//-------------------------------------------------------------------------------------------

#include <QtGlobal>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSemaphore>

#if defined(OMEGA_WIN32)
#include <windows.h>
#endif

#include "common/inc/Log.h"
#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Compare : public QThread
{
    public:
        Q_OBJECT

    public:
        typedef enum
        {
            e_intCompare = 0,
            e_floatCompare,
            e_doubleCompare,
            e_icsCompare,
            e_uint16Compare,
            e_int24ByteCompare
        } CompareType;

    public:
        Compare(QObject *parent=0);
        virtual ~Compare();

        void compareA(tint *mem,tint len);
        void compareB(tint *mem,tint len);

        void compareA(tuint16 *mem,tint len);
        void compareB(tuint16 *mem,tint len);

        void compareA(tfloat32 *mem,tint len);
        void compareB(tfloat32 *mem,tint len);

        void compareA(tfloat32 *mem,tint len,bool printF=false);
        void compareB(tfloat32 *mem,tint len,bool printF=false);

        void compareA(tfloat64 *mem,tint len);
        void compareB(tfloat64 *mem,tint len);

        void compareAInt24(tubyte *mem, tint len);
        void compareBInt24(tint32 *mem, tint len);

        tint frameA() const;
        tint frameB() const;

        void setThreadA();
        bool isThreadA();

    protected:

        QMutex m_mutex;
        QWaitCondition m_condition;
        QSemaphore m_semaphore;

        Qt::HANDLE m_threadID;

        volatile CompareType m_type;
        volatile tint m_frameA;
        volatile tint m_frameB;
        volatile tint m_len;
        volatile tint *m_memIntA,*m_memIntB;
        volatile tuint16 *m_memUInt16A,*m_memUInt16B;
        volatile tfloat32 *m_memFloatA,*m_memFloatB;
        volatile tfloat64 *m_memDoubleA,*m_memDoubleB;
        volatile tubyte *m_memInt24A;
        volatile tint32 *m_memInt24B;
        volatile bool m_printA,m_printB;

        void comp(tint *a,tint *b,tint len);
        void comp(tuint16 *a,tuint16 *b,tint len);
        void comp(tfloat32 *a,tfloat32 *b,tint len,tfloat64 tolerance);
        void comp(tfloat64 *a,tfloat64 *b,tint len,tfloat64 tolerance);

        void compInt24(tubyte *a, tint32 *b, tint len);

        virtual void run();
};

//-------------------------------------------------------------------------------------------

inline tint Compare::frameA() const
{
    return m_frameA;
}

//-------------------------------------------------------------------------------------------

inline tint Compare::frameB() const
{
    return m_frameB;
}

//-------------------------------------------------------------------------------------------

extern ENGINE_EXPORT Compare g_Compare;

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
