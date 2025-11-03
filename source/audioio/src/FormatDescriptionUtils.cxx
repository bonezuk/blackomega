#include "audioio/inc/FormatDescriptionUtils.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

FormatDescriptionUtils::FormatDescriptionUtils()
{}

//-------------------------------------------------------------------------------------------

int *FormatDescriptionUtils::closestFrequencyOrder(int freq)
{
    const int rates[NUMBER_MAXFREQUENCIES] = {
        8000, 11025, 12000, 16000, 22050,
        24000, 32000, 44100, 48000, 64000,
        88200, 96000, 176400, 192000, 352800,
        384000, 705600, 768000
    };

    int i,idx;
    int *order = new int [NUMBER_MAXFREQUENCIES];

    idx = 0;

    for(i=0;i<NUMBER_MAXFREQUENCIES;i++)
    {
        int cRate = rates[i];

        if(cRate >= freq)
        {
            while(cRate > freq)
            {
                cRate >>= 1;
            }
            if(freq==cRate)
            {
                order[idx++] = i;
            }
        }
    }

    for(i=0;i<NUMBER_MAXFREQUENCIES;i++)
    {
        int cRate = rates[i];

        if(cRate >= freq)
        {
            while(cRate > freq)
            {
                cRate >>= 1;
            }
            if(freq!=cRate)
            {
                order[idx++] = i;
            }
        }
    }

    for(i=NUMBER_MAXFREQUENCIES-1;i>=0;i--)
    {
        int cRate = rates[i];

        if(cRate < freq)
        {
            order[idx++] = i;
        }
    }

    return order;
}

//-------------------------------------------------------------------------------------------
//  0 -  8(I)
//  1 - 16(I)
//  2 - 24(I)
//  3 - 32(I)
//  4 - 32(F)
//  5 - 64(F)
//  6 -  1(I)
//  7 -  2(I)
//  8 -  3(I)
//  9 -  4(I)
// 10 -  5(I)
// 11 -  6(I)
// 12 -  7(I)
// 13 -  9(I)
// 14 - 10(I)
// 15 - 11(I)
// 16 - 12(I)
// 17 - 13(I)
// 18 - 14(I)
// 19 - 15(I)
// 20 - 17(I)
// 21 - 18(I)
// 22 - 19(I)
// 23 - 20(I)
// 24 - 21(I)
// 25 - 22(I)
// 26 - 23(I)
// 27 - 25(I)
// 28 - 26(I)
// 29 - 27(I)
// 30 - 28(I)
// 31 - 29(I)
// 32 - 30(I)
// 33 - 31(I)
//-------------------------------------------------------------------------------------------

bool FormatDescriptionUtils::isBitIndexFloatingPoint(tint bitIndex)
{
    return (bitIndex==4 || bitIndex==5);
}

//-------------------------------------------------------------------------------------------

void FormatDescriptionUtils::closestBitOrder(tint bitIndex,QVector<tint>& hList,QVector<tint>& lList)
{
    const tint c_orderLow[33] = {
         3, 33, 32, 31, 30, 29, 28, 27, //  0 - 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 25(I),
         2, 26, 25, 24, 23, 22, 21, 20, //  8 - 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I), 17(I),
         1,  4, 19, 18, 17, 16, 15, 14, // 16 - 16(I), 32(F), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),
        13,  0, 12, 11, 10,  9,  8,  7, // 24 -  9(I),  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),
          6                              // 32 -  1(I)
    };

    tint i;

    lList.clear();
    hList.clear();

    if(isBitIndexFloatingPoint(bitIndex))
    {
        if(bitIndex==4)
        {
            hList.append(4);
        }
        hList.append(5);
        for(i=0;i<33;i++)
        {
            if(!(c_orderLow[i]==4 && bitIndex==4))
            {
                lList.append(c_orderLow[i]);
            }
        }
    }
    else
    {
        const tint c_orderHigh[32] = {
             3, 33, 32, 31, 30, 29, 28, 27, //  0 - 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 25(I),
             2, 26, 25, 24, 23, 22, 21, 20, //  8 - 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I), 17(I),
             1, 19, 18, 17, 16, 15, 14, 13, // 16 - 16(I), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),  9(I),
             0, 12, 11, 10,  9,  8,  7,  6  // 24 -  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I),
        };

        bool singleFlag = false;

        hList.append(bitIndex);
        for(i=0;i<32 && c_orderHigh[i]!=bitIndex;i++)
        {
            hList.append(c_orderHigh[i]);
            if(c_orderHigh[i]==1)
            {
                singleFlag = true;
            }
        }
        hList.append(5);
        if(singleFlag)
        {
            hList.append(4);
        }

        for(i=0;i<33 && c_orderLow[i]!=bitIndex;)
        {
            i++;
        }

        i++;
        while(i<33)
        {
            lList.append(c_orderLow[i]);
            i++;
        }
    }
}

