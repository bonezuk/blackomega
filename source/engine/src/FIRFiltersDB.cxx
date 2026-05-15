#include "common/inc/BinaryArrayStream.h"
#include "engine/inc/FIRFilterDB.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

double *getFIRFilterFromDB(FIRFilterType filterType, int& len)
{
    double *data = NULL;
    QString fileName;

    switch(filterType)
    {
        case e_lowPassHalf_8192:
            fileName = ":/fir/resource/fir/lowpass_half_8192.bin";
            break;
    }
    if(!fileName.isEmpty())
    {
        common::BinaryArrayStream<double> stream;
        data = stream.load(fileName, len);
    }
    return data;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

