#include "remote/inc/KeyAssignment.h"
#include "remote/inc/QKeyLineEdit.h"
#include <QFile>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

const tchar *KeyAssignment::c_keyKeyAssignmentMap = "KeyAssignmentMap";
const tchar *KeyAssignment::c_keyCommand = "command";
const tchar *KeyAssignment::c_keyKey = "key";
const tchar *KeyAssignment::c_keyRemote = "remote";
const tchar *KeyAssignment::c_attributeId = "id";

//-------------------------------------------------------------------------------------------

QSharedPointer<KeyAssignment> KeyAssignment::m_instance;

//-------------------------------------------------------------------------------------------

KeyAssignment::KeyAssignment() : common::XMLNodeParser(),
    m_map(),
    m_xmlParseState(0),
    m_currentBuildKey(KeyAssignment::e_keyUnassigned),
    m_assignMap()
{}

//-------------------------------------------------------------------------------------------

KeyAssignment::~KeyAssignment()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<KeyAssignment> KeyAssignment::instance()
{
    if(m_instance.data()==0)
    {
        QSharedPointer<KeyAssignment> nInstance(new KeyAssignment);
        m_instance = nInstance;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

KeyCode KeyAssignment::codeForKeyEvent(QKeyEvent *evt)
{
    KeyCode k(QKeyLineEdit::codeForKeyEvent(evt));
    return k;
}

//-------------------------------------------------------------------------------------------


QMap<QString,KeyAssignment::Key>& KeyAssignment::assignMap()
{
    return m_assignMap;
}

//-------------------------------------------------------------------------------------------

const QMap<QString,KeyAssignment::Key>& KeyAssignment::assignMapConst() const
{
    return m_assignMap;
}

//-------------------------------------------------------------------------------------------

void KeyAssignment::buildAssignmentMap()
{
    const QMap<Key,KeyCodesContainer>& kMap = mapConst();
    QMap<Key,KeyCodesContainer>::const_iterator ppI;

    clearAssignmentMap();
    for(ppI=kMap.begin();ppI!=kMap.end();ppI++)
    {
        const KeyCodesContainer& keys = ppI.value();
        QList<KeyCode>::const_iterator ppJ;
        
        for(ppJ=keys.list().begin();ppJ!=keys.list().end();ppJ++)
        {
            QString v = (*ppJ).variant().toString();
            assignMap().insert(v,ppI.key());
        }
    }
}

//-------------------------------------------------------------------------------------------

void KeyAssignment::clearAssignmentMap()
{
    assignMap().clear();
}

//-------------------------------------------------------------------------------------------

KeyAssignment::Key KeyAssignment::assignedTo(const KeyCode& k) const
{
    Key kCode = e_keyUnassigned;
    QMap<Key,KeyCodesContainer>::const_iterator ppI;
    
    if(assignMapConst().isEmpty())
    {
        for(ppI=mapConst().begin();ppI!=mapConst().end() && kCode==e_keyUnassigned;ppI++)
        {
            const KeyCodesContainer& keys = ppI.value();
            QList<KeyCode>::const_iterator ppJ;
            
            for(ppJ=keys.list().begin();ppJ!=keys.list().end() && kCode==e_keyUnassigned;ppJ++)
            {
                if(k == (*ppJ))
                {
                    kCode = ppI.key();
                }
            }
        }
    }
    else
    {
        QString v = k.variant().toString();
        QMap<QString,Key>::const_iterator ppI = assignMapConst().find(v);
        if(ppI!=assignMapConst().end())
        {
            kCode = ppI.value();
        }
    }
    return kCode;
}

//-------------------------------------------------------------------------------------------

QByteArray KeyAssignment::loadInFile(const QString& fileName) const
{
    QByteArray fMem;
    QFile fileIn(fileName);

    if(fileIn.open(QIODevice::ReadOnly))
    {
        fMem = fileIn.readAll();
    }
    return fMem;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::loadFromXML(const QString& fileName)
{
    QMap<Key,KeyCodesContainer>& mMap = map();
    QByteArray fMem;
    bool res = false;
    
    clearAssignmentMap();
    mMap.clear();    
    fMem = loadInFile(fileName);
    if(!fMem.isEmpty())
    {
        xmlDocPtr doc = common::XMLLibIF::instance()->xmlParseMemory(fMem.constData(),fMem.size());
        if(doc!=0)
        {
            xmlNodePtr root = common::XMLLibIF::instance()->xmlDocGetRootElement(doc);
            if(root!=0)
            {
                if(isRootNode(root))
                {
                    parse(root);
                    buildAssignmentMap();
                    res = true;
                }
            }
            common::XMLLibIF::instance()->xmlFreeDoc(doc);
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint KeyAssignment::getXMLParseState() const
{
    return m_xmlParseState;
}

//-------------------------------------------------------------------------------------------

void KeyAssignment::setXMLParseState(tint s)
{
    m_xmlParseState = s;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::isRootNode(xmlNodePtr pNode) const
{
    bool res;

    if(getXMLParseState()==0)
    {
        res = isSpecifiedNode(pNode,c_keyKeyAssignmentMap);
    }
    else
    {
        res = isSpecifiedNode(pNode,c_keyCommand);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void KeyAssignment::processNode(xmlNodePtr pNode)
{
    QString cName = getNameOfNode(pNode).toLower();

    if(getXMLParseState()==0)
    {
        if(cName==comparisonKey(c_keyCommand) && isAttribute(pNode,c_attributeId))
        {
            QString cmdId = getAttribute(pNode,c_attributeId);
            
            if(startBuildContainer(keyFromCommandId(cmdId)))
            {
                setXMLParseState(1);
                parse(pNode);
                setXMLParseState(0);
            }
        }
    }
    else
    {
        if(cName==comparisonKey(c_keyKey))
        {
            tuint32 num = 0;
            QString kString = getTextOfElement(pNode).toLower().trimmed();
            if(kString.indexOf(QChar('x'))>=0)
            {
                kString = kString.mid(kString.indexOf(QChar('x')) + 1);
                num = kString.toUInt(0,16);
            }
            else
            {
                num = kString.toUInt();
            }
            if(num>0)
            {
                KeyCode k(num);
                assignToCurrent(k);
            }
        }
        else if(cName==comparisonKey(c_keyRemote))
        {
            QString rString = getTextOfElement(pNode).trimmed();
            if(!rString.isEmpty())
            {
                KeyCode k(rString);
                assignToCurrent(k);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

KeyAssignment::Key KeyAssignment::keyFromCommandId(const QString& cmdId) const
{
    Key k = e_keyUnassigned;

    if(cmdId=="play")
    {
        k = e_keyPlay;
    }
    else if(cmdId=="previous_track")
    {
        k = e_keyPreviousTrack;
    }
    else if(cmdId=="next_track")
    {
        k = e_keyNextTrack;
    }
    else if(cmdId=="volume_down")
    {
        k = e_keyVolumeDown;
    }
    else if(cmdId=="volume_up")
    {
        k = e_keyVolumeUp;
    }
    return k;
}

//-------------------------------------------------------------------------------------------

QString KeyAssignment::commandIdFromKey(Key k) const
{
    QString kStr;

    switch(k)
    {
        case e_keyPlay:
            kStr = "play";
            break;
        case e_keyPreviousTrack:
            kStr = "previous_track";
            break;
        case e_keyNextTrack:
            kStr = "next_track";
            break;
        case e_keyVolumeDown:
            kStr = "volume_down";
            break;
        case e_keyVolumeUp:
            kStr = "volume_up";
            break;
        case e_keyUnassigned:
        default:
            kStr = "unassigned";
            break;
    }
    return kStr;
}

//-------------------------------------------------------------------------------------------

void KeyAssignment::assignToCurrent(const KeyCode& k)
{
    if(currentBuildKeyConst()!=e_keyUnassigned && assignedTo(k)==e_keyUnassigned)
    {
        QMap<Key,KeyCodesContainer>::iterator ppI = map().find(currentBuildKeyConst());
        if(ppI!=map().end())
        {
            ppI.value().list().append(k);
        }
    }
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::startBuildContainer(Key cmd)
{
    bool res;
    
    currentBuildKey() = cmd;
    if(cmd!=e_keyUnassigned)
    {
        if(map().find(cmd)!=map().end())
        {
            map().find(cmd).value().list().clear();
        }
        else
        {
            KeyCodesContainer kContainer;
            map().insert(cmd,kContainer);
        }
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

KeyAssignment::Key& KeyAssignment::currentBuildKey()
{
    return m_currentBuildKey;
}

//-------------------------------------------------------------------------------------------

const KeyAssignment::Key& KeyAssignment::currentBuildKeyConst() const
{
    return m_currentBuildKey;
}

//-------------------------------------------------------------------------------------------

QMap<KeyAssignment::Key,KeyCodesContainer>& KeyAssignment::map()
{
    return m_map;
}

//-------------------------------------------------------------------------------------------

const QMap<KeyAssignment::Key,KeyCodesContainer>& KeyAssignment::mapConst() const
{
    return m_map;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::writeXMLKeyCode(xmlTextWriterPtr pWriter,const KeyCode& k) const
{
    int rc;

    if(k.isKeyboard())
    {
        QString kText = "0x" + QString::number(k.keyCode(),16);
        rc = common::XMLLibIF::instance()->xmlTextWriterWriteElement(pWriter,c_keyKey,kText.toUtf8().constData());
    }
    else
    {
        rc = common::XMLLibIF::instance()->xmlTextWriterWriteElement(pWriter,c_keyRemote,k.remote().toUtf8().constData());
    }
    return (rc>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::writeXMLKeyContainer(xmlTextWriterPtr pWriter,const Key& keyCmd,const KeyCodesContainer& cont) const
{
    int rc;
    
    rc = common::XMLLibIF::instance()->xmlTextWriterStartElement(pWriter,c_keyCommand);
    if(rc>=0)
    {
        rc = common::XMLLibIF::instance()->xmlTextWriterWriteAttribute(pWriter,c_attributeId,commandIdFromKey(keyCmd).toUtf8().constData());
        if(rc>=0)
        {
            for(QList<KeyCode>::const_iterator ppI=cont.list().begin();ppI!=cont.list().end() && rc>=0;ppI++)
            {
                if(!writeXMLKeyCode(pWriter,*ppI))
                {
                    rc = -1;
                }
            }
            
            if(rc>=0)
            {
                rc = common::XMLLibIF::instance()->xmlTextWriterEndElement(pWriter);
            }
        }
    }
    return (rc>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::writeXMLKeyMap(xmlTextWriterPtr pWriter) const
{
    int rc;
    
    rc = common::XMLLibIF::instance()->xmlTextWriterStartElement(pWriter,c_keyKeyAssignmentMap);
    if(rc>=0)
    {
        for(QMap<Key,KeyCodesContainer>::const_iterator ppI=mapConst().begin();ppI!=mapConst().end() && rc>=0;ppI++)
        {
            if(!writeXMLKeyContainer(pWriter,ppI.key(),ppI.value()))
            {
                rc = -1;
            }
        }
        if(rc>=0)
        {
            rc = common::XMLLibIF::instance()->xmlTextWriterEndElement(pWriter);
        }
    }
    return (rc>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::saveToXML(const QString& fileName) const
{
    bool res = false;
    xmlDocPtr pDoc;
    xmlTextWriterPtr pWriter = 0;

    pWriter = common::XMLLibIF::instance()->xmlNewTextWriterDoc(&pDoc,0);
    if(pWriter!=0)
    {
        if(writeXMLKeyMap(pWriter))
        {
            common::XMLLibIF::instance()->xmlFreeTextWriter(pWriter);
            int rc = common::XMLLibIF::instance()->xmlSaveFileEnc(fileName.toUtf8().constData(),pDoc,"utf-8");
            if(rc>=0)
            {
                res = true;
            }
        }
        else
        {
            common::XMLLibIF::instance()->xmlFreeTextWriter(pWriter);
        }
        common::XMLLibIF::instance()->xmlFreeDoc(pDoc);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyAssignment::loadFromModel(QAbstractItemModel *model)
{
    tint maxRows = static_cast<tint>(e_keyUnassigned);
    bool res = false;
    
    clearAssignmentMap();
    map().clear();
    if(model!=0 && model->rowCount()>0 && model->columnCount()>0)
    {
        for(int i=0;i<model->rowCount() && i<maxRows;i++)
        {
            QModelIndex modelIndex = model->index(i,0,QModelIndex());
            QVariant v = model->data(modelIndex,Qt::EditRole);
            KeyCodesContainer cont(v);
            if(!cont.isEmpty())
            {
                map().insert(static_cast<Key>(i),cont);
            }
        }
        buildAssignmentMap();
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QStandardItemModel *KeyAssignment::saveToModel(QObject *parent) const
{
    QMap<Key,KeyCodesContainer>::const_iterator ppI;
    int noRows = static_cast<int>(e_keyUnassigned);
    QStandardItemModel *model = new QStandardItemModel(5,1,parent);

    for(int i=0;i<noRows;i++)
    {
        Key k = static_cast<Key>(i);
        QString name = getDisplayNameForKey(k);
        QModelIndex modelIndex = model->index(i,0,QModelIndex());
        
        model->setHeaderData(i,Qt::Vertical,QVariant(name));
        model->setData(modelIndex,QVariant(name),Qt::UserRole);
        
        ppI = mapConst().find(k);
        if(ppI!=mapConst().end())
        {
            model->setData(modelIndex,ppI.value().variant(),Qt::EditRole);
        }
        else
        {
            model->setData(modelIndex,QVariant(),Qt::EditRole);
        }
    }
    return model;
}

//-------------------------------------------------------------------------------------------

QString KeyAssignment::getDisplayNameForKey(Key key) const
{
    QString name;

    switch(key)
    {
        case e_keyPlay:
            name = "Play / Pause";
            break;
        case e_keyPreviousTrack:
            name = "Previous Track";
            break;
        case e_keyNextTrack:
            name = "Next Track";
            break;
        case e_keyVolumeDown:
            name = "Volume Down";
            break;
        case e_keyVolumeUp:
            name = "Volume Up";
            break;
        case e_keyUnassigned:
        default:
            name = "Unassigned";
            break;
    }
    return name;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
