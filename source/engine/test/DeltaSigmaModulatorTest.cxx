#include "gtest/gtest.h"

#include "engine/inc/DeltaSigmaModulator.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

void pack1BitEncodingMSB(const uint8_t *x, uint8_t *y, int len)
{
    const uint8_t mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

    memset(y, 0, len >> 3);
    for(int i = 0; i < len; i++)
    {
        if(x[i])
        {
            y[i >> 3] |= mask[i & 0x7];
        }
    }
}

//-------------------------------------------------------------------------------------------

void pack1BitEncodingLSB(const uint8_t *x, uint8_t *y, int len)
{
    const uint8_t mask[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

    memset(y, 0, len >> 3);
    for(int i = 0; i < len; i++)
    {
        if(x[i])
        {
            y[i >> 3] |= mask[i & 0x7];
        }
    }
}

//-------------------------------------------------------------------------------------------

void testDeltaSigmaModulator(bool isLSB)
{
    const double c_noiseShapeCoefficient[2][8] = {
        {
            7.9959288660876267,
            -27.975578426642503,
            55.938953909594289,
            -69.91860869807897,
            55.938953909594282,
            -27.975578426642503,
            7.9959288660876267,
            -0.99999999999999978
        },
        {
            -0.54184091028035863,
            4.6211255639903257,
            -17.319327891504788,
            37.237233820988472,
            -50.213208401223746,
            43.471176498498245,
            -23.588887732076959,
            7.3337285721047616
        }
    };

    const int c_sampleLen = DSM_DELTA_SIZE * 2;
    const double c_incAngle = c_PI_D / 1000.0;

    int i, idx;
    double angle = 0.0;
    double *in = new double [c_sampleLen];
    for(idx = 0; idx < c_sampleLen; idx++)
    {
        in[idx] = sin(angle);
        angle += c_incAngle;
    }

    int s = 0;
    int order = 8;
	double** NS = new double* [2];
	NS[0] = new double[order];
	NS[1] = new double[order];
    for(s = 0; s < order; s++)
    {
        NS[0][s] = c_noiseShapeCoefficient[0][s];
        NS[1][order - s - 1] = c_noiseShapeCoefficient[1][s];
    }
	for(i = 0; i < order; i++) 
    {
		NS[1][i] = NS[1][i];
		NS[0][i] = NS[0][i] - NS[1][i];
	}

	double* deltabuffer = new double[order + 1];
	for (i = 0; i < order; i++) 
    {
		deltabuffer[i] = 0;
	}

	double x_in = 0;
	double error_y = 0;
	double deltagain = 0.5;

    uint8_t *expect = new uint8_t [c_sampleLen];
    uint8_t *expectP = new uint8_t [c_sampleLen >> 3];

    int q, t;
    for (q = 0; q < c_sampleLen; q++)
    {
        x_in = in[q] * deltagain;

        for (t = 0; t < order; t++) 
        {
            x_in += NS[0][t] * deltabuffer[t];
        }

        if (x_in >= 0.0) 
        {
            expect[q] = 1;
            error_y = -1.0;
        }
        else 
        {
            expect[q] = 0;
            error_y = 1.0;
        }
        for (t = order; t > 0; t--) 
        {
            deltabuffer[t] = deltabuffer[t - 1];
        }

        deltabuffer[0] = x_in + error_y;

        for (t = 0; t < order; t++) 
        {
            deltabuffer[0] += NS[1][t] * deltabuffer[t + 1];
        }
    }

    if(isLSB)
    {
        pack1BitEncodingLSB(expect, expectP, c_sampleLen);
    }
    else
    {
        pack1BitEncodingMSB(expect, expectP, c_sampleLen);
    }

    uint8_t *out = new uint8_t [c_sampleLen >> 3];
    engine::DeltaSigmaModulator dsMod;
    dsMod.init(isLSB);
    dsMod.process(in, out, c_sampleLen);

    for(i = 0; i < c_sampleLen >> 3; i++)
    {
        EXPECT_EQ(out[i], expectP[i]);
    }

    delete [] expectP;
    delete [] expect;
    delete [] NS[0];
    delete [] NS[1];
    delete [] NS;
    delete [] in;
}

//-------------------------------------------------------------------------------------------

TEST(DeltaSigmaModulator, processLSB)
{
    testDeltaSigmaModulator(true);
}

//-------------------------------------------------------------------------------------------

TEST(DeltaSigmaModulator, processMSB)
{
    testDeltaSigmaModulator(true);
}

//-------------------------------------------------------------------------------------------
