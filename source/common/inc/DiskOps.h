#ifndef __OMEGA_COMMON_DISKOPS_H
#define __OMEGA_COMMON_DISKOPS_H

#include "common/inc/BIOStream.h"
#include "common/inc/TimeStamp.h"

#include <QString>
#include <QMutex>
#include <QByteArray>
#include <QDir>
#include <QMap>
#include <QVector>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT DiskOps
{
    public:
        class TreeCommonRoot;

    public:
        DiskOps();

        static void formatPath(const tchar *in,BString& name);
        static void formatPath(const BString& in,BString& name);
        static void formatPath(const QString& in,QString& name);

        static bool path(const tchar *name,bool dirFlag=false);
        static bool path(const BString& name,bool dirFlag=false);
        static bool path(const QString& name,bool dirFlag=false);

        static bool exist(const tchar *name);
        static bool exist(const BString& name);
        static bool exist(const QString& name);

        static int fileType(const QString& name);

        static bool deleteDirectory(const tchar *dirName);
        static bool deleteDirectory(const BString& dirName);
        static bool deleteDirectory(const QString& dirName);

        static tint checkFile(const tchar *name);
        static tint checkFile(const BString& name);
        static tint checkFile(const QString& name);

        static bool copy(const tchar *source,const tchar *dest);
        static bool copy(const tchar *source,const BString& dest);
        static bool copy(const BString& source,const tchar *dest);
        static bool copy(const BString& source,const BString& dest);
        static bool copy(const QString& source,const QString& dest);

        static BString find(const BString& source,const BString& target,bool caseSenstive=false);

        static bool canWriteFile(const tchar *name);
        static bool canWriteFile(const BString& name);
        static bool canWriteFile(const QString& name);

        static bool getTempFileName(const QString& dir,const QString& ext,QString& s);

        static void formatDirectoryPath(BString& dirName);
        static void formatDirectoryPath(QString& dirName);

        static bool dotCheck(const tchar *name);
        static bool dotCheck(const BString& name);
        static bool dotCheck(const QString& name);

        static tint remove(const tchar *path);
        static tint remove(const BString& path);
        static tint remove(const QString& path);

        static void deleteFile(const QString& name);

        static QString commonRoot(const QVector<QString>& pathList);

        static common::TimeStamp getModifiedTime(const QString& fileName);
        static common::TimeStamp getModifiedTime(const QString& fileName,tint& fileSize);

        static QString toNativeSeparators(const QString& iName);

        static QString mergeName(const tchar *dirName,const tchar *fileName);
        static QString mergeName(const tchar *dirName,const QString& fileName);
        static QString mergeName(const QString& dirName,const tchar *fileName);
        static QString mergeName(const QString& dirName,const QString& fileName);

    protected:

        static QMutex m_mutex;
        static QByteArray m_hashArray;

        static bool copyDir(const QString& source,const QString& dest);

        static bool copyFile(const QString& source,const QString& dest);
#if defined(OMEGA_POSIX)
        static bool existLatin1(const QString& name, bool& useLatin);
#endif
};

//-------------------------------------------------------------------------------------------

class DiskOps::TreeCommonRoot
{
    public:
        TreeCommonRoot();
        virtual ~TreeCommonRoot();

        QMap<QString,TreeCommonRoot *>& nodes();
        const QString& name() const;
        const QString& postFix() const;
        bool endNode() const;

        TreeCommonRoot *append(const QString& nName,const QString& pFix,bool eN);

    protected:

        TreeCommonRoot *m_parent;
        QMap<QString,TreeCommonRoot *> m_children;
        QString m_name;
        QString m_postFix;
        bool m_endNode;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

