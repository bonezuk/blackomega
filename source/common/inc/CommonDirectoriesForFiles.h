//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_COMMONDIRECTORIESFORFILES_H
#define __OMEGA_COMMON_COMMONDIRECTORIESFORFILES_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QStringList>
#include <QSet>
#include <QChar>
#include <QDir>

#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT CommonDirectoriesForFiles
{
    public:
        class Node;

    public:
        CommonDirectoriesForFiles();
        virtual ~CommonDirectoriesForFiles();

        static QSet<QString> find(const QStringList& fileNameList);

        QString path(const QString& fileName,int level);

    protected:

        QSet<Node *> m_roots;

        bool isSeparator(const QChar& c) const;
        bool isSameRelative(const QString& name) const;
        bool isSameParent(const QString& name) const;

        QString getVolumeName(const QString& fileName,int& index) const;
        QString nextResourceName(const QString& fileName,int& index) const;

        QStringList buildPathOfNode(Node *n,bool incFile);
        bool canBuildValidPath(Node *n);

        Node *add(const QString& fileName);
        QSet<QString> process();
};

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT CommonDirectoriesForFiles::Node
{
    public:
        Node(Node *parent,const QString& n,bool dirFlag);
        virtual ~Node();

        bool isEqual(const QString& n) const;

        bool isDirectory() const;
        const QString& name() const;

        Node *parent();
        QSet<Node *>& children();

        void updateNameAsRequired(const QString& n);

        bool isEnd() const;
        void setAsEnd();

        void increment();
        void decrement();
        int referenceCount() const;

    protected:

        Node *m_parent;
        QSet<Node *> m_children;
        QString m_name;
        bool m_directory;
        bool m_end;
        int m_referenceCount;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

