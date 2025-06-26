#include "common/inc/ProcessInfo.h"
#include "common/inc/DiskOps.h"
#include "common/inc/Log.h"

#if defined(OMEGA_WIN32)
#include <Tlhelp32.h>
#elif defined(OMEGA_MACOSX)
#include <sys/sysctl.h>
#endif

#if defined(OMEGA_LINUX)
#include <dirent.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

ProcessInfo *ProcessInfo::m_instance = 0;

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

ProcessInfo::ProcessInfo() : m_sParse()
{
    m_sState[0] = m_sParse.String("Name");
    m_sState[1] = m_sParse.String(":");
    m_sState[2] = m_sParse.String("\r\n");
    m_sState[3] = m_sParse.String("\n");
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

ProcessInfo::ProcessInfo()
{}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

ProcessInfo::~ProcessInfo()
{}

//-------------------------------------------------------------------------------------------

void ProcessInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ProcessInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

ProcessInfo& ProcessInfo::instance()
{
    if(m_instance==0)
    {
        m_instance = new ProcessInfo();
    }
    return *m_instance;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool ProcessInfo::getProcessNames(QStringList& nameList)
{
    HANDLE pHandle;
    bool res = false;

    nameList.clear();
    pHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(pHandle!=INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe;

        ::memset(&pe,0,sizeof(PROCESSENTRY32));
        pe.dwSize = sizeof(PROCESSENTRY32);

        if(Process32First(pHandle,&pe))
        {
            res = true;

            do
            {
                QString pName(QString::fromUtf16(reinterpret_cast<const char16_t *>(pe.szExeFile)));

                if(!pName.isEmpty())
                {
                    nameList.append(pName);
                }
            } while(Process32Next(pHandle,&pe));
        }
        else
        {
            printError("getProcessNames","Error getting first process information");
        }
        ::CloseHandle(pHandle);
    }
    else
    {
        printError("getProcessNames","Failed to get snapshot of processes");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

bool ProcessInfo::getProcessNames(QStringList& nameList)
{
    static const int name[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

    int i,amount,err;
    size_t length;
    struct kinfo_proc *pInfo;
    bool res = false;

    nameList.clear();
    do
    {
        length = 0;

        err = ::sysctl((int *)name,(sizeof(name) / sizeof(*name)) - 1,0,&length,0,0);
        if(err==0)
        {
            pInfo = reinterpret_cast<struct kinfo_proc *>(::malloc(length));
            if(pInfo!=0)
            {
                err = ::sysctl((int *)name,(sizeof(name) / sizeof(*name)) - 1,(void *)pInfo,&length,0,0);
                if(err==0)
                {
                    struct kinfo_proc *info = pInfo;

                    amount = static_cast<int>(length / sizeof(struct kinfo_proc));
                    for(i=0;i<amount;++i)
                    {
                        QString pName(QString::fromUtf8(info->kp_proc.p_comm));
                        nameList.append(pName);
                        info++;
                    }
                    res = true;
                }
                else
                {
                    if(err==-1)
                    {
                        err = errno;
                    }
                    if(err==ENOMEM)
                    {
                        err = 0;
                    }
                }
                ::free(pInfo);
            }
            else
            {
                err = ENOMEM;
            }
        }
        else
        {
            err = errno;
        }
    } while(err==0 && !res);

    return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

bool ProcessInfo::getProcessNames(QStringList& nameList)
{
    tint i;
    DIR *h;
    struct dirent *entry;
    struct stat pStat;
    bool res = false;

    h = ::opendir("/proc");
    if(h!=0)
    {
        while(entry=::readdir(h),entry!=0)
        {
            QString n,pName(QString::fromUtf8(entry->d_name));

            if(!pName.isEmpty() && pName!="." && pName!="..")
            {
                for(i=0;i<pName.size();++i)
                {
                    if(pName.at(i)>=QChar('0') && pName.at(i)<='9')
                    {
                        break;
                    }
                }

                if(i==pName.size())
                {
                    pName = "/proc/" + pName;
                    if(::stat(pName.toUtf8().constData(),&pStat)==0)
                    {
                        if(S_IFDIR & pStat.st_mode)
                        {
                            if(parseProcessInfo(pName,n))
                            {
                                nameList.append(n);
                            }
                        }
                    }
                }
            }
        }
        ::closedir(h);

        res = true;
    }
    else
    {
        printError("getProcessNames","Failed to open /proc directory");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ProcessInfo::parseProcessInfo(const QString& pidDir,QString& name)
{
    QString sName;
    BIOStream sFile(e_BIOStream_FileRead);
    bool res = false;

    sName = pidDir + "/status";
    if(sFile.open(sName))
    {
        tint sSize = sFile.size();

        if(sSize>0)
        {
            tchar *mem = new tchar [sSize + 1];

            if(sFile.read(mem,sSize)==sSize)
            {
                tint state = 0,start = 0;
                BO_Parse_Unit *item;

                mem[sSize] = '\0';
                item = m_sParse.Lexical(mem);
                while(item!=0 && !res)
                {
                    switch(state)
                    {
                        case 0:
                            if(item->state==m_sState[0])
                            {
                                state = 1;
                            }
                            break;

                        case 1:
                            if(item->state==m_sState[1])
                            {
                                start = item->start + item->length;
                                state = 2;
                            }
                            else
                            {
                                state = 0;
                            }
                            break;

                        case 2:
                            if(item->state==m_sState[2] || item->state==m_sState[3])
                            {
                                mem[item->start] = '\0';
                                name = QString::fromUtf8(&mem[start]).trimmed();
                                res = true;
                            }
                            else
                            {
                                state = 0;
                            }
                            break;
                    }
                    item = item->next;
                }
            }
            delete [] mem;
        }
        sFile.close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
