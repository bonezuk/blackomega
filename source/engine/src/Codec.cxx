#include "engine/inc/Codec.h"
#include "engine/inc/OmegaDCT.h"
#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"

#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(CodecFactory,Codec)

//-------------------------------------------------------------------------------------------
// CodecInitialize
//-------------------------------------------------------------------------------------------

CodecInitialize *CodecInitialize::m_instance = 0;

//-------------------------------------------------------------------------------------------

CodecInitialize::CodecInitialize()
{
	Bitstream::start();
}

//-------------------------------------------------------------------------------------------

CodecInitialize::~CodecInitialize()
{
	try
	{
		OmegaDCT::stop();
		Bitstream::stop();
	}
	catch(...) {}
}

//-------------------------------------------------------------------------------------------

void CodecInitialize::start()
{
	if(m_instance==0)
	{
		m_instance = new CodecInitialize();
	}
}

//-------------------------------------------------------------------------------------------

void CodecInitialize::end()
{
	if(m_instance!=0)
	{
		delete m_instance;
		m_instance = 0;
	}
}

//-------------------------------------------------------------------------------------------
// Codec
//-------------------------------------------------------------------------------------------

Qt::HANDLE Codec::m_audioThreadID = 0;

//-------------------------------------------------------------------------------------------

Codec::Codec(CodecType type,QObject *parent) : QObject(parent),
	m_type(type),
	m_initFlag(false),
	m_bufferTimeLength(),
	m_noOutputChannels(2)
{}

//-------------------------------------------------------------------------------------------

Codec::~Codec()
{}

//-------------------------------------------------------------------------------------------

void Codec::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "Codec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

Codec::CodecType Codec::type() const
{
	return m_type;
}

//-------------------------------------------------------------------------------------------

QString Codec::getFileExtension(const QString& name)
{
	QString pro;
	return getFileExtension(name,pro);
}

//-------------------------------------------------------------------------------------------

QString Codec::getFileExtension(const QString& name,QString& pro)
{
	int i;
	QString ext;

	pro = "";
	for(i=0;i<name.length() && pro.isEmpty();++i)
	{
		if(name.at(i)==QChar(':'))
		{
			pro = name.left(i).toLower();
		}
	}
	
	for(i=name.length()-2;i>=0 && ext.isEmpty();--i)
	{
		if(name.at(i)==QChar('.'))
		{
			ext = name.mid(i + 1).toLower();
		}
	}
	return ext;
}

//-------------------------------------------------------------------------------------------

Codec *Codec::get(const QString& name)
{
	Codec *c = 0;
	QString ext,pro;
	
	ext = getFileExtension(name,pro);
	
	if(pro=="rtp" && ext=="ogg")
	{
		ext += ":" + pro;
		c = CodecFactory::createUnmanaged(ext);
	}
	else if(ext=="wav" || ext=="mp3" || ext=="ogg" || ext=="m4a" || ext=="m4b" || ext=="flac" || ext=="caf" || ext=="aif" || ext=="aiff" || ext=="mpc" || ext=="mp+" || ext=="mpp" || ext=="wv" || ext=="tone")
	{
		if(ext=="m4b")
		{
			ext = "m4a";
		}
		else if(ext=="aif")
		{
			ext = "aiff";
		}
		else if(ext=="mp+" || ext=="mpp")
		{
			ext = "mpc";
		}
		c = CodecFactory::createUnmanaged(ext);
	}

	if(c!=0)
	{
		if(!c->open(name))
		{
			delete c;
			c = 0;
		}
	}
	return c;
}

//-------------------------------------------------------------------------------------------

void Codec::buffer(const common::TimeStamp& bLen)
{
	m_bufferTimeLength = bLen;
}

//-------------------------------------------------------------------------------------------

void Codec::resync()
{}

//-------------------------------------------------------------------------------------------

bool Codec::isInitialized() const
{
	return m_initFlag;
}

//-------------------------------------------------------------------------------------------

bool Codec::loadFileToMemory() const
{
	bool flag;
	if(QThread::currentThreadId()==m_audioThreadID)
	{
		QSettings settings;
		settings.beginGroup("audio");
		if(settings.contains("loadFileToMemory"))
		{
			flag = settings.value("loadFileToMemory").toBool();
		}
		else
		{
			flag = false;
		}
		settings.endGroup();
	}
	else
	{
		flag = false;
	}
	return flag;
}

//-------------------------------------------------------------------------------------------

void Codec::setAudioThreadID()
{
	m_audioThreadID = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

tint Codec::noOutputChannels() const
{
	return m_noOutputChannels;
}

//-------------------------------------------------------------------------------------------

void Codec::setNoOutputChannels(tint noChs)
{
	if(noChs<=0)
	{
		noChs = 1;
	}
	else if(noChs>8)
	{
		noChs = 8;
	}
	m_noOutputChannels = noChs;
}

//-------------------------------------------------------------------------------------------

CodecDataType Codec::dataTypesSupported() const
{
	return e_SampleFloat;
}

//-------------------------------------------------------------------------------------------

bool Codec::setDataTypeFormat(CodecDataType type)
{
	return (!(type & ~e_SampleFloat) && type == e_SampleFloat) ? true : false;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
