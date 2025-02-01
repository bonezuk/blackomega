//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DSD_DSD2PCMCONVERTER_H
#define __OMEGA_ENGINE_DSD_DSD2PCMCONVERTER_H
//-------------------------------------------------------------------------------------------

#include "engine/dsdomega/inc/DSFFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

class DSDOMEGA_EXPORT DSD2PCMConverter
{
	public:
		DSD2PCMConverter();
		virtual ~DSD2PCMConverter();
		
		virtual bool setup(int dsdSampleFreq, int pcmSampleFreq, bool isLSB);
		
		virtual void push(const QByteArray& dsdInArray);
		virtual void pull(QByteArray& pcmOutArray, bool isFinal);
};

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "engine/dsdomega/inc/DSD2PCMConverter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

DSD2PCMConverter::DSD2PCMConverter()
{}

//-------------------------------------------------------------------------------------------

DSD2PCMConverter::~DSD2PCMConverter()
{}

//-------------------------------------------------------------------------------------------

bool DSD2PCMConverter::setup(int dsdSampleFreq, int pcmSampleFreq, bool isLSB)
{}

//-------------------------------------------------------------------------------------------

void DSD2PCMConverter::push(const QByteArray& dsdInArray)
{}

//-------------------------------------------------------------------------------------------

void DSD2PCMConverter::pull(QByteArray& pcmOutArray, bool isFinal)
{}

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
