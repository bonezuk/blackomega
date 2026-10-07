//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMSORT_H
#define __OMEGA_ENGINE_SDMSORT_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FIRFilterDB.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

const constexpr int c_maxNoSDMTrellisPaths = 32;

//-------------------------------------------------------------------------------------------

template <typename T> class SDMSort
{
	public:
		SDMSort();
		virtual ~SDMSort();
		virtual const int *sort(const T *data, int N);
		
	private:
		int *m_indArray[2];
		int m_indSize;
		
		virtual void reallocateAsRequired(int N);
		virtual int insertIndex(const T *data, const int *indices, int N, T value);
		virtual void merge(const T *data, int left, int mid, int right, int arrayIndex);
		virtual void mergeSort(const T *data, int N, int& arrayIndex);
};

//-------------------------------------------------------------------------------------------

template <typename T> SDMSort<T>::SDMSort()
{
	constexpr int c_initialSize = 2 * c_maxNoSDMTrellisPaths;
	for(int idx = 0; idx < 2; idx++)
	{
		m_indArray[idx] = new int [c_initialSize];
	}
	m_indSize = c_initialSize;
};

//-------------------------------------------------------------------------------------------

template <typename T> SDMSort<T>::~SDMSort()
{
	for(int idx = 0; idx < 2; idx++)
	{
		delete [] m_indArray[idx];
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMSort<T>::insertIndex(const T *data, const int *indices, int N, T value)
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

template <typename T> void SDMSort<T>::merge(const T *data, int left, int mid, int right, int arrayIndex)
{
	int *src = m_indArray[arrayIndex];
	int *dst = m_indArray[arrayIndex ^ 1];
	int n1 = mid - left;
	int n2 = right - mid;
	
	int *sL = &src[left];
	int *sR = &src[mid];
	dst = &dst[left];

	for(int idx = 0; idx < n1; idx++)
	{
		int ind = sL[idx];
		int j = idx + insertIndex(data, sR, n2, data[ind]);
		dst[j] = ind;
	}
	for(int idx = 0; idx < n2; idx++)
	{
		int ind = sR[idx];
		int j = idx + insertIndex(data, sL, n1, data[ind]);
		dst[j] = ind;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMSort<T>::mergeSort(const T *data, int N, int& arrayIndex)
{
	for(int step = 2; step < (N << 1); step <<= 1)
	{
		for(int pos = 0; pos < N; pos += step)
		{
			int left = pos;
			int right = pos + step;
			int diff = right - left;
			int mid = left + (diff >> 1);
			merge(data, left, mid, right, arrayIndex);
		}
		arrayIndex = (arrayIndex + 1) & 0x1;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> const int *SDMSort<T>::sort(const T *data, int N)
{
	int arrayIndex = 0;
	int *arr;
	
	reallocateAsRequired(N);
	arr = m_indArray[arrayIndex];
	for(int idx = 0; idx < N; idx++)
	{
		arr[idx] = idx;
	}
	mergeSort(data, N, arrayIndex);
	
	return m_indArray[arrayIndex];
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMSort<T>::reallocateAsRequired(int N)
{
	if(N > m_indSize)
	{
		for(int idx = 0; idx < 2; idx++)
		{
			delete [] m_indArray[idx];
			m_indArray[idx] = new int [N];
		}
		m_indSize = N;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
