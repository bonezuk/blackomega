//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FFTRADIX2BASE_H
#define __OMEGA_ENGINE_FFTRADIX2BASE_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Complex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT FFTRadix2Base
{
	public:
		FFTRadix2Base();
		virtual ~FFTRadix2Base();

		virtual bool init(int N);

	protected:
		int m_N;
		int *m_reverseIndex;
		tfloat64 **m_coeff;
		tfloat64 **m_stack;
		tfloat64 *m_xIO;

		virtual void done();

		virtual int noBits(int N) const;
		virtual int getReverseIndex(int index, int noBits) const;
		virtual tfloat64 complexMultiplyReal(const tfloat64 *X, const tfloat64 *Y) const;
		virtual tfloat64 complexMultiplyImaginary(const tfloat64 *X, const tfloat64 *Y) const;
		virtual void reverse(const tfloat64* x, tfloat64* y) const;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
