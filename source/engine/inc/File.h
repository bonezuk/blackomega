//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FILE_H
#define __OMEGA_ENGINE_FILE_H
//-------------------------------------------------------------------------------------------

#include <QString>

#include "common/inc/BIOStream.h"
#include "common/inc/BString.h"
#include "common/inc/Log.h"
#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT File
{
    public:
        typedef enum
        {
            e_startPosition = 0,
            e_currentPosition,
            e_endPosition
        } Position;
        
    public:
        File();
        virtual ~File();
        
        virtual bool open(const tchar *fileName);
        virtual bool open(const QString& fileName);
        virtual void close();
        
        virtual tint read(tchar *mem,tint len);
        virtual tint read(tubyte *mem,tint len);
        
        virtual bool seek(tint offset,Position position);
        
        virtual tint length() const;
        
        virtual tint offset() const;
        
    protected:
    
        common::file_type m_file;
        QString m_name;
        tint m_length;
        tint m_offset;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE1,const tchar *strE2) const;
        virtual void printError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint errNo) const;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

