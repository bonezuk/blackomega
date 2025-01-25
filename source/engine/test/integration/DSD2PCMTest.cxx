#include "gtest/gtest.h"

#include "common/inc/DiskOps.h"
#include "common/inc/BIOBufferedStream.h"
#include "engine/inc/Codec.h"
#include "engine/dsdomega/inc/DSFFileReader.h"
#include "engine/blueomega/inc/WaveEngine.h"
#include "engine/inc/FormatTypeFromFloat.h"
#include "engine/inc/BiQuadFilter.h"
#include "engine/inc/RemezFIR.h"
#include "engine/inc/FIRFilter.h"

#include <iostream>
#include <vector>
#include <cstring>

#include "dsd2pcm.hpp"
#include "noiseshape.hpp"

using namespace omega;

//-------------------------------------------------------------------------------------------

void intToMemoryDSD(tuint32 v, tubyte *mem)
{
    mem[0] = static_cast<tubyte>(v & 0x000000ff);
    mem[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    mem[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    mem[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void intToMemoryDSD(tint32 v, tubyte *mem)
{
    intToMemoryDSD(static_cast<tuint32>(v), mem);
}

//-------------------------------------------------------------------------------------------

void shortToMemoryDSD(tuint16 v, tubyte *mem)
{
    mem[0] = static_cast<tubyte>(v & 0x00ff);
    mem[1] = static_cast<tubyte>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void shortToMemoryDSD(tint16 v, tubyte *mem)
{
    shortToMemoryDSD(static_cast<tuint16>(v), mem);
}

//-------------------------------------------------------------------------------------------

void populateWavHeader(tint noChannels, tint frequency, tint bitsPerSample, tubyte hdr[44])
{
	intToMemoryDSD(RIFF_ID, &hdr[0]); // Chunk ID
	intToMemoryDSD(0, &hdr[4]); // Chunk size
	intToMemoryDSD(WAVE_ID, &hdr[8]); // Wave format
	
	intToMemoryDSD(FMT_ID, &hdr[12]); // fmt ID
	intToMemoryDSD(16, &hdr[16]);
	
	shortToMemoryDSD(static_cast<tuint16>(engine::blueomega::WaveInformation::e_formatPCM), &hdr[20]); // audio format
    shortToMemoryDSD(static_cast<tuint16>(noChannels), &hdr[22]); // number of channels
	intToMemoryDSD(frequency, &hdr[24]); // sample rate
	intToMemoryDSD(frequency * noChannels * bitsPerSample, &hdr[28]); // bytes per second
    shortToMemoryDSD(static_cast<tuint16>(noChannels * bitsPerSample), &hdr[32]); // block align
    shortToMemoryDSD(static_cast<tuint16>(bitsPerSample), &hdr[34]); // bits per sample
    
    intToMemoryDSD(BLUEOMEGA_ID('d','a','t','a'), &hdr[36]);
    intToMemoryDSD(0, &hdr[16]);
}

//-------------------------------------------------------------------------------------------

bool saveWaveHeader(tint noChannels, tint frequency, tint bitsPerSample, common::BIOStream *out)
{
	tubyte hdr[44];
	populateWavHeader(noChannels, frequency, bitsPerSample, hdr);
	return (out->write(hdr, 44) == 44);
}

//-------------------------------------------------------------------------------------------

bool saveWaveHeaderSize(tint noChannels, tint frequency, tint bitsPerSample, int totalDataSize, common::BIOStream *out)
{
	bool res = false;
	tubyte hdr[44];
	
	populateWavHeader(noChannels, frequency, bitsPerSample, hdr);
	intToMemoryDSD(totalDataSize + 36, &hdr[4]);
	intToMemoryDSD(totalDataSize, &hdr[40]);
	if(out->seek(0, common::e_Seek_Start) && out->write(hdr, 44) == 44)
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

TEST(DSD2PCM, dsfToWavDSD64)
{
	QString inFilename = "/Users/bonez/Development/DSD/Samples/her.dsf";
	QString outFilename = "/Users/bonez/Development/DSD/Samples/her.wav";
	
	common::BIOBufferedStream input(common::e_BIOStream_FileRead);
	ASSERT_TRUE(input.open(inFilename));
	common::BIOBufferedStream output(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
	ASSERT_TRUE(output.open(outFilename));
	
	engine::dsd::DSFFileReader dsf(&input);
	ASSERT_TRUE(dsf.parse());
	
	int i, blockIdx, channelIdx;
	int blockSize = dsf.channelBlockSize();
	int noChannels = dsf.numberOfChannels();
	int wavFrequency = 352800;
	int wavBitsPerSample = 24;
	int noByteSamples = 0;
	int totalSamples = 0;
	
	ASSERT_TRUE(saveWaveHeader(noChannels, wavFrequency, wavBitsPerSample, &output));
	
	bool runFlag = true;
	QByteArray inArray;
	tubyte *dsdData = new tubyte [noChannels * blockSize];
	float *floatData = new float [blockSize];
	tubyte *pcmData = new tubyte [blockSize * noChannels * 3];
	std::vector<dxd> dxds(noChannels);
	
	for(blockIdx = 0; runFlag; blockIdx++)
	{
		for(channelIdx = 0; channelIdx < noChannels && runFlag; channelIdx++)
		{
			runFlag = dsf.data(blockIdx, channelIdx, inArray);
			if(runFlag)
			{
				const tubyte *inBlock = reinterpret_cast<const tubyte *>(inArray.constData());
				noByteSamples = inArray.size();
				for(i = 0; i < noByteSamples; i++)
				{
					dsdData[channelIdx + (i * noChannels)] = inBlock[i];
				}
			}
		}
		if(runFlag)
		{
			for(channelIdx = 0; channelIdx < noChannels; channelIdx++)
			{
				dxds[channelIdx].translate(noByteSamples, &dsdData[channelIdx], noChannels, dsf.isLSB(), floatData, 1);
				for(i = 0; i < noByteSamples; i++)
				{
					engine::write24BitsBigEndianFromSample(floatData[i], reinterpret_cast<tchar *>(&pcmData[(channelIdx + (i * noChannels)) * 3]));
				}
			}
			output.write(pcmData, noByteSamples * noChannels * 3);
			totalSamples += noByteSamples * noChannels * 3;
		}
	}
	
	ASSERT_TRUE(saveWaveHeaderSize(noChannels, wavFrequency, wavBitsPerSample, totalSamples, &output));
	
	delete [] pcmData;
	delete [] floatData;
	delete [] dsdData;
	
	input.close();
	output.close();
}

//-------------------------------------------------------------------------------------------
