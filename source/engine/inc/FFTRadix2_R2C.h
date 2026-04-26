//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FFTRADIX2_R2C_H
#define __OMEGA_ENGINE_FFTRADIX2_R2C_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FFTRadix2Base.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT FFTRadix2_R2C : public FFTRadix2Base
{
	public:
		FFTRadix2_R2C();
		virtual ~FFTRadix2_R2C();
		
		virtual void DFT(const tfloat64 *x, tfloat64 *X);

	protected:
		virtual void FFT8(const tfloat64* x, tfloat64* Xout) const;
		virtual void FFTRecursive(const tfloat64 *xIn, tfloat64 *Xout, int index, int N);
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
