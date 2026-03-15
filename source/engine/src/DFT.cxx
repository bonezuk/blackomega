#include "engine/inc/DFT.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

Complex *DFT8_Full(Complex *x)
{
	Complex *X = new Complex [8];
	
	for(int k=0;k<8;k++)
	{
		Complex c;
		
		for(int n=0;n<8;n++)
		{
			c += x[n] * Complex::W(k,n,8);
		}
		X[k] = c;
	}
	return X;
}

//-------------------------------------------------------------------------------------------

Complex *DFT16_Full(Complex *x)
{
	Complex *X = new Complex [16];
	
	for(int k=0;k<16;k++)
	{
		Complex c;
		
		for(int n=0;n<16;n++)
		{
			c += x[n] * Complex::W(k,n,16);
		}
		X[k] = c;
	}
	return X;
}

//-------------------------------------------------------------------------------------------

Complex *DFT_N_Full(Complex *x,tint N)
{
	Complex *X = new Complex [N];
	
	for(int k=0;k<N;k++)
	{
		Complex c;
		
		for(int n=0;n<N;n++)
		{
			c += x[n] * Complex::W(k,n,N);
		}
		X[k] = c;
	}
	return X;
}

//-------------------------------------------------------------------------------------------

Complex *IDFT_N_Full(Complex *x,tint N)
{
    tfloat64 invN = 1.0 / static_cast<tfloat64>(N);
    Complex *X = DFT_N_Full(x, N);
    for(int k = 0; k < N; k++)
    {
        X[k].R() *= invN;
        X[k].I() *= invN;
    }
	return X;
}


//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