//-------------------------------------------------------------------------------------------

void FormatDescriptionUtils::findClosestFormatTypeChannelList(const FormatDescription& format, bool isAscending, QVector<tint>& chList)
{
    chList.clear();
    if(isAscending)
    {
        for(tint chIdx=format.channelsIndex();chIdx<NUMBER_MAXCHANNELS;chIdx++)
        {
            chList.append(chIdx);
        }
    }
    else
    {
        for(tint chIdx=format.channelsIndex()-1;chIdx>=0;chIdx--)
        {
            chList.append(chIdx);
        }
    }
}

//-------------------------------------------------------------------------------------------

bool FormatDescriptionUtils::findClosestFormatTypeCase(const FormatDescription& format, const FormatsSupported& support, FormatDescription& closeFormat, const QVector<tint>& listA, const QVector<tint>& listB, bool isBitFirst)
{
    bool res = false;

    for(tint i=0;i<2 && !res;i++)
    {
        QVector<tint>::const_iterator ppI,ppJ;

        for(ppI=listA.begin();ppI!=listA.end() && !res;ppI++)
        {
            for(ppJ=listB.begin();ppJ!=listB.end() && !res;ppJ++)
            {
                tint bitIdx,chIdx;

                bitIdx = (isBitFirst) ? *ppI : *ppJ;
                chIdx = (isBitFirst) ? *ppJ : *ppI;

                for(tint freqIdx=0;freqIdx<NUMBER_MAXFREQUENCIES && !res;freqIdx++)
                {
                    for(tint endianIdx=0;endianIdx<2;endianIdx++)
                    {
                        FormatDescription cFormat(format);
                        cFormat.setBitsIndex(bitIdx);
                        cFormat.setChannelsIndex(chIdx);
                        cFormat.setFrequencyIndex(freqIdx);
                        cFormat.setEndian(!endianIdx);

                        if(!i)
                        {
                            if(format.frequency() <= cFormat.frequency())
                            {
                                if(support.isSupported(cFormat))
                                {
                                    closeFormat = cFormat;
                                    res = true;
                                }
                            }
                        }
                        else
                        {
                            if(format.frequency() > cFormat.frequency())
                            {
                                if(support.isSupported(cFormat))
                                {
                                    closeFormat = cFormat;
                                    res = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FormatDescriptionUtils::findClosestFormatType(const FormatDescription& format,const FormatsSupported& support,FormatDescription& closeFormat)
{
    tint *frequencyOrder;
    QVector<tint> hBitList,lBitList,chList;
    bool res;

    frequencyOrder = closestFrequencyOrder(format.frequency());
    closestBitOrder(format.bitsIndex(),hBitList,lBitList);

    // case 0
    findClosestFormatTypeChannelList(format,true,chList);
    res = findClosestFormatTypeCase(format, support, closeFormat, hBitList, chList, true);

    // case 1
    if(!res)
    {
        findClosestFormatTypeChannelList(format,false,chList);
        res = findClosestFormatTypeCase(format, support, closeFormat, chList, hBitList, false);
    }

    // case 2
    if(!res)
    {
        findClosestFormatTypeChannelList(format,true,chList);
        res = findClosestFormatTypeCase(format, support, closeFormat, lBitList, chList, true);
    }

    // case 3
    if(!res)
    {
        findClosestFormatTypeChannelList(format,false,chList);
        res = findClosestFormatTypeCase(format, support, closeFormat, lBitList, chList, true);
    }

    delete [] frequencyOrder;
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
