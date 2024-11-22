#include "remote/test/integration/WinLIRCDummyServer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(network::ServiceFactory,network::Service, \
                                    WinLIRCDummyServiceFactory,WinLIRCDummyService, \
                                    "lirc_dummy_server",false)

//-------------------------------------------------------------------------------------------
// WinLIRCDummySession
//-------------------------------------------------------------------------------------------

WinLIRCDummySession::WinLIRCDummySession(const QMap<double,QPair<QString,int> >& cmdMap,WinLIRCDummyService *svr,QObject *parent) :  network::TCPConnServerSocket(svr,parent),
	m_commandMap(cmdMap)
{
	m_startTime = common::TimeStamp::reference();
}

//-------------------------------------------------------------------------------------------

WinLIRCDummySession::~WinLIRCDummySession()
{}

//-------------------------------------------------------------------------------------------

bool WinLIRCDummySession::process()
{
	bool res = !m_commandMap.isEmpty();
	
	if(!res)
	{
        WinLIRCDummyService *lircService = dynamic_cast<WinLIRCDummyService *>(service());
		lircService->complete();
	}
	
	while(res)
	{
		common::TimeStamp rT = common::TimeStamp::reference();
		
		if(rT > m_startTime)
		{
			QMap<double,QPair<QString,int> >::iterator ppI = m_commandMap.begin();
			if(ppI!=m_commandMap.end())
			{
				common::TimeStamp dT = rT - m_startTime;
				double dF = static_cast<double>(dT);
				if(dF >= ppI.key())
				{
					QString line = "0000123abcd " + QString::number(ppI.value().second,16) + " " + ppI.value().first + " dummy\n";
					QByteArray lineD = line.toLatin1();
					if(write(lineD.constData(),lineD.size()))
					{
						m_commandMap.erase(ppI);
					}
					else
					{
						res = false;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
// WinLIRCDummyService
//-------------------------------------------------------------------------------------------

WinLIRCDummyService::WinLIRCDummyService(QObject *parent) : network::TCPService(parent)
{}

//-------------------------------------------------------------------------------------------

WinLIRCDummyService::~WinLIRCDummyService()
{}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyService::run(const QString& xmlCmd)
{
	WinLIRCDummyServiceEvent *e = new WinLIRCDummyServiceEvent(WinLIRCDummyServiceEvent::e_runWinLIRCServiceEventType);
	e->xmlCommand() = xmlCmd;
    QCoreApplication::postEvent(this,e);
}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyService::wait()
{
	m_waitCondition.wait(&m_waitMutex);
	m_waitMutex.unlock();

	WinLIRCDummyServiceEvent *e = new WinLIRCDummyServiceEvent(WinLIRCDummyServiceEvent::e_shutdownWinLIRCServiceEventType);
    QCoreApplication::postEvent(this,e);

	m_waitCondition.wait(&m_waitMutex);
	m_waitMutex.unlock();
}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyService::complete()
{
	m_waitMutex.lock();
	m_waitCondition.wakeAll();
	m_waitMutex.unlock();
}

//-------------------------------------------------------------------------------------------

bool WinLIRCDummyService::event(QEvent *e)
{
	if(e!=0 && static_cast<WinLIRCDummyServiceEvent::WinLIRCServiceEventType>(e->type())>=WinLIRCDummyServiceEvent::e_runWinLIRCServiceEventType)
	{
		WinLIRCDummyServiceEvent::WinLIRCServiceEventType t = static_cast<WinLIRCDummyServiceEvent::WinLIRCServiceEventType>(e->type());
		
		switch(t)
		{
			case WinLIRCDummyServiceEvent::e_runWinLIRCServiceEventType:
				{
					WinLIRCDummyServiceEvent *lircE = reinterpret_cast<WinLIRCDummyServiceEvent *>(e);
                    processRunEvent(lircE->xmlCommand());
				}
				break;
			
			case WinLIRCDummyServiceEvent::e_shutdownWinLIRCServiceEventType:
				processShutdownEvent();
				break;
			
			default:
				return QObject::event(e);
		}
		return true;
	}
	else
	{
		return QObject::event(e);
	}
}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyService::processRunEvent(const QString& xmlCommand)
{
	int serverPort;
	QMap<double,QPair<QString,int> > cmdMap;
	WinLIRCDummyCommands cmds;
	bool res = false;
	
	serverPort = cmds.load(xmlCommand,cmdMap);
	if(serverPort!=0)
	{
		WinLIRCDummyServer *server = new WinLIRCDummyServer(cmdMap,this,this);
		if(server->open(serverPort))
		{
			m_serverSet.insert(reinterpret_cast<network::TCPServerSocket *>(server));
            res = true;
		}
	}
	
	if(!res)
	{
		complete();
	}
}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyService::processShutdownEvent()
{
	QSet<network::TCPServerSocket *>::iterator ppI;
	
	while(ppI=m_serverSet.begin(),ppI!=m_serverSet.end())
	{
		network::TCPServerSocket *server = *ppI;
		m_serverSet.erase(ppI);
		server->close();
		delete server;
	}
	complete();
}

//-------------------------------------------------------------------------------------------
// WinLIRCDummyServer
//-------------------------------------------------------------------------------------------

WinLIRCDummyServer::WinLIRCDummyServer(const QMap<double,QPair<QString,int> >& cmdMap,network::Service *svr,QObject *parent) : network::TCPServerSocket(svr,parent),
	m_commandMap(cmdMap)
{}

//-------------------------------------------------------------------------------------------

WinLIRCDummyServer::~WinLIRCDummyServer()
{}

//-------------------------------------------------------------------------------------------

network::TCPConnServerSocket *WinLIRCDummyServer::newIO()
{
    WinLIRCDummySession *io = new WinLIRCDummySession(m_commandMap,reinterpret_cast<WinLIRCDummyService *>(service()),this);
	return reinterpret_cast<network::TCPConnServerSocket *>(io);
}

//-------------------------------------------------------------------------------------------
// WinLIRCDummyCommands
//-------------------------------------------------------------------------------------------

WinLIRCDummyCommands::WinLIRCDummyCommands() : common::XMLNodeParser(),
	m_commandMap()
{}

//-------------------------------------------------------------------------------------------

WinLIRCDummyCommands::~WinLIRCDummyCommands()
{}

//-------------------------------------------------------------------------------------------

int WinLIRCDummyCommands::load(const QString& xmlText,QMap<double,QPair<QString,int> >& cmdMap)
{
	int port = 0;
	
	m_commandMap.clear();
	xmlDocPtr doc = common::XMLLibIF::instance()->xmlParseMemory(xmlText.toUtf8().constData(),xmlText.toUtf8().length());
	if(doc!=0)
	{
		xmlNodePtr root = common::XMLLibIF::instance()->xmlDocGetRootElement(doc);
		if(root!=0)
		{
            if(isRootNode(root) && isAttribute(root,"port"))
			{
                port = getAttribute(root,"port").toInt();
				parse(root);
				cmdMap = m_commandMap;
			}
		}
		common::XMLLibIF::instance()->xmlFreeDoc(doc);
	}
	return port;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCDummyCommands::isRootNode(xmlNodePtr pNode) const
{
	return isSpecifiedNode(pNode,"server");
}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyCommands::processNode(xmlNodePtr pNode)
{
	QString nName = getNameOfNode(pNode).toLower();
	
    if(nName==comparisonKey("key"))
	{
		if(isAttribute(pNode,"name") && isAttribute(pNode,"repeat") && isAttribute(pNode,"time"))
		{
			int repeat = getAttribute(pNode,"repeat").toInt();
			double time = getAttribute(pNode,"time").toDouble();
			QString button = getAttribute(pNode,"name");
			m_commandMap.insert(time,QPair<QString,int>(button,repeat));
		}
	}
}

//-------------------------------------------------------------------------------------------
// WinLIRCServiceEvent
//-------------------------------------------------------------------------------------------

WinLIRCDummyServiceEvent::WinLIRCDummyServiceEvent(WinLIRCServiceEventType t) : QEvent(static_cast<QEvent::Type>(t))
{}

//-------------------------------------------------------------------------------------------

QString& WinLIRCDummyServiceEvent::xmlCommand()
{
	return m_xmlCommand;
}

//-------------------------------------------------------------------------------------------

const QString& WinLIRCDummyServiceEvent::xmlCommand() const
{
	return m_xmlCommand;
}

//-------------------------------------------------------------------------------------------
// WinLIRCDummyApplication
//-------------------------------------------------------------------------------------------

WinLIRCDummyApplication::WinLIRCDummyApplication(QObject *parent) : QObject(parent)
{
	QTimer::singleShot(100,this,SLOT(onExecute()));
}

//-------------------------------------------------------------------------------------------

WinLIRCDummyApplication::~WinLIRCDummyApplication()
{}

//-------------------------------------------------------------------------------------------

void WinLIRCDummyApplication::onExecute()
{
	QStringList x;
    x << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	x << "<server port=\"8765\">";
	x << "<key name=\"play\" repeat=\"0\" time=\"3.0\" />";
	x << "<key name=\"play\" repeat=\"0\" time=\"4.0\" />";
	x << "<key name=\"play\" repeat=\"0\" time=\"5.0\" />";
	x << "</server>";
	QString xmlCmd = x.join("");
	
	network::Controller::ControllerSPtr networkCtrl = network::Controller::instance();
    if(networkCtrl.data()!=0)
	{
		WinLIRCDummyService *service = dynamic_cast<WinLIRCDummyService *>(networkCtrl->newService("lirc_dummy_server"));
		if(service!=0)
		{
            service->run(xmlCmd);
			service->wait();
			networkCtrl->deleteService(service);	
		}
	}
	QCoreApplication::exit();
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace omega::remote;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(WinLIRCDummyServer,ServerWith3PlayButtonPresses)
{
    int argc = 1;
    char argv[] = "test";
    common::XMLLibIFSPtr pAPI = common::XMLLibIF::instance("xml");
    QCoreApplication app(argc,(char **)&argv);
	WinLIRCDummyApplication eApp;
	app.exec();
    common::XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

