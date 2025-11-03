#include "audioio/inc/AOASIO.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace omega;
using namespace omega::audioio;

//-------------------------------------------------------------------------------------------

TEST(ASIODSDTest,queryASIODevices)
{
    if(ASIODriverService::start())
    {
        int i;
        ASIODriverService& asioService = ASIODriverService::instance();
        bool res = false;

        for(i=0;i<asioService.noDrivers();i++)
        {
            if(asioService.open(i))
            {
                ASIODriver *driver = asioService.driverPtr(i);
                if(driver!=0)
                {
                    ASIODriverInfo dInfo;
                                    
                    if(driver->ASIOInit(&dInfo)==ASE_OK)
                    {
                        QString name = dInfo.name;
                        fprintf(stdout,"ASIO Device - %s\n",name.toUtf8().constData());
                        fprintf(stdout,"ASIO Version - %d\n",dInfo.asioVersion);
                        fprintf(stdout,"Driver Version - %d\n",dInfo.driverVersion);

                        ASIOIoFormat dsdFormat = {kASIODSDFormat};
                        if(driver->ASIOFuture(kAsioSetIoFormat,&dsdFormat) == ASE_SUCCESS)
                        {
                            fprintf(stdout,"DSD format supported =D\n");
                        }
                        else
                        {
                            fprintf(stdout,"No support for DSD format.\n");
                        }

                        fprintf(stdout,"\n");
                    }
                }
                asioService.close(i);
            }
        }
        ASIODriverService::stop();
    }
}

//-------------------------------------------------------------------------------------------
