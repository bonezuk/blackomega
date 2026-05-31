#include "common/inc/DiskOps.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/BinaryDoubleArrayFile.h"

#define BINARY_DOUBLE_ARRAY_FILE_MAGICNO 0x34fe79ac

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

BinaryDoubleArrayFile::BinaryDoubleArrayFile()
{}

//-------------------------------------------------------------------------------------------

BinaryDoubleArrayFile::~BinaryDoubleArrayFile()
{}

//-------------------------------------------------------------------------------------------

int BinaryDoubleArrayFile::readInteger(const uint8_t *mem)
{
    uint32_t x;
    x = ((static_cast<uint32_t>(mem[3]) << 24) & 0xff000000) |
        ((static_cast<uint32_t>(mem[2]) << 16) & 0x00ff0000) |
        ((static_cast<uint32_t>(mem[1]) <<  8) & 0x0000ff00) |
        ((static_cast<uint32_t>(mem[0])      ) & 0x000000ff);
    return static_cast<int>(x);
}

//-------------------------------------------------------------------------------------------

void BinaryDoubleArrayFile::writeInteger(int x, uint8_t *mem)
{
    mem[0] = static_cast<uint8_t>(x & 0x000000ff);
    mem[1] = static_cast<uint8_t>((x >> 8) & 0x000000ff);
    mem[2] = static_cast<uint8_t>((x >> 16) & 0x000000ff);
    mem[3] = static_cast<uint8_t>((x >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

double BinaryDoubleArrayFile::readDouble(const uint8_t *mem)
{
    union
    {
        uint64_t a;
        double b;
    } v;

    v.a = ((static_cast<uint64_t>(mem[7]) << 56) & 0xff00000000000000ULL) |
          ((static_cast<uint64_t>(mem[6]) << 48) & 0x00ff000000000000ULL) |
          ((static_cast<uint64_t>(mem[5]) << 40) & 0x0000ff0000000000ULL) |
          ((static_cast<uint64_t>(mem[4]) << 32) & 0x000000ff00000000ULL) |
          ((static_cast<uint64_t>(mem[3]) << 24) & 0x00000000ff000000ULL) |
          ((static_cast<uint64_t>(mem[2]) << 16) & 0x0000000000ff0000ULL) |
          ((static_cast<uint64_t>(mem[1]) <<  8) & 0x000000000000ff00ULL) |
          ((static_cast<uint64_t>(mem[0])      ) & 0x00000000000000ffULL);
    return v.b;
}

//-------------------------------------------------------------------------------------------

void BinaryDoubleArrayFile::writeDouble(double x, uint8_t *mem)
{
    union
    {
        uint64_t a;
        double b;
    } v;

    v.b = x;
    mem[0] = static_cast<uint8_t>(v.a & 0x00000000000000ffULL);
    mem[1] = static_cast<uint8_t>((v.a >>  8) & 0x00000000000000ffULL);
    mem[2] = static_cast<uint8_t>((v.a >> 16) & 0x00000000000000ffULL);
    mem[3] = static_cast<uint8_t>((v.a >> 24) & 0x00000000000000ffULL);
    mem[4] = static_cast<uint8_t>((v.a >> 32) & 0x00000000000000ffULL);
    mem[5] = static_cast<uint8_t>((v.a >> 40) & 0x00000000000000ffULL);
    mem[6] = static_cast<uint8_t>((v.a >> 48) & 0x00000000000000ffULL);
    mem[7] = static_cast<uint8_t>((v.a >> 56) & 0x00000000000000ffULL);
}

//-------------------------------------------------------------------------------------------

bool BinaryDoubleArrayFile::save(const QString& fileName, const double *data, int len)
{
    int compLen;
    bool res = false;

    if(data == NULL || len <= 0)
    {
        return false;
    }

    if(DiskOps::exist(fileName))
    {
        DiskOps::deleteFile(fileName);
    }

    compLen = 8; // magic number + size
    compLen += 8 * len;
    uint8_t *mem = new uint8_t [compLen];
    writeInteger(BINARY_DOUBLE_ARRAY_FILE_MAGICNO, mem);
    writeInteger(len, &mem[4]);
    uint8_t *in = &mem[8];
    for(int idx = 0; idx < len; idx++)
    {
        writeDouble(data[idx], in);
        in += 8;
    }

    BIOStream file(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    if(file.open(fileName))
    {
        if(file.write(mem, compLen) == compLen)
        {
            res = true;
        }
        file.close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

double *BinaryDoubleArrayFile::load(const QString& fileName, int& len)
{
    double *x = NULL;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray arr = file.readAll();
        if(arr.length() > 8)
        {
            const uint8_t *mem = reinterpret_cast<const uint8_t *>(arr.constData());
            if(readInteger(mem) == BINARY_DOUBLE_ARRAY_FILE_MAGICNO)
            {
                int compLen = readInteger(&mem[4]);
                int expectLen = 8 + (compLen * 8);
                if(expectLen == arr.length())
                {
                    mem += 8;
                    len = compLen;
                    x = new double [len];
                    for(int idx = 0; idx < len; idx++)
                    {
                        x[idx] = readDouble(mem);
                        mem += 8;
                    }
                }
            }
        }
        file.close();
    }
    return x;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

