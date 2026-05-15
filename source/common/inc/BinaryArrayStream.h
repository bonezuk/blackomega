//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BINARYARRAYSTREAM_H
#define __OMEGA_COMMON_BINARYARRAYSTREAM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Mutex.h"

#include <QDataStream>
#include <QFile>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

#define BINNARY_ARRAY_STREAM_MAGICNO 0x34fe89ab

template <typename X> class BinaryArrayStream
{
    public:
        BinaryArrayStream();
        virtual ~BinaryArrayStream();

        virtual bool save(const QString& fileName, const X *data, int len);

        virtual X *load(const QString& fileName, int& len);

    private:
};

//-------------------------------------------------------------------------------------------

template <typename X> BinaryArrayStream<X>::BinaryArrayStream()
{}

//-------------------------------------------------------------------------------------------

template <typename X> BinaryArrayStream<X>::~BinaryArrayStream()
{}

//-------------------------------------------------------------------------------------------

template <typename X> bool BinaryArrayStream<X>::save(const QString& fileName, const X *data, int len)
{
    bool res = false;
    QFile file(fileName);

    if(len > 0 && file.open(QIODeviceBase::ReadWrite))
    {
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_6_0);
        out << (tuint32)BINNARY_ARRAY_STREAM_MAGICNO;
        out << (tint32)len;
        for(int i = 0; i < len; i++)
        {
            out << data[i];
        }
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

template <typename X> X *BinaryArrayStream<X>::load(const QString& fileName, int& len)
{
    X *data = NULL;
    QFile file(fileName);

    len = 0;
    if(file.open(QIODeviceBase::ReadOnly))
    {
        QDataStream in(&file);
        tuint32 magicNo, dLen;

        in >> magicNo;
        if(magicNo == BINNARY_ARRAY_STREAM_MAGICNO)
        {
            tuint32 idx;

            in >> dLen;
            data = new X [dLen];
            for(idx = 0; !in.atEnd() && idx < dLen; idx++)
            {
                in >> data[idx];
            }
            if(idx == dLen)
            {
                len = static_cast<int>(dLen);
            }
            else
            {
                delete [] data;
                data = NULL;
            }
        }
    }
    return data;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
