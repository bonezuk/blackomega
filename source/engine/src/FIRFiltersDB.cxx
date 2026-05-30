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
        case e_lowPassHalf_4097:
        case e_lpHalf_DSD2:
            fileName = ":/fir/resource/fir/lowpass_half_4097.bin";
            break;
        case e_lowPassQuarter_8193:
        case e_lpQuarter_DSD4:
            fileName = ":/fir/resource/fir/lowpass_quarter_8193.bin";
            break;
        case e_lpQuarter_DSD2:
            fileName = ":/fir/resource/fir/lpQuarter_DSD2.bin";
            break;
        case e_lpHalf_DSD4:
            fileName = ":/fir/resource/fir/lpHalf_DSD4.bin";
            break;
        case e_lpHalf_DSD8:
            fileName = ":/fir/resource/fir/lpHalf_DSD8.bin";
            break;
        case e_lpQuarter_DSD8:
            fileName = ":/fir/resource/fir/lpQuarter_DSD8.bin";
            break;
        case e_lpQuarter_DSD16:
            fileName = ":/fir/resource/fir/lpQuarter_DSD16.bin";
            break;
        case e_lpQuarter_DSD32:
            fileName = ":/fir/resource/fir/lpQuarter_DSD32.bin";
            break;
        case e_lpQuarter_DSD64:
            fileName = ":/fir/resource/fir/lpQuarter_DSD64.bin";
            break;
        case e_lpQuarter_DSD128:
            fileName = ":/fir/resource/fir/lpQuarter_DSD128.bin";
            break;
        case e_lpQuarter_DSD256:
            fileName = ":/fir/resource/fir/lpQuarter_DSD256.bin";
            break;
        case e_lpQuarter_DSD512:
            fileName = ":/fir/resource/fir/lpQuarter_DSD512.bin";
            break;
        case e_lpQuarter_DSD1024:
            fileName = ":/fir/resource/fir/lpQuarter_DSD1024.bin";
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

