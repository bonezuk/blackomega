//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_TEST_TRACKDBTESTENVIROMENT_H
#define __OMEGA_TRACK_MODEL_TEST_TRACKDBTESTENVIROMENT_H
//-------------------------------------------------------------------------------------------

#include "common/test/UPnPProviderTestEnviroment.h"

#include <QVariant>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TrackDBTestEnviroment
{
	public:
		virtual ~TrackDBTestEnviroment();
		
		static TrackDBTestEnviroment *instance();
		
		bool buildTestDB(const QString& dbFileName,const QString& dirName);
		
		bool load();
		void unload();
		
		bool compareResults(const QVector<QVector<QVariant> >& results,const QString& resultPrefix,int resultID);
		
		static QString testUtilitySQLNormaliseWhitespace(const QString& iSql);

		QString getDBDirectory();
		QString getTempDirectory();
		
		void setExecPath(const QString &path);
		const QString& execPath() const;
		void setSpawnICSProcess(bool flag);
		bool spawnICSProcess() const;

	protected:

		static TrackDBTestEnviroment *m_instance;
	
#if defined(OMEGA_WIN32)
		HMODULE m_blueOmegaDLL;
		HMODULE m_silverOmegaDLL;
		HMODULE m_blackOmegaDLL;
		HMODULE m_whiteOmegaDLL;
		HMODULE m_redOmegaDLL;
		HMODULE m_greenOmegaDLL;
		HMODULE m_RTPsilverOmegaDLL;
#endif
		QString m_execPath;
		bool m_spawnICSProcess;

		TrackDBTestEnviroment();

		bool scanDBDirectory(const QString& dirName);
		QString getDBFilename();
		bool setupDirectory(const QVector<QPair<QString,QString> >& fileList);

		bool loadCVS(const QString& cvsFileName,QVector<QVector<QVariant> >& entries);
		
		QVector<QVector<QVariant> >::iterator indexOfResult(const QVector<QVariant>& results,QVector<QVector<QVariant> >& list);
		bool isAnyEqual(const QVariant& a,const QVariant& b);
		
		QString removeWhitespace(const QString& str);
		bool isInteger(const QString& str);
		tint getIntegerFromString(const QString& str);
		bool isDouble(const QString& str);
		tfloat64 getDoubleFromString(const QString& str);
};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
