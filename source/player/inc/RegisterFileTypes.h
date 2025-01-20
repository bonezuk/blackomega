//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_REGISTERFILETYPES_H
#define __OMEGA_PLAYER_REGISTERFILETYPES_H
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

#include <QStringList>
#include "common/inc/RegistryScript.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class RegisterFileType
{
	public:
	
		typedef enum
		{
			e_fileTypeMP3 = 0,
			e_fileTypeM4A,
			e_fileTypeM4B,
			e_fileTypeOGG,
			e_fileTypeFLAC,
			e_fileTypeWAV,
			e_fileTypeAIF,
			e_fileTypeAIFF,
			e_fileTypePLS,
			e_fileTypeM3U,
			e_fileTypeM3U8,
			e_fileTypeXSPF,
			e_fileTypeMPC,
			e_fileTypeMPP,
			e_fileTypeMPPlus,
			e_fileTypeAPE,
			e_fileTypeWV
		} FileType;
	
	public:
		RegisterFileType(const QString& appPath);
		virtual ~RegisterFileType();
		
		bool isTypeRegistered(FileType type);
		
		bool registerFileType(FileType type,bool enDefault);
		bool unregisterFileType(FileType type);
		
		bool isDirectoryShell();
		void addDirectoryShell();
		void delDirectoryShell();

	protected:
	
		QString m_appPath;

		QString typeExt(FileType t);
		QString contentType(FileType t);
		QString precievedType(FileType t);
		QString progID();
		QString progID(FileType t);
		QString getRegisteredProgID(FileType type);
		QString getBackupProgID(FileType type);
		bool setAppPath();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
