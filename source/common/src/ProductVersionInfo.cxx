#include "common/inc/ProductVersionInfo.h"

#include <QDir>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

ProductVersionInfoSPtr ProductVersionInfo::m_instance;

//-------------------------------------------------------------------------------------------

ProductVersionInfo::ProductVersionInfo() : m_versionMajor(1),
    m_versionMinor(0),
    m_versionRevision(0),
    m_build(0),
    m_copyright()
{}

//-------------------------------------------------------------------------------------------

ProductVersionInfo::~ProductVersionInfo()
{}

//-------------------------------------------------------------------------------------------

ProductVersionInfoSPtr ProductVersionInfo::instance(const QString& fileName)
{
    if(m_instance.data()==0)
    {
        ProductVersionInfoSPtr pVersion(new ProductVersionInfo);
        if(pVersion->read(fileName))
        {
            m_instance = pVersion;
        }
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

ProductVersionInfoSPtr ProductVersionInfo::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

bool ProductVersionInfo::read(const QString& fileName)
{
    bool res = true;
    QMap<QString,QString> dict;
    QMap<QString,QString>::iterator ppI;
    
    dict = DictionaryXMLMap::read(fileName);
    ppI = dict.find("VersionMajor");
    if(ppI!=dict.end())
    {
        m_versionMajor = ppI.value().toInt();
        
        ppI = dict.find("VersionMinor");
        if(ppI!=dict.end())
        {
            m_versionMinor = ppI.value().toInt();
        }
        ppI = dict.find("Revision");
        if(ppI!=dict.end())
        {
            m_versionRevision = ppI.value().toInt();
        }
        ppI = dict.find("Build");
        if(ppI!=dict.end())
        {
            m_build = ppI.value().toInt();
        }
        ppI = dict.find("Copyright");
        if(ppI!=dict.end())
        {
            m_copyright = ppI.value();
        }
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ProductVersionInfo::write(const QString& fileName)
{
    QMap<QString,QString> dict;
    dict.insert(QString::fromLatin1("VersionMajor"),QString::number(m_versionMajor));
    dict.insert(QString::fromLatin1("VersionMinor"),QString::number(m_versionMinor));
    dict.insert(QString::fromLatin1("Revision"),QString::number(m_versionRevision));
    dict.insert(QString::fromLatin1("Build"),QString::number(m_build));
    dict.insert(QString::fromLatin1("Copyright"),m_copyright);
    return DictionaryXMLMap::write(fileName,dict);
}

//-------------------------------------------------------------------------------------------

const int& ProductVersionInfo::versionMajor() const
{
    return m_versionMajor;
}

//-------------------------------------------------------------------------------------------

const int& ProductVersionInfo::versionMinor() const
{
    return m_versionMinor;
}

//-------------------------------------------------------------------------------------------

const int& ProductVersionInfo::versionRevision() const
{
    return m_versionRevision;
}

//-------------------------------------------------------------------------------------------

const int& ProductVersionInfo::build() const
{
    return m_build;
}

//-------------------------------------------------------------------------------------------

int& ProductVersionInfo::build()
{
    return m_build;
}

//-------------------------------------------------------------------------------------------

QString ProductVersionInfo::version() const
{
    QString v;
    v = QString::number(m_versionMajor) + "." + QString::number(m_versionMinor) + "." + QString::number(m_versionRevision);
    return v;
}

//-------------------------------------------------------------------------------------------

const QString& ProductVersionInfo::copyright() const
{
    return m_copyright;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
