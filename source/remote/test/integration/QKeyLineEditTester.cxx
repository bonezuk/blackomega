#include "remote/test/integration/QKeyLineEditTester.h"
#include "remote/inc/KeyCodeExclusion.h"

//-------------------------------------------------------------------------------------------
// QKeyLineEditTestDialog
//-------------------------------------------------------------------------------------------

QKeyLineEditTestDialog::QKeyLineEditTestDialog(QWidget *parent,Qt::WindowFlags f) : QDialog(parent,f)
{
	QStandardItemModel *model = new QStandardItemModel(5,1,this);
	
	QTableView *tableView = new QTableView(this);
	tableView->setModel(model);

	model->setHeaderData(0,Qt::Vertical,QVariant("Play / Pause"));
	model->setHeaderData(1,Qt::Vertical,QVariant("Previous Track"));
	model->setHeaderData(2,Qt::Vertical,QVariant("Next Track"));
	model->setHeaderData(3,Qt::Vertical,QVariant("Volume Down"));
	model->setHeaderData(4,Qt::Vertical,QVariant("Volume Up"));

    model->setData(model->index(0,0,QModelIndex()),QVariant("Play / Pause"),Qt::UserRole);
    model->setData(model->index(1,0,QModelIndex()),QVariant("Previous Track"),Qt::UserRole);
    model->setData(model->index(2,0,QModelIndex()),QVariant("Next Track"),Qt::UserRole);
    model->setData(model->index(3,0,QModelIndex()),QVariant("Volume Down"),Qt::UserRole);
    model->setData(model->index(4,0,QModelIndex()),QVariant("Volume Up"),Qt::UserRole);

	// Setup test key exclusion
	KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();
	
	KeyCode keyPref(static_cast<tuint32>(Qt::Key_Comma) | static_cast<tuint32>(Qt::ControlModifier));
	pKeyExclusion->add(keyPref,"Preferences");
	KeyCode keySelectAll(static_cast<tuint32>(Qt::Key_A) | static_cast<tuint32>(Qt::ControlModifier));
	pKeyExclusion->add(keySelectAll,"Select All");
	KeyCode keyCut(static_cast<tuint32>(Qt::Key_X) | static_cast<tuint32>(Qt::ControlModifier));
	pKeyExclusion->add(keyCut,"Cut");
	KeyCode keyCopy(static_cast<tuint32>(Qt::Key_C) | static_cast<tuint32>(Qt::ControlModifier));
	pKeyExclusion->add(keyCopy,"Copy");
	KeyCode keyPaste(static_cast<tuint32>(Qt::Key_V) | static_cast<tuint32>(Qt::ControlModifier));
	pKeyExclusion->add(keyPaste,"Paste");
	KeyCode keyHelp(static_cast<tuint32>(Qt::Key_F1));
	pKeyExclusion->add(keyHelp,"Show Help");
	KeyCode keyDelete(static_cast<tuint32>(Qt::Key_Delete));
	pKeyExclusion->add(keyDelete,"Delete Tracks");

	QKeyLineEditDelegate *delegate = new QKeyLineEditDelegate(this);
	tableView->setItemDelegate(delegate);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);
	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->horizontalHeader()->setVisible(false);
	
	QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(tableView);
    vLayout->setContentsMargins(0,0,0,0);
    
    setFixedSize(500,200);
	
	QTimer::singleShot(0,this,SLOT(onStart()));
}

//-------------------------------------------------------------------------------------------

QKeyLineEditTestDialog::~QKeyLineEditTestDialog()
{}

//-------------------------------------------------------------------------------------------

void QKeyLineEditTestDialog::onStart()
{
	WinLIRCRemoteSPtr remote(new WinLIRCRemote(this));
    QObject::connect(remote.data(),SIGNAL(onStatusUpdate(QString,int)),this,SLOT(onStatusUpdate(QString,int)));
	if(remote->start())
	{
		remote->connect("192.168.5.10",8765);
		m_remote = remote;
	}
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditTestDialog::onStop()
{
	WinLIRCRemoteSPtr eRemote;
    if(m_remote.data()!=0)
	{
		m_remote->stop();
		m_remote = eRemote;
	}
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditTestDialog::closeEvent(QCloseEvent *evt)
{
	onStop();
	QDialog::closeEvent(evt);
}

//-------------------------------------------------------------------------------------------

WinLIRCRemoteSPtr QKeyLineEditTestDialog::getWinLIRCRemote()
{
	return m_remote;
}

//-------------------------------------------------------------------------------------------

void QKeyLineEditTestDialog::onStatusUpdate(QString text,int statusCode)
{
    omega::common::Log::g_Log.print("WinLIRC Status %d - %s\n",statusCode,text.toLatin1().constData());
}

//-------------------------------------------------------------------------------------------

TEST(QKeyLineEdit,testTable)
{
	int argc = 0;
	QApplication app(argc,0);
	
	QKeyLineEditTestDialog dlg;
	dlg.show();
	app.exec();
}

//-------------------------------------------------------------------------------------------
