#include "track/info/inc/Info.h"
#include "track/info/inc/ID3Info1.h"
#include "track/info/inc/ID3Info2.h"
#include "track/info/inc/VorbisInfo.h"
#include "track/info/inc/FileInfo.h"
#include "track/info/inc/AtomInfo.h"
#include "track/info/inc/FLACInfo.h"
#include "track/info/inc/AIFFInfo.h"
#include "track/info/inc/APEInfo.h"
#include "track/info/inc/WavInfo.h"
#include "engine/dsdomega/inc/DSFFileReader.h"
#include "engine/inc/Codec.h"
#include "common/inc/CommonFunctions.h"

#if defined(OMEGA_POSIX)
#include <dirent.h>
#endif

#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// Info::ChildInfo
//-------------------------------------------------------------------------------------------

Info::ChildInfo::ChildInfo() : m_name(),
	m_startTime(),
	m_lengthTime()
{}

//-------------------------------------------------------------------------------------------

Info::ChildInfo::ChildInfo(const ChildInfo& rhs) : m_name()
{
	copy(rhs);
}

//-------------------------------------------------------------------------------------------

Info::ChildInfo::~ChildInfo()
{}

//-------------------------------------------------------------------------------------------

const Info::ChildInfo& Info::ChildInfo::operator = (const ChildInfo& rhs)
{
	if(this!=&rhs)
	{
		copy(rhs);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void Info::ChildInfo::copy(const ChildInfo& rhs)
{
	m_name = rhs.m_name;
	m_startTime = rhs.m_startTime;
	m_lengthTime = rhs.m_lengthTime;
}

//-------------------------------------------------------------------------------------------
// Info
//-------------------------------------------------------------------------------------------

Info::ChildInfo Info::m_nullChild;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(InfoFactory,Info)

//-------------------------------------------------------------------------------------------

Info::Info(QObject *parent) : QObject(parent),
	m_type(e_InfoUnknown),
	m_Artist(),
	m_Title(),
	m_Album(),
	m_Year(),
	m_Comment(),
	m_Genre(),
	m_Track(),
	m_Disc("1"),
	m_Composer(),
	m_OriginalArtist(),
	m_Copyright(),
	m_Encoder(),
	m_fileName(),
	m_length(),
	m_bitrate(0),
	m_frequency(0),
	m_noChannels(0),
	m_dirImageFormat(e_imageUnknown),
	m_dirImageArray(0),
	m_hash(0)
{}

//-------------------------------------------------------------------------------------------

Info::~Info()
{
	if(m_dirImageArray!=0)
	{
		delete m_dirImageArray;
		m_dirImageArray = 0;
	}
}

//-------------------------------------------------------------------------------------------

void Info::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "Info::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void Info::blank()
{
	m_Artist = "";
	m_Title = "";
	m_Album = "";
	m_Year = "";
	m_Comment = "";
	m_Genre = "";
	m_Track = "";
	m_Disc = "1";
	m_Composer = "";
	m_OriginalArtist = "";
	m_Copyright = "";
	m_Encoder = "";
	m_fileName = "";
	m_length = 0;
	m_hash = 0;
}

//-------------------------------------------------------------------------------------------

bool Info::isSupported(const QString& name)
{
	tint i;
	QString ext;
	bool res;
		
	for(i=name.length()-2;i>=0 && ext.isEmpty();--i)
	{
		if(name.at(i)==QChar('.'))
		{
			ext = name.mid(i + 1).toLower();
		}
	}
	if(engine::Codec::isSupported(name))
	{
		common::BIOStream sFile(common::e_BIOStream_FileRead);
		res = isAppleFinderFile(name,&sFile);
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool Info::isAppleFinderFile(const QString& name,common::BIOStream *reader)
{
	bool res = true;
	
	if(reader!=0)
	{
		tint i;

		for(i=name.length()-1;i>=0;i--)
		{
			if(name.at(i)==QChar('/') || name.at(i)==QChar('\\'))
			{
				break;
			}
		}
		i++;

		if(name.mid(i,2)=="._")
		{
			if(reader->open(name))
			{
				tubyte mem[4];
				if(reader->read(mem,4)==4)
				{
					if(mem[0]==0x00 && mem[1]==0x05 && mem[2]==0x16 && mem[3]==0x00) // AppleSingle Finder file magic number
					{
						res = false;
					}
					else if(mem[0]==0x00 && mem[1]==0x05 && mem[2]==0x16 && mem[3]==0x07) // AppleDouble Finder file magic number
					{
						res = false;
					}
				}
				else
				{
					res = false;
				}
				reader->close();
			}
			else
			{
				res = false;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool Info::get(IDTagInformation info,common::BString& str) const
{
	QString tmp;
	bool res;
	
	res = get(info,tmp);
	str = tmp.toUtf8().constData();
	return res;
}

//-------------------------------------------------------------------------------------------

bool Info::get(IDTagInformation info,QString& str) const
{
	bool res = true;
	
	switch(info)
	{
		case e_TagInfo_Artist:
			str = m_Artist;
			break;
			
		case e_TagInfo_Title:
			str = m_Title;
			break;
			
		case e_TagInfo_Album:
			str = m_Album;
			break;
			
		case e_TagInfo_Year:
			str = m_Year;
			break;
			
		case e_TagInfo_Comment:
			str = m_Comment;
			break;
			
		case e_TagInfo_Genre:
			str = m_Genre;
			break;
			
		case e_TagInfo_Track:
			str = m_Track;
			break;
			
		case e_TagInfo_Composer:
			str = m_Composer;
			break;
			
		case e_TagInfo_OriginalArtist:
			str = m_OriginalArtist;
			break;
			
		case e_TagInfo_Copyright:
			str = m_Copyright;
			break;
			
		case e_TagInfo_Encoder:
			str = m_Encoder;
			break;
			
		case e_TagInfo_FileName:
			if(!m_fileName.isEmpty())
			{
				QFileInfo fInfo(m_fileName);
				str = fInfo.baseName();
			}
			else
			{
				str = "File";
			}
			break;
			
		case e_TagInfo_DirectoryName:
			if(!m_fileName.isEmpty())
			{
				QFileInfo fInfo(m_fileName);
				str = fInfo.dir().dirName();
			}
			else
			{
				str = "Directory";
			}
			break;

		case e_TagInfo_Disc:
			str = m_Disc;
			break;
		
		default:
			str = "";
			res = false;
			break;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool Info::isImage() const
{
	return (m_dirImageArray!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *Info::getImageData(ImageFormat& format) const
{
	format = m_dirImageFormat;
	return m_dirImageArray;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *Info::getImageData(IDTagImageType type,ImageFormat& format) const
{
	ImageInfoArray *pArray;

	if(type==e_TagImage_CoverFront)
	{
		format = m_dirImageFormat;
		pArray = m_dirImageArray;
	}
	else
	{
		format = e_imageUnknown;
		pArray = 0;
	}
	return pArray;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<Info> Info::readInfo(common::BIOStream *input)
{
    QSharedPointer<Info> tag;
	
	if(input!=0)
	{
		QString ext,name(input->name());
		
		ext = engine::Codec::getFileExtension(name).toLower();
		
		if(ext=="mp3")
		{
			if(seekType2(input))
			{
                QSharedPointer<Info> tagN(new ID3Info2());
		
				if(tagN->read(input))
				{
					tag = tagN;
				}
			}
			
            if(tag.data()==0 && seekType1(input))
			{
                QSharedPointer<Info> tagN(new ID3Info1());
		
				if(tagN->read(input))
				{
					tag = tagN;
				}
			}			
		}
		else if(ext=="ogg")
		{
			if(seekType2(input))
			{
                QSharedPointer<Info> tagN(new ID3Info2());
				
				if(tagN->read(input))
				{
					tag = tagN;
				}
			}
			
            if(tag.data()==0)
			{
                QSharedPointer<Info> tagN(new VorbisInfo());
			
				if(tagN->read(input))
				{
					tag = tagN;
				}
			}
		}
		else if(ext=="m4a" || ext=="m4b")
		{
            QSharedPointer<Info> tagN(new AtomInfo());

			if(tagN->read(input))
			{
				tag = tagN;
			}
		}
		else if(ext=="flac")
		{
			if(seekType2(input))
			{
                QSharedPointer<Info> tagN(new ID3Info2());
		
				if(tagN->read(input))
				{
					tag = tagN;
				}
			}
            if(tag.data()==0)
			{
                QSharedPointer<Info> tagN(new FLACInfo());

				if(tagN->read(input))
				{
					tag = tagN;
				}
			}
		}
		else if(ext=="aif" || ext=="aiff")
		{
            QSharedPointer<Info> tagN(new AIFFInfo());

			if(tagN->read(input))
			{
				tag = tagN;
			}
		}
		else if(ext=="wav")
		{
			QSharedPointer<Info> tagN(new WavInfo());

			if(tagN->read(input))
			{
				tag = tagN;
			}
		}
        else if(ext=="mpc" || ext=="mp+" || ext=="mpp" || ext=="ape" || ext=="wv")
		{
			QSharedPointer<Info> tagN(new APEInfo());
			
			if(tagN->read(input))
			{
				tag = tagN;
			}
			else if(seekType2(input))
			{
				QSharedPointer<Info> tagID(new ID3Info2());
		
				if(tagID->read(input))
				{
					tag = tagID;
				}
			}
		}
		else if(ext == "dsf")
		{
			engine::dsd::DSFFileReader dsf(input);
			if(dsf.parse() && dsf.isMetadata())
			{
				QSharedPointer<Info> tagID(new ID3Info2());
				
				if(input->seek64(dsf.metaOffset(), common::e_Seek_Start) && tagID->read(input))
				{
					tag = tagID;
				}
			}
		}
		
        if(tag.data()==0)
		{
            QSharedPointer<Info> tagN(new FileInfo());
			
			if(tagN->read(input))
			{
				tag = tagN;
			}
		}
		
        if(tag.data()!=0)
		{
			engine::Codec *codec = engine::Codec::get(input->name());
			
			if(codec!=0)
			{
				tag->setLength(codec->length());
				tag->setBitrate(codec->bitrate());
				tag->setFrequency(codec->frequency());
				tag->setNoChannels(codec->noChannels());
				delete codec;
			}
			tag->setFilename(input->name());
			tag->calcHashID(input);
		}
	}
	return tag;
}

//-------------------------------------------------------------------------------------------

bool Info::seekType1(common::BIOStream *input)
{
	tbyte tag[3];

	if(!input->seek(-128,common::e_Seek_End))
	{
		return false;
	}
	if(input->read(tag,3)!=3)
	{
		return false;
	}
	if(!(tag[0]=='T' && tag[1]=='A' && tag[2]=='G'))
	{
		input->seek(0,common::e_Seek_Start);
		return false;
	}
	if(!input->seek(-128,common::e_Seek_End))
	{
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool Info::seekType2(common::BIOStream *input)
{
	tbyte tag[4];
	
	if(!input->seek(0,common::e_Seek_Start))
	{
		return false;
	}
	if(input->read(tag,4)!=4)
	{
		return false;
	}
	if(!(tag[0]=='I' && tag[1]=='D' && tag[2]=='3'))
	{
		input->seek(0,common::e_Seek_Start);
		return false;		
	}
	if(!input->seek(0,common::e_Seek_Start))
	{
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------

bool Info::read(common::BIOStream *input)
{
	return false;
}

//-------------------------------------------------------------------------------------------

void Info::setFilename(const QString& name)
{
	m_fileName = QDir::toNativeSeparators(name);
}

//-------------------------------------------------------------------------------------------

QString Info::getName() const
{
	QString name;
	bool contentFlag = false;

	name = getName("%L - %T",contentFlag,false);
	if(!contentFlag)
	{
		name = getName("%O");
	}
	return name;
}

//-------------------------------------------------------------------------------------------

QString Info::getName(const QString& scheme,bool sortFlag) const
{
	bool contentFlag;
	return getName(scheme,contentFlag,sortFlag);
}

//-------------------------------------------------------------------------------------------

QString Info::getName(const QString& scheme,bool& contentFlag,bool sortFlag) const {
	tint i, state = 0, size = scheme.size();
	const QChar *x = scheme.constData();
	QString n;
	contentFlag = false;

	for (i = 0; i < size; ++i) {
		switch (state) {
			case 0:
				if (x[i] == QChar('%')) {
					state = 1;
				}
				else {
					n += x[i];
				}
				break;

			case 1:
				if (x[i] == QChar('A') || x[i] == QChar('a')) {
					if (!m_Artist.isEmpty()) {
						contentFlag = true;
					}
					n += m_Artist.trimmed();
				}
				else if (x[i] == QChar('T') || x[i] == QChar('t')) {
					if (!m_Title.isEmpty()) {
						contentFlag = true;
					}
					n += m_Title.trimmed();
				}
				else if (x[i] == QChar('L') || x[i] == QChar('l')) {
					if (!m_Album.isEmpty()) {
						contentFlag = true;
					}
					n += m_Album.trimmed();
				}
				else if (x[i] == QChar('N') || x[i] == QChar('n')) {
					if (!m_Track.isEmpty()) {
						contentFlag = true;
					}
					if (sortFlag) {
						n += common::BString::Int(m_Track.toInt(), 5, true).GetString();
					}
					else {
						n += m_Track.trimmed();
					}
				}
				else if (x[i] == QChar('I') || x[i] == QChar('i')) {
					if (!m_Genre.isEmpty()) {
						contentFlag = true;
					}
					n += m_Genre.trimmed();
				}
				else if (x[i] == QChar('Y') || x[i] == QChar('y')) {
					if (!m_Year.isEmpty()) {
						contentFlag = true;
					}
					n += m_Year.trimmed();
				}
				else if (x[i] == QChar('O') || x[i] == QChar('o')) {
					QString m;
					contentFlag = true;
					get(e_TagInfo_FileName, m);
					n += m;
				}
				else if (x[i] == QChar('D') || x[i] == QChar('d')) {
					QString m;
					contentFlag = true;
					get(e_TagInfo_DirectoryName, m);
					n += m;
				}
				else if (x[i] == QChar('Z') || x[i] == QChar('z')) {
					if (sortFlag) {
						n += common::BString::Int(m_Disc.toInt(), 5, true).GetString();
					}
					else {
						n += m_Disc.trimmed();
					}
				}
				else {
					n += x[i];
				}
				state = 0;
				break;

			default:
				break;
		}
	}
	return n;
}

//-------------------------------------------------------------------------------------------

const QString& Info::getFilename() const
{
	return m_fileName;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& Info::length()
{
	return m_length;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& Info::length() const
{
	return m_length;
}

//-------------------------------------------------------------------------------------------

void Info::setLength(const common::TimeStamp& l)
{
	m_length = l;
}

//-------------------------------------------------------------------------------------------

bool Info::isChildren() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

tint Info::noChildren() const
{
	return 0;
}

//-------------------------------------------------------------------------------------------

const Info::ChildInfo& Info::child(tint idx) const
{
	return m_nullChild;
}

//-------------------------------------------------------------------------------------------

QString Info::directory() const
{
	tint i;
	QString dName;
	
	for(i=m_fileName.length()-1;i>=0;i--)
	{
		if(m_fileName.at(i)==QChar('/') || m_fileName.at(i)==QChar('\\'))
		{
			dName = m_fileName.mid(0,i+1);
			break;
		}
	}
	return dName;
}

//-------------------------------------------------------------------------------------------

QString Info::filename() const
{
	tint i;
	QString fName;
	
	for(i=m_fileName.length()-1;i>=0;i--)
	{
		if(m_fileName.at(i)==QChar('/') || m_fileName.at(i)==QChar('\\'))
		{
			fName = m_fileName.mid(i+1);
			break;
		}
	}
	return fName;
}

//-------------------------------------------------------------------------------------------

QString Info::albumName() const
{
	QString name;

	if(m_Album.isEmpty())
	{
		tint i;
		
		if(noChildren() > 0)
		{
			name = filename();
			for(i=name.length()-1;i>=0;i--)
			{
				if(name.at(i)==QChar('.'))
				{
					name = name.mid(0,i);
					break;
				}
			}
		}
		else
		{
			name = directory();
			for(i=name.length()-2;i>=0;i--)
			{
				if(name.at(i)==QChar('/') || name.at(i)==QChar('\\'))
				{
					name = name.mid(i+1,name.length() - (i+2));
					break;
				}
			}
		}

		if(name.isEmpty())
		{
			name = "Music";
		}
	}
	else
	{
		name = m_Album.trimmed();
	}
	return name;
}

//-------------------------------------------------------------------------------------------

QString Info::directoryGroup() const
{
	return directory();
}

//-------------------------------------------------------------------------------------------

bool Info::playPreGap()
{
	QSettings settings;
	bool pFlag;
	
	settings.beginGroup("playback");
	if(settings.contains("pregap"))
	{
		pFlag = settings.value("pregap",QVariant(false)).toBool();
	}
	else
	{
		pFlag = false;
	}
	settings.endGroup();
	return pFlag;
}

//-------------------------------------------------------------------------------------------

void Info::setPlayPreGap(bool flag)
{
	QSettings settings;
	settings.beginGroup("playback");
	settings.setValue("pregap",QVariant(flag));
	settings.endGroup();
}

//-------------------------------------------------------------------------------------------

tint Info::bitrate() const
{
	return m_bitrate;
}

//-------------------------------------------------------------------------------------------

tint Info::noChannels() const
{
	return m_noChannels;
}

//-------------------------------------------------------------------------------------------

tint Info::frequency() const
{
	return m_frequency;
}

//-------------------------------------------------------------------------------------------

void Info::setBitrate(tint v)
{
	m_bitrate = v;
}

//-------------------------------------------------------------------------------------------

void Info::setFrequency(tint v)
{
	m_frequency = v;
}

//-------------------------------------------------------------------------------------------

void Info::setNoChannels(tint v)
{
	m_noChannels = v;
}

//-------------------------------------------------------------------------------------------

tuint64 Info::calculateELFHash(common::BIOStream *input)
{
	const int c_numberOfSamples = 8;
	tuint64 hash = 0;
	tint64 totalSize = input->size64();
	
	if(totalSize > 0)
	{
		tint i, len;
		tint64 samplePosition;
		tubyte mem[128];
		
		for(i = 0; i < c_numberOfSamples; i++)
		{
			samplePosition = ((totalSize * i) / c_numberOfSamples);			
			len = static_cast<tint>(((samplePosition + 128) < totalSize) ? 128 : (totalSize - samplePosition));
			
			if(input->seek64(samplePosition, common::e_Seek_Start))
			{
				if(input->read(mem, len) == len)
				{
					hash = common::elfHash64(mem, len, hash);
				}
			}
		}
	}
	return hash;
}

//-------------------------------------------------------------------------------------------

tuint64 Info::hashID() const
{
	return m_hash;
}

//-------------------------------------------------------------------------------------------

void Info::calcHashID(common::BIOStream *input)
{
	m_hash = calculateELFHash(input);
}

//-------------------------------------------------------------------------------------------
} // namespace idtag
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
