//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLIS_H
#define __OMEGA_ENGINE_SDMTRELLIS_H
//-------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include <type_traits>

#include "hwy/highway.h"

#include "engine/inc/SDMSort.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

/* With each step the number of new candidates grows by two.
*/
template <typename T> struct SDMTrellisStates
{
    HWY_ALIGN T states[8][2 * c_maxNoSDMTrellisPaths];
    HWY_ALIGN T cost[2 * c_maxNoSDMTrellisPaths];
    HWY_ALIGN uint32_t path[2 * c_maxNoSDMTrellisPaths];
    HWY_ALIGN uint32_t cand[2 * c_maxNoSDMTrellisPaths];
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
template <typename T> ENGINE_EXPORT SDMTrellisStates<T> *allocateSDMTrellisStates();
template <typename T> ENGINE_EXPORT void freeSDMTrellisStates(SDMTrellisStates<T> *states);

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
		bool init(int dsdRate, int trellisOrder, int latency);
		int rate() const;
		int order() const;
		int latency() const;
		
	protected:
		int m_rate;
		int m_order;
		int m_latency;
		int m_noCandidates;
		
		SDMTrellisStates<T> *m_states[2];
		SDMTrellisBlockFilter<T> *m_filter;
		
		uint32_t m_pathMask;
		uint32_t m_trellisMask;
		uint32_t m_latencyMask;
		
		HWY_ALIGN int *m_stateHashTable;
		
		CalcLaneFn stepCalc;
		
		void release();
		bool isSIMDSupported();
		bool isRateSupported(int rate) const;
		
		int currentIndexFromNext(int nextPathIdx) const;
		
		// The path history from the current state. pathIdx = path index into current states
		uint32_t currentPath(int pathIdx) const;
		// The trellis state from the current state. pathIdx = path index into current states
		uint32_t currentTrellisState(int pathIdx) const;
		// The path history from the next state. pathIdx = path index into next states
		uint32_t nextPath(int pathIdx) const;
		// The trellis state from the next state. pathIdx = path index into next states
		uint32_t nextTrellisState(int pathIdx) const;
		// The output bit for given path, where pathIdx = path index into current states
		int outputFromCurrent(int pathIdx) const;
		// The output bit for given path, where pathIdx = path index into next states
		int outputFromNext(int pathIdx) const;
		
		// Step the path history from the current state to the next state.
		void stepPath();
		
		void stepCalc4Lanes(T sample);
		void stepCalc8Lanes(T sample);
		void calc(T sample);
		
		T stepMinCostAndResetHash(int& minIdx);
		
		T costOfCand(int idx) const;
		int insertIndex(const T *data, const uint32_t *indices, int N, T value) const;
		
		int vibertiStep();
};

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellis<T>::SDMTrellis() : m_rate(0),
	m_order(0),
	m_latency(0),
	m_noCandidates(1),
	m_pathMask(0),
	m_trellisMask(0),
	m_latencyMask(0),
	m_stateHashTable(nullptr),
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

