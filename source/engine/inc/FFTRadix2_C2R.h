//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FFTRADIX2_C2R_H
#define __OMEGA_ENGINE_FFTRADIX2_C2R_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FFTRadix2Base.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT FFTRadix2_C2R : public FFTRadix2Base
{
	public:
		FFTRadix2_C2R();
		virtual ~FFTRadix2_C2R();

		virtual bool init(int N);
		virtual void iDFT(const tfloat64* x, tfloat64* X);

	private:

		tfloat64 *m_xIn;

		virtual void done();
		virtual void FFT8(const tfloat64* xIn, tfloat64* X) const;
		virtual void FFTRecursive(const tfloat64 *xIn, tfloat64 *Xout, int index, int N);
		virtual void reorder(const tfloat64 *x, tfloat64 *y);
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
