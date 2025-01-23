//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_FORMATTYPE_H
#define __OMEGA_ENGINE_VIOLETOMEGA_FORMATTYPE_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

ENGINE_EXPORT bool isLittleEndian();

ENGINE_EXPORT tbyte to8BitSignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tbyte to8BitSignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tbyte to8BitSignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tbyte to8BitSignedFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tubyte to8BitUnsignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tubyte to8BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tubyte to8BitUnsignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tubyte to8BitUnsignedFromBigEndian(const tchar *mem,int offset,int len);


ENGINE_EXPORT tint16 to16BitSignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tint16 to16BitSignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tint16 to16BitSignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tint16 to16BitSignedFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tuint16 to16BitUnsignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tuint16 to16BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tuint16 to16BitUnsignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tuint16 to16BitUnsignedFromBigEndian(const tchar *mem,int offset,int len);


ENGINE_EXPORT tint32 to24BitSignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tint32 to24BitSignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tint32 to24BitSignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tint32 to24BitSignedFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tuint32 to24BitUnsignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tuint32 to24BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tuint32 to24BitUnsignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tuint32 to24BitUnsignedFromBigEndian(const tchar *mem,int offset,int len);


ENGINE_EXPORT tint32 to32BitSignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tint32 to32BitSignedFromLittleEndian(const tubyte *mem);
ENGINE_EXPORT tint32 to32BitSignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tint32 to32BitSignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tint32 to32BitSignedFromBigEndian(const tubyte *mem);
ENGINE_EXPORT tint32 to32BitSignedFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tuint32 to32BitUnsignedFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tuint32 to32BitUnsignedFromLittleEndian(const tubyte *mem);
ENGINE_EXPORT tuint32 to32BitUnsignedFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tuint32 to32BitUnsignedFromBigEndian(const tchar *mem);
ENGINE_EXPORT tuint32 to32BitUnsignedFromBigEndian(const tubyte *mem);
ENGINE_EXPORT tuint32 to32BitUnsignedFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tuint64 to64BitUnsignedFromLittleEndian(const tchar* mem);
ENGINE_EXPORT tuint64 to64BitUnsignedFromBigEndian(const tchar* mem);

