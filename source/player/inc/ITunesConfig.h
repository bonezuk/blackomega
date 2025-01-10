//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_ITUNESCONFIG_H
#define __OMEGA_PLAYER_ITUNESCONFIG_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include <QDesktopServices>
#include <QMenu>

#include "common/inc/ProcessThread.h"
#include "common/inc/ServiceEvent.h"
#include "common/inc/DiskOps.h"
#include "track/info/inc/PListXMLParser.h"
#include "player/inc/QPlaylistWidget.h"
#include "track/db/inc/ITunesLocation.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/CommonDirectoriesForFiles.h"
#include "widget/inc/ImportPlaylistDialog.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class ITunesConfig : public common::ProcessService
{
	public:
		Q_OBJECT
	
	public:
		ITunesConfig(QObject *parent = 0);
		virtual ~ITunesConfig();
		
		QString getMusicDirectory();
		void setPlaylistWidget(QPlaylistWidget *plWidget,QMenu *contextMenu);
		
		virtual void process();
		
		virtual void startEdit();
		virtual void stopEdit();

	protected:
	
		volatile bool m_editFlag;

        QRecursiveMutex m_mutex;
		
		QString m_musicFolder;
		QMap<QString,QMap<tint,track::info::InfoSPtr> > m_fileMaps;
		QMap<QString,QList<QPair<QString,QList<int> > > > m_playlistMaps;
		QMap<QString,QVector<QPair<QString,int> > > m_albumMap;
		QMap<QString,QVector<QPair<QString,int> > > m_artistMap;
		QMap<QString,common::TimeStamp> m_lastUpdateMap;
		
		QPlaylistWidget *m_playlistWidget;
		QMenu *m_collectionMenu;
		QSet<QAction *> m_actionSet;
		
		void buildCollectionMenu();
		void clearCollectionMenu();
		
		void getDBDictionary(QMap<QString,QSet<QString> >& iTunesDict);
		QString getDataItem(const QVariant& v);
		QString getURLFilename(const QString& uPath);
		QString parseURL(const QString& pUrl);
		void processTracks(const QVariant& tRoot,QMap<int,track::info::InfoSPtr>& trackMap);
		void processPlaylist(const QVariant& pRoot,QString& name,QList<int>& pList);
		void processPlaylists(const QVariant& pRoot,QList<QPair<QString,QList<int> > >& pLists);
		void parseDatabase(const QString& fileName);
		void updateConfig(const QString& fName,const QMap<int,track::info::InfoSPtr>& trackMap,const QList<QPair<QString,QList<int> > >& playlistMap);
		
		void buildAlbumConfig();
		void buildArtistConfig();

#if defined(OMEGA_MAC_STORE)
        QStringList processFileListForAppSandbox(const QStringList& fileList);
#endif
		
	protected slots:
	
		void onUpdateMenus();
		void onNoUpdateMenus();
		void onSelectPlaylist(QAction *action);
		void onSelectAlbum(QAction *action);
		void onSelectArtist(QAction *action);
	
	signals:
	
		void onUpdate();
		void onNoUpdate();
};

//-------------------------------------------------------------------------------------------

void showHelp();

//-------------------------------------------------------------------------------------------
}  // namespace player
}  // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
