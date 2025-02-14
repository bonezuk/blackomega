#include "common/inc/DiskOps.h"

#if defined(OMEGA_POSIX)
#include <dirent.h>
#endif

#include "common/inc/BOQueueTree.h"
#include "common/inc/TTime.h"
#include "common/inc/SHA1Digest.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER BString BOQueueTree<BString>::nullObject = BString::null();
QMutex DiskOps::m_mutex;
QByteArray DiskOps::m_hashArray;

//-------------------------------------------------------------------------------------------

DiskOps::DiskOps() {}

//-------------------------------------------------------------------------------------------

void DiskOps::formatPath(const tchar *in,BString& name)
{
	QString a(QString::fromUtf8(in)),b;
	formatPath(a,b);
	name = b.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

void DiskOps::formatPath(const BString& in,BString& name)
{
	QString a(QString::fromUtf8(in.c_str())),b;
	formatPath(a,b);
	name = b.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

void DiskOps::formatPath(const QString& in,QString& name)
{
	name = QDir::toNativeSeparators(in);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::path(const tchar *name,bool dirFlag)
{
	QString n(QString::fromUtf8(name));
	return path(n,dirFlag);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::path(const BString& name,bool dirFlag)
{
	QString n(QString::fromUtf8(name.c_str()));
	return path(n,dirFlag);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool DiskOps::path(const QString& name,bool dirFlag)
{
	tint i,j,count;
	QString fileName;
	
	formatPath(name,fileName);
	
	if(fileName.isEmpty())
	{
		return false;
	}
	if(dirFlag && fileName.at(fileName.length() - 1)!=QChar('\\'))
	{
		fileName += "\\";
	}
	
	count = (fileName.at(0)==QChar('\\')) ? 3 : 1;
	for(i=0,j=0;j<count && i<fileName.length();++i)
	{
		if(fileName.at(i)==QChar('\\'))
		{
			j++;
		}
	}
	i--;
	
	if(i<fileName.length())
	{
		LPCWSTR wStr;
		struct _stat pathStat;
		QString tmp;
		
		j = 0;
		while(i<fileName.length())
		{
			if(fileName.at(i)==QChar('\\'))
			{
				tmp = fileName.mid(0,((!j) ? i+1 : i));
				wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
				if(::_wstat(wStr,&pathStat)==0)
				{
					if(!(S_IFDIR & pathStat.st_mode))
					{
						return false;
					}
				}
				else
				{
					if(!::CreateDirectoryW(wStr,0))
					{
						return false;
					}
				}
				j++;
			}
			i++;
		}
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool DiskOps::path(const QString& name,bool dirFlag)
{
	tint i, r;
	bool useLatin = false;
	struct stat pathStat;
	QString fileName,tmp;
	
	formatPath(name,fileName);
	
	if(fileName.isEmpty())
	{
		return false;
	}
	if(dirFlag && fileName.at(fileName.length() - 1)!=QChar('/'))
	{
		fileName += "/";
	}
	
	for(i=0;i<fileName.length();++i)
	{
		if(fileName.at(i)==QChar('/'))
		{
			tmp = fileName.mid(0,i+1);
			
			r = 1;
			if(!useLatin)
			{
				r = ::stat(tmp.toUtf8().constData(),&pathStat);
			}
			if(r)
			{
				r = ::stat(tmp.toLatin1().constData(),&pathStat);
				if(!r)
				{
					useLatin = true;
				}
			}
			
			if(!r)
			{
				if(!(S_IFDIR & pathStat.st_mode))
				{
					return false;
				}
			}
			else
			{
				if(!useLatin)
				{
					r = mkdir(tmp.toUtf8().constData(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
				}
				else
				{
					r = mkdir(tmp.toLatin1().constData(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
				}
				if(r!=0)
				{
					return false;
				}
			}
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool DiskOps::exist(const tchar *name)
{
	QString n(QString::fromUtf8(name));
	return exist(n);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::exist(const BString& name)
{
	QString n(QString::fromUtf8(name.c_str()));
	return exist(n);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool DiskOps::exist(const QString& name)
{
	LPCWSTR wStr;
	struct _stat pathStat;
	
	wStr = reinterpret_cast<LPCWSTR>(name.utf16());
	if(::_wstat(wStr,&pathStat)==0)
	{
		if(S_IFREG & pathStat.st_mode)
		{
			return true;
		}
		else if(S_IFDIR & pathStat.st_mode)
		{
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool DiskOps::exist(const QString& name)
{
	bool useLatin = false;
	return existLatin1(name, useLatin);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::existLatin1(const QString& name, bool& useLatin)
{
	int r;
	struct stat pathStat;
	
	r = ::stat(name.toUtf8().constData(),&pathStat);
	if(r)
	{
		r = ::stat(name.toLatin1().constData(),&pathStat);
		if(!r)
		{
			useLatin = true;
		}
	}
	else
	{
		useLatin = false;
	}
	if(!r)
	{
		if(S_IFREG & pathStat.st_mode || S_IFDIR & pathStat.st_mode || S_IFIFO & pathStat.st_mode)
		{
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

int DiskOps::fileType(const QString& name)
{
	QString nName = toNativeSeparators(name);
	LPCWSTR wStr;
	struct __stat64 pathStat;
	
	wStr = reinterpret_cast<LPCWSTR>(nName.utf16());
	if(::_wstat64(wStr, &pathStat)==0)
	{
		if(S_IFREG & pathStat.st_mode)
		{
			return 1;
		}
		else if(S_IFDIR & pathStat.st_mode)
		{
			return 2;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

int DiskOps::fileType(const QString& name)
{
	int r;
	struct stat pathStat;
	
	r = ::stat(name.toUtf8().constData(),&pathStat);
	if(r)
	{
		r = ::stat(name.toLatin1().constData(),&pathStat);
	} 
	if(r==0)
	{
		if(S_IFREG & pathStat.st_mode)
		{
			return 1;
		}
		else if(S_IFDIR & pathStat.st_mode)
		{
			return 2;
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void DiskOps::formatDirectoryPath(BString& dirName)
{
	QString n(QString::fromUtf8(dirName.c_str()));
	formatDirectoryPath(n);
	dirName = n.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

void DiskOps::formatDirectoryPath(QString& dirName)
{
	dirName = QDir::toNativeSeparators(dirName);
	if(dirName.at(dirName.length() - 1)==QChar(c_SeparatorChar))
	{
		dirName = dirName.mid(0,dirName.length() - 1);
	}
}

//-------------------------------------------------------------------------------------------

bool DiskOps::dotCheck(const QString& name)
{
	return (name=="." || name=="..") ? false : true;
}

//-------------------------------------------------------------------------------------------

bool DiskOps::dotCheck(const BString& name)
{
	return dotCheck(QString::fromUtf8(name.c_str()));
}

//-------------------------------------------------------------------------------------------

bool DiskOps::dotCheck(const tchar *name)
{
	return dotCheck(QString::fromUtf8(name));
}

//-------------------------------------------------------------------------------------------

bool DiskOps::deleteDirectory(const tchar *dirName)
{
	QString name(QString::fromUtf8(dirName));
	return deleteDirectory(name);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::deleteDirectory(const BString& dirName)
{
	QString name(QString::fromUtf8(dirName.c_str()));
	return deleteDirectory(name);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool DiskOps::deleteDirectory(const QString& dirName)
{
	LPCWSTR wStr;
	HANDLE h;
	struct _stat fileStat;
	WIN32_FIND_DATAW fData;
	QString tmp,fullPath(dirName);
	QStringList pathList;
	QStringList::iterator ppI;
	
	formatDirectoryPath(fullPath);
	wStr = reinterpret_cast<LPCWSTR>(fullPath.utf16());
	if(_wstat(wStr,&fileStat)!=0)
	{
		return false;
	}
	if(!(S_IFDIR & fileStat.st_mode))
	{
		return false;
	}
	
	::memset(&fData,0,sizeof(WIN32_FIND_DATAW));
	tmp = fullPath + "\\*";
	
	wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
	h = ::FindFirstFileW(wStr,&fData);
	if(h!=INVALID_HANDLE_VALUE)
	{
		do
		{
			QString cName(QString::fromUtf16(reinterpret_cast<const char16_t *>(fData.cFileName)));
			
			if(dotCheck(cName))
			{
				tmp = fullPath + "\\" + cName;
				wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
				if(_wstat(wStr,&fileStat)==0)
				{
					if(S_IFDIR & fileStat.st_mode)
					{
						pathList.append(tmp);
					}
					else if(S_IFREG & fileStat.st_mode)
					{
						if(!::SetFileAttributesW(wStr,FILE_ATTRIBUTE_NORMAL))
						{
							return false;
						}
						if(!::DeleteFileW(wStr))
						{
							return false;
						}
					}
				}
			}
		} while(::FindNextFileW(h,&fData));
		::FindClose(h);

		for(ppI=pathList.begin();ppI!=pathList.end();++ppI)
		{
			const QString& name = *ppI;
			
			if(!deleteDirectory(name))
			{
				return false;
			}
		}		
	}
	else
	{
		return false;
	}
	
	wStr = reinterpret_cast<LPCWSTR>(fullPath.utf16());
	if(::RemoveDirectoryW(wStr)==0)
	{
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool DiskOps::deleteDirectory(const QString& dirName)
{
	int r;
	DIR *h;
	struct dirent *entry;
	struct stat fileStat;
	QString tmp,fullPath(dirName);
	QStringList pathList,fileList;
	QStringList::iterator ppI;
	bool useLatin = false;
	
	formatDirectoryPath(fullPath);
	r = ::stat(fullPath.toUtf8().constData(),&fileStat);
	if(r)
	{
		r = ::stat(fullPath.toLatin1().constData(),&fileStat);
		if(!r)
		{
			useLatin = true;
		}
	}
	if(r!=0)
	{
		return false;
	}
	if(!(S_IFDIR & fileStat.st_mode))
	{
		return false;
	}
	
	if(!useLatin)
	{
		h = ::opendir(fullPath.toUtf8().constData());
	}
	else
	{
		h = ::opendir(fullPath.toLatin1().constData());
	}
	if(h!=0)
	{
		while(entry=::readdir(h),entry!=0)
		{
			if(dotCheck(entry->d_name))
			{
				tmp = fullPath + "/" + QString::fromUtf8(entry->d_name);
				if(!useLatin)
				{
					r = ::stat(tmp.toUtf8().constData(),&fileStat); 
				}
				else
				{
					r = ::stat(tmp.toLatin1().constData(),&fileStat);
				}
				if(r)
				{
					tmp = fullPath + "/" + QString::fromLatin1(entry->d_name);
					r = ::stat(tmp.toLatin1().constData(),&fileStat);
				}
				if(r==0)
				{
					if(S_IFDIR & fileStat.st_mode)
					{
						pathList.append(tmp);
					}
					else if(S_IFREG & fileStat.st_mode)
					{
						fileList.append(tmp);
					}
				}
			}
		}
		::closedir(h);
		
		for(ppI=fileList.begin();ppI!=fileList.end();++ppI)
		{
			const QString& name = *ppI;
			r = ::stat(name.toUtf8().constData(),&fileStat); 
			if(!r)
			{
				::unlink(name.toUtf8().constData());
			}
			else
			{
				r = ::stat(name.toLatin1().constData(),&fileStat); 
				if(!r)
				{
					::unlink(name.toLatin1().constData());
				}
			}
		}
		for(ppI=pathList.begin();ppI!=pathList.end();++ppI)
		{
			const QString& name = *ppI;
			if(!deleteDirectory(name))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	
	if(!useLatin)
	{
		r = ::rmdir(fullPath.toUtf8().constData());
	}
	else
	{
		r = ::rmdir(fullPath.toLatin1().constData());
	}
	if(r!=0)
	{
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
// method checkFile - if a file has been newly created and is expected to contain contents
// then the purpose of this method is to wait until the system OS sees that the file has
// registered the file contents before returning back. The algorithm waits upto 3 seconds for
// completion if file hasn't been registered.
//-------------------------------------------------------------------------------------------

tint DiskOps::checkFile(const tchar *name)
{
	QString n(QString::fromUtf8(name));
	return checkFile(n);
}


//-------------------------------------------------------------------------------------------

tint DiskOps::checkFile(const BString& name)
{
	QString n(QString::fromUtf8(name.c_str()));
	return checkFile(n);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

tint DiskOps::checkFile(const QString& name)
{
	tint fileSize = 0;
	TTime endTime,currentTime;
	bool loop = true;
	
	endTime = TTime::Now();
	endTime += 2;
	do
	{
		if(exist(name))
		{
			HANDLE h;
			DWORD size;
			LPCWSTR wStr;
			
			wStr = reinterpret_cast<LPCWSTR>(name.utf16());
			h = ::CreateFileW(wStr,FILE_READ_EA,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
			if(h!=INVALID_HANDLE_VALUE)
			{
				size = ::GetFileSize(h,0);
				if(size!=INVALID_FILE_SIZE)
				{
					if(size>0)
					{
						fileSize = static_cast<tint>(size);
						loop = false;
					}
				}
				::CloseHandle(h);
			}
		}
		if(loop)
		{
			::Sleep(100);
		}
		currentTime = TTime::Now();
	} while(loop && currentTime<endTime);
	return fileSize;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

tint DiskOps::checkFile(const QString& name)
{
	tint fileSize = 0;
	TTime endTime,currentTime;
	bool loop = true, useLatin = false;
	
	endTime = TTime::Now();
	endTime += 2;
	do
	{
		if(existLatin1(name, useLatin))
		{
			QByteArray nameArr = (!useLatin) ? name.toUtf8() : name.toLatin1();
			int h;
			
			h = ::open(nameArr.constData(),O_RDWR);
			if(h!=-1)
			{
				struct stat fileStat;
				
				if(::stat(nameArr.constData(),&fileStat)==0 && (S_IFREG & fileStat.st_mode))
				{
					if(fileStat.st_size>0)
					{
						struct flock lock;
						
						::memset(&lock,0,sizeof(struct flock));
						lock.l_type = F_RDLCK;
						lock.l_start = 0;
						lock.l_whence = SEEK_SET;
						lock.l_len = fileStat.st_size;
						
						if(::fcntl(h,F_SETLK,&lock)!=-1)
						{
							fileSize = static_cast<tint>(fileStat.st_size);
							loop = false;
						}
					}
				}
				::close(h);
			}
		}
		if(loop)
		{
			::usleep(100);
		}
		currentTime = TTime::Now();
	} while(loop && currentTime<endTime);
	return fileSize;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool DiskOps::copy(const tchar *source,const tchar *dest)
{
	BString src(source),dst(dest);
	return copy(src,dst);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::copy(const tchar *source,const BString& dest)
{
	BString s(source);
	return copy(s,dest);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::copy(const BString& source,const tchar *dest)
{
	BString dst(dest);
	return copy(source,dst);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::copy(const BString& source,const BString& dest)
{
	QString s(QString::fromUtf8(source.c_str()));
	QString d(QString::fromUtf8(dest.c_str()));
	return copy(s,d);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool DiskOps::copy(const QString& source,const QString& dest)
{
	LPCWSTR wStr;
	QString src(source),dst(dest);
	struct _stat fileStat;
	
	formatDirectoryPath(src);
	formatDirectoryPath(dst);
	
	wStr = reinterpret_cast<LPCWSTR>(src.utf16());
	
	if(_wstat(wStr,&fileStat)!=0)
	{
		return false;
	}
	if(S_IFDIR & fileStat.st_mode)
	{
		return copyDir(src,dst);
	}
	else if(S_IFREG & fileStat.st_mode)
	{
		return copyFile(src,dst);
	}
	else
	{
		return false;
	}
	
}

//-------------------------------------------------------------------------------------------

bool DiskOps::copyDir(const QString& source,const QString& dest)
{
	tint i;
	LPCWSTR wStr;
	HANDLE h;
	WIN32_FIND_DATAW fData;
	QString src(source),dst(dest),tmp;
	struct _stat fileStat;
	QStringList sourceList,destList;
	
	formatDirectoryPath(src);
	formatDirectoryPath(dst);
	
	wStr = reinterpret_cast<LPCWSTR>(src.utf16());
	if(_wstat(wStr,&fileStat)!=0)
	{
		return false;
	}
	if(!(S_IFDIR & fileStat.st_mode))
	{
		return false;
	}
	if(!path(dst,true))
	{
		return false;
	}
	
	::memset(&fData,0,sizeof(WIN32_FIND_DATAW));
	tmp = src + "\\*";
	wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
	h = ::FindFirstFileW(wStr,&fData);
	if(h!=INVALID_HANDLE_VALUE)
	{
		bool res = true;
		
		do
		{
			QString cName(QString::fromUtf16(reinterpret_cast<const char16_t *>(fData.cFileName)));
			
			if(dotCheck(cName))
			{
				tmp = src + "\\" + cName;
				wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
				if(_wstat(wStr,&fileStat)==0)
				{
					if(S_IFDIR & fileStat.st_mode)
					{
						sourceList.append(tmp);
						tmp = dst + "\\" + cName;
						destList.append(tmp);
					}
					else if(S_IFREG & fileStat.st_mode)
					{
						res = copyFile(tmp,dst);
					}
				}
			}
		} while(res && ::FindNextFileW(h,&fData));
		::FindClose(h);
		
		if(res)
		{
			for(i=0;i<sourceList.size();++i)
			{
				if(!copyDir(sourceList.at(i),destList.at(i)))
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool DiskOps::copy(const QString& source,const QString& dest)
{
	QString src(source),dst(dest);
	struct stat fileStat;
	
	formatDirectoryPath(src);
	formatDirectoryPath(dst);
	
	if(::stat(src.toUtf8().constData(),&fileStat)!=0)
	{
		return false;
	}
	if(S_IFDIR & fileStat.st_mode)
	{
		return copyDir(src,dst);
	}
	else if(S_IFREG & fileStat.st_mode)
	{
		return copyFile(src,dst);
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------

bool DiskOps::copyDir(const QString& source,const QString& dest)
{
	tint i;
	DIR *h;
	struct dirent *entry;
	struct stat fileStat;
	QString src(source),dst(dest),tmp;
	QStringList sourceList,destList;
	
	formatDirectoryPath(src);
	formatDirectoryPath(dst);
	
	if(::stat(src.toUtf8().constData(),&fileStat)!=0)
	{
		return false;
	}
	if(!(S_IFDIR & fileStat.st_mode))
	{
		return false;
	}
	if(!path(dst,true))
	{
		return false;
	}
	
	h = ::opendir(src.toUtf8().constData());
	if(h!=0)
	{
		bool res = true;
		
		while(entry=::readdir(h),res && entry!=0)
		{
			QString cName(QString::fromUtf8(entry->d_name));
			
			if(dotCheck(cName))
			{
				tmp = src + "/" + cName;
				if(::stat(tmp.toUtf8().constData(),&fileStat)==0)
				{
					if(S_IFDIR & fileStat.st_mode)
					{
						sourceList.append(tmp);
						tmp = dst + "/" + cName;
						destList.append(tmp);
					}
					else
					{
						res = copyFile(tmp,dst);
					}
				}
			}
		}
		::closedir(h);
		
		if(res)
		{
			for(i=0;i<sourceList.size();++i)
			{
				if(!copyDir(sourceList.at(i),destList.at(i)))
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool DiskOps::copyFile(const QString& source,const QString& dest)
{
	int i;
	QString src,dst,name;
	bool res = false;

	formatPath(source,src);
	formatPath(dest,dst);
	
	for(i=src.length()-1;i>=0;--i)
	{
		if(src.at(i)==QChar(c_SeparatorChar))
		{
			break;
		}
	}
	if(i>=0)
	{
		name = src.mid(i+1);
		if(dst.at(dst.length() - 1)!=QChar(c_SeparatorChar))
		{
			dst += c_Separator;
		}
		dst += name;

		{
			int amount;
			BIOStream in(e_BIOStream_FileRead);
			BIOStream out(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
			tint8 tmp[128];
			bool loop = true;

			if(in.open(src))
			{
				if(out.open(dst))
				{
					res = true;

					while(res && loop)
					{
						amount = in.read(tmp,128);
						if(amount>=0)
						{
							if(amount>0)
							{
								if(out.write(tmp,amount)==amount)
								{
									if(amount<128)
									{
										loop = false;
									}
								}
								else
								{
									res = false;
								}
							}
							else
							{
								loop = false;
							}
						}
						else
						{
							res = false;
						}
					}
					out.close(); //lint !e534
				}
				in.close(); //lint !e534
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

BString DiskOps::find(const BString& source,const BString& target,bool caseSenstive)
{
	tint i,pos;
	HANDLE h;
	struct _stat fileStat;
	WIN32_FIND_DATAA fData;
	BString src(source),tmp,targetSource,a,b;
	BOQueueTree<BString> sourceList;

	a = target.LCase();

	formatDirectoryPath(src);
	if(_stat(static_cast<const tchar *>(src),&fileStat)==0 && (S_IFDIR & fileStat.st_mode))
	{
		::memset(&fData,0,sizeof(WIN32_FIND_DATAA));
		tmp = src + "\\*";

		h = ::FindFirstFileA(static_cast<const tchar *>(tmp),&fData);
		if(h!=INVALID_HANDLE_VALUE) //lint !e1924 Windows definition
		{
			do {
				if(dotCheck(fData.cFileName))
				{
					tmp = src;
					tmp << "\\" << fData.cFileName;
					if(_stat(static_cast<const tchar *>(tmp),&fileStat)==0)
					{
						if(S_IFDIR & fileStat.st_mode)
						{
							pos = sourceList.Size();
							sourceList.Add(pos,tmp); //lint !e534
						}
						else if(S_IFREG & fileStat.st_mode)
						{
							if(caseSenstive)
							{
								if(target==fData.cFileName)
								{
									targetSource = src;
								}
							}
							else
							{
								b = fData.cFileName;
								b.SelfLCase();
								if(a==b)
								{
									targetSource = src;
								}
							}
						}
					}
				}
			} while(targetSource.IsEmpty() && ::FindNextFileA(h,&fData));
			::FindClose(h); //lint !e534

			for(i=0;targetSource.IsEmpty() && i<sourceList.Size();++i)
			{
				targetSource = find(sourceList.Find(i),target,caseSenstive);
			}
		}
	}
	return targetSource;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

BString DiskOps::find(const BString& source,const BString& target,bool caseSenstive)
{
	tint i,pos;
	DIR *h;
	struct dirent *entry;
	struct stat fileStat;
	BString src(source),tmp,targetSource,a,b;
	BOQueueTree<BString> sourceList;

	a = target.LCase();

	formatDirectoryPath(src);
	if(::stat(static_cast<const tchar *>(src),&fileStat)==0 && (S_IFDIR & fileStat.st_mode))
	{
		h = ::opendir(static_cast<const tchar *>(src));
		if(h!=0)
		{
			while(entry=::readdir(h),targetSource.IsEmpty() && entry!=0)
			{
				if(dotCheck(entry->d_name))
				{
					tmp = src;
					tmp << "/" << entry->d_name;
					if(::stat(static_cast<const tchar *>(tmp),&fileStat)==0)
					{
						if(S_IFDIR & fileStat.st_mode)
						{
							pos = sourceList.Size();
							sourceList.Add(pos,tmp);
						}
						else if(S_IFREG & fileStat.st_mode)
						{
							if(caseSenstive)
							{
								if(target==entry->d_name)
								{
									targetSource = src;
								}
							}
							else
							{
								b = entry->d_name;
								b.SelfLCase();
								if(a==b)
								{
									targetSource = src;
								}
							}
						}
					}
				}
			}
			::closedir(h);

			for(i=0;targetSource.IsEmpty() && i<sourceList.Size();++i)
			{
				targetSource = find(sourceList.Find(i),target,caseSenstive);
			}
		}
	}
	return targetSource;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool DiskOps::canWriteFile(const tchar *name)
{
	QString n(QString::fromUtf8(name));
	return canWriteFile(n);
}

//-------------------------------------------------------------------------------------------

bool DiskOps::canWriteFile(const BString& name)
{
	QString n(QString::fromUtf8(name.c_str()));
	return canWriteFile(n);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool DiskOps::canWriteFile(const QString& name)
{
	bool res = false;

	if(!name.isEmpty())
	{
		HANDLE file;
		LPCWSTR wStr;

		wStr = reinterpret_cast<LPCWSTR>(name.utf16());
		file = ::CreateFileW(wStr,GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if(file!=INVALID_HANDLE_VALUE)
		{
			::CloseHandle(file);
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool DiskOps::canWriteFile(const QString& name)
{
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |  S_IROTH | S_IWOTH;
	bool res = false;

	if(!name.isEmpty())
	{
		int file;

		file = ::open(name.toUtf8().constData(),O_WRONLY | O_CREAT,mode);
		if(file==-1)
		{
			file = ::open(name.toLatin1().constData(),O_WRONLY | O_CREAT,mode);
		}
		if(file!=-1)
		{
			::close(file);
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool DiskOps::getTempFileName(const QString& dir,const QString& ext,QString& s)
{
	BString tmp,dirName;

	if(dir.isEmpty())
	{
		tmp = QDir::tempPath().toUtf8().constData();
	}
	else
	{
		tmp = dir.toUtf8().constData();
	}
	formatPath(tmp,dirName);
	if(dirName[dirName.GetLength() - 1]!=c_SeparatorChar)
	{
		dirName += c_Separator;
	}
	if(!path(dirName,true))
	{
		return false;
	}
	
	m_mutex.lock();
	{
		int i;
		char *mem;
		unsigned char t[11];
		BString name;
		bool loop = true;
		
		if(m_hashArray.size()!=64)
		{
			clock_t c;
			time_t tm;
			struct tm *gmt;
			
			m_hashArray.resize(64);
			mem = m_hashArray.data();
			::memset(mem,0,sizeof(char) * 64);
			
			tm = ::time(0);
			gmt = ::gmtime(&tm);
			c = ::clock();
			::memcpy(mem,reinterpret_cast<char *>(gmt),sizeof(struct tm));
			::memcpy(&mem[sizeof(struct tm)],reinterpret_cast<char *>(&c),sizeof(clock_t));
		}
		else
		{
			mem = m_hashArray.data();
		}
		
		while(loop)
		{
			SHA1Digest digest;
		
			digest.input(reinterpret_cast<tint8 *>(mem),64);
			digest.getDigestFinal(reinterpret_cast<tint8 *>(mem),c_SHA1HashSize);
			
			for(i=0;i<10;++i)
			{
				t[i] = static_cast<unsigned char>(mem[i] & 0x1f);
				if(t[i]>=0x00 && t[i]<0x0a)
				{
					t[i] += 0x30;  // generate char between '0' and '9'
				}
				else
				{
					t[i] += 0x57;  // generate char between 'a' and 'z'
				}
			}
			t[10] = 0x00;
			
			name = dirName + reinterpret_cast<char *>(t);
			if(!ext.isEmpty())
			{
				name += ".";
				name += ext.toUtf8().constData();
			}
			
			if(!exist(name))
			{
				loop = false;
			}
		}
		
        QDir d(QString::fromLatin1(name.c_str()));
		s = d.absolutePath();
	}
	m_mutex.unlock();

	return true;
}

//-------------------------------------------------------------------------------------------

tint DiskOps::remove(const tchar *path)
{
	return remove(QString::fromUtf8(path));
}

//-------------------------------------------------------------------------------------------

tint DiskOps::remove(const BString& path)
{
	return remove(QString::fromUtf8(path.c_str()));
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------


void DiskOps::deleteFile(const QString& name)
{
	QString n(QDir::toNativeSeparators(name));
	LPCWSTR wStr = reinterpret_cast<LPCWSTR>(n.utf16());
	::DeleteFileW(wStr);
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

void DiskOps::deleteFile(const QString& name)
{
	QString n(QDir::toNativeSeparators(name));
	bool useLatin = false;
	
	if(existLatin1(n, useLatin))
	{
		if(!useLatin)
		{
			::unlink(n.toUtf8().constData());
		}
		else
		{
			::unlink(n.toLatin1().constData());
		}
	}
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

tint DiskOps::remove(const QString& path)
{
	QString n(QDir::toNativeSeparators(path));
	LPCWSTR wStr = reinterpret_cast<LPCWSTR>(n.utf16());
	return ::_wremove(wStr);
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

tint DiskOps::remove(const QString& path)
{
	int r = 0;
	QString n(QDir::toNativeSeparators(path));
	bool useLatin = false;
	
	if(existLatin1(n, useLatin))
	{
		if(!useLatin)
		{
			r = ::remove(n.toUtf8().constData());
		}
		else
		{
			r = ::remove(n.toLatin1().constData());
		}
	}
	return r;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
// Given a list of paths this method looks to find there shared parent directory
//-------------------------------------------------------------------------------------------

DiskOps::TreeCommonRoot::TreeCommonRoot() : m_parent(0),
	m_children(),
	m_name(),
	m_postFix(),
	m_endNode(false)
{}

//-------------------------------------------------------------------------------------------

DiskOps::TreeCommonRoot::~TreeCommonRoot()
{
	QMap<QString,TreeCommonRoot *>::iterator ppI;

	for(ppI=m_children.begin();ppI!=m_children.end();ppI++)
	{
		TreeCommonRoot *node = ppI.value();
		delete node;
	}
}

//-------------------------------------------------------------------------------------------

QMap<QString,DiskOps::TreeCommonRoot *>& DiskOps::TreeCommonRoot::nodes()
{
	return m_children;
}

//-------------------------------------------------------------------------------------------

const QString& DiskOps::TreeCommonRoot::name() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------

const QString& DiskOps::TreeCommonRoot::postFix() const
{
	return m_postFix;
}

//-------------------------------------------------------------------------------------------

bool DiskOps::TreeCommonRoot::endNode() const
{
	return m_endNode;
}

//-------------------------------------------------------------------------------------------

DiskOps::TreeCommonRoot *DiskOps::TreeCommonRoot::append(const QString& nName,const QString& pFix,bool eN)
{
	TreeCommonRoot *node;
	QMap<QString,TreeCommonRoot *>::iterator ppI = m_children.find(nName);

	if(ppI==m_children.end())
	{
		node = new TreeCommonRoot;
		node->m_parent = this;
		node->m_name = nName;
		node->m_postFix = pFix;
		node->m_endNode = eN;
		m_children.insert(nName,node);
	}
	else
	{
		node = ppI.value();
		if(eN)
		{
			node->m_endNode = eN;
		}
	}
	return node;
}

//-------------------------------------------------------------------------------------------

QString DiskOps::commonRoot(const QVector<QString>& pathList)
{
	QString cPath;
	TreeCommonRoot *root = new TreeCommonRoot,*currentNode;
	QVector<QString>::const_iterator ppI;
	bool found = true;

	for(ppI=pathList.begin();ppI!=pathList.end() && found;ppI++)
	{
		int idx = 1;
		QDir fDir(*ppI);
		QString fPath,part;
		bool eNFlag;

		currentNode = root;

		fPath = fDir.absolutePath();
		if(fPath.size()>=3 && (fPath.mid(1,2)==":\\" || fPath.mid(1,2)==":/"))
		{
			part = fPath.mid(0,1);
			idx = 3;
			eNFlag = (idx==fPath.size()) ? true : false;
			currentNode = currentNode->append(part,":\\",eNFlag);
		}
		else if(fPath.size()>=2 && (fPath.mid(0,2)=="\\\\" || fPath.mid(0,2)=="//"))
		{
			idx = 2;
			eNFlag = (idx==fPath.size()) ? true : false;
			currentNode = currentNode->append("\\\\","",eNFlag);
		}
		else if(fPath.size()>=1 && (fPath.mid(0,1)=="\\" || fPath.mid(0,1)=="/"))
		{
			idx = 1;
			eNFlag = (idx==fPath.size()) ? true : false;
			currentNode = currentNode->append("/","",eNFlag);
		}
		else
		{
			found = false;
		}

		if(found)
		{
			tint sIdx = idx;

			while(idx<fPath.size())
			{
				if(fPath.at(idx)==QChar('\\') || fPath.at(idx)==QChar('/'))
				{
					if(sIdx < idx)
					{
						part = fPath.mid(sIdx,idx - sIdx);
						currentNode = currentNode->append(part,"/",((idx+1)==fPath.size()) ? true : false);
					}
					sIdx = idx + 1;
				}
				idx++;
			}
			if(sIdx < idx)
			{
				part = fPath.mid(sIdx,idx - sIdx);
				currentNode = currentNode->append(part,"",true);
			}
		}
	}

	if(found)
	{
		currentNode = root;
		while(currentNode!=0)
		{
			cPath += currentNode->name() + currentNode->postFix();
			if(currentNode->nodes().size()==1 && !currentNode->endNode())
			{
				currentNode = currentNode->nodes().begin().value();
			}
			else
			{
				currentNode = 0;
			}
		}
		cPath = QDir::toNativeSeparators(cPath);
	}
	delete root;

	return cPath;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp DiskOps::getModifiedTime(const QString& fileName)
{
	tint fileSize;
	return getModifiedTime(fileName,fileSize);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

common::TimeStamp DiskOps::getModifiedTime(const QString& fileName,tint& fileSize)
{
	QString fName(fileName);
	common::TimeStamp t;
	struct __stat64 mStat;
	LPCWSTR wStr;
	
	if(!fName.isEmpty())
	{
		if(fName.at(fName.length()-1)==QChar('/') || fName.at(fName.length()-1)==QChar('\\'))
		{
			fName = fName.mid(0,fName.length()-1);
		}
		fName = QDir::toNativeSeparators(fName);
		
		wStr = reinterpret_cast<LPCWSTR>(fName.utf16());
		if(::_wstat64(wStr,&mStat)==0)
		{
			if(S_IFREG & mStat.st_mode)
			{
				fileSize = static_cast<tint>(mStat.st_size);
				t.set(&mStat.st_mtime);
			}
			else if(S_IFDIR & mStat.st_mode)
			{
				fileSize = 0;
				t.set(&mStat.st_mtime);
			}
		}
	}
	return t;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

common::TimeStamp DiskOps::getModifiedTime(const QString& fileName,tint& fileSize)
{
	int r;
	QString fName(fileName);
	common::TimeStamp t;
	struct stat mStat;

	if(!fName.isEmpty())
	{
		if(fName.at(fName.length()-1)==QChar('/') || fName.at(fName.length()-1)==QChar('\\'))
		{
			fName = fName.mid(0,fName.length()-1);
		}
		fName = QDir::toNativeSeparators(fName);
		
		r = ::stat(fileName.toUtf8().constData(),&mStat); 
		if(r)
		{
			r = ::stat(fileName.toLatin1().constData(),&mStat); 
		}
		if(r==0)
		{
			if(S_IFREG & mStat.st_mode)
			{
				fileSize = static_cast<tint>(mStat.st_size);
#if defined(OMEGA_MACOSX)
				t.set(&mStat.st_mtimespec);
#elif defined (OMEGA_LINUX)
				t.set(&mStat.st_mtime);
#endif
			}
			else if(S_IFDIR & mStat.st_mode)
			{
				fileSize = 0;
#if defined(OMEGA_MACOSX)
				t.set(&mStat.st_mtimespec);
#elif defined (OMEGA_LINUX)
				t.set(&mStat.st_mtime);
#endif	
			}
		}
	}
	return t;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QString DiskOps::toNativeSeparators(const QString& iName)
{
    QString oName;
    for(int i=0;i<iName.size();i++)
    {
#if defined(OMEGA_WIN32)
        if(iName.at(i)==QChar('/'))
        {
            oName += '\\';
        }
#else
        if(iName.at(i)==QChar('\\'))
        {
            oName += '/';
        }
#endif
        else
        {
            oName += iName.at(i);
        }
    }
    return oName;
}

//-------------------------------------------------------------------------------------------

QString DiskOps::mergeName(const tchar *dirName,const tchar *fileName)
{
	return mergeName(QString::fromUtf8(dirName),QString::fromUtf8(fileName));
}

//-------------------------------------------------------------------------------------------

QString DiskOps::mergeName(const tchar *dirName,const QString& fileName)
{
	return mergeName(QString::fromUtf8(dirName),fileName);
}

//-------------------------------------------------------------------------------------------

QString DiskOps::mergeName(const QString& dirName,const tchar *fileName)
{
	return mergeName(dirName,QString::fromUtf8(fileName));
}

//-------------------------------------------------------------------------------------------

QString DiskOps::mergeName(const QString& dirName,const QString& fileName)
{
	QString dName(dirName),fName(fileName),fullName;
	
	while(!dName.isEmpty() && (dName.at(dName.length()-1)==QChar('/') || dName.at(dName.length()-1)==QChar('\\')))
	{
		dName = dName.mid(0,dName.length() - 1);
	}
	while(!fName.isEmpty() && (fName.at(0)==QChar('/') || fName.at(0)==QChar('\\')))
	{
		fName = fName.mid(1,fName.length() - 1);
	}
	fullName = dName;
	if(!dName.isEmpty() && !fName.isEmpty())
	{
		fullName += "/";
	}
	fullName += fName;
	fullName = toNativeSeparators(fullName);
	return fullName;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

