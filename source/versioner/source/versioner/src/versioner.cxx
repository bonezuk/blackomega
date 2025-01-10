#include "inc/ProductVersionInfo.h"

#include <QVector>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDomElement>


//-------------------------------------------------------------------------------------------
namespace omega
{
namespace versioner
{
//-------------------------------------------------------------------------------------------

QVector<QPair<QString,QString> > dictionaryFromPList(const QString& fileName)
{
	QVector<QPair<QString,QString> > dict;
	QFile file(fileName);
	
	if(file.open(QIODevice::ReadOnly))
	{
		QDomDocument doc;
		
		if(doc.setContent(&file))
		{
			QDomElement plistElement = doc.documentElement();
			if(plistElement.tagName().toLower()=="plist")
			{
				QDomElement dictElement = plistElement.firstChild().toElement();
				if(!dictElement.isNull())
				{
					if(dictElement.tagName().toLower()=="dict")
					{
						QString keyV,valueV;
						QDomNode n = dictElement.firstChild();
						while(!n.isNull())
						{
							QDomElement e = n.toElement();
							if(!e.isNull())
							{
								if(e.tagName().toLower()=="key")
								{
									keyV = e.text();
								}
								else if(e.tagName().toLower()=="string")
								{
									valueV = e.text();
									if(!keyV.isEmpty())
									{
										dict.append(QPair<QString,QString>(keyV,valueV));
									}
									keyV = "";
								}
							}
							n = n.nextSibling();
						}
					}
				}
			}
		}
		file.close();
	}
	return dict;
}

//-------------------------------------------------------------------------------------------

bool writePList(const QString& fileName,const QVector<QPair<QString,QString> >& dictList)
{
	bool res = false;
	QStringList x;
	x << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	x << "<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">\n";
	x << "<plist version=\"0.9\">\n";
	x << "<dict>\n";
	for(int i=0;i<dictList.size();i++)
	{
		const QPair<QString,QString>& p = dictList.at(i);
		x << "\t<key>" << p.first << "</key>\n";
		x << "\t<string>" << p.second << "</string>\n";
	}
	x << "</dict>\n";
	x << "</plist>\n";
		
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

bool processPList(const QString& inPListName,const QString& outPListName)
{
	bool res = false;
	QVector<QPair<QString,QString> > dictList = dictionaryFromPList(inPListName);
	if(!dictList.isEmpty())
	{
		ProductVersionInfoSPtr pVersion = ProductVersionInfo::instance();
		QVector<QPair<QString,QString> >::iterator ppI;
		
		for(ppI=dictList.begin();ppI!=dictList.end();ppI++)
		{
			QPair<QString,QString>& p = *ppI;
			if(p.first=="CFBundleGetInfoString")
			{
				p.second = pVersion->copyright();
			}
			else if(p.first=="CFBundleShortVersionString")
			{
				p.second = pVersion->version();
			}
			else if(p.first=="CFBundleVersion")
			{
				p.second = QString::number(p.second.toInt() + pVersion->build());
			}
		}
		
		if(writePList(outPListName,dictList))
		{
			res = true;
		}
	}
	else
	{
		fprintf(stderr,"Failed to read contents of PList XML\n");
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool writeWindowsResource(const QString& fileName)
{
	bool res = false;
	ProductVersionInfoSPtr pVersion = ProductVersionInfo::instance();
	QStringList x;

	QString versionA = QString::number(pVersion->versionMajor()) + "," + QString::number(pVersion->versionMinor()) + "," + QString::number(pVersion->versionRevision()) + "," + QString::number(pVersion->build());
	QString versionB = QString::number(pVersion->versionMajor()) + "." + QString::number(pVersion->versionMinor()) + "." + QString::number(pVersion->versionRevision()) + "." + QString::number(pVersion->build());

	x << "IDI_ICON1               ICON    DISCARDABLE     \"omega.ico\"\r\n";
	x << "appicon ICON \"omega.ico\"\r\n";
	x << "\r\n";
	x << "#include <winver.h>\r\n";
	x << "\r\n";
	x << "// Get updates from this appcast feed:\r\n";
	x << "FeedURL     APPCAST     {\"http://www.blackomega.co.uk/sparkle/appcast_win.xml\"}\r\n";
	x << "\r\n";
	x << "// Version information; this, too, is used by WinSparkle\r\n";
	x << "1 VERSIONINFO\r\n";
	x << " FILEVERSION     " << versionA << "\r\n";
	x << " PRODUCTVERSION  " << versionA << "\r\n";
	x << " FILEFLAGSMASK   VS_FFI_FILEFLAGSMASK\r\n";
	x << " FILEFLAGS       0\r\n";
	x << " FILEOS          VOS_NT_WINDOWS32\r\n";
	x << " FILETYPE        VFT_APP\r\n";
	x << " FILESUBTYPE     VFT2_UNKNOWN\r\n";
	x << "BEGIN\r\n";
	x << "    BLOCK \"StringFileInfo\"\r\n";
	x << "    BEGIN\r\n";
	x << "        BLOCK \"040904B0\" // US English, Unicode\r\n";
	x << "        BEGIN\r\n";
	x << "            VALUE \"Comments\", \"Black Omega\"\r\n";
	x << "            VALUE \"CompanyName\", \"www.blackomega.co.uk\\0\"\r\n";
	x << "            VALUE \"FileDescription\", \"Black Omega audiophile music player\\0\"\r\n";
	x << "            VALUE \"FileVersion\", \"" << versionB << "\\0\"\r\n";
	x << "            VALUE \"InternalName\", \"Black Omega\\0\"\r\n";
	x << "            VALUE \"OriginalFilename\", \"Black Omega.exe\\0\"\r\n";
	x << "            VALUE \"LegalCopyright\", \"" + pVersion->copyright() + "\\0\"\r\n";
	x << "            VALUE \"ProductName\", \"Black Omega\\0\"\r\n";
	x << "            VALUE \"ProductVersion\", \"" + versionB + "\\0\"\r\n";
	x << "        END\r\n";
	x << "    END\r\n";
	x << "\r\n";
	x << "    BLOCK \"VarFileInfo\"\r\n";
	x << "    BEGIN\r\n";
	x << "        VALUE \"Translation\", 0x0409, 0x04B0\r\n";
	x << "    END\r\n";
	x << "END\r\n";

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

bool writeNullSoftVersion(const QString& fileName)
{
	bool res = false;
	ProductVersionInfoSPtr pVersion = ProductVersionInfo::instance();
	QStringList x;

	x << "!define VERSION_MAJOR " + QString::number(pVersion->versionMajor()) + "\n";
	x << "!define VERSION_MINOR " + QString::number(pVersion->versionMinor()) + "\n";
	x << "!define VERSION_REVISION " + QString::number(pVersion->versionRevision()) + "\n";
	x << "!define VERSION_BUILD " + QString::number(pVersion->build()) + "\n";

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

bool readBuildInfo(const QString& fileName,int buildNumber)
{
	bool res = false;
	if(ProductVersionInfo::instance(fileName).data()!=0)
	{
		ProductVersionInfo::instance()->build() = buildNumber;
		res = ProductVersionInfo::instance()->write(fileName);
		res = true;
	}
	return res;
}


//-------------------------------------------------------------------------------------------
} // namespace versioner
} // namespace omega
//-------------------------------------------------------------------------------------------
// Usage:
// versioner <buildInfo.xml> <build number> <input.plist> <output.plist> <player.rc> <NSIS version header>
//-------------------------------------------------------------------------------------------

int main(int argc,char **argv)
{
	int result = -1;

	if(argc >= 7)
	{
		fprintf(stdout,"Reading version information\n");
		if(omega::versioner::readBuildInfo(QString::fromUtf8(argv[1]),QString::fromUtf8(argv[2]).toInt()))
		{
			fprintf(stdout,"Create MacOS PList\n");
			if(omega::versioner::processPList(QString::fromUtf8(argv[3]),QString::fromUtf8(argv[4])))
			{
				fprintf(stdout,"Create Win32 resource file\n");
				if(omega::versioner::writeWindowsResource(QString::fromUtf8(argv[5])))
				{
					fprintf(stdout,"Create nullsoft installer version\n");
					if(omega::versioner::writeNullSoftVersion(QString::fromUtf8(argv[6])))
					{
						fprintf(stdout,"Version information setup done!\n");
						result = 0;
					}
				}
			}
		}
		else
		{
			fprintf(stderr,"Failed to load required version information\n");
		}
	}
	else
	{
		fprintf(stderr,"Unexpected number of parameters specified\n");
	}
	return result;
}
