//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_COMPLEX_H
#define __OMEGA_ENGINE_COMPLEX_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"
#include "common/inc/Allocation.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/Random.h"
#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Complex
{
	public:
		Complex();
		Complex(const tfloat64& rNum,const tfloat64& iNum);
		Complex(const Complex & rhs);
		
		const Complex& operator = (const Complex& rhs);
		
		const Complex& operator += (const Complex& b);
		const Complex& operator -= (const Complex& b);
		const Complex& operator *= (const Complex& b);
		
		tfloat64& R();
		const tfloat64& R() const;
		tfloat64& I();
		const tfloat64& I() const;
		
		static Complex W(int n,int m,int N);
		static Complex W(int n,int N);
		
	protected:
	
		tfloat64 m_R;
		tfloat64 m_I;
		
		void copy(const Complex& rhs);
};

ENGINE_EXPORT Complex operator + (const Complex& a,const Complex& b);
ENGINE_EXPORT Complex operator - (const Complex& a,const Complex& b);
ENGINE_EXPORT Complex operator * (const Complex& a,const Complex& b);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

