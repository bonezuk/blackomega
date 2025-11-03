//-------------------------------------------------------------------------------------------
/*
 * Parks-McClellan algorithm for FIR filter design
 *
 * https://en.wikipedia.org/wiki/Parks–McClellan_filter_design_algorithm
 * https://github.com/janovetz/remez-exchange/tree/master
 * https://bitbucket.org/andrzejc/dsp/src/master/
 * git clone https://bitbucket.org/andrzejc/dsp.git
 *
 */
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REMEZFIR_H
#define __OMEGA_ENGINE_REMEZFIR_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/AData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

typedef enum
{
    NEGATIVE = 0,
    POSITIVE = 1
} RemezSymmetryType;

typedef enum
{
    REMEZ_FILTER_BANDPASS,             //!< @internal Band-pass (LP, HP, BP, BS) filter.
    REMEZ_FILTER_DIFFERENTIATOR,    //!< @internal Differentiator.
    REMEZ_FILTER_HILBERT            //!< @internal Hilbert transformer.
} RemezFilterType;

//! @internal Error code indicating that the algorithm was unable to allocate internal arrays.
#define REMEZ_ERRNOMEM         (-1)
//! @internal Warning code indicating that the design was interrupted due to algorithm reaching max iteration count, the results may be invalid.
#define REMEZ_WARNMAXITER    (1)
//! @internal No-error status code.
#define REMEZ_NOERR            (0)
#define REMEZ_FAILED(stat)     ((stat) < 0)

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT RemezFIR
{
    public:

        static bool designBandPass(tint order, tint bandCount, double freqs[], const double amps[],
            const double weights[], double h[]);

        static bool designHilbert(tint order, tint bandCount, double freqs[], const double amps[],
            const double weights[], double h[]);

    private:
        RemezFIR();

        void printError(const tchar *strR, const tchar *strE) const;

        void createDenseGrid(int r, const int numtaps, const int numband, double bands[],
                const double des[], const double weight[], int *gridsize, double Grid[], double D[],
                double W[], RemezSymmetryType symmetry, int grid_density);

        void initialGuess(int r, int Ext[], int gridsize);

        void calcParms(int r, int Ext[], double Grid[], double D[], double W[],
                double ad[], double x[], double y[]);

        double computeA(double freq, int r, double ad[], double x[], double y[]);

        void calcError(int r, double ad[], double x[], double y[], int gridsize,
                double Grid[], double D[], double W[], double E[]);

        void search(int r, int Ext[], int gridsize, double E[], int* foundExt);

        void freqSample(const int N, double A[], double h[], RemezSymmetryType symm);

        short isDone(int r, int Ext[], double E[]);

        bool remez(double h[], const int numtaps, const int numband, double bands[], const double des[],
            const double weight[], const RemezFilterType type, const int grid_density,
            const int max_iterations);

        bool designFIR(tint order, tint bandCount, double freqs[], const double amps[],
            const double weights[], double h[], RemezFilterType type);

};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