template <typename T> int SDMTrellis<T>::rate() const
{
	return m_rate;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::order() const
{
	return m_order;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::latency() const
{
	return m_latency;
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SDMTrellis<T>::init(int dsdRate, int trellisOrder, int latency)
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
	m_noCandidates = 1;
	
	m_pathMask = (1 << m_latency) - 1;
	m_trellisMask = (1 << m_order) - 1;
	m_latencyMask = 1 << (m_latency - 1);
	
	m_stateHashTable = new int [1 << m_order];
	if(m_stateHashTable == nullptr)
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
		m_states[idx] = allocateSDMTrellisStates<T>();
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
			freeSDMTrellisStates<T>(m_states[idx]);
			m_states[idx] = nullptr;
		}
	}
	if(m_filter != nullptr)
	{
		freeSDMFreeTrellisBlockFilter<T>(m_filter);
		m_filter = nullptr;
	}
	if(m_stateHashTable != nullptr)
	{
		delete [] m_stateHashTable;
		m_stateHashTable = nullptr;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SDMTrellis<T>::isSIMDSupported()
{
	bool res = false;
	
	if constexpr (std::is_same<T, double>::value)
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
	else if constexpr (std::is_same<T, float>::value)
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
	return (m_states[1]->path[pathIdx] & m_trellisMask);
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::outputFromCurrent(int pathIdx) const
{
	return (m_states[0]->path[pathIdx] & m_latencyMask) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::outputFromNext(int pathIdx) const
{
	return outputFromCurrent(currentIndexFromNext(pathIdx));
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::stepCalc4Lanes(T sample)
{
	for(int idx = 0; idx < m_noCandidates; idx += 8)
	{
		sdmCalcTrellisBlockFilter4Lanes(m_states[0], m_states[1], m_filter, sample, idx);
		sdmCalcTrellisBlockFilter4Lanes(m_states[0], m_states[1], m_filter, sample, idx + 4);
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::stepCalc8Lanes(T sample)
{
	for(int idx = 0; idx < m_noCandidates; idx += 8)
	{
		sdmCalcTrellisBlockFilter8Lanes(m_states[0], m_states[1], m_filter, sample, idx);
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::stepPath()
{
	for(int idx = 0; idx < m_noCandidates; idx++)
	{
		int d = idx >> 3;
		int r = idx & 0x7;
		int nIdxA = (d << 4) + r;
		int nIdxB = nIdxA + 8;
		int n = m_states[0]->path[idx] << 1;
		m_states[1]->path[nIdxA] = n & m_pathMask;
		m_states[1]->path[nIdxB] = (n & m_pathMask) + 1;
		m_states[1]->cand[idx << 1] = nIdxA;
		m_states[1]->cand[(idx << 1) + 1] = nIdxB;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellis<T>::calc(T sample)
{
	(this->*stepCalc)(sample);
}

//-------------------------------------------------------------------------------------------

template <typename T> T SDMTrellis<T>::stepMinCostAndResetHash(int& minIdx)
{
	T min;
	for(int idx = 0; idx < 2 * m_noCandidates; idx++)
	{
		if(!idx || m_states[1]->cost[idx] < min)
		{
			min = m_states[1]->cost[idx];
			minIdx = idx;
		}
		int nState = nextTrellisState(idx);
		m_stateHashTable[nState] = -1;
	}
	return min;
}

//-------------------------------------------------------------------------------------------

template <typename T> T SDMTrellis<T>::costOfCand(int idx) const
{
	return m_states[1]->cost[idx];
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::insertIndex(const T *data, const uint32_t *indices, int N, T value) const
{
	int lo = 0;
	int hi = N;
	
	while(lo < hi)
	{
		int mid = lo + ((hi - lo) >> 1);
		if(data[indices[mid]] < value)
		{
			lo = mid + 1;
		}
		else
		{
			hi = mid;
		}
	}
	return lo;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellis<T>::vibertiStep()
{
	T min;
	int minIdx = 0, oBit, tmp;
	uint32_t *candIn  = m_states[1]->cand;
	uint32_t *candOut = m_states[0]->cand;
	
	stepPath();
	min = stepMinCostAndResetHash(minIdx);
	oBit = outputFromNext(minIdx);
	
	int outIdx = 0;
	for(int i = 0; i < 2 * m_noCandidates; i++)
	{
		int inIdx = candIn[i];
		// Ensure the candidate output bit is equal to the output given in this step.
		if(outputFromNext(inIdx) == oBit)
		{
			// Skip if max number of candidates and its cost is greater than all the
			// potential candidates.
			if(outIdx >= c_maxNoSDMTrellisPaths && costOfCand(inIdx) >= costOfCand(candOut[outIdx - 1]))
			{
				continue;
			}
			
			// Get the next trellis state
			uint32_t stateN = nextTrellisState(inIdx);
			// Query the hash for existing stateN
			int hashN = m_stateHashTable[stateN];
			// If the hash has been previously occupied
			if(hashN >= 0)
			{
				// The trellis state has been occupied so the costs must be compared.
				if(costOfCand(inIdx) >= costOfCand(hashN))
				{
					// The existing trellis state has a lower cost and thus is kept.
					continue;
				}
				// The trellis state is has a lower cost and thus is replaced
				// Find the insertion index position in candidate list.
				int pos = insertIndex(m_states[1]->cost, m_states[0]->cand, outIdx, costOfCand(inIdx));
				// The next value in the insertion list, starting with the new candidate
				int val = inIdx;
				// Increment output index as required
				if(outIdx < c_maxNoSDMTrellisPaths)
				{
					outIdx++;
				}
				// Perform insertion sort of new candidate within list.
				for(int j = pos; j < outIdx; j++)
				{
					tmp = m_states[0]->cand[j];
					m_states[0]->cand[j] = val;
					val = tmp;
					// The conflicting trellis state with the higher cost is guaranteed
					// to be above starting pos due to ordering of the list by ascending cost.
					if(tmp == hashN)
					{
						// Once the conflicting state is moved out into tmp we can stop
						// the insertion memory move operation.
						break;
					}
				}								
			}
			else
			{
				// The trellis state has NOT been occupied.
				// Find the insertion index position in candidate list.
				int pos = insertIndex(m_states[1]->cost, m_states[0]->cand, outIdx, costOfCand(inIdx));
				// The next value in the insertion list, starting with the new candidate
				int val = inIdx;
				// Increment output index as required
				if(outIdx < c_maxNoSDMTrellisPaths)
				{
					outIdx++;
				}
				// Perform insertion sort of new candidate within list.
				for(int j = pos; j < outIdx; j++)
				{
					tmp = m_states[0]->cand[j];
					m_states[0]->cand[j] = val;
					val = tmp;
				}
			}
			m_stateHashTable[stateN] = inIdx;
		}
	}
	m_noCandidates = outIdx;
	return oBit;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

