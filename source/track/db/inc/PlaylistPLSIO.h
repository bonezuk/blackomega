//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_PLAYLISTPLSIO_H
#define __OMEGA_TRACK_DB_PLAYLISTPLSIO_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/PlaylistAbstractIO.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT PlaylistPLSIO : public PlaylistAbstractIO
{
    public:
    
        typedef enum
        {
            e_linePlaylist = 0,
            e_lineFile,
            e_lineTitle,
            e_lineLength,
            e_lineNoOfEntries,
            e_lineVersion,
            e_lineUnknown
        } LineType;
        
    public:
        PlaylistPLSIO();
        virtual ~PlaylistPLSIO();
        
        virtual bool load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
        virtual bool save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);

    protected:
    
        common::BOParse *m_plsLang;
        tint m_plsState[9];
        
        LineType parseLineInfo(const QString& line,QString& txtData,tint& noData);
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
