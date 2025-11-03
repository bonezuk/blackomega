//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_PLAYLISTXSPFIO_H
#define __OMEGA_TRACK_DB_PLAYLISTXSPFIO_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/PlaylistAbstractIO.h"

#include <QStack>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class PlaylistXSPFIO : public PlaylistAbstractIO
{
    public:
        PlaylistXSPFIO();
        virtual ~PlaylistXSPFIO();

        virtual bool load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
        virtual bool save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
    
    protected:
        
        QString m_outFilename;
        
        void loadXMLNode(xmlDocPtr doc,xmlNodePtr pNode,QStack<QString>& nameStack,QList<QPair<QString,QByteArray> >& fileList);
        void loadXMLTrack(xmlDocPtr doc,xmlNodePtr pNode,QList<QPair<QString,QByteArray> >& fileList);
        bool loadXMLBookmarkExtension(xmlDocPtr doc,xmlNodePtr eNode,QByteArray& bkArray);
        void loadXMLFilename(xmlDocPtr doc,xmlNodePtr fNode,QString& fileName);

        bool saveXMLTrack(xmlTextWriterPtr writer,track::info::InfoSPtr& pInfo);
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
