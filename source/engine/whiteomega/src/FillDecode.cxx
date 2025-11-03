#include "engine/whiteomega/inc/FillDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

FillDecode::FillDecode()
{}

//-------------------------------------------------------------------------------------------

bool FillDecode::decode(Sequence *s)
{
    tint cnt,amount;
    WSequence *seq = dynamic_cast<WSequence *>(s);

    if(seq==0)
    {
        return false;
    }

    cnt = seq->readBitsI(4);
    if(cnt==15)
    {
        cnt += seq->readBitsI(8) - 1;
    }
    while(cnt>0)
    {
        amount = extensionPayload(seq,cnt);
        if(amount>=0)
        {
            cnt -= amount;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

tint FillDecode::extensionPayload(WSequence *seq,tint cnt)
{
    tint i,len = cnt - 1,align = 4;
    FillType type;

    type = static_cast<FillType>(seq->readBitsI(4));
    switch(type)
    {
        case e_extDynamicRange:
            cnt = -1; // to be implemented
            break;

        case e_extFillData:
            {
                seq->readBitsI(4);
                for(i=0;i<len;++i)
                {
                    seq->readBitsI(8);
                }
            }
            break;

        case e_extDataElement:
            {
                tint dataElementVersion = seq->readBitsI(4);

                switch(dataElementVersion)
                {
                    case ANC_DATA:
                        {
                            tint part,loopCounter = 0;
                            tint dataElementLength = 0;

                            do
                            {
                                part = seq->readBitsI(8);
                                dataElementLength += part;
                                loopCounter++;
                            } while(part==255);

                            for(i=0;i<dataElementLength;++i)
                            {
                                seq->readBitsI(8);
                            }
                            return (dataElementVersion + loopCounter + 1);
                        }

                    default:
                        align = 0;
                }
            }

        case e_extFill:
        default:
            {
                seq->readBitsI(align);
                for(i=0;i<len;++i)
                {
                    seq->readBitsI(8);
                }
            }
            break;
    }
    return cnt;
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
