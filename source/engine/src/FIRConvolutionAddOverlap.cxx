#include "engine/inc/FIRConvolutionAddOverlap.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

FIRConvolutionAddOverlap::FIRConvolutionAddOverlap() : m_M(0),
	m_L(0),
	m_N(0),
	m_FFT(),
	m_iFFT(),
    m_Nout(0),
	m_firH(NULL),
    m_in(NULL),
    m_X(NULL),
    m_y(NULL),
    m_olap(NULL)
{}

//-------------------------------------------------------------------------------------------

FIRConvolutionAddOverlap::~FIRConvolutionAddOverlap()
{
	FIRConvolutionAddOverlap::done();
}

//-------------------------------------------------------------------------------------------


bool FIRConvolutionAddOverlap::init(const tfloat64 *firFilter, int firSize, int outputSize)
{
	int i;

	m_M = firSize;
	m_L = outputSize;
	m_N = m_L + m_M - 1;

	if(!m_FFT.init(m_N) || !m_iFFT.init(m_N))
	{
		return false;
	}

	m_Nout = (m_N / 2) + 1;

	m_firH = new tfloat64 [m_Nout * 2];
	m_in = new tfloat64 [m_N];
	m_X = new tfloat64 [m_Nout * 2];
	m_y = new tfloat64 [m_N];
	m_olap = new tfloat64 [m_M - 1];
	if(m_firH == NULL || m_in == NULL || m_X == NULL || m_y == NULL || m_olap == NULL)
	{
		return false;
	}
	
	for(i = 0; i < m_M; i++)
	{
		m_in[i] = firFilter[i];
	}
	for(; i < m_N; i++)
	{
		m_in[i] = 0.0;
	}
	m_FFT.DFT(m_in, m_firH);

	for(i = 0; i < m_M - 1; i++)
	{
		m_olap[i] = 0.0;
	}

	return true;
}

//-------------------------------------------------------------------------------------------

void FIRConvolutionAddOverlap::done()
{
	if(m_firH != NULL)
	{
		delete [] m_firH;
		m_firH = NULL;
	}
	if(m_in != NULL)
	{
		delete [] m_in;
		m_in = NULL;
	}
	if(m_X != NULL)
	{
		delete [] m_X;
		m_X = NULL;
	}
	if(m_y != NULL)
	{
		delete [] m_y;
		m_y = NULL;
	}
	if(m_olap != NULL)
	{
		delete [] m_olap;
		m_olap = NULL;
	}
}

//-------------------------------------------------------------------------------------------

void FIRConvolutionAddOverlap::convolution(tfloat64 * out)
{
	int i, j;
	tfloat64(*X)[2] = reinterpret_cast<tfloat64(*)[2]>(m_X);
	tfloat64(*H)[2] = reinterpret_cast<tfloat64(*)[2]>(m_firH);

	for(i = 0; i < m_Nout; i++)
	{
		tfloat64 t[2];
		t[0] = (X[i][0] * H[i][0]) - (X[i][1] * H[i][1]);
		t[1] = (X[i][0] * H[i][1]) + (X[i][1] * H[i][0]);
		X[i][0] = t[0];
		X[i][1] = t[1];
	}

	m_iFFT.iDFT(m_X, m_y);

	for(i = 0; i < m_M - 1; i++)
	{
		m_y[i] += m_olap[i];
	}

	for(i = 0; i < m_L; i++)
	{
		out[i] = m_y[i];
	}

	for(i = m_N + 1 - m_M, j = 0; i < m_N; i++, j++)
	{
		m_olap[j] = m_y[i];
	}
}

//-------------------------------------------------------------------------------------------

void FIRConvolutionAddOverlap::process(const tfloat64 *in, tfloat64 *out)
{
	int i;

	for(i = 0; i < m_L; i++)
	{
		m_in[i] = in[i];
	}
	for(; i < m_N; i++)
	{
		m_in[i] = 0.0;
	}
	m_FFT.DFT(m_in, m_X);

	convolution(out);
}

//-------------------------------------------------------------------------------------------
// FIRConvolutionAddOverlapOctaveUpscale
//-------------------------------------------------------------------------------------------

FIRConvolutionAddOverlapOctaveUpscale::FIRConvolutionAddOverlapOctaveUpscale()
{}

//-------------------------------------------------------------------------------------------

FIRConvolutionAddOverlapOctaveUpscale::~FIRConvolutionAddOverlapOctaveUpscale()
{}

//-------------------------------------------------------------------------------------------

bool FIRConvolutionAddOverlapOctaveUpscale::init(const tfloat64 *firFilter, int firSize, int outputSize)
{
	bool res;

	if(FIRConvolutionAddOverlap::init(firFilter, firSize, outputSize))
	{
		res = m_FFTUpscale.init(m_N);
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void FIRConvolutionAddOverlapOctaveUpscale::process(const tfloat64 *in, tfloat64 *out)
{
	int i;

	for(i = 0; i < m_L / 2; i++)
	{
		m_in[i] = in[i];
	}
	for(; i < m_N / 2; i++)
	{
		m_in[i] = 0.0;
	}
	m_FFTUpscale.DFT(m_in, m_X);

	convolution(out);

	for(i = 0; i < m_L; i++)
	{
		out[i] *= 2.0;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

