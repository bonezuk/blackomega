#include "player/inc/RegisterFileTypes.h"

#include <QDir>
#include <QFileInfo>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

RegisterFileType::RegisterFileType(const QString& appPath) : m_appPath(appPath)
{
    m_appPath = QDir::toNativeSeparators(m_appPath);
}

//-------------------------------------------------------------------------------------------

RegisterFileType::~RegisterFileType()
{}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::typeExt(FileType t)
{
    QString tExt;
    
    switch(t)
    {
        case e_fileTypeMP3:
            tExt = "mp3";
            break;
            
        case e_fileTypeM4A:
            tExt = "m4a";
            break;
            
        case e_fileTypeM4B:
            tExt = "m4b";
            break;
            
        case e_fileTypeOGG:
            tExt = "ogg";
            break;
            
        case e_fileTypeFLAC:
            tExt = "flac";
            break;

        case e_fileTypePLS:
            tExt = "pls";
            break;

        case e_fileTypeM3U:
            tExt = "m3u";
            break;

        case e_fileTypeM3U8:
            tExt = "m3u8";
            break;

        case e_fileTypeXSPF:
            tExt = "xspf";
            break;

        case e_fileTypeAIF:
            tExt = "aif";
            break;

        case e_fileTypeAIFF:
            tExt = "aiff";
            break;
            
        case e_fileTypeMPC:
            tExt = "mpc";
            break;
            
        case e_fileTypeMPP:
            tExt = "mpp";
            break;
            
        case e_fileTypeMPPlus:
            tExt = "mp+";
            break;

        case e_fileTypeAPE:
            tExt = "ape";
            break;
            
        case e_fileTypeWV:
            tExt = "wv";
            break;

        case e_fileTypeWAV:
        default:
            tExt = "wav";
            break;
    }
    return tExt;
}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::contentType(FileType t)
{
    QString tExt;
    
    switch(t)
    {
        case e_fileTypeMP3:
            tExt = "audio/mpeg";
            break;
            
        case e_fileTypeM4A:
            tExt = "audio/m4a";
            break;
            
        case e_fileTypeM4B:
            tExt = "audio/m4b";
            break;
            
        case e_fileTypeOGG:
            tExt = "audio/ogg";
            break;
            
        case e_fileTypeFLAC:
            tExt = "audio/flac";
            break;

        case e_fileTypePLS:
            tExt = "audio/x-scpls";
            break;

        case e_fileTypeM3U:
            tExt = "audio/x-winamp-playlist";
            break;

        case e_fileTypeM3U8:
            tExt = "audio/x-winamp-playlist";
            break;

        case e_fileTypeXSPF:
            tExt = "application/xspf+xml";
            break;

        case e_fileTypeAIF:
            tExt = "audio/x-aiff";
            break;

        case e_fileTypeAIFF:
            tExt = "audio/x-aiff";
            break;
            
        case e_fileTypeMPC:
            tExt = "audio/x-musepack";
            break;

        case e_fileTypeMPP:
            tExt = "audio/x-musepack";
            break;
    
        case e_fileTypeMPPlus:
            tExt = "audio/x-musepack";
            break;
            
        case e_fileTypeAPE:
            tExt = "audio/monkeys-audio";
            break;
            
        case e_fileTypeWV:
            tExt = "audio/wavpack";
            break;

        case e_fileTypeWAV:
        default:
            tExt = "audio/wav";
            break;
    }
    return tExt;
}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::precievedType(FileType t)
{
    QString tExt = "audio";
    return tExt;
}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::progID()
{
    QString pS = "Black.Omega.2";
    return pS;
}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::progID(FileType t)
{
    QString pS = "Black.Omega." + typeExt(t);
    return pS;
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::isTypeRegistered(FileType type)
{
    if(getRegisteredProgID(type)==progID(type))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::getRegisteredProgID(FileType type)
{
    DWORD kType,amount;
    HKEY key;
    LONG res;
    QString tProgID;
    QString kName(".");
    
    kName += typeExt(type);
    res = ::RegOpenKeyW(HKEY_CLASSES_ROOT,reinterpret_cast<LPCWSTR>(kName.utf16()),&key);
    if(res==ERROR_SUCCESS)
    {
        res = ::RegQueryValueExW(key,0,0,&kType,0,&amount);
        if(res==ERROR_SUCCESS && kType==REG_SZ)
        {
            tuint16 *data = new tuint16 [(amount>>1) + 1];

            res = ::RegQueryValueExW(key,0,0,&kType,reinterpret_cast<LPBYTE>(data),&amount);
            if(res==ERROR_SUCCESS)
            {
                data[amount>>1] = 0;
                tProgID = QString::fromUtf16(reinterpret_cast<const char16_t *>(data));
            }
            delete [] data;
        }
        ::RegCloseKey(key);
    }
    return tProgID;
}

//-------------------------------------------------------------------------------------------

QString RegisterFileType::getBackupProgID(FileType type)
{
    DWORD kType,amount;
    HKEY key;
    LONG res;
    QString tProgID;
    QString kName(".");
    
    kName += typeExt(type);
    res = ::RegOpenKeyW(HKEY_CLASSES_ROOT,reinterpret_cast<LPCWSTR>(kName.utf16()),&key);
    if(res==ERROR_SUCCESS)
    {
        QString backID = "BlackOmega.backup";
        
        res = ::RegQueryValueExW(key,reinterpret_cast<LPCWSTR>(backID.utf16()),0,&kType,0,&amount);
        if(res==ERROR_SUCCESS && kType==REG_SZ)
        {
            tuint16 *data = new tuint16 [(amount>>1) + 1];

            res = ::RegQueryValueExW(key,reinterpret_cast<LPCWSTR>(backID.utf16()),0,&kType,reinterpret_cast<LPBYTE>(data),&amount);
            if(res==ERROR_SUCCESS)
            {
                data[amount>>1] = 0;
                tProgID = QString::fromUtf16(reinterpret_cast<const char16_t*>(data));
            }
            delete [] data;
        }
        ::RegCloseKey(key);
    }
    return tProgID;
}

//-------------------------------------------------------------------------------------------
// <root id="HKEY_LOCAL_MACHINE">
//     <node id="Software">
//        <node id="Classes">
//            <key id=".mp3" value="Black.Omega.mp3" delete="false">
//                <string id="Content Type" value="audio/mpeg" />
//                <string id="Percieved Type" value="audio" />
//                <string id="BlackOmega.backup" value="VLC.mp3" />
//            </key>
//            <key id="Black.Omega.mp3" value="Black Omega media file">
//                <key id="DefaultIcon" value="%APPPROGRAM\Black Omega.exe" />
//                <key id="shell" value="Queue">
//                    <key id="Play" value="Play with Black Omega">
//                        <key id="command" value="&quot;%APPPROGRAM\Black Omega.exe&quot; -A &quot;%1&quot;" />
//                    </key>
//                    <key id="Queue" value="Add to Black Omega's Playlist">
//                        <key id="command" value="&quot;%APPPROGRAM\Black Omega.exe&quot; -E &quot;%1&quot;" />
//                    </key>
//                </key>
//            </key>
//        </node>
//    </node>
// </root>
//-------------------------------------------------------------------------------------------

bool RegisterFileType::registerFileType(FileType type,bool enDefault)
{
    bool resA,resB;
    unregisterFileType(type);
    resA = registerFileTypeR(type,enDefault,true);
    resB = registerFileTypeR(type,enDefault,false);
    return (resA || resB) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::registerFileTypeR(FileType type,bool enDefault,bool allFlag)
{
    QStringList x;
    QString xmlS;
    
    if(isTypeRegistered(type))
    {
        return true;
    }
    if(!setAppPath(allFlag))
    {
        return false;
    }
    
    x << "<root id=\"";
    if(allFlag)
    {
        x << "HKEY_LOCAL_MACHINE";
    }
    else
    {
        x << "HKEY_CURRENT_USER";
    }
    x << "\">";
    x << "<node id=\"Software\">";
    x << "<node id=\"Classes\">";
    x << "<key id=\"." + typeExt(type) + "\" value=\"" + progID(type) + "\" delete=\"false\">";
    x << "<string id=\"Content Type\" value=\"" + contentType(type) + "\" />";
    x << "<string id=\"Percieved Type\" value=\"" + precievedType(type) + "\" />";
    if(!getRegisteredProgID(type).isEmpty())
    {
        x << "<string id=\"BlackOmega.backup\" value=\"" + getRegisteredProgID(type) + "\" />";
    }
    x << "</key>";
    x << "<key id=\"" + progID(type) + "\" value=\"Black Omega media file\">";
    x << "<key id=\"DefaultIcon\" value=\"" + m_appPath + ",0\" />";
    x << "<key id=\"shell\" value=\"";
    if(enDefault)
    {
        x << "Play";
    }
    else
    {
        x << "Queue";
    }
    x << "\">";
    x << "<key id=\"Play\" value=\"Play with Black Omega\">";
    x << "<key id=\"command\" value=\"&quot;" + m_appPath + "&quot; -A &quot;%1&quot;\" />";
    x << "</key>";
    x << "<key id=\"Queue\" value=\"Add to Black Omega's Playlist\">";
    x << "<key id=\"command\" value=\"&quot;" + m_appPath + "&quot; -E &quot;%1&quot;\" />";
    x << "</key>";
    x << "</key>";
    x << "</key>";
    x << "</node>";
    x << "</node>";
    x << "</root>";
    
    xmlS = x.join("");
    return common::RegistryScript::exec(xmlS);
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::unregisterFileType(FileType type)
{
    bool resA,resB;
    resA = unregisterFileTypeR(type,true);
    resB = unregisterFileTypeR(type,false);
    return (resA || resB) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::unregisterFileTypeR(FileType type,bool allFlag)
{
    QStringList x;
    QString xmlS;
    
    if(!isTypeRegistered(type))
    {
        return true;
    }
    
    x << "<root id=\"";
    if(allFlag)
    {
        x << "HKEY_LOCAL_MACHINE";
    }
    else
    {
        x << "HKEY_CURRENT_USER";
    }
    x << "\">";
    x << "<node id=\"Software\">";
    x << "<node id=\"Classes\">";
    x << "<key id=\"." + typeExt(type) + "\" value=\"" + getBackupProgID(type) + "\" delete=\"false\" />";
    x << "<delete id=\"" + progID(type) + "\" />";
    x << "</node>";
    x << "</node>";
    x << "</root>";
    
    xmlS = x.join("");
    return common::RegistryScript::exec(xmlS);
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::setAppPath(bool allUser)
{
    QStringList x;
    QString xmlS;
    QFileInfo appInfo(m_appPath);

    if(allUser)
    {
        x << "<root id=\"HKEY_LOCAL_MACHINE\">";
    }
    else
    {
        x << "<root id=\"HKEY_CURRENT_USER\">";
    }
    x << "<node id=\"Software\">";
    x << "<node id=\"Microsoft\">";
    x << "<node id=\"Windows\">";
    x << "<node id=\"CurrentVersion\">";
    x << "<node id=\"App Paths\">";
    x << "<key id=\"Black Omega.exe\" value=\"" + m_appPath + "\">";
    x << "<string id=\"Path\" value=\"" + appInfo.absolutePath() + "\" />";
    x << "</key>";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</root>";

    xmlS = x.join("");
    return common::RegistryScript::exec(xmlS);
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::isDirectoryShell()
{
    bool resA,resB;
    resA = isDirectoryShellR(true);
    resB = isDirectoryShellR(false);
    return (resA || resB) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool RegisterFileType::isDirectoryShellR(bool allFlag)
{
    QString kName;
    HKEY rootK,keyA,keyB,keyC,keyD,keyE;
    LONG res;
    bool flag = false;

    rootK = (allFlag) ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

    kName = "Software";
    res = ::RegOpenKeyW(rootK,reinterpret_cast<LPCWSTR>(kName.utf16()),&keyA);
    if(res==ERROR_SUCCESS)
    {
        kName = "Classes";
        res = ::RegOpenKeyW(keyA,reinterpret_cast<LPCWSTR>(kName.utf16()),&keyB);
        if(res==ERROR_SUCCESS)
        {
            kName = "Directory";
            res = ::RegOpenKeyW(keyB,reinterpret_cast<LPCWSTR>(kName.utf16()),&keyC);
            if(res==ERROR_SUCCESS)
            {
                kName = "shell";
                res = ::RegOpenKeyW(keyC,reinterpret_cast<LPCWSTR>(kName.utf16()),&keyD);
                if(res==ERROR_SUCCESS)
                {
                    kName = "BlackOmega.Enqueue";
                    res = ::RegOpenKeyW(keyD,reinterpret_cast<LPCWSTR>(kName.utf16()),&keyE);
                    if(res==ERROR_SUCCESS)
                    {
                        flag = true;
                        ::RegCloseKey(keyE);
                    }
                }
                ::RegCloseKey(keyC);
            }
            ::RegCloseKey(keyB);
        }
        ::RegCloseKey(keyA);
    }
    return flag;
}

//-------------------------------------------------------------------------------------------

void RegisterFileType::addDirectoryShell()
{
    addDirectoryShellR(true);
    addDirectoryShellR(false);
}

//-------------------------------------------------------------------------------------------

void RegisterFileType::addDirectoryShellR(bool allFlag)
{
    QStringList x;
    QString xmlS;

    x << "<root id=\"";
    if(allFlag)
    {
        x << "HKEY_LOCAL_MACHINE";
    }
    else
    {
        x << "HKEY_CURRENT_USER";
    }
    x << "\">";
    x << "<node id=\"Software\">";
    x << "<node id=\"Classes\">";
    x << "<node id=\"Directory\" value=\"File Folder\">";
    x << "<node id=\"shell\" value=\"none\">";
    x << "<key id=\"BlackOmega.Enqueue\" value=\"Enqueue in Black Omega\">";
    x << "<key id=\"command\" value=\"&quot;" + m_appPath + "&quot; -E &quot;%1&quot;\" />";
    x << "</key>";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</root>";

    xmlS = x.join("");
    common::RegistryScript::exec(xmlS);
}

//-------------------------------------------------------------------------------------------

void RegisterFileType::delDirectoryShell()
{
    delDirectoryShellR(true);
    delDirectoryShellR(false);
}

//-------------------------------------------------------------------------------------------

void RegisterFileType::delDirectoryShellR(bool allFlag)
{
    QStringList x;
    QString xmlS;

    x << "<root id=\"";
    if(allFlag)
    {
        x << "HKEY_LOCAL_MACHINE";
    }
    else
    {
        x << "HKEY_CURRENT_USER";
    }
    x << "\">";
    x << "<node id=\"Software\">";
    x << "<node id=\"Classes\">";
    x << "<node id=\"Directory\" value=\"File Folder\">";
    x << "<node id=\"shell\" value=\"none\">";
    x << "<delete id=\"BlackOmega.Enqueue\" />";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</node>";
    x << "</root>";

    xmlS = x.join("");
    common::RegistryScript::exec(xmlS);
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
