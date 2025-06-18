#include "common/inc/CommonDirectoriesForFiles.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// CommonDirectoriesForFiles::Node
//-------------------------------------------------------------------------------------------

CommonDirectoriesForFiles::Node::Node(Node *parent,const QString& n,bool dirFlag) : m_parent(parent),
    m_children(),
    m_name(n),
    m_directory(dirFlag),
    m_end(false),
    m_referenceCount(1)
{
    if(m_parent!=0)
    {
        m_parent->m_children.insert(this);
        m_parent->m_directory = true;
    }
}

//-------------------------------------------------------------------------------------------

CommonDirectoriesForFiles::Node::~Node()
{
    QSet<Node *>::iterator ppI;
    
    while(ppI=m_children.begin(),ppI!=m_children.end())
    {
        Node *n = *ppI;
        m_children.erase(ppI);
        delete n;
    }
}

//-------------------------------------------------------------------------------------------

const QString& CommonDirectoriesForFiles::Node::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::Node::isEqual(const QString& n) const
{
#if defined(OMEGA_WIN32)
    return (m_name.compare(n,Qt::CaseInsensitive)==0);
#else
    return (m_name.compare(n,Qt::CaseSensitive)==0);
#endif
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::Node::isDirectory() const
{
    return m_directory;
}

//-------------------------------------------------------------------------------------------

CommonDirectoriesForFiles::Node *CommonDirectoriesForFiles::Node::parent()
{
    return m_parent;
}

//-------------------------------------------------------------------------------------------

QSet<CommonDirectoriesForFiles::Node *>& CommonDirectoriesForFiles::Node::children()
{
    return m_children;
}

//-------------------------------------------------------------------------------------------

void CommonDirectoriesForFiles::Node::updateNameAsRequired(const QString& n)
{
#if defined(OMEGA_WIN32)
    if(m_name!=n)
    {
        m_name = m_name.toLower();
    }
#endif
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::Node::isEnd() const
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

void CommonDirectoriesForFiles::Node::setAsEnd()
{
    m_end = true;
}

//-------------------------------------------------------------------------------------------

void CommonDirectoriesForFiles::Node::increment()
{
    m_referenceCount++;
}

//-------------------------------------------------------------------------------------------

void CommonDirectoriesForFiles::Node::decrement()
{
    m_referenceCount--;
}

//-------------------------------------------------------------------------------------------

int CommonDirectoriesForFiles::Node::referenceCount() const
{
    return m_referenceCount;
}

//-------------------------------------------------------------------------------------------
// CommonDirectoriesForFiles
//-------------------------------------------------------------------------------------------

CommonDirectoriesForFiles::CommonDirectoriesForFiles() : m_roots()
{}

//-------------------------------------------------------------------------------------------

CommonDirectoriesForFiles::~CommonDirectoriesForFiles()
{
    QSet<Node *>::iterator ppI;
    
    while(ppI=m_roots.begin(),ppI!=m_roots.end())
    {
        Node *n = *ppI;
        m_roots.erase(ppI);
        delete n;
    }
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::isSeparator(const QChar& c) const
{
    return (c==QChar('/') || c==QChar('\\'));
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::isSameRelative(const QString& name) const
{
    return (name==".") ? true : false;
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::isSameParent(const QString& name) const
{
    return (name=="..") ? true : false;
}

//-------------------------------------------------------------------------------------------

CommonDirectoriesForFiles::Node *CommonDirectoriesForFiles::add(const QString& fileName)
{
    QString rName;
    int index = 0,prevIndex = 0;
    Node *n = 0;
    
    while(index < fileName.size())
    {
        QSet<Node *>& nodes = (n!=0) ? n->children() : m_roots;
        
        rName = (n==0) ? getVolumeName(fileName,index) : nextResourceName(fileName,index);
        if(!rName.isEmpty())
        {
            if(!isSameRelative(rName))
            {
                if(!isSameParent(rName))
                {
                    Node *cNode = 0;
                    QSet<Node *>::iterator ppI;
                    
                    for(ppI=nodes.begin();ppI!=nodes.end() && cNode==0;ppI++)
                    {
                        Node *c = *ppI;
                        if(c->isEqual(rName))
                        {
                            cNode = c;
                        }
                    }
                    
                    if(cNode==0)
                    {
                        bool dirFlag = (n==0 || (prevIndex + rName.size()) < fileName.size());
                        cNode = new Node(n,rName,dirFlag);
                        if(n==0)
                        {
                            m_roots.insert(cNode);
                        }
                    }
                    else
                    {
                        cNode->updateNameAsRequired(rName);
                        cNode->increment();
                    }
                    
                    if(index >= fileName.size())
                    {
                        cNode->setAsEnd();
                    }
                    n = cNode;
                }
                else
                {
                    if(n!=0)
                    {
                        Node *cNode = n;
                        n = n->parent();
                        
                        cNode->decrement();
                        if(cNode->referenceCount()<=0)
                        {
                            QSet<Node *>& lNodes = (n!=0) ? n->children() : m_roots;
                            QSet<Node *>::iterator ppI = lNodes.find(cNode);
                            if(ppI!=lNodes.end())
                            {
                                lNodes.erase(ppI);
                            }
                            delete cNode;
                        }

                        if(n==0)
                        {
                            index = fileName.size();
                        }
                    }
                    else
                    {
                        index = fileName.size();
                    }
                }
            }
        }
        else
        {
            index = fileName.size();
        }
        prevIndex = index;
    }
    return n;
}

//-------------------------------------------------------------------------------------------

QStringList CommonDirectoriesForFiles::buildPathOfNode(Node *n,bool incFile)
{
    QStringList pathList;
    QString path;
    
#if defined(OMEGA_MACOSX)
    Node *rNode = n;
#endif
    
    while(n!=0)
    {
        if(n->isDirectory() || incFile)
        {
            QString p;

            if(!path.isEmpty())
            {
#if defined(OMEGA_WIN32)
                p = "/" + path;
#elif defined(OMEGA_MACOSX)
                if(n->name()!="/")
                {
                    p = "/" + path;
                }
                else
                {
                    p = path;
                }
#endif
            }
            path = n->name() + p;
        }
        n = n->parent();
    }

#if defined(OMEGA_WIN32)
    pathList << QDir::toNativeSeparators(path);
#elif defined(OMEGA_MACOSX)
    if(path=="/" && rNode!=0)
    {
        if(!incFile)
        {
            while(rNode->parent()!=0)
            {
                rNode = rNode->parent();
            }
            for(QSet<Node *>::iterator ppI=rNode->children().begin(); ppI!=rNode->children().end(); ppI++)
            {
                Node *cNode = *ppI;
                if(cNode->isDirectory())
                {
                    path = "/" + cNode->name();
                    pathList << path;
                }
            }
        }
    }
    else
    {
        pathList << QDir::toNativeSeparators(path);
    }
#endif

    return pathList;
}

//-------------------------------------------------------------------------------------------

QSet<QString> CommonDirectoriesForFiles::process()
{
    QStringList list;
    QSet<Node *>::iterator ppI;
    QSet<QString> paths;

    for(ppI=m_roots.begin();ppI!=m_roots.end();ppI++)
    {
        Node *n = *ppI;
        
        while(n->children().size()==1 && !n->isEnd())
        {
            n = *(n->children().begin());
        }

        list = buildPathOfNode(n,false);
        for(QStringList::iterator ppJ=list.begin();ppJ!=list.end();++ppJ)
        {
            paths.insert(*ppJ);
        }
    }
    return paths;
}

//-------------------------------------------------------------------------------------------

QSet<QString> CommonDirectoriesForFiles::find(const QStringList& fileNameList)
{
    CommonDirectoriesForFiles commonDir;

    for(QStringList::const_iterator ppI=fileNameList.begin();ppI!=fileNameList.end();ppI++)
    {
        commonDir.add(*ppI);
    }
    return commonDir.process();
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------
// C:<index> -> C:
// d:\<index> -> D:
// E://<index> -> E:
// \\Oracle\\Name\\<index> -> \\Oracle\Name
//-------------------------------------------------------------------------------------------

QString CommonDirectoriesForFiles::getVolumeName(const QString& fileName,int& index) const
{
    QString volumeName;
    int driveNameStart = -1,driveNameEnd = -1;
    int machineNameStart = -1,machineNameEnd = -1;
    int resourceNameStart = -1,resourceNameEnd = -1;
    int i,state = 0;
    
    index = -1;
    for(i=0;i<fileName.size() && state>=0 && state<100;i++)
    {
        const QChar& c = fileName.at(i);
        
        switch(state)
        {
            case 0:
                if(c.isLetter())
                {
                    driveNameStart = i;
                    state = 1;
                }
                else if(isSeparator(c))
                {
                    state = 3;
                }
                else
                {
                    state = -1;
                }
                break;
                
            case 1:
                if(c==QChar(':'))
                {
                    index = driveNameEnd = i + 1;
                    state = 2;
                }
                else
                {
                    state = -1;
                }
                break;

            case 2:
                if(isSeparator(c))
                {
                    index++;
                }
                else
                {
                    state = 100;
                }
                break;
                
            case 3:
                state = (isSeparator(c)) ? 4 : -1;
                break;
                
            case 4:
                if(!isSeparator(c))
                {
                    machineNameStart = i;
                    state = 5;
                }
                else
                {
                    state = -1;
                }
                break;
                
            case 5:
                if(isSeparator(c))
                {
                    machineNameEnd = i;
                    state = 6;
                }
                break;
                
            case 6:
                if(!isSeparator(c))
                {
                    index = i + 1;
                    resourceNameStart = i;
                    state = 7;
                }
                break;
                
            case 7:
                index = i + 1;
                if(isSeparator(c))
                {
                    resourceNameEnd = i;
                    state = 8;
                }
                break;
                
            case 8:
                index = i + 1;
                if(!isSeparator(c))
                {
                    index = i;
                    state = 100;
                }
                break;
            
            default:
                break;
        }
    }
    
    if(index>=0)
    {
        if(driveNameStart>=0)
        {
            volumeName = fileName.mid(driveNameStart,driveNameEnd - driveNameStart);
        }
        else if(machineNameStart>=0 && resourceNameStart>=0)
        {
            if(machineNameEnd>=0)
            {
                machineNameEnd -= machineNameStart;
            }
            volumeName  = "\\\\" + fileName.mid(machineNameStart,machineNameEnd);
            if(resourceNameEnd>=0)
            {
                resourceNameEnd -= resourceNameStart;
            }
            volumeName += "\\" + fileName.mid(resourceNameStart,resourceNameEnd);
        }
    }
    else 
    {
        volumeName = "";
    }
    return volumeName;
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------
// / -> /
// /var -> /
// /usr/bin -> /
// /Users -> /
// /Users/git -> /Users/git
// /Users/bonez/dir -> /Users/bonez
// /Volumes -> /
// /Volumes/LaCie -> /Volumes/LaCie
// /Volumes/BOOTCAMP/dir -> /Volumes/BOOTCAMP
//-------------------------------------------------------------------------------------------

QString CommonDirectoriesForFiles::getVolumeName(const QString& fileName,int& index) const
{
    int i,state = 0,rootStart = 0,rootEnd = -1;
    QString rootName,volumeName;
    bool flag = false;

    index = -1;
    for(i=0;i<fileName.length() && state>=0 && state<100;i++)
    {
        const QChar& c = fileName.at(i);
        
        switch(state)
        {
            case 0:
                if(isSeparator(c))
                {
                    index = i + 1;
                    state = 1;
                }
                else
                {
                    state = -1;
                }
                break;
                
            case 1:
                if(isSeparator(c))
                {
                    index = i + 1;
                }
                else
                {
                    rootStart = i;
                    state = 2;
                }
                break;
            
            case 2:
                if(isSeparator(c))
                {
                    rootName = fileName.mid(rootStart,i - rootStart);
                    if(rootName=="Volumes" || rootName=="Users")
                    {
                        flag = true;
                        state = 3;
                    }
                    else
                    {
                        state = 100;
                    }
                }
                break;
            
            case 3:
                rootStart = i;
                if(!isSeparator(c))
                {
                    state = 4;
                    index = i + 1;
                }
                break;
                
            case 4:
                index = i + 1;
                if(isSeparator(c))
                {
                    rootEnd = i;
                    state = 5;
                }
                break;
                
            case 5:
                if(!isSeparator(c))
                {
                    index = i;
                    state = 100;
                }
                else
                {
                    index = i + 1;
                }
                break;

            default:
                break;
        }
    }
    
    if(index>=0)
    {
        if(!flag)
        {
            volumeName = "/";
        }
        else
        {
            if(rootEnd>=0)
            {
                rootEnd -= rootStart;
            }
            volumeName = "/" + rootName + "/" + fileName.mid(rootStart,rootEnd);
        }
    }
    return volumeName;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QString CommonDirectoriesForFiles::nextResourceName(const QString& fileName,int& index) const
{
    QString rName;
    
    if(index>=0 && index<fileName.size())
    {
        int s,count = 0,sPos = -1,sEnd = -1;
        
        while(index<fileName.size() && count<3)
        {
            const QChar& c = fileName.at(index);

            s = isSeparator(c) ? 0 : 1;
            if((count & 1) == s)
            {
                index++;
            }
            else
            {
                if(count==0)
                {
                    sPos = index;
                }
                else if(count==1)
                {
                    sEnd = index;
                }
                count++;
            }
        }

        if(sPos>=0)
        {
            if(sEnd>=0)
            {
                sEnd -= sPos;
            }
            rName = fileName.mid(sPos,sEnd);
        }
    }
    return rName;
}

//-------------------------------------------------------------------------------------------

bool CommonDirectoriesForFiles::canBuildValidPath(Node *n)
{
    bool res;
    
    if(n!=0)
    {
        res = true;
#if defined(OMEGA_MACOSX)
        if(!n->isDirectory() && n->parent()!=0)
        {
            Node *parentNode = n->parent();
            if(parentNode->parent()==0 && parentNode->name()=="/")
            {
                res = false;
            }
        }        
#endif
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString CommonDirectoriesForFiles::path(const QString& fileName,int level)
{
    QString dPath;
    Node *n;
    
    n = add(fileName);
    for(int i=0;i<level && n!=0;i++)
    {
        n = n->parent();
    }
    if(canBuildValidPath(n))
    {
        QStringList dList = buildPathOfNode(n,true);
        if(!dList.isEmpty())
        {
            dPath = dList.at(0);
        }
    }
    return dPath;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
