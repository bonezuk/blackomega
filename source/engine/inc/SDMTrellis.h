//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLIS_H
#define __OMEGA_ENGINE_SDMTRELLIS_H
//-------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include <type_traits>

#include "hwy/highway.h"

#include "engine/inc/FIRFilterDB.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

const constexpr int c_maxNoSDMTrellisPaths = 32;

/* With each step the number of new candidates grows by two.
*/
template <typename T> struct SDMTrellisStates
{
    HWY_ALIGN T states[8][2 * c_maxNoSDMTrellisPaths];
    HWY_ALIGN T cost[2 * c_maxNoSDMTrellisPaths];
    HWY_ALIGN uint32_t path[2 * c_maxNoSDMTrellisPaths];
};

using SDMTrellisStates_Float = SDMTrellisStates<float>;
using SDMTrellisStates_Double = SDMTrellisStates<double>;

//-------------------------------------------------------------------------------------------

template <typename T> struct SDMTrellisBlockFilter
{
    HWY_ALIGN T a[8][8]; // a[0][] = (a0, a0, a0, ...), a[1][] = (a1, a1, a1, ...)
    // As g[odd] == 0.0 then these calculations are ignored.
    HWY_ALIGN T g[4][8]; // g[0][] = (g0, g0, g0, ...), g[1][] = (g2, g2, g2, ...)
};

using SDMTrellisBlockFilter_Float = SDMTrellisBlockFilter<float>;
using SDMTrellisBlockFilter_Double = SDMTrellisBlockFilter<double>;

//-------------------------------------------------------------------------------------------

template <typename T> ENGINE_EXPORT SDMTrellisBlockFilter<T> *getSDMTrellisBlockFilter(int dsdRate, bool isClans);
template <typename T> ENGINE_EXPORT void freeSDMFreeTrellisBlockFilter(SDMTrellisBlockFilter<T> *filter);
template <typename T> ENGINE_EXPORT SDMTrellisStates<T> *allocateSMDTrellisStates();
template <typename T> ENGINE_EXPORT void freeSMDTrellisStates(SDMTrellisStates<T> *states);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT bool sdmCalcTrellisBlockFilter4Lanes(const SDMTrellisStates_Float *src, SDMTrellisStates_Float *dest, 
    const SDMTrellisBlockFilter_Float *filter, float x, int fromPathIndex);

ENGINE_EXPORT bool sdmCalcTrellisBlockFilter4Lanes(const SDMTrellisStates_Double *src, SDMTrellisStates_Double *dest, 
    const SDMTrellisBlockFilter_Double *filter, double x, int fromPathIndex);

ENGINE_EXPORT bool sdmCalcTrellisBlockFilter8Lanes(const SDMTrellisStates_Float *src, SDMTrellisStates_Float *dest, 
    const SDMTrellisBlockFilter_Float *filter, float x, int fromPathIndex);

ENGINE_EXPORT bool sdmCalcTrellisBlockFilter8Lanes(const SDMTrellisStates_Double *src, SDMTrellisStates_Double *dest, 
    const SDMTrellisBlockFilter_Double *filter, double x, int fromPathIndex);

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT bool is4LanesFloatSupported();
ENGINE_EXPORT bool is8LanesFloatSupported();
ENGINE_EXPORT bool is4LanesDoubleSupported();
ENGINE_EXPORT bool is8LanesDoubleSupported();

//-------------------------------------------------------------------------------------------
// Highway target selection for the SIMD trellis calculations. Highway is linked statically
// into the engine so the chosen target must be set from within the engine and not the caller.
//-------------------------------------------------------------------------------------------

// Targets (HWY_AVX2, HWY_SSE4, ...) that are both compiled in and supported by the CPU.
ENGINE_EXPORT std::vector<int64_t> sdmTrellisSupportedTargets();
ENGINE_EXPORT const char *sdmTrellisTargetName(int64_t target);
// Force dispatch to the given target. Passing 0 restores automatic selection of the best target.
ENGINE_EXPORT void sdmTrellisSetTarget(int64_t target);

//-------------------------------------------------------------------------------------------

