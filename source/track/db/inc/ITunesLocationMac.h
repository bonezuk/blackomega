//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_ITUNESLOCATIONMAC_H
#define __OMEGA_TRACK_DB_ITUNESLOCATIONMAC_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/ITunesLocation.h"

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT ITunesLocationMac
{
    public:
        static QString IVLocationHomeDirectory();
        static QStringList lastITunesDirectory();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

