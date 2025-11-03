#include "common/inc/DictionaryXMLMap.h"

#include <QtXml/QDomElement>
#include <QFile>
#include <QStringList>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QMap<QString,QString> DictionaryXMLMap::read(const QString& fileName)
{
    QMap<QString,QString> dictMap;
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc;

        if(doc.setContent(&file))
        {
            QString keyV,nameV;
            QDomElement dictElement = doc.documentElement();
            if(dictElement.tagName().toLower()=="dict")
            {
                QDomNode n = dictElement.firstChild();
                while(!n.isNull())
                {
                    QDomElement e = n.toElement();
                    if(!e.isNull())
                    {
                        if(e.tagName().toLower()=="key")
                        {
                            keyV = e.text().trimmed();
                        }
                        else if(e.tagName().toLower()=="string" && !keyV.isEmpty())
                        {
                            nameV = e.text().trimmed();
                            dictMap.insert(keyV,nameV);
                            keyV = "";
                        }
                    }
                    n = n.nextSibling();
                }
            }
        }
        file.close();
    }
    return dictMap;
}

//-------------------------------------------------------------------------------------------

bool DictionaryXMLMap::write(const QString& fileName,const QMap<QString,QString>& dict)
{
    QMap<QString,QString>::const_iterator ppI;
    QStringList x;
    bool res = false;

    x << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    x << "<dict>\n";
    for(ppI=dict.begin();ppI!=dict.end();ppI++)
    {
        x << "<key>";
        x << ppI.key();
        x << "</key>\n";
        x << "<string>";
        x << ppI.value();
        x << "</string>\n";
    }
    x << "</dict>\n";

    QString outTxt = x.join("");
    QFile outFile(fileName);
    if(outFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QByteArray outArray = outTxt.toUtf8();
        if(outFile.write(outArray)==outArray.size())
        {
            res = true;
        }
        outFile.close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