template <typename T> class SDMTrellis
{
	public:
		using CalcLaneFn = void (SDMTrellis::*)(T);
		
	public:
		SDMTrellis();
		virtual ~SDMTrellis();
		virtual bool init(int dsdRate, int trellisOrder, int latency);
		virtual int rate() const;
		virtual int order() const;
		virtual int latency() const;
		
	protected:
		int m_rate;
		int m_order;
		int m_latency;
		SDMTrellisStates<T> *m_states[2];
		SDMTrellisBlockFilter<T> *m_filter;
		
		uint32_t m_pathMask;
		uint32_t m_trellisMask;
		uint32_t m_latencyMask;
		
		HWY_ALIGN uint8_t *m_pathHashTable;
		
		CalcLaneFn stepCalc;
		
		virtual void release();
		virtual bool isSIMDSupported();
		virtual bool isRateSupported(int rate) const;
		
		virtual int currentIndexFromNext(int nextPathIdx) const;
		
		// The path history from the current state. pathIdx = path index into current states
		virtual uint32_t currentPath(int pathIdx) const;
		// The trellis state from the current state. pathIdx = path index into current states
		virtual uint32_t currentTrellisState(int pathIdx) const;
		// The path history from the next state. pathIdx = path index into next states
		virtual uint32_t nextPath(int pathIdx) const;
		// The trellis state from the next state. pathIdx = path index into next states
		virtual uint32_t nextTrellisState(int pathIdx) const;
		// The output bit for given path, where pathIdx = path index into current states
		virtual int outputFromCurrent(int pathIdx) const;
		// The output bit for given path, where pathIdx = path index into next states
		virtual int outputFromNext(int pathIdx) const;
		
		// Step the path history from the current state to the next state.
		virtual void stepPath();
		
		virtual void stepCalc4Lanes(T sample);
		virtual void stepCalc8Lanes(T sample);
		virtual void calc(T sample);
		
		virtual T stepMinCostAndResetHash(int& minIdx);
};

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellis<T>::SDMTrellis() : m_rate(0),
	m_order(0),
	m_latency(0)
	m_pathMask(0),
	m_trellisMask(0),
	m_latencyMask(0),
	m_pathHashTable(nullptr),
	m_filter(nullptr)
{
	m_states[0] = nullptr;
	m_states[1] = nullptr;
}

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellis<T>::~SDMTrellis()
{
	SDMTrellis<T>::release();
}

//-------------------------------------------------------------------------------------------

