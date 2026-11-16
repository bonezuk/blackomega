#include "engine/inc/SDMTrellis.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

const SDMTrellisFilterBase *getTrellisBaseFilter(int dsdRate, bool isClans)
{
    const SDMTrellisFilterBase *bFilter = nullptr;

    for(int idx = 0; idx < SDM_TRELLIS_NO_OF_FILTERS && bFilter == nullptr; idx++)
    {
        if(dsdRate == c_sdmTrellisFilters[idx].rate)
        {
            if(isClans && strncmp("clans", c_sdmTrellisFilters[idx].name, 5) == 0)
            {
                bFilter = &c_sdmTrellisFilters[idx];
            }
            else if(!isClans && strncmp("sdm", c_sdmTrellisFilters[idx].name, 3) == 0)
            {
                bFilter = &c_sdmTrellisFilters[idx];
            }
        }
    }
    return bFilter;
}

//-------------------------------------------------------------------------------------------

double stepSDMTrellisSpliceFilter(const struct SDMTrellisSpliceFilter *filter, const double *z, double *zNext, double x)
{
	double in = x;
	for(int k = 0; k < c_SDMTrellisBlockSpliceSections; k++)
	{
		double y = filter->lpB0[k] * in + z[2 * k];
		zNext[2 * k] = filter->lpB1[k] * in - filter->lpA1[k] * y + z[2 * k + 1];
		zNext[2 * k + 1] = filter->lpB2[k] * in - filter->lpA2[k] * y;
		in = y;
	}
	return in;
}

//-------------------------------------------------------------------------------------------

double spliceSDMTrellisEnergy(const struct SDMTrellisSpliceFilter *filter, const double *z)
{
	double e = 0.0;
	for(int i = 0; i < c_SDMTrellisBlockSpliceOrder; i++)
	{
		double t = 0.0;
		for(int j = 0; j < c_SDMTrellisBlockSpliceOrder; j++)
		{
			t += filter->gramian[i][j] * z[j];
		}
		e += z[i] * t;
	}
	return e;
}

//-------------------------------------------------------------------------------------------

void initSDMTrellisSpliceFilter(int rate, int baseFrequency, struct SDMTrellisSpliceFilter *filter)
{
	const int N = c_SDMTrellisBlockSpliceOrder;
	const double fs = static_cast<double>(rate * baseFrequency);
	const double w0 = 2.0 * c_PI_D * c_SDMTrellisBlockSpliceCutoff / fs;

	assert(baseFrequency == 44100 || baseFrequency == 48000);

	for(int k = 0; k < c_SDMTrellisBlockSpliceSections; k++)
	{
		double Q = 1.0 / (2.0 * cos(static_cast<double>(2 * k + 1) * c_PI_D / (2.0 * N)));
		double alpha = sin(w0) / (2.0 * Q);
		double cw = cos(w0);
		double a0 = 1.0 + alpha;
		filter->lpB0[k] = ((1.0 - cw) / 2.0) / a0;
		filter->lpB1[k] = (1.0 - cw) / a0;
		filter->lpB2[k] = ((1.0 - cw) / 2.0) / a0;
		filter->lpA1[k] = (-2.0 * cw) / a0;
		filter->lpA2[k] = (1.0 - alpha) / a0;
	}

	// The impulse response decays by e per ~3 / w0 samples. 400 * rate covers well over 50
	// time constants for all supported rates.
	const int noSteps = 400 * rate;
	std::vector<std::vector<double> > response(N, std::vector<double>(noSteps));
	for(int i = 0; i < N; i++)
	{
		double z[N], zNext[N];
		for(int j = 0; j < N; j++)
		{
			z[j] = (i == j) ? 1.0 : 0.0;
		}
		for(int t = 0; t < noSteps; t++)
		{
			response[i][t] = stepSDMTrellisSpliceFilter(filter, z, zNext, 0.0);
			memcpy(z, zNext, sizeof(z));
		}
	}
	for(int i = 0; i < N; i++)
	{
		for(int j = i; j < N; j++)
		{
			double sum = 0.0;
			for(int t = 0; t < noSteps; t++)
			{
				sum += response[i][t] * response[j][t];
			}
			filter->gramian[i][j] = sum;
			filter->gramian[j][i] = sum;
		}
	}
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
