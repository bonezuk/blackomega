#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "common/inc/Log.h"
#include "common/inc/BIOStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

Log Log::g_Log;

//-------------------------------------------------------------------------------------------

Log::Log() : m_logName(),
	m_line(),
	m_timeStamp(true)
{
	m_printBuffer = new tchar [2048];
}

//-------------------------------------------------------------------------------------------

Log::Log(const tchar *fileName) : m_logName(fileName),
	m_line(),
	m_timeStamp(true)
{
	m_printBuffer = new tchar [2048];
}

//-------------------------------------------------------------------------------------------

Log::Log(const BString& fileName) : m_logName(fileName),
	m_line(),
	m_timeStamp(true)
{
	m_printBuffer = new tchar [2048];
}

//-------------------------------------------------------------------------------------------

Log::Log(const Log& log) : m_logName(),
	m_line(),
	m_timeStamp(true)
{
	m_printBuffer = new tchar [2048];
	copy(log);
}

//-------------------------------------------------------------------------------------------

Log::~Log()
{
	delete [] m_printBuffer;
	m_printBuffer = 0;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

void Log::writeOutLine(const BString& line) const
{
	HANDLE file;
	DWORD amount;
	BString str;
	
	if(!m_logName.IsEmpty())
	{
		if(m_timeStamp)
		{
			buildTimeStamp(str);
		}
		else
		{
			str += "\t";
		}
		str += line + c_endl;
		
		file = ::CreateFileA(static_cast<const tchar *>(m_logName),GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if(file!=INVALID_HANDLE_VALUE)
		{
			::SetFilePointer(file,0,0,FILE_END);
			::WriteFile(file,static_cast<const tchar *>(str),static_cast<DWORD>(str.GetLength()),&amount,0);
			::CloseHandle(file);
		}
	}
	fprintf(stdout,"%s\n",static_cast<const tchar *>(line));
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

void Log::writeOutLine(const BString& line) const
{
	file_type file;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	BString str;
	
	if(!m_logName.IsEmpty())
	{
		if(m_timeStamp)
		{
			buildTimeStamp(str);
		}
		else
		{
			str += "\t";
		}
		str += line + c_endl;
		
		file = ::open(static_cast<const tchar *>(m_logName),O_WRONLY | O_CREAT,mode);
		if(file!=c_invalidFile)
		{
			::lseek(file,0,SEEK_END);
			::write(file,static_cast<const tchar *>(str),static_cast<size_t>(str.GetLength()));
			::close(file);
		}
	}
	fprintf(stdout,"%s\n",static_cast<const tchar *>(line));
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void Log::print(const char *fmt,...)
{
	va_list argp;
	va_start(argp, fmt);
	vsnprintf(m_printBuffer, 2048, fmt, argp);
	write(m_printBuffer);
	va_end(argp);
}

//-------------------------------------------------------------------------------------------

void Log::write(const QString& str)
{
	write(str.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

void Log::write(const tchar *str)
{
	BString s(str);
	write(s);
}

//-------------------------------------------------------------------------------------------

void Log::write(const BString& str)
{
	tint i=0,j=0,k=0,state=0;
	const tchar *s = static_cast<const tchar *>(str);
	
	if(!str.IsEmpty())
	{
		while(s[i]!='\0')
		{
			switch(state)
			{
				case 0:
					if(s[i]=='\r')
					{
						j = i;
						state = 1;
					}
					else if(s[i]=='\n')
					{
						j = i;
						state = 2;
					}
					break;
				
				case 1:
					if(s[i]=='\n')
					{
						state = 2;
					}
					else if(s[i]!='\r')
					{
						state = 0;
					}
					break;

				default:
					break;
			}
		
			if(state==2)
			{
				if(k<j)
				{
					m_line += str.Sub(static_cast<tuint>(k),static_cast<tuint>(j - k));
				}
				if(m_line.trim().length()>0)
				{
					writeOutLine(m_line);
				}
				m_line = "";
				k = i + 1;
				state = 0;
			}
			i++;
		}
		if(k<i)
		{
			m_line += str.Sub(static_cast<tuint>(k),static_cast<tuint>(i - k));
		}
	}
}

//-------------------------------------------------------------------------------------------

void Log::buildTimeStamp(BString& str) const
{
	static const char *weekDays[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	static const char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	
	time_t t;
	struct tm *tdate;
	
	::time(&t);
	tdate = ::gmtime(&t);
	str = "";
	
	if(tdate!=0)
	{
		str += weekDays[tdate->tm_wday];
		str += ", ";
		str += BString::Int(static_cast<tuint>(tdate->tm_mday),2);
		str += " ";
		str += months[tdate->tm_mon];
		str += " ";
		str += BString::Int(static_cast<tuint>(tdate->tm_year + 1900),4);
		str += " ";
		str += BString::Int(static_cast<tuint>(tdate->tm_hour),2);
		str += ":";
		str += BString::Int(static_cast<tuint>(tdate->tm_min),2);
		str += ":";
		str += BString::Int(static_cast<tuint>(tdate->tm_sec),2);
		str += " GMT : ";
	}
}

//-------------------------------------------------------------------------------------------

void Log::copy(const Log& log)
{
	m_logName = log.m_logName;
	m_line = log.m_line;
	m_timeStamp = log.m_timeStamp;
}

//-------------------------------------------------------------------------------------------

void Log::set(const QString& str)
{
	set(str.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

void Log::set(const tchar *str)
{
	m_logName = str;
}

//-------------------------------------------------------------------------------------------

void Log::set(const BString& str)
{
	m_logName = str;
}

//-------------------------------------------------------------------------------------------

const Log& Log::operator = (const Log& log)
{
	if(this!=&log)
	{
		copy(log);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void Log::on()
{
	m_timeStamp = true;
}

//-------------------------------------------------------------------------------------------

void Log::off()
{
	m_timeStamp = false;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,const char *str)
{
	in.write(str);
	return in;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,const BString& str)
{
	in.write(str);
	return in;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,tint i)
{
	BString s;
	
	s = BString::Int(i);
	in.write(s);
	return in;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,tchar s)
{
	tchar t[2] = {'\0','\0'};
	
	t[0] = s;
	in.write(t);
	return in;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,const tushort *str)
{
	UString s(str);
	BString a;
	
	a = s.AStr();
	in.write(a);
	return in;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,const UString& str)
{
	BString a;
	
	a = str.AStr();
	in.write(a);
	return in;
}

//-------------------------------------------------------------------------------------------

Log& operator << (Log& in,const QString& str)
{
	in.write(str);
	return in;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