template <typename T> int SMDTrellis<T>::rate() const
{
	return m_rate;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SMDTrellis<T>::order() const
{
	return m_order;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SMDTrellis<T>::latency() const
{
	return m_latency;
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SMDTrellis<T>::init(int dsdRate, int trellisOrder, int latency)
{
	if(!isRateSupported(dsdRate))
	{
		return false;
	}
	if(!(trellisOrder > 0 && trellisOrder < 31))
	{
		return false;
	}
	if(!(latency > trellisOrder && latency < 32))
	{
		return false;
	}
	
	release();
	
	if(!isSIMDSupported())
	{
		return false;
	}
	
	m_rate = dsdRate;
	m_order = trellisOrder;
	m_latency = latency;
	
	m_pathMask = (1 << m_latency) - 1;
	m_trellisMask = (1 << m_order) - 1;
	m_latencyMask = 1 << (m_latency - 1);
	
	m_pathHashTable = new uint8_t [1 << m_order];
	if(m_pathHashTable == nullptr)
	{
		return false;
	}
	
	m_filter = getSDMTrellisBlockFilter<T>(m_rate, false);
	if(m_filter == nullptr)
	{
		return false;
	}
	
	bool res = true;
	for(int idx = 0; idx < 2 && res; idx++)
	{
		m_states[idx] = allocateSMDTrellisStates<T>();
		if(m_states[idx] == nullptr)
		{
			res = false;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::release()
{
	for(int idx = 0; idx < 2; idx++)
	{
		if(m_states[idx] != nullptr)
		{
			freeSMDTrellisStates<T>(m_states[idx]);
			m_states[idx] = nullptr;
		}
	}
	if(m_filter != nullptr)
	{
		freeSDMFreeTrellisBlockFilter<T>(m_filter);
		m_filter = nullptr;
	}
	if(m_pathHashTable != nullptr)
	{
		delete [] m_pathHashTable;
		m_pathHashTable = nullptr;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SDMTrellis<T>::isSIMDSupported()
{
	bool res = false;
	
	if constexpr (std::is_same<T, double>)
	{
		if(is8LanesDoubleSupported())
		{
			stepCalc = &SDMTrellis::stepCalc8Lanes;
			res = true;
		}
		else if(is4LanesDoubleSupported())
		{
			stepCalc = &SDMTrellis::stepCalc4Lanes;
			res = true;
		}
	}
	else if constexpr (std::is_same<T, float>)
	{
		if(is8LanesFloatSupported())
		{
			stepCalc = &SDMTrellis::stepCalc8Lanes;
			res = true;
		}
		else if(is4LanesFloatSupported())
		{
			stepCalc = &SDMTrellis::stepCalc4Lanes;
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SDMTrellis<T>::isRateSupported(int rate) const
{
	constexpr int c_noRates = 3;
	constexpr int rates[c_noRates] = { 64, 128, 256 };
	bool res = false;
	
	for(int idx = 0; idx < c_noRates && !res; idx++)
	{
		if(rate == rates[idx])
		{
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::currentIndexFromNext(int nextPathIdx) const
{
	int d = nextPathIdx >> 4;
	int r = nextPathIdx & 0x7;
	return (d << 3) + r;
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellis<T>::currentPath(int pathIdx) const
{
	return (m_states[0]->path[pathIdx] & m_pathMask);
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellis<T>::currentTrellisState(int pathIdx) const
{
	return (m_states[0]->path[pathIdx] & m_trellisMask);
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellis<T>::nextPath(int pathIdx) const
{
	return (m_states[1]->path[pathIdx] & m_pathMask);
}

//-------------------------------------------------------------------------------------------

template <typename T> uint32_t SDMTrellis<T>::nextTrellisState(int pathIdx) const
{
	return (m_states[0]->path[pathIdx] & m_trellisMask);
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::outputFromCurrent(int pathIdx) const
{
	return (m_states[0]->path[pathIdx] & m_latencyMask);
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::outputFromNext(int pathIdx) const
{
	return outputFromCurrent(currentIndexFromNext(pathIdx));
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::stepCalc4Lanes(T sample)
{
	for(int idx = 0; idx < c_maxNoSDMTrellisPaths; idx += 8)
	{
		sdmCalcTrellisBlockFilter4Lanes(m_states[0], m_states[1], m_filter, sample, idx);
		sdmCalcTrellisBlockFilter4Lanes(m_states[0], m_states[1], m_filter, sample, idx + 4);
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::stepCalc8Lanes(T sample)
{
	for(int idx = 0; idx < c_maxNoSDMTrellisPaths; idx += 8)
	{
		sdmCalcTrellisBlockFilter8Lanes(m_states[0], m_states[1], m_filter, sample, idx);
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::stepPath()
{
	for(int idx = 0; idx < c_maxNoSDMTrellisPaths; idx++)
	{
		int d = idx >> 3;
		int r = idx & 0x7;
		int nIdxA = idx << 4;
		int nIdxB = nIdxA + r;
		int n = m_states[0]->path[idx];
		m_states[1]->path[nIdxA] = n & m_pathMask;
		m_states[1]->path[nIdxB] = (n & m_pathMask) + 1;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::calc(T sample)
{
	(this->*stepCalc(sample));
}

//-------------------------------------------------------------------------------------------

template <typename T> T SDMTrellis<T>::stepMinCostAndResetHash(int& minIdx)
{
	T min;
	for(int idx = 0; idx < 2 * c_maxNoSDMTrellisPaths; idx++)
	{
		if(!idx || m_states[1]->cost[idx] < min)
		{
			min = m_states[1]->cost[idx];
			minIdx = idx;
		}
		int cIdx = currentIndexFromNext(idx);
		m_pathHashTable[cIdx] = 0;
	}
	return min;
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::sortCandidates()
{

}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
