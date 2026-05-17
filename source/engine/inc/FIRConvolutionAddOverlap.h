//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FIRCONVOLUTIONADDOVERLAP_H
#define __OMEGA_ENGINE_FIRCONVOLUTIONADDOVERLAP_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FIRFilterDB.h"
#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2_C2R.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT FIRConvolutionAddOverlap
{
	public:
		FIRConvolutionAddOverlap();
		virtual ~FIRConvolutionAddOverlap();
		
		bool init(const tfloat64 *firFilter, int firSize, int inputSize);
		void process(const tfloat64 *in, tfloat64 *out);

	private:
		// length of FIR filter = firSize
		int m_M;
		// length of input audio block = inputSize
		int m_L;
		// length of FFT (N = L + M - 1)
		int m_N;

		FFTRadix2_R2C m_FFT;
		FFTRadix2_C2R m_iFFT;

		int m_Nout;
		tfloat64 *m_firH;
		tfloat64 *m_in;
		tfloat64 *m_X;
		tfloat64 *m_y;
		tfloat64 *m_olap;

		void done();
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
