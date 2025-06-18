#include "engine/silveromega/inc/VSilverInfoObject.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

VSilverInfoObject::VSilverInfoObject()
{}

//-------------------------------------------------------------------------------------------

VSilverInfoObject::~VSilverInfoObject()
{}

//-------------------------------------------------------------------------------------------

void VSilverInfoObject::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "VSilverInfoObject::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VSilverInfoObject::isValid() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool VSilverInfoObject::read(engine::Sequence *seq)
{
    return false;
}

//-------------------------------------------------------------------------------------------

void VSilverInfoObject::skipPacketIdentifer(engine::Sequence *seq) const
{
    if(seq!=0)
    {
        seq->readBits(32);
        seq->readBits(24);
    }
}

//-------------------------------------------------------------------------------------------

bool VSilverInfoObject::completePacket(engine::Sequence *seq) const
{
    if(seq==0)
    {
        printError("completePacket","No sequence instance given");
        return false;
    }
    if(!seq->readBit())
    {
        printError("completePacket","Packet sequence not properly terminated");
        return false;
    }
    seq->readBits(7);
    if(seq->isEnd())
    {
        printError("completePacket","End of sequence reached when not expected");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void VSilverInfoObject::readString(engine::Sequence *seq,QString& out) const
{
    tint i,len;
    tchar *tmp;

    len = seq->readBits(32);
    tmp = new tchar [len + 1];

    for(i=0;i<len;++i)
    {
        tmp[i] = static_cast<tchar>(seq->readBits(8));
    }
    tmp[len] = '\0';

    out = QString::fromUtf8(tmp,len);
    delete [] tmp;
}

//-------------------------------------------------------------------------------------------

bool VSilverInfoObject::setup()
{
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
