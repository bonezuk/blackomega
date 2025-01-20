//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_ADATA_H
#define __OMEGA_ENGINE_ADATA_H
//-------------------------------------------------------------------------------------------

#include "common/inc/TimeStamp.h"
#include "engine/inc/EngineDLL.h"

#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

/* In order to support native integer mode and DSD audio the codec must have the ability  
   output its audio format in its native format. The endianness of the data is that of 
   the executing CPU.
   
   DSD data is composed of 1-bit samples packed together as a packet of 8-bits to form
   a single byte. The order of these bytes is either
   * LSB - Least Significant Bit is first and MSB is the last.
   * MSB - Most Significant Bit is first and LSB is the last.
   
   Encapsulated inside AData(RData and ASIOData) the bytes are packed into sizeof(sample_t)=8 byte
   blocks. This is to ensure the memory block is filled with DSD data. Bytes are written in sequential 
   order to fill a given block. If padding is required then use use zero 0s (as DSF file format does 
   its padding).
   
   DSD channel data is presented in the same manner as PCM data in that channel data is interleaved
   blocks. e.g. L=a[0..8] R=[0..8] using a 4 byte block size
   
   a[0]a[1]a[2]a[3]a[4]a[5]a[6]a[7] | b[0]b[1]b[2]b[3]b[4]b[5]b[6]b[7] |...
   
   The length and offset functions report the number of blocks.
*/

typedef int CodecDataType;
const CodecDataType e_SampleFloat = 0x01;
const CodecDataType e_SampleInt16 = 0x02;
const CodecDataType e_SampleInt24 = 0x04;
const CodecDataType e_SampleInt32 = 0x08;
const CodecDataType e_SampleDSD8LSB = 0x10;
const CodecDataType e_SampleDSD8MSB = 0x20;

//-------------------------------------------------------------------------------------------

typedef enum
{
	e_invalidChannelIndex = -1,
	e_centerChannelIndex = -2,
	e_lfeChannelIndex = -3
} InputVirtualChannelIndex;

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT AData
{
	public:
		AData();
		AData(tint len,tint inChannel,tint outChannel);
		AData(const AData& rhs);
		virtual ~AData();
		
		const AData& operator = (const AData& rhs);
		
		virtual tint noInChannels() const;
		virtual tint noOutChannels() const;
		
		virtual sample_t *data();
		virtual const sample_t *dataConst() const;
		virtual sample_t *dataOut();
		virtual const sample_t *dataOutConst() const;
		
		virtual tint length() const;
		
		virtual void reset();
				
		virtual common::TimeStamp& start();
		virtual const common::TimeStamp& startConst() const;
		
		virtual common::TimeStamp& end();
		virtual const common::TimeStamp& endConst() const;
		
		virtual bool isComplete() const;
		virtual void setComplete(bool flag);

		virtual bool isMixing() const;
		virtual void mixChannels();
		
		virtual sample_t *filterData(tint filterIdx);
		virtual const sample_t *filterDataConst(tint filterIdx) const;
		
		virtual sample_t *center();
		
		virtual bool isCenter() const;
		virtual bool isLFE() const;
		
	protected:
	
		sample_t *m_data;
		sample_t *m_outData;
		tint m_length;
		tint m_noChannels;
		tint m_noOutChannels;
		common::TimeStamp m_start;
		common::TimeStamp m_end;
		bool m_completeFlag;
		QMap<tint, sample_t *> m_filterDataMap;
		sample_t *m_centreData;
		bool m_isCenterValid;
		
		virtual void init();
		virtual void copy(const AData& rhs);
		
		virtual void freeFilterData();
		
		virtual void mixAToA();
		virtual void mixAToB();
		virtual void mixAToC();
		virtual void mixAToD();
		virtual void mixAToE();
		virtual void mixAToF();
		virtual void mixAToG();
		virtual void mixAToH();

		virtual void mixBToA();
		virtual void mixBToB();
		virtual void mixBToC();
		virtual void mixBToD();
		virtual void mixBToE();
		virtual void mixBToF();
		virtual void mixBToG();
		virtual void mixBToH();

		virtual void mixCToA();
		virtual void mixCToB();
		virtual void mixCToC();
		virtual void mixCToD();
		virtual void mixCToE();
		virtual void mixCToF();
		virtual void mixCToG();
		virtual void mixCToH();
		
		virtual void mixDToA();
		virtual void mixDToB();
		virtual void mixDToC();
		virtual void mixDToD();
		virtual void mixDToE();
		virtual void mixDToF();
		virtual void mixDToG();
		virtual void mixDToH();

		virtual void mixEToA();
		virtual void mixEToB();
		virtual void mixEToC();
		virtual void mixEToD();
		virtual void mixEToE();
		virtual void mixEToF();
		virtual void mixEToG();
		virtual void mixEToH();
		
		virtual void mixFToA();
		virtual void mixFToB();
		virtual void mixFToC();
		virtual void mixFToD();
		virtual void mixFToE();
		virtual void mixFToF();
		virtual void mixFToG();
		virtual void mixFToH();
		
		virtual void mixGToA();
		virtual void mixGToB();
		virtual void mixGToC();
		virtual void mixGToD();
		virtual void mixGToE();
		virtual void mixGToF();
		virtual void mixGToG();
		virtual void mixGToH();
		
		virtual void mixHToA();
		virtual void mixHToB();
		virtual void mixHToC();
		virtual void mixHToD();
		virtual void mixHToE();
		virtual void mixHToF();
		virtual void mixHToG();
		virtual void mixHToH();
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

