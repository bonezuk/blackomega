#include "gtest/gtest.h"

#include <string.h>

#include "common/inc/Random.h"
#include "engine/inc/SDMSort.h"
#include "engine/inc/SDMTrellis.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

template <typename T> void testSDMSortOfRandomIndices(int noIters)
{
	constexpr int c_maxNoEntries = 2 * engine::c_maxNoSDMTrellisPaths;
	int *indices;
	T data[2 * c_maxNoEntries];
	common::Random *rand = common::Random::instance();
	
	for(int iter = 0; iter < noIters; iter++)
	{
		int idx;
		for(idx = 0; idx < 2 * c_maxNoEntries; idx++)
		{
			data[idx] = static_cast<T>(rand->randomReal1());
		}
		
		engine::SDMSort<T> sorter;
		
		for(int N = 2; N < 2 * c_maxNoEntries; N <<= 1)
		{
			indices = const_cast<int *>(sorter.sort(data, N));
			ASSERT_TRUE(indices != nullptr);
			
			for(idx = 0; idx < N - 1; idx++)
			{
				int iA = indices[idx];
				int iB = indices[idx + 1];
				ASSERT_LE(data[iA], data[iB]);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

TEST(SDMSort, sortIndicesFloat)
{
	testSDMSortOfRandomIndices<float>(1000);
}

//-------------------------------------------------------------------------------------------

TEST(SDMSort, sortIndicesDouble)
{
	testSDMSortOfRandomIndices<double>(1000);
}

//-------------------------------------------------------------------------------------------
