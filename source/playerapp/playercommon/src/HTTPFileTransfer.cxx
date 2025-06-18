#include "playerapp/playercommon/inc/HTTPFileTransfer.h"
#include "common/inc/BIOStream.h"
#include "common/inc/DiskOps.h"

#include <QFileInfo>
#include <QDir>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

HTTPFileTransfer::HTTPFileTransfer(const QString& rootDir) : m_rootDir(rootDir)
{}

//-------------------------------------------------------------------------------------------

HTTPFileTransfer::~HTTPFileTransfer()
{}

//-------------------------------------------------------------------------------------------

void HTTPFileTransfer::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "HTTPFileTransfer::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool HTTPFileTransfer::isValid(network::http::HTTPReceive *recieve) const
{
    if(recieve == 0)
    {
        printError("isValid", "No HTTP request given from server");
        return false;
    }
    if(recieve->header().type() != network::http::Unit::e_Request)
    {
        printError("isValid", "HTTP header is not a request");
        return false;
    }
    if(recieve->header().request() != network::http::Unit::e_Get)
    {
        printError("isValid", "HTTP header is not a GET request");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void HTTPFileTransfer::postErrorResponse(int code, network::http::HTTPReceive *recieve)
{
    network::http::Unit hdr;
    
    hdr.response(code);
    hdr.add("Connection","close");
    recieve->connection()->postResponse(hdr);
}

//-------------------------------------------------------------------------------------------

void HTTPFileTransfer::transferFile(const QString& fileName, network::http::HTTPReceive *recieve, network::http::Unit& hdr)
{
    const int c_blockLength = 128000;
    // Timeout period where transfer is canceled if no progress occurs.
    const tfloat64 c_timeout = 5.0 * 60.0;
    common::BIOStream file(common::e_BIOStream_FileRead);
    
    if(file.open(fileName))
    {
        int amount, loopCount;
        bool timeoutFlag = false;
        common::TimeStamp startTime = common::TimeStamp::now();
    
        hdr.add("Content-Length", QString::number(file.size()));
        recieve->connection()->postResponse(hdr);
        
        for(amount = 0, loopCount = 1; amount < file.size() && !timeoutFlag; loopCount++)
        {
            if(recieve->connection()->getMessageQueueSize() < 5)
            {
                int len = ((file.size() - amount) < c_blockLength) ? (file.size() - amount) : c_blockLength;
                network::NetArraySPtr dataArray(new network::NetArray);
                
                dataArray->SetSize(len);
                if(file.read(dataArray->GetData(), len) == len)
                {
                    recieve->connection()->postBody(dataArray);
                    amount += len;
                }
                else
                {
                    QString err = QString("Error reading from file '%1' at pos=%2 for %3 bytes")
                        .arg(fileName).arg(amount).arg(len);
                    printError("transferFile", err.toUtf8().constData());
                    break;
                }
            }
            else
            {
                QThread::usleep(1000);
            }
            // Check for timeout condition every second.
            if(loopCount % 1000 && (startTime + c_timeout) < common::TimeStamp::now())
            {
                timeoutFlag = true;
            }
        }
        
        file.close();
    }
    else
    {
        QString err = QString("Error opening file '%1' for transfer").arg(fileName);
        printError("transferFile", err.toUtf8().constData());
        postErrorResponse(404, recieve);
    }
}

//-------------------------------------------------------------------------------------------

void HTTPFileTransfer::process(network::http::HTTPReceive *recieve)
{
    network::http::Unit hdr;
    
    if(isValid(recieve))
    {
        const network::http::Unit& request = recieve->header();
        QString fileName = QDir::toNativeSeparators(common::DiskOps::mergeName(m_rootDir, request.resource()));
        QFileInfo fileInfo(fileName);

        if(fileInfo.isFile())
        {
            QString type = network::http::contentTypeFromExtension(fileName);
            if(!type.isEmpty())
            {
                hdr.response(200);
                hdr.add("Content-Type", type);
                transferFile(fileName, recieve, hdr);
            }
            else
            {
                QString err = QString("File '%1' has unknown content type").arg(fileName);
                printError("process", err.toUtf8().constData());
                postErrorResponse(400, recieve);
            }
        }
        else
        {
            QString err = QString("Request for unknown resource '%1' which equates to file '%2'").arg(request.resource()).arg(fileName);
            printError("process", err.toUtf8().constData());
            postErrorResponse(404, recieve);
        }
    }
    else
    {
        printError("process", "Invalid HTTP request");
        postErrorResponse(400, recieve);
    }
    recieve->connection()->complete();
    recieve->endProcess();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
