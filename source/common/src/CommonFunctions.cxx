#include "common/inc/CommonFunctions.h"
#include <QtGlobal>
#include <QThread>

#if defined(OMEGA_LINUX)
#include <dlfcn.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QString getOSErrorString(tint errCode)
{
    QString err;

#if defined(OMEGA_WIN32)
    tushort *wMem = new tushort [1024];

    if(::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,0,static_cast<DWORD>(errCode),0,reinterpret_cast<LPWSTR>(wMem),1024,0)!=0)
    {
        err = QString::fromUtf16(reinterpret_cast<const char16_t *>(wMem));
    }
    delete [] wMem;

#elif defined(OMEGA_POSIX)
    const tchar *e = ::strerror(errCode);
    if(e!=0)
    {
        err = QString::fromUtf8(e);
    }
#endif
    return err;
}

//-------------------------------------------------------------------------------------------

void msleepThread(tint msecs)
{
#if QT_VERSION >= 0x050000
    QThread::msleep(msecs);
#else
    usleepThread(msecs * 1000);
#endif
}

//-------------------------------------------------------------------------------------------

void usleepThread(tint usecs)
{
#if QT_VERSION >= 0x050000
    QThread::usleep(usecs);
#else
#if defined(OMEGA_WIN32)
    LARGE_INTEGER lFrequency;
    LARGE_INTEGER lEndTime;
    LARGE_INTEGER lCurTime;

    QueryPerformanceFrequency(&lFrequency);
    if (lFrequency.QuadPart)
    {
        QueryPerformanceCounter(&lEndTime);
        lEndTime.QuadPart += (LONGLONG) usec * lFrequency.QuadPart / 1000000;
        do
        {
            QueryPerformanceCounter(&lCurTime);
            Sleep(0);
        } while (lCurTime.QuadPart < lEndTime.QuadPart);
    }
#elif defined(OMEGA_POSIX)
    ::usleep(usecs);
#endif
#endif
}

//-------------------------------------------------------------------------------------------

void loadSharedLibrary(const char *libName)
{
#if defined(OMEGA_WIN32)
    QString name = QString::fromUtf8(libName) + ".dll";
    LoadLibraryA(name.toUtf8().constData());
#elif defined(OMEGA_LINUX)
    void *handle;
    QString name = "lib" + QString::fromUtf8(libName) + ".so";
    handle = dlopen(name.toUtf8().constData(), RTLD_NOW);
    Q_ASSERT(handle != NULL);
#endif
}

//-------------------------------------------------------------------------------------------

tuint64 elfHash64(tuint8 *mem, int len)
{
    tuint64 hash = 0;
    return elfHash64(mem, len, hash);
}

//-------------------------------------------------------------------------------------------

tuint64 elfHash64(tuint8 *mem, int len, tuint64 hash)
{
    int i;
    tuint64 high;

    if(mem != 0)
    {
        for(i = 0; i < len; i++)
        {
            hash = (hash << 4) + static_cast<tuint64>(mem[i]);
            high = hash & 0xf000000000000000ULL;
            if(high)
            {
                hash ^= high >> (24 + 32);
            }
            hash &= ~high;
        }
    }
    return hash;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
