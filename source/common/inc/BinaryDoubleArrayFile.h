//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BINARYDOUBLEARRAYFILE_H
#define __OMEGA_COMMON_BINARYDOUBLEARRAYFILE_H
//-------------------------------------------------------------------------------------------

#include <QFile>

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT BinaryDoubleArrayFile
{
    public:
        BinaryDoubleArrayFile();
        virtual ~BinaryDoubleArrayFile();

        bool save(const QString& fileName, const double *data, int len);
        double *load(const QString& fileName, int& len);

    private:
        int readInteger(const uint8_t *mem);
        void writeInteger(int x, uint8_t *mem);
        double readDouble(const uint8_t *mem);
        void writeDouble(double x, uint8_t *mem);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
