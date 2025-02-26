#include "player/inc/SettingsFile.h"
#if defined(OMEGA_WIN32)
#include <shlobj.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

SettingsFile::SettingsFile(QWidget *parent,Qt::WindowFlags f) : SettingsBase(parent,f)
{
	ui.setupUi(this);
	
	connect(ui.m_fileTable,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(onCheckChange(QTableWidgetItem*)));
	connect(ui.m_directoryCheckbox,SIGNAL(toggled(bool)),this,SLOT(onDirectoryCheck()));
	connect(ui.m_expPlayRadio,SIGNAL(toggled(bool)),this,SLOT(onExplorerCheck()));
	connect(ui.m_expQueueRadio,SIGNAL(toggled(bool)),this,SLOT(onExplorerCheck()));

	populateFileTypeTable();

	QHeaderView *header = ui.m_fileTable->horizontalHeader();
	header->resizeSections(QHeaderView::ResizeToContents);
}

//-------------------------------------------------------------------------------------------

SettingsFile::~SettingsFile()
{}

//-------------------------------------------------------------------------------------------

void SettingsFile::onSelected(int index)
{
	onFileTab(index);
}

//-------------------------------------------------------------------------------------------

void SettingsFile::populateFileTypeTable()
{
	ui.m_fileTable->blockSignals(true);
	
	while(ui.m_fileTable->rowCount() > 0)
	{
		ui.m_fileTable->removeRow(ui.m_fileTable->rowCount() - 1);
	}
	
	addFileTypeRow(".flac","Free Lossless Audio Codec");
	addFileTypeRow(".mp3","MPEG Audio Layer-3");
	addFileTypeRow(".m4a","MPEG-4 Audio File");
	addFileTypeRow(".m4b","MPEG-4 Audiobook File");
	addFileTypeRow(".ogg","Vorbis Ogg");
	addFileTypeRow(".wav .wave","Waveform Audio File");
	addFileTypeRow(".aif .aiff *.aifc","Audio Interchange File Format");
	addFileTypeRow(".mpc .mpp .mp+","Musepack Codec");
	addFileTypeRow(".wv","WavePak Codec");
	addFileTypeRow(".caf","Apple Lossless using CAF container");
	addFileTypeRow(".dsf", "DSD Stream file");
	addFileTypeRow(".dff", "DSD Interchange File Format");
	addFileTypeRow(".pls","Generic Playlist File");
	addFileTypeRow(".m3u .m3u8","MP3 Playlist File");
	addFileTypeRow(".xspf","XML Shareable Playlist Format");
	
	ui.m_fileTable->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsFile::addFileTypeRow(const QString& typeStr,const QString& description)
{
	int nRow = ui.m_fileTable->rowCount();
	ui.m_fileTable->insertRow(nRow);
	
	QTableWidgetItem *typeItem = new QTableWidgetItem;
	typeItem->setText(typeStr);
	typeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	ui.m_fileTable->setItem(nRow,0,typeItem);
	
	QTableWidgetItem *descItem = new QTableWidgetItem;
	descItem->setText(description);
	descItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	ui.m_fileTable->setItem(nRow,1,descItem);
}

//-------------------------------------------------------------------------------------------

QStringList SettingsFile::getExtensions(const QString& text)
{
	int i,state = 0,start = 0;
	QStringList list;
	
	for(i=0;i<text.length();i++)
	{
		switch(state)
		{
			case 0:
				if(text.at(i)==QChar('.'))
				{
					start = i;
					state = 1;
				}
				break;
				
			case 1:
				if(text.at(i).isSpace())
				{
					QString x = text.mid(start,i-start).toLower();
					list.append(x);
					state = 0;
				}
				break;
		}
	}
	if(state>0)
	{
		QString x = text.mid(start).toLower();
		list.append(x);
	}
	return list;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

RegisterFileType::FileType SettingsFile::getTableFileType(const QString& ext)
{
	RegisterFileType::FileType t = RegisterFileType::e_fileTypeWAV;
	
	if(ext==".mp3")
	{
		t = RegisterFileType::e_fileTypeMP3;
	}
	else if(ext==".m4a")
	{
		t = RegisterFileType::e_fileTypeM4A;
	}
	else if(ext==".m4b")
	{
		t = RegisterFileType::e_fileTypeM4B;
	}
	else if(ext==".ogg")
	{
		t = RegisterFileType::e_fileTypeOGG;
	}
	else if(ext==".flac")
	{
		t = RegisterFileType::e_fileTypeFLAC;
	}
	else if(ext==".wav")
	{
		t = RegisterFileType::e_fileTypeWAV;
	}
	else if(ext==".wave")
	{
		t = RegisterFileType::e_fileTypeWAVE;
	}
	else if(ext==".aif")
	{
		t = RegisterFileType::e_fileTypeAIF;
	}
	else if(ext==".aiff")
	{
		t = RegisterFileType::e_fileTypeAIFF;
	}
	else if(ext==".aifc")
	{
		t = RegisterFileType::e_fileTypeAIFC;
	}
	else if(ext==".pls")
	{
		t = RegisterFileType::e_fileTypePLS;
	}
	else if(ext==".m3u")
	{
		t = RegisterFileType::e_fileTypeM3U;
	}
	else if(ext==".m3u8")
	{
		t = RegisterFileType::e_fileTypeM3U8;
	}
	else if(ext==".xspf")
	{
		t = RegisterFileType::e_fileTypeXSPF;
	}
	else if(ext==".mpc")
	{
		t = RegisterFileType::e_fileTypeMPC;
	}
	else if(ext==".mpp")
	{
		t = RegisterFileType::e_fileTypeMPP;
	}
	else if(ext==".mp+")
	{
		t = RegisterFileType::e_fileTypeMPPlus;
	}
	else if(ext==".wv")
	{
		t = RegisterFileType::e_fileTypeWV;
	}
	else if(ext==".caf")
	{
		t = RegisterFileType::e_fileTypeCAF;
	}
	else if(ext==".dsf")
	{
		t = RegisterFileType::e_fileTypeDSF;
	}
	else if(ext==".dff")
	{
		t = RegisterFileType::e_fileTypeDFF;
	}
	return t;
}

//-------------------------------------------------------------------------------------------

bool SettingsFile::getLocalUser()
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool SettingsFile::getExplorerFlag()
{
	RegisterFileType regFile(QCoreApplication::applicationFilePath());
	return regFile.isDirectoryShell();
}

//-------------------------------------------------------------------------------------------

bool SettingsFile::getShellDefault()
{
	QSettings settings;
	bool pFlag;
	
	settings.beginGroup("fileAssociation");
	if(settings.contains("playOnShell"))
	{
		pFlag = settings.value("playOnShell",QVariant(true)).toBool();
	}
	else
	{
		pFlag = false;
	}
	settings.endGroup();
	return pFlag;
}

//-------------------------------------------------------------------------------------------

void SettingsFile::onFileTab(int index)
{
	if(index==4)
	{
		int i;
		RegisterFileType regFile(QCoreApplication::applicationFilePath());
		
		ui.m_directoryCheckbox->blockSignals(true);
		ui.m_directoryCheckbox->setChecked(regFile.isDirectoryShell());
		ui.m_directoryCheckbox->blockSignals(false);

		ui.m_expQueueRadio->blockSignals(true);
		ui.m_expPlayRadio->blockSignals(true);
		if(getShellDefault())
		{
			ui.m_expPlayRadio->setChecked(true);
		}
		else
		{
			ui.m_expQueueRadio->setChecked(true);
		}
		ui.m_expQueueRadio->blockSignals(false);
		ui.m_expPlayRadio->blockSignals(false);

		ui.m_fileTable->blockSignals(true);
		for(i=0;i<ui.m_fileTable->rowCount();i++)
		{
			bool regFlag = true;
			QTableWidgetItem *item = ui.m_fileTable->item(i,0);
			QStringList extList = getExtensions(item->text());
			
			for(QStringList::const_iterator ppI=extList.begin();ppI!=extList.end() && regFlag;++ppI)
			{
				regFlag = regFile.isTypeRegistered(getTableFileType(*ppI));
			}
			item->setCheckState((regFlag) ? Qt::Checked : Qt::Unchecked);
		}
		ui.m_fileTable->blockSignals(false);
	}
}

//-------------------------------------------------------------------------------------------

void SettingsFile::onCheckChange(QTableWidgetItem *item)
{
	RegisterFileType regFile(QCoreApplication::applicationFilePath());
	bool cFlag = false;
		
	if(item->checkState()==Qt::Checked)
	{
		bool res = true;
		QStringList extList = getExtensions(item->text());
		
		for(QStringList::const_iterator ppI=extList.begin();ppI!=extList.end();++ppI)
		{
			if(!regFile.isTypeRegistered(getTableFileType(*ppI)))
			{
				if(regFile.registerFileType(getTableFileType(*ppI),getShellDefault()))
				{
					cFlag = true;
				}
				else
				{
					res = false;
				}
			}
		}
		if(!res)
		{
		
			ui.m_fileTable->blockSignals(true);
			item->setCheckState(Qt::Unchecked);
			ui.m_fileTable->blockSignals(false);
		}
	}
	else
	{
		bool res = true;
		QStringList extList = getExtensions(item->text());
		
		for(QStringList::const_iterator ppI=extList.begin();ppI!=extList.end();++ppI)
		{
			if(regFile.isTypeRegistered(getTableFileType(*ppI)))
			{
				if(regFile.unregisterFileType(getTableFileType(*ppI)))
				{
					cFlag = true;
				}
				else
				{
					res = false;
				}
			}
		}
		if(!res)
		{
			ui.m_fileTable->blockSignals(true);
			item->setCheckState(Qt::Checked);
			ui.m_fileTable->blockSignals(false);
		}
	}
	
	if(cFlag)
	{
		SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);
	}
}

//-------------------------------------------------------------------------------------------

void SettingsFile::onExplorerCheck()
{
	int i;
	bool pFlag;
	QSettings settings;
	
	pFlag = (ui.m_expQueueRadio->isChecked()) ? false : true;
	
	settings.beginGroup("fileAssociation");
	settings.setValue("playOnShell",QVariant(pFlag));
	settings.endGroup();
	
	RegisterFileType regFile(QCoreApplication::applicationFilePath());

	for(i=0;i<ui.m_fileTable->rowCount();i++)
	{
		QTableWidgetItem *item = ui.m_fileTable->item(i,0);
		QStringList extList = getExtensions(item->text());
		for(QStringList::const_iterator ppI=extList.begin();ppI!=extList.end();++ppI)
		{
			regFile.unregisterFileType(getTableFileType(*ppI));
		}
	}
	for(i=0;i<ui.m_fileTable->rowCount();i++)
	{
		QTableWidgetItem *item = ui.m_fileTable->item(i,0);
		QStringList extList = getExtensions(item->text());
		for(QStringList::const_iterator ppI=extList.begin();ppI!=extList.end();++ppI)
		{
			if(item->checkState()==Qt::Checked)
			{
				regFile.registerFileType(getTableFileType(*ppI),pFlag);
			}
		}
	}
	SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);
}

//-------------------------------------------------------------------------------------------

void SettingsFile::onDirectoryCheck()
{
	RegisterFileType regFile(QCoreApplication::applicationFilePath());
	if(ui.m_directoryCheckbox->isChecked())
	{
		regFile.addDirectoryShell();
	}
	else
	{
		regFile.delDirectoryShell();
	}
}

//-------------------------------------------------------------------------------------------
#else
//-------------------------------------------------------------------------------------------

void SettingsFile::onFileTab(int index)
{}

//-------------------------------------------------------------------------------------------

void SettingsFile::onCheckChange(QTableWidgetItem *item)
{}

//-------------------------------------------------------------------------------------------

void SettingsFile::onExplorerCheck()
{}

//-------------------------------------------------------------------------------------------

void SettingsFile::onDirectoryCheck()
{}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