ENGINE_EXPORT tint64 to64BitSignedFromLittleEndian(const tchar* mem);
ENGINE_EXPORT tint64 to64BitSignedFromLittleEndian(const tubyte* mem);
ENGINE_EXPORT tint64 to64BitSignedFromBigEndian(const tchar* mem);
ENGINE_EXPORT tint64 to64BitSignedFromBigEndian(const tubyte* mem);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT tfloat32 toSampleFrom1Bit(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom1Bit(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom2Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom2Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom3Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom3Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom4Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom4Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom5Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom5Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom6Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom6Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom7Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom7Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom8Bits(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom8Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom9BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom9BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom9BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom9BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom10BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom10BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom10BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom10BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom11BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom11BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom11BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom11BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom12BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom12BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom12BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom12BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom13BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom13BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom13BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom13BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom14BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom14BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom14BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom14BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom15BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom15BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom15BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom15BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom16BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom16BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom16BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom16BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom17BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom17BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom17BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom17BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom18BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom18BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom18BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom18BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom19BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom19BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom19BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom19BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom20BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom20BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom20BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom20BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom21BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom21BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom21BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom21BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom22BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom22BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom22BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom22BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom23BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom23BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom23BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom23BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom24BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom24BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom24BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom24BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom25BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom25BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom25BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom25BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom26BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom26BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom26BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom26BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom27BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom27BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom27BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom27BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom28BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom28BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom28BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom28BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom29BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom29BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom29BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom29BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom30BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom30BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom30BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom30BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom31BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom31BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom31BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom31BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat32 toSampleFrom32BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom32BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat32 toSampleFrom32BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat32 toSampleFrom32BitsFromBigEndian(const tchar *mem,int offset,int len);

//-------------------------------------------------------------------------------------------
// 64-bit double precision routines

ENGINE_EXPORT tfloat64 toSample64From1Bit(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From1Bit(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From2Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From2Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From3Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From3Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From4Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From4Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From5Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From5Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From6Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From6Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From7Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From7Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From8Bits(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From8Bits(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From9BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From9BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From9BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From9BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From10BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From10BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From10BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From10BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From11BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From11BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From11BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From11BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From12BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From12BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From12BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From12BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From13BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From13BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From13BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From13BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From14BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From14BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From14BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From14BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From15BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From15BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From15BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From15BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From16BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From16BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From16BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From16BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From17BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From17BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From17BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From17BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From18BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From18BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From18BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From18BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From19BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From19BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From19BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From19BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From20BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From20BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From20BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From20BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From21BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From21BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From21BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From21BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From22BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From22BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From22BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From22BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From23BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From23BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From23BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From23BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From24BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From24BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From24BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From24BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From25BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From25BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From25BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From25BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From26BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From26BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From26BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From26BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From27BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From27BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From27BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From27BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From28BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From28BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From28BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From28BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From29BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From29BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From29BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From29BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From30BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From30BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From30BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From30BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From31BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From31BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From31BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From31BitsFromBigEndian(const tchar *mem,int offset,int len);

ENGINE_EXPORT tfloat64 toSample64From32BitsFromLittleEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From32BitsFromLittleEndian(const tchar *mem,int offset,int len);
ENGINE_EXPORT tfloat64 toSample64From32BitsFromBigEndian(const tchar *mem);
ENGINE_EXPORT tfloat64 toSample64From32BitsFromBigEndian(const tchar *mem,int offset,int len);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT tfloat64 doubleFromExtended(const tbyte *mem);
ENGINE_EXPORT tfloat64 doubleFromExtended(const tubyte *mem);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void shiftLeft32BitsLittleEndian(const tubyte *x,tubyte *y,int s);
ENGINE_EXPORT void shiftLeft24BitsLittleEndian(const tubyte *x,tubyte *y,int s);
ENGINE_EXPORT void shiftLeft16BitsLittleEndian(const tubyte *x,tubyte *y,int s);

ENGINE_EXPORT void shiftLeft32BitsBigEndian(const tubyte *x,tubyte *y,int s);
ENGINE_EXPORT void shiftLeft24BitsBigEndian(const tubyte *x,tubyte *y,int s);
ENGINE_EXPORT void shiftLeft16BitsBigEndian(const tubyte *x,tubyte *y,int s);

ENGINE_EXPORT void shiftLeft8Bits(const tubyte *x,tubyte *y,int s);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT tfloat32 readSampleLittleEndian(const tbyte *mem,int noBits);
ENGINE_EXPORT tfloat32 readSampleLittleEndian(const tubyte *mem,int noBits);
ENGINE_EXPORT tfloat32 readSampleBigEndian(const tbyte *mem,int noBits);
ENGINE_EXPORT tfloat32 readSampleBigEndian(const tubyte *mem,int noBits);

ENGINE_EXPORT tfloat64 readSample64LittleEndian(const tbyte *mem,int noBits);
ENGINE_EXPORT tfloat64 readSample64LittleEndian(const tubyte *mem,int noBits);
ENGINE_EXPORT tfloat64 readSample64BigEndian(const tubyte *mem,int noBits);
ENGINE_EXPORT tfloat64 readSample64BigEndian(const tubyte *mem,int noBits);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT tfloat32 toSampleFromBits(tint32 x,tint N);
ENGINE_EXPORT tfloat64 toSample64FromBits(tint32 x,tint N);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void write16BitsLittleEndianFromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void write16BitsLittleEndianFromSampleInt24(tint32 v, tchar *mem);
ENGINE_EXPORT void write16BitsLittleEndianFromSampleInt32(tint32 v, tchar *mem);

ENGINE_EXPORT void write16BitsBigEndianFromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void write16BitsBigEndianFromSampleInt24(tint32 v, tchar *mem);
ENGINE_EXPORT void write16BitsBigEndianFromSampleInt32(tint32 v, tchar *mem);

ENGINE_EXPORT void write24BitsLittleEndianFromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void write24BitsLittleEndianFromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void write24BitsLittleEndianFromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void write24BitsBigEndianFromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void write24BitsBigEndianFromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void write24BitsBigEndianFromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void write32BitsLittleEndianFromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void write32BitsLittleEndianFromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void write32BitsLittleEndianFromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void write32BitsBigEndianFromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void write32BitsBigEndianFromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void write32BitsBigEndianFromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32LSB16FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB16FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB16FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32MSB16FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB16FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB16FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32LSB18FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB18FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB18FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32MSB18FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB18FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB18FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32LSB20FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB20FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB20FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32MSB20FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB20FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB20FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32LSB24FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB24FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32LSB24FromSampleInt32(tint32 v,tchar *mem);

ENGINE_EXPORT void writeInt32MSB24FromSampleInt16(tint16 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB24FromSampleInt24(tint32 v,tchar *mem);
ENGINE_EXPORT void writeInt32MSB24FromSampleInt32(tint32 v,tchar *mem);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT tint16 readInt16SampleLittleEndian(const tbyte *mem, tint noBits);
ENGINE_EXPORT tint16 readInt16SampleLittleEndian(const tubyte *mem, tint noBits);
ENGINE_EXPORT tint16 readInt16SampleBigEndian(const tbyte *mem, tint noBits);
ENGINE_EXPORT tint16 readInt16SampleBigEndian(const tubyte *mem, tint noBits);

ENGINE_EXPORT tint32 readInt24SampleLittleEndian(const tbyte *mem, tint noBits);
ENGINE_EXPORT tint32 readInt24SampleLittleEndian(const tubyte *mem, tint noBits);
ENGINE_EXPORT tint32 readInt24SampleBigEndian(const tbyte *mem, tint noBits);
ENGINE_EXPORT tint32 readInt24SampleBigEndian(const tubyte *mem, tint noBits);

ENGINE_EXPORT tint32 readInt32SampleLittleEndian(const tbyte *mem, tint noBits);
ENGINE_EXPORT tint32 readInt32SampleLittleEndian(const tubyte *mem, tint noBits);
ENGINE_EXPORT tint32 readInt32SampleBigEndian(const tbyte *mem, tint noBits);
ENGINE_EXPORT tint32 readInt32SampleBigEndian(const tubyte *mem, tint noBits);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT tubyte lsb2msb(tubyte u);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

