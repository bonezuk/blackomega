#include "track/info/inc/FileInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

FileInfo::FileInfo() : Info()
{}

//-------------------------------------------------------------------------------------------

FileInfo::~FileInfo()
{}

//-------------------------------------------------------------------------------------------

void FileInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FileInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool FileInfo::read(common::BIOStream *input)
{
    QDir d(input->name());
    QString dName(d.path()),fName;
    bool res = true;
    
    fName = dName.right(d.dirName().length());
    dName.chop(d.dirName().length());
    
    if(!fName.isEmpty())
    {
        QDir dir(dName);

        m_Artist = dir.dirName();
        m_Album = m_Artist;
        m_Year = "";
        m_Comment = "";
        m_Genre = "";
        m_Composer = m_Artist;
        m_OriginalArtist = m_Artist;
        m_Copyright = "";
        m_Encoder = "";
                
        QFileInfo fInfo(fName);
        m_Title = fInfo.baseName();
        m_Track = "0";

        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
