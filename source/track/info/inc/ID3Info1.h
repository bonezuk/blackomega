//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_ID3INFO1_H
#define __OMEGA_TRACK_INFO_ID3INFO1_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT ID3Info1 : public Info
{
    public:
        ID3Info1();
        virtual ~ID3Info1();

        virtual bool read(common::BIOStream *input);

    protected:

        static const tchar *m_ID3Genres[];

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual void extractData(QString& str,tbyte *tag,tbyte *mem,tint start,tint end);
        virtual void toString(QString& str,int n);
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
