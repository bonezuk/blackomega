#include "common/inc/Log.h"
#include "engine/inc/RemezFIR.h"

//-------------------------------------------------------------------------------------------

#define BANDPASS       1
#define DIFFERENTIATOR 2
#define HILBERT        3

#define Pi             (c_PI_D) // 3.1415926535897932
#define Pi2            (c_PI_D * 2.0) // 6.2831853071795865

#define GRIDDENSITY    16
#define MAXITERATIONS  40

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine 
{
//-------------------------------------------------------------------------------------------

RemezFIR::RemezFIR()
{}

//-------------------------------------------------------------------------------------------

void RemezFIR::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "RemezFIR::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------
/*
 * createDenseGrid
 *=================
 * Creates the dense grid of frequencies from the specified bands.
 * Also creates the Desired Frequency Response function (D[]) and
 * the Weight function (W[]) on that dense grid
 *
 * INPUT:
 * ------
 * int      r        - 1/2 the number of filter coefficients
 * int      numtaps  - Number of taps in the resulting filter
 * int      numband  - Number of bands in user specification
 * double   bands[]  - User-specified band edges [2*numband]
 * double   des[]    - Desired response at band edges [2*numband]
 * double   weight[] - Weight per band [numband]
 * int      symmetry - Symmetry of filter - used for grid check
 *
 * OUTPUT:
 * -------
 * int    gridsize   - Number of elements in the dense frequency grid
 * double Grid[]     - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double D[]        - Desired response on the dense grid [gridsize]
 * double W[]        - Weight function on the dense grid [gridsize]
 */
//-------------------------------------------------------------------------------------------

void RemezFIR::createDenseGrid(int r, const int numtaps, const int numband, double bands[],
        const double des[], const double weight[], int *gridsize, double Grid[], double D[],
        double W[], RemezSymmetryType symmetry, int grid_density) {
    int i, j, k, band;
    double delf, lowf, highf, lowd, highd, deld;

    delf = 0.5 / (grid_density * r);

    /*
     * For differentiator, hilbert,
     *   symmetry is odd and Grid[0] = max(delf, band[0])
     */

    if ((symmetry == NEGATIVE) && (delf > bands[0]))
        bands[0] = delf;

    j = 0;
    for (band = 0; band < numband; band++) {
        Grid[j] = bands[2 * band];
        lowf = bands[2 * band];
        highf = bands[2 * band + 1];
        lowd = des[2 * band];
        highd = des[2 * band + 1];
        k = (int) ((highf - lowf) / delf + 0.5); /* .5 for rounding */
        deld = (highd - lowd) / k;
        for (i = 0; i < k; i++) {
            D[j] = lowd;
            W[j] = weight[band];
            Grid[j] = lowf;
            lowf += delf;
            lowd += deld;
            j++;
        }
        Grid[j - 1] = highf;
    }

    /*
     * Similar to above, if odd symmetry, last grid point can't be .5
     *  - but, if there are even taps, leave the last grid point at .5
     */
    if ((symmetry == NEGATIVE) && (Grid[*gridsize - 1] > (0.5 - delf))
            && (numtaps % 2)) {
        Grid[*gridsize - 1] = 0.5 - delf;
    }
}

//-------------------------------------------------------------------------------------------
/*
 * initialGuess
 *==============
 * Places Extremal Frequencies evenly throughout the dense grid.
 *
 * INPUT: 
 * ------
 * int r        - 1/2 the number of filter coefficients
 * int gridsize - Number of elements in the dense frequency grid
 *
 * OUTPUT:
 * -------
 * int Ext[]    - Extremal indexes to dense frequency grid [r+1]
 */
//-------------------------------------------------------------------------------------------

void RemezFIR::initialGuess(int r, int Ext[], int gridsize) {
    int i;

    for (i = 0; i <= r; i++)
        Ext[i] = i * (gridsize - 1) / r;
}

//-------------------------------------------------------------------------------------------
/*
 * calcParms
 *===========
 *
 * INPUT:
 * ------
 * int    r      - 1/2 the number of filter coefficients
 * int    Ext[]  - Extremal indexes to dense frequency grid [r+1]
 * double Grid[] - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double D[]    - Desired response on the dense grid [gridsize]
 * double W[]    - Weight function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double ad[]   - 'b' in Oppenheim & Schafer [r+1]
 * double x[]    - [r+1]
 * double y[]    - 'C' in Oppenheim & Schafer [r+1]
 */
//-------------------------------------------------------------------------------------------

void RemezFIR::calcParms(int r, int Ext[], double Grid[], double D[], double W[],
        double ad[], double x[], double y[]) {
    int i, j, k, ld;
    double sign, xi, delta, denom, numer;

    /*
     * Find x[]
     */
    for (i = 0; i <= r; i++)
        x[i] = cos(Pi2 * Grid[Ext[i]]);

    /*
     * Calculate ad[]  - Oppenheim & Schafer eq 7.132
     */
    ld = (r - 1) / 15 + 1; /* Skips around to avoid round errors */
    for (i = 0; i <= r; i++) {
        denom = 1.0;
        xi = x[i];
        for (j = 0; j < ld; j++) {
            for (k = j; k <= r; k += ld)
                if (k != i)
                    denom *= 2.0 * (xi - x[k]);
        }
        if (::fabs(denom) < 0.00001)
            denom = 0.00001;
        ad[i] = 1.0 / denom;
    }

    /*
     * Calculate delta  - Oppenheim & Schafer eq 7.131
     */
    numer = denom = 0;
    sign = 1;
    for (i = 0; i <= r; i++) {
        numer += ad[i] * D[Ext[i]];
        denom += sign * ad[i] / W[Ext[i]];
        sign = -sign;
    }
    delta = numer / denom;
    sign = 1;

    /*
     * Calculate y[]  - Oppenheim & Schafer eq 7.133b
     */
    for (i = 0; i <= r; i++) {
        y[i] = D[Ext[i]] - sign * delta / W[Ext[i]];
        sign = -sign;
    }
}

//-------------------------------------------------------------------------------------------
/*
 * computeA
 *
 * Using values calculated in CalcParms, ComputeA calculates the
 * actual filter response at a given frequency (freq).  Uses
 * eq 7.133a from Oppenheim & Schafer.
 *
 * INPUT:
 * ------
 * double freq - Frequency (0 to 0.5) at which to calculate A
 * int    r    - 1/2 the number of filter coefficients
 * double ad[] - 'b' in Oppenheim & Schafer [r+1]
 * double x[]  - [r+1]
 * double y[]  - 'C' in Oppenheim & Schafer [r+1]
 *
 * OUTPUT:
 * -------
 * Returns double value of A[freq]
 */
//-------------------------------------------------------------------------------------------

double RemezFIR::computeA(double freq, int r, double ad[], double x[], double y[]) {
    int i;
    double xc, c, denom, numer;

    denom = numer = 0;
    xc = cos(Pi2 * freq);
    for (i = 0; i <= r; i++) {
        c = xc - x[i];
        if (fabs(c) < 1.0e-7) {
            numer = y[i];
            denom = 1;
            break;
        }
        c = ad[i] / c;
        denom += c;
        numer += c * y[i];
    }
    return numer / denom;
}

//-------------------------------------------------------------------------------------------
/*
 * calcError
 *
 * Calculates the Error function from the desired frequency response
 * on the dense grid (D[]), the weight function on the dense grid (W[]),
 * and the present response calculation (A[])
 *
 * INPUT:
 * ------
 * int    r      - 1/2 the number of filter coefficients
 * double ad[]   - [r+1]
 * double x[]    - [r+1]
 * double y[]    - [r+1]
 * int gridsize  - Number of elements in the dense frequency grid
 * double Grid[] - Frequencies on the dense grid [gridsize]
 * double D[]    - Desired response on the dense grid [gridsize]
 * double W[]    - Weight function on the desnse grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double E[]    - Error function on dense grid [gridsize]
 */
//-------------------------------------------------------------------------------------------

void RemezFIR::calcError(int r, double ad[], double x[], double y[], int gridsize,
        double Grid[], double D[], double W[], double E[]) {
    int i;
    double A;

    for (i = 0; i < gridsize; i++) {
        A = computeA(Grid[i], r, ad, x, y);
        E[i] = W[i] * (D[i] - A);
    }
}

//-------------------------------------------------------------------------------------------
/*
 * search
 *
 * Searches for the maxima/minima of the error curve.  If more than
 * r+1 extrema are found, it uses the following heuristic (thanks
 * Chris Hanson):
 * 1) Adjacent non-alternating extrema deleted first.
 * 2) If there are more than one excess extrema, delete the
 *    one with the smallest error.  This will create a non-alternation
 *    condition that is fixed by 1).
 * 3) If there is exactly one excess extremum, delete the smaller
 *    of the first/last extremum
 *
 * INPUT:
 * ------
 * int    r        - 1/2 the number of filter coefficients
 * int    Ext[]    - Indexes to Grid[] of extremal frequencies [r+1]
 * int    gridsize - Number of elements in the dense frequency grid
 * double E[]      - Array of error values.  [gridsize]
 *
 * OUTPUT:
 * -------
 * int    Ext[]    - New indexes to extremal frequencies [r+1]
 */
//-------------------------------------------------------------------------------------------

void RemezFIR::search(int r, int Ext[], int gridsize, double E[], int* foundExt) {
    int i, j, k, l, extra; /* Counters */
    int up, alt;

    /*
     * Allocate enough space for found extremals.
     */
    k = 0;

    /*
     * Check for extremum at 0.
     */
    if (((E[0] > 0.0) && (E[0] > E[1])) || ((E[0] < 0.0) && (E[0] < E[1])))
        foundExt[k++] = 0;

    /*
     * Check for extrema inside dense grid
     */
    for (i = 1; i < gridsize - 1; i++) {
        if (((E[i] >= E[i - 1]) && (E[i] > E[i + 1]) && (E[i] > 0.0))
                || ((E[i] <= E[i - 1]) && (E[i] < E[i + 1]) && (E[i] < 0.0)))
            foundExt[k++] = i;
    }

    /*
     * Check for extremum at 0.5
     */
    j = gridsize - 1;
    if (((E[j] > 0.0) && (E[j] > E[j - 1]))
            || ((E[j] < 0.0) && (E[j] < E[j - 1])))
        foundExt[k++] = j;

    /*
     * Remove extra extremals
     */
    extra = k - (r + 1);

    while (extra > 0) {
        if (E[foundExt[0]] > 0.0)
            up = 1; /* first one is a maxima */
        else
            up = 0; /* first one is a minima */

        l = 0;
        alt = 1;
        for (j = 1; j < k; j++) {
            if (fabs(E[foundExt[j]]) < fabs(E[foundExt[l]]))
                l = j; /* new smallest error. */
            if ((up) && (E[foundExt[j]] < 0.0))
                up = 0; /* switch to a minima */
            else if ((!up) && (E[foundExt[j]] > 0.0))
                up = 1; /* switch to a maxima */
            else {
                alt = 0;
                break; /* Ooops, found two non-alternating */
            } /* extrema.  Delete smallest of them */
        } /* if the loop finishes, all extrema are alternating */

        /*
         * If there's only one extremal and all are alternating,
         * delete the smallest of the first/last extremals.
         */
        if ((alt) && (extra == 1)) {
            if (fabs(E[foundExt[k - 1]]) < fabs(E[foundExt[0]]))
                l = foundExt[k - 1]; /* Delete last extremal */
            else
                l = foundExt[0]; /* Delete first extremal */
        }

        for (j = l; j < k; j++) /* Loop that does the deletion */
        {
            foundExt[j] = foundExt[j + 1];
        }
        k--;
        extra--;
    }

    for (i = 0; i <= r; i++) {
        Ext[i] = foundExt[i]; /* Copy found extremals to Ext[] */
    }
}

//-------------------------------------------------------------------------------------------
/*
 * FreqSample
 *
 * Simple frequency sampling algorithm to determine the impulse
 * response h[] from A's found in ComputeA
 *
 * INPUT:
 * ------
 * int      N        - Number of filter coefficients
 * double   A[]      - Sample points of desired response [N/2]
 * int      symmetry - Symmetry of desired filter
 *
 * OUTPUT:
 * -------
 * double h[] - Impulse Response of final filter [N]
 */
//-------------------------------------------------------------------------------------------

void RemezFIR::freqSample(const int N, double A[], double h[], RemezSymmetryType symm) {
    int n, k;
    double x, val, M;

    M = (N - 1.0) / 2.0;
    if (symm == POSITIVE) {
        if (N % 2) {
            for (n = 0; n < N; n++) {
                val = A[0];
                x = Pi2 * (n - M) / N;
                for (k = 1; k <= M; k++)
                    val += 2.0 * A[k] * ::cos(x * k);
                h[n] = val / N;
            }
        } else {
            for (n = 0; n < N; n++) {
                val = A[0];
                x = Pi2 * (n - M) / N;
                for (k = 1; k <= (N / 2 - 1); k++)
                    val += 2.0 * A[k] * ::cos(x * k);
                h[n] = val / N;
            }
        }
    } else {
        if (N % 2) {
            for (n = 0; n < N; n++) {
                val = 0;
                x = Pi2 * (n - M) / N;
                for (k = 1; k <= M; k++)
                    val += 2.0 * A[k] * ::sin(x * k);
                h[n] = val / N;
            }
        } else {
            for (n = 0; n < N; n++) {
                val = A[N / 2] * ::sin(Pi * (n - M));
                x = Pi2 * (n - M) / N;
                for (k = 1; k <= (N / 2 - 1); k++)
                    val += 2.0 * A[k] * ::sin(x * k);
                h[n] = val / N;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
/*
 * isDone
 *
 * Checks to see if the error function is small enough to consider
 * the result to have converged.
 *
 * INPUT:
 * ------
 * int    r     - 1/2 the number of filter coeffiecients
 * int    Ext[] - Indexes to extremal frequencies [r+1]
 * double E[]   - Error function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * Returns 1 if the result converged
 * Returns 0 if the result has not converged
 */
//-------------------------------------------------------------------------------------------

short RemezFIR::isDone(int r, int Ext[], double E[]) {
    int i;
    double min, max, current;

    min = max = ::fabs(E[Ext[0]]);
    for (i = 1; i <= r; i++) {
        current = ::fabs(E[Ext[i]]);
        if (current < min)
            min = current;
        if (current > max)
            max = current;
    }
    if (((max - min) / max) < 0.0001)
        return 1;
    return 0;
}

//-------------------------------------------------------------------------------------------
/*
 * remez
 *
 * Calculates the optimal (in the Chebyshev/minimax sense)
 * FIR filter impulse response given a set of band edges,
 * the desired reponse on those bands, and the weight given to
 * the error in those bands.
 *
 * INPUT:
 * ------
 * int     numtaps     - Number of filter coefficients
 * int     numband     - Number of bands in filter specification
 * double  bands[]     - User-specified band edges [2 * numband]
 * double  des[]       - User-specified responses at band edges [2 * numband]
 * double  weight[]    - User-specified error weights [numband]
 * int     type        - Type of filter
 *
 * OUTPUT:
 * -------
 * double h[]      - Impulse response of final filter [numtaps]
 */
//-------------------------------------------------------------------------------------------

bool RemezFIR::remez(double h[], const int numtaps, const int numband, double bands[], const double des[],
    const double weight[], const RemezFilterType type, const int grid_density,
    const int max_iterations)
{
    double *Grid, *W, *D, *E, *mem;
    int i, iter, gridsize, r, *Ext, *imem, *foundExt;
    double *taps, c;
    double *x, *y, *ad;
    RemezSymmetryType symmetry;
    int status = REMEZ_NOERR;

    if (type == REMEZ_FILTER_BANDPASS)
        symmetry = POSITIVE;
    else
        symmetry = NEGATIVE;

    r = numtaps / 2; /* number of extrema */
    if ((numtaps % 2) && (symmetry == POSITIVE))
        r++;

    /*
     * Predict dense grid size in advance for memory allocation
     *   .5 is so we round up, not truncate
     */
    gridsize = 0;
    for (i = 0; i < numband; i++) {
        gridsize += (int) (2 * r * grid_density
                * (bands[2 * i + 1] - bands[2 * i]) + .5);
    }
    if (symmetry == NEGATIVE) {
        gridsize--;
    }

    /*
     * Dynamically allocate memory for arrays with proper sizes
     */
    mem = (double*)::malloc(4 * (gridsize + r + 1) * sizeof(double));
    imem = (int*)::malloc((3 * r + 1) * sizeof(int));
    if (NULL == mem || NULL == imem)
    {
        status = REMEZ_ERRNOMEM;
        goto finish;
    }

    foundExt = imem;
    Ext = foundExt + (2 * r);

    Grid = mem;
    D = Grid + gridsize;
    W = D + gridsize;
    E = W + gridsize;
    taps = E + gridsize;
    x = taps + (r + 1);
    y = x + (r + 1);
    ad = y + (r + 1);

    /*
     * Create dense frequency grid
     */
    createDenseGrid(r, numtaps, numband, bands, des, weight, &gridsize, Grid, D,
            W, symmetry, grid_density);
    initialGuess(r, Ext, gridsize);

    /*
     * For Differentiator: (fix grid)
     */
    if (type == REMEZ_FILTER_DIFFERENTIATOR) {
        for (i = 0; i < gridsize; i++) {
            /* D[i] = D[i]*Grid[i]; */
            if (D[i] > 0.0001)
                W[i] = W[i] / Grid[i];
        }
    }

    /*
     * For odd or Negative symmetry filters, alter the
     * D[] and W[] according to Parks McClellan
     */
    if (symmetry == POSITIVE) {
        if (numtaps % 2 == 0) {
            for (i = 0; i < gridsize; i++) {
                c = ::cos(Pi * Grid[i]);
                D[i] /= c;
                W[i] *= c;
            }
        }
    } else {
        if (numtaps % 2) {
            for (i = 0; i < gridsize; i++) {
                c = ::sin(Pi2 * Grid[i]);
                D[i] /= c;
                W[i] *= c;
            }
        } else {
            for (i = 0; i < gridsize; i++) {
                c = ::sin(Pi * Grid[i]);
                D[i] /= c;
                W[i] *= c;
            }
        }
    }

    /*
     * Perform the Remez Exchange algorithm
     */
    for (iter = 0; iter < max_iterations; iter++)
    {
        calcParms(r, Ext, Grid, D, W, ad, x, y);
        calcError(r, ad, x, y, gridsize, Grid, D, W, E);
        search(r, Ext, gridsize, E, foundExt);

        if (isDone(r, Ext, E))
            break;
    }
    if (iter == max_iterations) {
        /* printf("Reached maximum iteration count.\nResults may be bad.\n"); */
        status |= REMEZ_WARNMAXITER;
    }

    calcParms(r, Ext, Grid, D, W, ad, x, y);

    /*
     * Find the 'taps' of the filter for use with Frequency
     * Sampling.  If odd or Negative symmetry, fix the taps
     * according to Parks McClellan
     */
    for (i = 0; i <= numtaps / 2; i++) {
        if (symmetry == POSITIVE) {
            if (numtaps % 2)
                c = 1;
            else
                c = ::cos(Pi * (double) i / numtaps);
        } else {
            if (numtaps % 2)
                c = ::sin(Pi2 * (double) i / numtaps);
            else
                c = ::sin(Pi * (double) i / numtaps);
        }
        taps[i] = computeA((double) i / numtaps, r, ad, x, y) * c;
    }

    /*
     * Frequency sampling design with calculated taps
     */
    freqSample(numtaps, taps, h, symmetry);

finish:
    /*
     * Delete allocated memory
     */
    ::free(imem);
    ::free(mem);
    return status;
}

//-------------------------------------------------------------------------------------------

bool RemezFIR::designFIR(tint order, tint bandCount, double freqs[], const double amps[],
    const double weights[], double h[], RemezFilterType type)
{
    const tint grid_density_default = 16;
    const tint max_iterations_default = 32;

    int rRes;
    double lf = 0.0;

    for (tint i = 0; i < 2 * bandCount; ++i)
    {
        double f = freqs[i];
        if (f < 0.0 || f > 0.5)
        {
            printError("designFIR","normalized frequencies outside [0, 0.5]");
            return false;
        }
        if (f < lf)
        {
            printError("designFIR","freqs non-monotonic");
            return false;
        }
        lf = f;
    }
    for (tint i = 0; i < bandCount; ++i)
    {
        if (weights[i] <= 0.0)
        {
            printError("designFIR","weights <= 0");
            return false;
        }
    }
    
    rRes = remez(h, order + 1, bandCount, freqs, amps, weights, type, grid_density_default, max_iterations_default);

    return (rRes == REMEZ_NOERR) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool RemezFIR::designBandPass(tint order, tint bandCount, double freqs[], const double amps[],
            const double weights[], double h[])
{
    RemezFIR r;
    return r.designFIR(order, bandCount, freqs, amps, weights, h, REMEZ_FILTER_BANDPASS);
}

//-------------------------------------------------------------------------------------------

bool RemezFIR::designHilbert(tint order, tint bandCount, double freqs[], const double amps[],
            const double weights[], double h[])
{
    RemezFIR r;
    return r.designFIR(order, bandCount, freqs, amps, weights, h, REMEZ_FILTER_HILBERT);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
