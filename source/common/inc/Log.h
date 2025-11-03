//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_LOG_H
#define __OMEGA_COMMON_LOG_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BString.h"
#include "common/inc/UString.h"
#include <QString>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT Log
{
    public:
        Log();
        Log(const tchar *fileName);
        Log(const BString& fileName);
        Log(const Log& log);
        virtual ~Log();
        
        virtual const Log& operator = (const Log& log);
        
        virtual void set(const tchar *str);
        virtual void set(const BString& str);
        virtual void set(const QString& str);
        
        virtual void write(const tchar *str);
        virtual void write(const BString& str);
        virtual void write(const QString& str);
        
        virtual void on();
        virtual void off();
        
        virtual void print(const char *fmt,...);
        
        static Log g_Log;

    protected:

        BString m_logName;
        BString m_line;
        bool m_timeStamp;
        tchar *m_printBuffer;
        
        void writeOutLine(const BString& line) const;
        void buildTimeStamp(BString& str) const;
        void copy(const Log& log);
};

COMMON_EXPORT Log& operator << (Log& in,const char *str);
COMMON_EXPORT Log& operator << (Log& in,const BString& str);
COMMON_EXPORT Log& operator << (Log& in,tint i);
COMMON_EXPORT Log& operator << (Log& in,tchar s);
COMMON_EXPORT Log& operator << (Log& in,const tushort *str);
COMMON_EXPORT Log& operator << (Log& in,const UString& str);
COMMON_EXPORT Log& operator << (Log& in,const QString& str);

const tchar c_endl[] = "\r\n";

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
