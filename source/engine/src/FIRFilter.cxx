#include "engine/inc/FIRFilter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

FIRFilter::FIRFilter(const sample_t *coefficients, tint noCoeff)
{
	m_coefficients = new sample_t [noCoeff];
	m_filterLength = noCoeff;
	m_offset = 0;
	m_pPrevious = NULL;
	::memcpy(m_coefficients, coefficients, noCoeff * sizeof(sample_t));
}

//-------------------------------------------------------------------------------------------

FIRFilter::~FIRFilter()
{
	try
	{
		delete [] m_coefficients;
		m_coefficients = NULL;
	}
	catch(...) {}
}

//-------------------------------------------------------------------------------------------

tint FIRFilter::offset() const
{
	return m_offset;
}

//-------------------------------------------------------------------------------------------

void FIRFilter::setOffset(tint of)
{
	m_offset = of % m_filterLength;
}

//-------------------------------------------------------------------------------------------

void FIRFilter::process(RData *pData, tint filterIdx, bool isLast, bool clip)
{
	tint i, j, idx, len, dLen, prevLen, prevIdx;
	sample_t y;
	sample_t *pPD = NULL;
	sample_t *pDA = pData->data();
	sample_t *pPrevFilter = NULL;
	sample_t *pFilter = pData->filterData(filterIdx);
	
	if(filterIdx == e_lfeChannelIndex)
		pDA = pData->center();
	
	i = 0;
	dLen = len = pData->length() - pData->rLength();
	if(!isLast)
	{
		len -= (m_filterLength - m_offset - 1);
	}
	
	if(m_pPrevious != NULL)
	{
		i = -(m_filterLength + m_offset - 1);
		pPD = (filterIdx != e_lfeChannelIndex) ? m_pPrevious->data() : m_pPrevious->center();
		prevLen = m_pPrevious->length() - m_pPrevious->rLength();
		pPrevFilter = m_pPrevious->filterData(filterIdx);
	}
	else
	{
		prevLen = 0;
	}
	
	while(i < len)
	{
		y = 0.0;
		for(j = 0; j < m_filterLength; j++)
		{
			idx = i + j - m_offset;
			if(idx < 0)
			{
				if(m_pPrevious != NULL)
				{
					prevIdx = prevLen + idx;
					y += m_coefficients[m_filterLength - (j + 1)] * pPD[prevIdx];
				}
			}
			else if(idx < dLen)
			{
				y += m_coefficients[m_filterLength - (j + 1)] * pDA[idx];
			}
		}
		
		if(clip)
		{
			if(y < -1.0)
			{
				y = -1.0;
			}
			else if(y > 1.0)
			{
				y = 1.0;
			}
		}
		
		if(i < 0)
		{
			prevIdx = prevLen + i;
			pPrevFilter[prevIdx] = y;
		}
		else
		{
			pFilter[i] = y;
		}
		i++;
	}
	m_pPrevious = pData;
}

//-------------------------------------------------------------------------------------------

void FIRFilter::process(RData *pData, tint channelIdx, tint filterIdx, bool isLast, bool clip)
{
	tint i, j, idx, len, dLen, prevLen, prevIdx, noChannels;
	sample_t y;
	sample_t *pPD = NULL;
	sample_t *pDA = pData->data();
	sample_t *pPrevFilter = NULL;
	sample_t *pFilter = pData->filterData(filterIdx);
	
	i = 0;
	dLen = len = pData->length() - pData->rLength();
	if(!isLast)
	{
		len -= (m_filterLength - m_offset - 1);
	}
	
	if(m_pPrevious != NULL)
	{
		i = -(m_filterLength + m_offset - 1);
		pPD = m_pPrevious->data();
		prevLen = m_pPrevious->length() - m_pPrevious->rLength();
		pPrevFilter = m_pPrevious->filterData(filterIdx);
	}
	else
	{
		prevLen = 0;
	}
	noChannels = pData->noInChannels();
	
	while(i < len)
	{
		y = 0.0;
		for(j = 0; j < m_filterLength; j++)
		{
			idx = i + j - m_offset;
			if(idx < 0)
			{
				if(m_pPrevious != NULL)
				{
					prevIdx = prevLen + idx;
					y += m_coefficients[m_filterLength - (j + 1)] * pPD[(prevIdx * noChannels) + channelIdx];
				}
			}
			else if(idx < dLen)
			{
				y += m_coefficients[m_filterLength - (j + 1)] * pDA[(idx * noChannels) + channelIdx];
			}
		}
		
		if(clip)
		{
			if(y < -1.0)
			{
				y = -1.0;
			}
			else if(y > 1.0)
			{
				y = 1.0;
			}
		}
		
		if(i < 0)
		{
			prevIdx = prevLen + i;
			pPrevFilter[(prevIdx * noChannels) + channelIdx] = y;
		}
		else
		{
			pFilter[(i * noChannels) + channelIdx] = y;
		}
		i++;
	}
	m_pPrevious = pData;
}

//-------------------------------------------------------------------------------------------

void FIRFilter::reset()
{
	m_pPrevious = NULL;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
