#include "gtest/gtest.h"

#include <cmath>
#include <cstdio>
#include <vector>

#include "engine/inc/SDMTrellis.h"

using namespace omega;
using namespace omega::engine;

//-------------------------------------------------------------------------------------------
// The trellis must be stable with the loop filters of every supported DSD rate. The original
// sox path cost was unstable at DSD128 and DSD256 (see SDMTrellis.h), so the in-band error of
// a 1kHz tone and the size of the loop filter states are checked at each rate.
//-------------------------------------------------------------------------------------------

namespace
{

constexpr double c_pi = 3.14159265358979323846;

// 8th order Butterworth low pass at 20kHz from 4 bilinear biquads.
void lowPass20k(std::vector<double>& y, double fs)
{
	for(int k = 0; k < 4; k++)
	{
		double Q = 1.0 / (2.0 * cos(static_cast<double>(2 * k + 1) * c_pi / 16.0));
		double w0 = 2.0 * c_pi * 20000.0 / fs;
		double alpha = sin(w0) / (2.0 * Q);
		double cw = cos(w0);
		double a0 = 1.0 + alpha;
		double b0 = ((1.0 - cw) / 2.0) / a0, b1 = (1.0 - cw) / a0, b2 = b0;
		double a1 = (-2.0 * cw) / a0, a2 = (1.0 - alpha) / a0;
		double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0;
		for(size_t i = 0; i < y.size(); i++)
		{
			double in = y[i];
			double out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
			x2 = x1; x1 = in; y2 = y1; y1 = out;
			y[i] = out;
		}
	}
}

// Exposes the trellis step so the loop filter states can be watched.
template <typename T> class RateTester : public SDMTrellis<T>
{
	public:
		double maxState() const { return m_maxState; }

		void run(const std::vector<float>& x, std::vector<int>& bits)
		{
			bits.resize(x.size());
			m_maxState = 0.0;
			for(size_t i = 0; i < x.size(); i++)
			{
				this->calc(static_cast<T>(x[i]) * c_SDMTrellisGain);
				bits[i] = this->vibertiStep();
				if((i & 0x3ff) == 0)
				{
					for(int j = 0; j < 8; j++)
					{
						m_maxState = (std::max)(m_maxState, fabs(static_cast<double>(this->m_states[0]->states[j][0])));
					}
				}
			}
		}

	private:
		double m_maxState = 0.0;
};

template <typename T> void testRate(int rate, const char *typeName)
{
	const int order = 8, latency = 31;
	const double fs = 44100.0 * rate;
	const int N = static_cast<int>(fs * 0.1);

	std::vector<float> x(N);
	for(int i = 0; i < N; i++)
	{
		x[i] = static_cast<float>(0.5 * sin(2.0 * c_pi * 1000.0 * static_cast<double>(i) / fs));
	}

	RateTester<T> mod;
	ASSERT_TRUE(mod.init(rate, order, latency));
	std::vector<int> bits;
	mod.run(x, bits);

	std::vector<double> yd(N), yx(N, 0.0);
	for(int i = 0; i < N; i++)
	{
		yd[i] = bits[i] ? 1.0 : -1.0;
		if(i >= latency)
		{
			yx[i] = static_cast<double>(x[i - latency]);
		}
	}
	lowPass20k(yd, fs);
	lowPass20k(yx, fs);

	double num = 0.0, den = 0.0, sig = 0.0, err = 0.0;
	const int from = N / 4;
	for(int i = from; i < N; i++)
	{
		num += yd[i] * yx[i];
		den += yx[i] * yx[i];
	}
	const double g = num / den;
	for(int i = from; i < N; i++)
	{
		double e = yd[i] - g * yx[i];
		err += e * e;
		sig += (g * yx[i]) * (g * yx[i]);
	}
	const double snr = 10.0 * log10(sig / err);
	printf("DSD%d %s: gain %.4f, 20kHz band SNR %.1f dB, largest loop filter state %.3g\n", rate, typeName, g, snr, mod.maxState());

	// The modulated output follows the input at the expected gain.
	EXPECT_NEAR(g, 0.5, 0.01);
	// Well above the floor of the 20kHz low pass used here; a diverged trellis gives < 0 dB.
	EXPECT_GT(snr, 85.0);
	// The 7th integrator of a stable loop sits around 1e6 at these rates.
	EXPECT_LT(mod.maxState(), 1.0e8);
}

} // namespace

//-------------------------------------------------------------------------------------------

TEST(SDMTrellis, stableAtDSD64Float) { testRate<float>(64, "float"); }
TEST(SDMTrellis, stableAtDSD64Double) { testRate<double>(64, "double"); }
TEST(SDMTrellis, stableAtDSD128Float) { testRate<float>(128, "float"); }
TEST(SDMTrellis, stableAtDSD128Double) { testRate<double>(128, "double"); }
TEST(SDMTrellis, stableAtDSD256Float) { testRate<float>(256, "float"); }
TEST(SDMTrellis, stableAtDSD256Double) { testRate<double>(256, "double"); }

//-------------------------------------------------------------------------------------------
