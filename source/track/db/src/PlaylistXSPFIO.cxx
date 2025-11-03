#include "track/db/inc/PlaylistXSPFIO.h"
#include "track/info/inc/SBBookmarkService.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/CommonDirectoriesForFiles.h"
#include "track/db/inc/TrackFileDependencies.h"
#include "widget/inc/ImportPlaylistDialog.h"
#endif

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(PlaylistIOFactory,PlaylistAbstractIO, \
                            PlaylistXSPFIOFactory,PlaylistXSPFIO, \
                            "xspf",false)

//-------------------------------------------------------------------------------------------

PlaylistXSPFIO::PlaylistXSPFIO() : PlaylistAbstractIO(),
    m_outFilename()
{}

//-------------------------------------------------------------------------------------------

PlaylistXSPFIO::~PlaylistXSPFIO()
{}

//-------------------------------------------------------------------------------------------

void PlaylistXSPFIO::loadXMLNode(xmlDocPtr doc,xmlNodePtr pNode,QStack<QString>& nameStack,QList<QPair<QString,QByteArray> >& fileList)
{
    if(pNode!=0 && pNode->type==XML_ELEMENT_NODE && !nameStack.isEmpty())
    {
        QString pNodeName = QString::fromUtf8(reinterpret_cast<const char *>(pNode->name)).toLower();
    
        if(pNodeName==nameStack.top())
        {
            QString pName = nameStack.pop();
        
            if(nameStack.isEmpty())
            {
                loadXMLTrack(doc,pNode,fileList);
            }
            else
            {
                xmlNode *cNode = pNode->children;
                while(cNode!=0)
                {
                    loadXMLNode(doc,cNode,nameStack,fileList);
                    cNode = cNode->next;
                }
            }
            nameStack.push(pName);
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlaylistXSPFIO::loadXMLTrack(xmlDocPtr doc,xmlNodePtr pNode,QList<QPair<QString,QByteArray> >& fileList)
{
    QString fileName;
    QByteArray bkArray;
    
    if(pNode!=0 && pNode->type==XML_ELEMENT_NODE)
    {
        xmlNode *cNode = pNode->children;
        while(cNode!=0)
        {
            if(cNode!=0 && cNode->type==XML_ELEMENT_NODE)
            {
                QString cName = QString::fromUtf8(reinterpret_cast<const char *>(cNode->name)).toLower();
                if(cName=="location")
                {
                    xmlNode *tNode = cNode->children;
                    if(tNode!=0)
                    {
                        loadXMLFilename(doc,tNode,fileName);
                    }                
                }
                else if(cName=="extension")
                {
                    QByteArray bkA;
                    if(loadXMLBookmarkExtension(doc,cNode,bkA))
                    {
                        bkArray = bkA;
                    }
                }
            }
            cNode = cNode->next;
        }
    }
    if(!fileName.isEmpty())
    {
        fileList.append(QPair<QString,QByteArray>(fileName,bkArray));
    }
}

//-------------------------------------------------------------------------------------------

bool PlaylistXSPFIO::loadXMLBookmarkExtension(xmlDocPtr doc,xmlNodePtr eNode,QByteArray& bkArray)
{
    bool res = false;
    
    if(eNode!=0 && eNode->type==XML_ELEMENT_NODE)
    {
        xmlChar *eAttr = xmlGetProp(eNode,reinterpret_cast<const xmlChar *>("application"));
        if(eAttr!=0)
        {
            QString eATxt = QString::fromUtf8(reinterpret_cast<const char *>(eAttr));
            if(eATxt=="http://www.blackomega.co.uk")
            {
                xmlNode *cNode = eNode->children;
                while(cNode!=0 && !res)
                {
                    if(cNode->type==XML_TEXT_NODE || cNode->type==XML_CDATA_SECTION_NODE)
                    {
                        xmlChar *dataStr = xmlNodeGetContent(cNode);
                        if(dataStr!=0)
                        {
                            QByteArray eArray(reinterpret_cast<const char *>(dataStr),xmlStrlen(dataStr));
                            bkArray = QByteArray::fromBase64(eArray);
                            if(bkArray.size()>0)
                            {
                                res = true;
                            }
                            xmlFree(dataStr);
                        }
                    }
                    cNode = cNode->next;
                }
            }
            xmlFree(eAttr);
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlaylistXSPFIO::loadXMLFilename(xmlDocPtr doc,xmlNodePtr fNode,QString& fileName)
{
    if(fNode!=0 && (fNode->type==XML_TEXT_NODE || fNode->type==XML_CDATA_SECTION_NODE))
    {
        xmlChar *fNameXML = xmlNodeGetContent(fNode);
        if(fNameXML!=0)
        {
            xmlURIPtr rURI = xmlCreateURI();
            if(rURI!=0)
            {
                bool uFlag = false;
                
                if(xmlParseURIReference(rURI,reinterpret_cast<const char *>(fNameXML))==0)
                {
                    if(rURI->scheme==0)
                    {
                        xmlChar *bNameXML = xmlNodeGetBase(doc,fNode);
                        if(bNameXML!=0)
                        {
                            xmlChar *rNameXML = xmlBuildURI(fNameXML,bNameXML);
                            if(rNameXML!=0)
                            {
                                if(xmlParseURIReference(rURI,reinterpret_cast<const char *>(rNameXML))==0 && rURI->scheme!=0)
                                {
                                    uFlag = true;
                                }
                                xmlFree(rNameXML);
                            }
                        }
                    }
                    else
                    {
                        uFlag = true;
                    }
                }
                if(uFlag)
                {
                    xmlChar *refS = xmlSaveUri(rURI);
                    if(refS!=0)
                    {
                        QUrl fUrl = QUrl::fromEncoded(QByteArray(reinterpret_cast<const char *>(refS)));
                        fUrl.setHost("");
                        QString fName = fUrl.toLocalFile();
                        fileName = QDir::toNativeSeparators(fName);
                        xmlFree(refS);
                    }
                }
                xmlFreeURI(rURI);
            }
            xmlFree(fNameXML);
        }
    }
}

//-------------------------------------------------------------------------------------------

bool PlaylistXSPFIO::load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    int i;
    xmlDocPtr doc;
    xmlNodePtr root;
    QList<QPair<QString,QByteArray> > fileList;
    QList<QPair<QString,QByteArray> >::iterator ppI;
    bool res = false;
    
    pList.clear();
    
#if defined(OMEGA_MAC_STORE)
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    if(!sbBookmark->has(fileName,true))
    {
        sbBookmark->add(fileName,true);
    }
#endif

    doc = xmlParseFile(fileName.toUtf8().constData());
    if(doc!=0)
    {
        root = xmlDocGetRootElement(doc);
        if(root!=0)
        {
            QStack<QString> nameStack;

            nameStack.push("track");
            nameStack.push("tracklist");
            nameStack.push("playlist");

            loadXMLNode(doc,root,nameStack,fileList);
            res = true;
        }
        xmlFreeDoc(doc);
    }

#if defined(OMEGA_MAC_STORE)
    QStringList accessFileList;
    track::db::TrackFileDependencies dependency;
    tint fCount = 0;

    for(ppI=fileList.begin();ppI!=fileList.end();ppI++)
    {
        const QString& fName = (*ppI).first;
        const QByteArray& bkArray = (*ppI).second;
        if(bkArray.size()>0)
        {
            if(track::info::SBBookmarkService::instance()->add(fileName,fName,true,bkArray))
            {
                fCount++;
            }
            else
            {
                if(sbBookmark->has(fName,true))
                {
                    fCount++;
                }
                else
                {
                    accessFileList << fName;
                }
            }
        }
        else
        {
            if(sbBookmark->has(fName,true))
            {
                fCount++;
            }
            else
            {
                accessFileList << fName;
                dependency.add(fName);
            }
        }
    }
    
    QSet<QString> allDependencies = dependency.allDependencies();
    for(QSet<QString>::iterator ppI=allDependencies.begin();ppI!=allDependencies.end();++ppI)
    {
        const QString& lPath = *ppI;
        if(!sbBookmark->has(lPath,true))
        {
            accessFileList << *ppI;
        }
    }
    
    if(accessFileList.size() > 0)
    {
        QFileInfo fInfo(fileName);
        QSet<QString> pathSet = common::CommonDirectoriesForFiles::find(accessFileList);
        QStringList pathList(pathSet.begin(), pathSet.end());
                
        widget::ImportPlaylistDialog importDialog(dynamic_cast<QWidget *>(m_parent));
        importDialog.setPlaylistFileName(fInfo.fileName());
        importDialog.setDirectories(pathList);
        importDialog.setModal(true);
        importDialog.exec();
                
        if(importDialog.result()==QDialog::Accepted)
        {
            for(QStringList::iterator ppI=pathList.begin();ppI!=pathList.end();ppI++)
            {
                if(sbBookmark->has(*ppI,true))
                {
                    fCount++;
                }
            }
        }
    }
    
    if(fCount==0)
    {
        return false;
    }
#endif

    for(i=0,ppI=fileList.begin();ppI!=fileList.end() && !progress->isCancelled();ppI++,i++)
    {
        const QString& fName = (*ppI).first;
        tfloat32 pAmount = (static_cast<tfloat32>(i * 90) / static_cast<tfloat32>(fileList.size())) + 10.0f;
        progress->setProgress(pAmount);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
#if defined(OMEGA_MAC_STORE)
        if(sbBookmark->has(fName,true))
        {
            appendToList(fName,pList,progress);
        }
#else
        appendToList(fName,pList,progress);
#endif
    }
    
    if(progress->isCancelled())
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool PlaylistXSPFIO::save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress)
{
    tint i;
    xmlDocPtr doc;
    xmlTextWriterPtr writer;
    bool res = false;
    
#if defined(OMEGA_MAC_STORE)
    tint count;
    common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
    
    if(!sbBookmark->has(fileName,false))
    {
        if(!common::DiskOps::exist(fileName))
        {
            common::BIOBufferedStream pFile(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
            pFile.setCheckOutFlag(false);
            if(pFile.open(fileName))
            {
                pFile.close();
            }
            
            while(!sbBookmark->add(fileName,false) && count<5)
            {
                ::usleep(1000);
                count++;
            }
        }
        
    }
#endif
    m_outFilename = fileName;

    progress->activate(true);
    writer = xmlNewTextWriterDoc(&doc,0);
    if(writer!=0)
    {
        if(xmlTextWriterStartDocument(writer,0,"UTF-8",0)>=0)
        {
            if(xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>("playlist"))>=0)
            {
                if(xmlTextWriterWriteAttribute(writer,reinterpret_cast<const xmlChar *>("version"),reinterpret_cast<const xmlChar *>("1"))>=0)
                {
                    if(xmlTextWriterWriteAttribute(writer,reinterpret_cast<const xmlChar *>("xmlns"),reinterpret_cast<const xmlChar *>("http://xspf.org/ns/0/"))>=0)
                    {
                        res = true;
                    }
                }
            }
        }
        if(res)
        {
            if(xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>("trackList"))>=0)
            {
                for(i=0;i<pList.size() && !progress->isCancelled() && res;i++)
                {
                    tfloat32 pAmount = static_cast<tfloat32>(i * 100) / static_cast<tfloat32>(pList.size());
                    progress->setProgress(pAmount);
                    QCoreApplication::processEvents(QEventLoop::AllEvents);
                    
                    track::info::InfoSPtr pInfo(pList[i]);
#if defined(OMEGA_MAC_STORE)
                    if(!sbBookmark->has(fileName,pInfo->getFilename(),true))
                    {
                        QStringList fList;
                        fList.append(pInfo->getFilename());
                        sbBookmark->add(fileName,fList,true);
                    }
#endif
                    res = saveXMLTrack(writer,pInfo);
                }
                
                if(res)
                {
                    res = false;
                    if(xmlTextWriterEndElement(writer)>=0)
                    {
                        if(xmlTextWriterEndElement(writer)>=0)
                        {
                            if(xmlTextWriterEndDocument(writer)>=0)
                            {
                                if(xmlSaveFileEnc(fileName.toUtf8().constData(),doc,"UTF-8")>=0)
                                {
                                    res = true;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                res = false;
            }
        }
        xmlSaveFileEnc(fileName.toUtf8().constData(),doc,0);
        xmlFreeTextWriter(writer);
        xmlFreeDoc(doc);
    }
    progress->deactivate();
    return res;
}

//-------------------------------------------------------------------------------------------

bool PlaylistXSPFIO::saveXMLTrack(xmlTextWriterPtr writer,track::info::InfoSPtr& pInfo)
{
    bool res = false;

    if(xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>("track"))>=0)
    {
        if(xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>("title"))>=0)
        {
            QString title = pInfo->artist() + " - " + pInfo->title();
            if(xmlTextWriterWriteString(writer,reinterpret_cast<const xmlChar *>(title.toUtf8().constData()))>=0)
            {
                if(xmlTextWriterEndElement(writer)>=0)
                {
                    res = true;
                }
            }
        }
        if(res)
        {
            res = false;
            if(xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>("location"))>=0)
            {
                QUrl u = QUrl::fromLocalFile(pInfo->getFilename());
                if(!u.isEmpty())
                {
                    if(xmlTextWriterWriteString(writer,reinterpret_cast<const xmlChar *>(u.toEncoded().constData()))>=0)
                    {
                        if(xmlTextWriterEndElement(writer)>=0)
                        {
#if defined(OMEGA_MAC_STORE)
                            {
                                QByteArray bkArray = track::info::SBBookmarkService::instance()->getBookmarkArray(m_outFilename,pInfo->getFilename());
                                if(bkArray.size()>0)
                                {
                                    if(xmlTextWriterStartElement(writer,reinterpret_cast<const xmlChar *>("extension"))>=0)
                                    {
                                        if(xmlTextWriterWriteAttribute(writer,reinterpret_cast<const xmlChar *>("application"),reinterpret_cast<const xmlChar *>("http://www.blackomega.co.uk"))>=0)
                                        {
                                            QByteArray eArray = bkArray.toBase64();
                                            QString eStr = QString::fromLatin1(eArray.data(),eArray.size());
                                            if(xmlTextWriterWriteString(writer,reinterpret_cast<const xmlChar *>(eStr.toLatin1().constData()))>=0)
                                            {
                                                xmlTextWriterEndElement(writer);
                                            }
                                        }
                                    }
                                }
                            }
#endif
                            if(xmlTextWriterEndElement(writer)>=0)
                            {
                                res = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
