//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYASSIGNMENT_H
#define __OMEGA_REMOTE_KEYASSIGNMENT_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "remote/inc/KeyCodesContainer.h"
#include "common/inc/XMLNodeParser.h"
#include "common/inc/XMLLibIF.h"

#include <QKeyEvent>
#include <QAbstractItemModel>
#include <QStandardItemModel>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyAssignment : public common::XMLNodeParser
{
	public:
		typedef enum
		{
			e_keyPlay = 0,
			e_keyPreviousTrack,
			e_keyNextTrack,
			e_keyVolumeDown,
			e_keyVolumeUp,
			e_keyUnassigned
		} Key;
	
		static const tchar *c_keyKeyAssignmentMap;
		static const tchar *c_keyCommand;
		static const tchar *c_keyKey;
		static const tchar *c_keyRemote;
		static const tchar *c_attributeId;
	
	public:
		KeyAssignment();
		virtual ~KeyAssignment();
		
        static QSharedPointer<KeyAssignment> instance();
		
		static KeyCode codeForKeyEvent(QKeyEvent *evt);
		
		virtual bool loadFromXML(const QString& fileName);
		virtual bool saveToXML(const QString& fileName) const;
		
		virtual bool loadFromModel(QAbstractItemModel *model);
		virtual QStandardItemModel *saveToModel(QObject *parent = 0) const;
		
		virtual Key assignedTo(const KeyCode& k) const;
		
	protected:
	
        static QSharedPointer<KeyAssignment> m_instance;
	
		QMap<Key,KeyCodesContainer> m_map;
		
		tint m_xmlParseState;
		Key m_currentBuildKey;
		
		QMap<QString,Key> m_assignMap;
		
		virtual QMap<Key,KeyCodesContainer>& map();
		virtual const QMap<Key,KeyCodesContainer>& mapConst() const;
		virtual KeyAssignment::Key& currentBuildKey();
		virtual const KeyAssignment::Key& currentBuildKeyConst() const;
		virtual tint getXMLParseState() const;
		virtual void setXMLParseState(tint s);
		
		virtual QByteArray loadInFile(const QString& fileName) const;
		
		virtual Key keyFromCommandId(const QString& cmdId) const;
		virtual QString commandIdFromKey(Key k) const;
		
		virtual bool startBuildContainer(Key cmd);
		virtual void assignToCurrent(const KeyCode& k);
		
		virtual bool isRootNode(xmlNodePtr pNode) const;
		virtual void processNode(xmlNodePtr pNode);
		
		virtual bool writeXMLKeyCode(xmlTextWriterPtr pWriter,const KeyCode& k) const;
		virtual bool writeXMLKeyContainer(xmlTextWriterPtr pWriter,const Key& keyCmd,const KeyCodesContainer& cont) const;
		virtual bool writeXMLKeyMap(xmlTextWriterPtr pWriter) const;
		
		virtual QMap<QString,Key>& assignMap();
		virtual const QMap<QString,Key>& assignMapConst() const;
		virtual void buildAssignmentMap();
		virtual void clearAssignmentMap();
		
		virtual QString getDisplayNameForKey(Key key) const;
};

//-------------------------------------------------------------------------------------------

typedef QSharedPointer<KeyAssignment> KeyAssignmentSPtr;

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
