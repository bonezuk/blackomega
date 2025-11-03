//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_IOFILE_H
#define __OMEGA_ENGINE_IOFILE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "engine/inc/File.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT IOFile : public File
{
    public:
        IOFile(common::BIOStream *f);
        virtual ~IOFile();

        virtual bool open(const tchar *fileName);
        virtual bool open(const QString& fileName);
        virtual void close();

        virtual tint read(tubyte *mem,tint len);
        virtual tint read(tchar *mem,tint len);

        virtual bool seek(tint offset,Position position);

        virtual tint length() const;

        virtual tint offset() const;

    protected:

        common::BIOStream *m_file;

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

