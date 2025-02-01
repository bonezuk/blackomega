#include <QFileInfo>
#include <QDir>

#include "test/inc/UnitTestEnviroment.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace test
{
//-------------------------------------------------------------------------------------------

UnitTestEnviroment *UnitTestEnviroment::m_instance = 0;

//-------------------------------------------------------------------------------------------

UnitTestEnviroment::UnitTestEnviroment() : m_sourcePath()
{}

//-------------------------------------------------------------------------------------------

UnitTestEnviroment::~UnitTestEnviroment()
{
	m_instance = 0;
}

//-------------------------------------------------------------------------------------------

void UnitTestEnviroment::printError(const char *strR, const QString& err) const
{
	fprintf(stderr, "%s - %s\n", strR, err.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

UnitTestEnviroment::UnitTestEnviroment *UnitTestEnviroment::nstance(const QString& unitExePath)
{
	if(m_instance == 0)
	{
		m_instance = new UnitTestEnviroment();
		if(!m_instance->init(unitExePath))
		{
			delete m_instance;
			m_instance = 0;
		}
	}
	return m_instance;
}

//-------------------------------------------------------------------------------------------

UnitTestEnviroment::UnitTestEnviroment *UnitTestEnviroment::instance()
{
	return m_instance;
}

//-------------------------------------------------------------------------------------------

bool UnitTestEnviroment::init(const QString& unitExePath)
{
	bool res = false;
	QFileInfo unitExe(unitExePath);
	
	if(unitExe.isExecutable())
	{
		QDir d = unitExe.dir();
		if(d.cdUp() && d.cdUp() && d.cdUp() && d.cd("source"))
		{
			m_sourcePath = d.absolutePath();
			res = true;
		}
		else
		{
			QString err = "Failed to find source path";
			printError("init", err);
		}
	}
	else
	{
		QString err = QString("Given executable path '%1' doesn't exist").arg(unitExePath);
		printError("init", err);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

QString UnitTestEnviroment::testFileName(const QString& name)
{
	QString fullName = common::DiskOps::mergeName(m_sourcePath, name);
	if(!common::DiskOps::exist(fullName))
	{
		fullName = "";
	}
	return fullName;
}

//-------------------------------------------------------------------------------------------
} // namespace test
} // namespace omega
//-------------------------------------------------------------------------------------------
