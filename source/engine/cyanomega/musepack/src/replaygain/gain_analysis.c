/*
 *  ReplayGainAnalysis - analyzes input samples and give the recommended dB change
 *  Copyright (C) 2001 David Robinson and Glen Sawyer
 *  Improvements and optimizations added by Frank Klemm, and by Marcel Mller
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  concept and filter values by David Robinson (David@Robinson.org)
 *    -- blame him if you think the idea is flawed
 *  original coding by Glen Sawyer (mp3gain@hotmail.com)
 *    -- blame him if you think this runs too slowly, or the coding is otherwise flawed
 *
 *  lots of code improvements by Frank Klemm ( http://www.uni-jena.de/~pfk/mpp/ )
 *    -- credit him for all the _good_ programming ;)
 *
 *
 *  For an explanation of the concepts and the basic algorithms involved, go to:
 *    http://www.replaygain.org/
 */

/*
 *  So here's the main source of potential code confusion:
 *
 *  The filters applied to the incoming samples are IIR filters,
 *  meaning they rely on up to <filter order> number of previous samples
 *  AND up to <filter order> number of previous filtered samples.
 *
 *  I set up the gain_analyze_samples routine to minimize memory usage and interface
 *  complexity. The speed isn't compromised too much (I don't think), but the
 *  internal complexity is higher than it should be for such a relatively
 *  simple routine.
 *
 *  Optimization/clarity suggestions are welcome.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <replaygain/gain_analysis.h>

typedef unsigned short  Uint16_t;
typedef signed short    Int16_t;
typedef unsigned int    Uint32_t;
typedef signed int      Int32_t;

#define YULE_ORDER         10
#define BUTTER_ORDER        2
#define YULE_FILTER     filterYule
#define BUTTER_FILTER   filterButter
#define RMS_PERCENTILE      0.95        // percentile which is louder than the proposed level
#define MAX_SAMP_FREQ   48000.          // maximum allowed sample frequency [Hz]
#define RMS_WINDOW_TIME     0.050       // Time slice size [s]
#define STEPS_per_dB      100           // Table entries per dB
#define MAX_dB            120           // Table entries for 0...MAX_dB (normal max. values are 70...80 dB)

#define MAX_ORDER               (BUTTER_ORDER > YULE_ORDER ? BUTTER_ORDER : YULE_ORDER)
#define MAX_SAMPLES_PER_WINDOW  (size_t) (MAX_SAMP_FREQ * RMS_WINDOW_TIME)      // max. Samples per Time slice
#define ANALYZE_SIZE			(STEPS_per_dB * MAX_dB)
#define PINK_REF                64.82 //298640883795                              // calibration value

static Float_t  linprebuf [MAX_ORDER * 2];
static Float_t* linpre;                                          // left input samples, with pre-buffer
static Float_t  lstepbuf  [MAX_SAMPLES_PER_WINDOW + MAX_ORDER];
static Float_t* lstep;                                           // left "first step" (i.e. post first filter) samples
static Float_t  loutbuf   [MAX_SAMPLES_PER_WINDOW + MAX_ORDER];
static Float_t* lout;                                            // left "out" (i.e. post second filter) samples
static Float_t  rinprebuf [MAX_ORDER * 2];
static Float_t* rinpre;                                          // right input samples ...
static Float_t  rstepbuf  [MAX_SAMPLES_PER_WINDOW + MAX_ORDER];
static Float_t* rstep;
static Float_t  routbuf   [MAX_SAMPLES_PER_WINDOW + MAX_ORDER];
static Float_t* rout;
static long     sampleWindow;                                    // number of samples required to reach number of milliseconds required for RMS window
static long     totsamp;
static double   lsum;
static double   rsum;
static int      freqindex;
static int      first;
static int		chap_void;	// if == 0 gain_analyze_samples as been called
static Uint32_t A [ANALYZE_SIZE];	// chapter
static Uint32_t B [ANALYZE_SIZE];	// title
static Uint32_t C [ANALYZE_SIZE];	// album

// for each filter:
// [0] 48 kHz, [1] 44.1 kHz, [2] 32 kHz, [3] 24 kHz, [4] 22050 Hz, [5] 16 kHz, [6] 12 kHz, [7] is 11025 Hz, [8] 8 kHz

#ifdef WIN32
#ifndef __GNUC__
#pragma warning ( disable : 4305 )
#endif
#endif

static const Float_t ABYule[9][2*YULE_ORDER + 1] = {
    {0.03857599435200, -3.84664617118067, -0.02160367184185,  7.81501653005538, -0.00123395316851,-11.34170355132042, -0.00009291677959, 13.05504219327545, -0.01655260341619,-12.28759895145294,  0.02161526843274,  9.48293806319790, -0.02074045215285, -5.87257861775999,  0.00594298065125,  2.75465861874613,  0.00306428023191, -0.86984376593551,  0.00012025322027,  0.13919314567432,  0.00288463683916 },
    {0.05418656406430, -3.47845948550071, -0.02911007808948,  6.36317777566148, -0.00848709379851, -8.54751527471874, -0.00851165645469,  9.47693607801280, -0.00834990904936, -8.81498681370155,  0.02245293253339,  6.85401540936998, -0.02596338512915, -4.39470996079559,  0.01624864962975,  2.19611684890774, -0.00240879051584, -0.75104302451432,  0.00674613682247,  0.13149317958808, -0.00187763777362 },
    {0.15457299681924, -2.37898834973084, -0.09331049056315,  2.84868151156327, -0.06247880153653, -2.64577170229825,  0.02163541888798,  2.23697657451713, -0.05588393329856, -1.67148153367602,  0.04781476674921,  1.00595954808547,  0.00222312597743, -0.45953458054983,  0.03174092540049,  0.16378164858596, -0.01390589421898, -0.05032077717131,  0.00651420667831,  0.02347897407020, -0.00881362733839 },
    {0.30296907319327, -1.61273165137247, -0.22613988682123,  1.07977492259970, -0.08587323730772, -0.25656257754070,  0.03282930172664, -0.16276719120440, -0.00915702933434, -0.22638893773906, -0.02364141202522,  0.39120800788284, -0.00584456039913, -0.22138138954925,  0.06276101321749,  0.04500235387352, -0.00000828086748,  0.02005851806501,  0.00205861885564,  0.00302439095741, -0.02950134983287 },
    {0.33642304856132, -1.49858979367799, -0.25572241425570,  0.87350271418188, -0.11828570177555,  0.12205022308084,  0.11921148675203, -0.80774944671438, -0.07834489609479,  0.47854794562326, -0.00469977914380, -0.12453458140019, -0.00589500224440, -0.04067510197014,  0.05724228140351,  0.08333755284107,  0.00832043980773, -0.04237348025746, -0.01635381384540,  0.02977207319925, -0.01760176568150 },
    {0.44915256608450, -0.62820619233671, -0.14351757464547,  0.29661783706366, -0.22784394429749, -0.37256372942400, -0.01419140100551,  0.00213767857124,  0.04078262797139, -0.42029820170918, -0.12398163381748,  0.22199650564824,  0.04097565135648,  0.00613424350682,  0.10478503600251,  0.06747620744683, -0.01863887810927,  0.05784820375801, -0.03193428438915,  0.03222754072173,  0.00541907748707 },
    {0.56619470757641, -1.04800335126349, -0.75464456939302,  0.29156311971249,  0.16242137742230, -0.26806001042947,  0.16744243493672,  0.00819999645858, -0.18901604199609,  0.45054734505008,  0.30931782841830, -0.33032403314006, -0.27562961986224,  0.06739368333110,  0.00647310677246, -0.04784254229033,  0.08647503780351,  0.01639907836189, -0.03788984554840,  0.01807364323573, -0.00588215443421 },
    {0.58100494960553, -0.51035327095184, -0.53174909058578, -0.31863563325245, -0.14289799034253, -0.20256413484477,  0.17520704835522,  0.14728154134330,  0.02377945217615,  0.38952639978999,  0.15558449135573, -0.23313271880868, -0.25344790059353, -0.05246019024463,  0.01628462406333, -0.02505961724053,  0.06920467763959,  0.02442357316099, -0.03721611395801,  0.01818801111503, -0.00749618797172 },
    {0.53648789255105, -0.25049871956020, -0.42163034350696, -0.43193942311114, -0.00275953611929, -0.03424681017675,  0.04267842219415, -0.04678328784242, -0.10214864179676,  0.26408300200955,  0.14590772289388,  0.15113130533216, -0.02459864859345, -0.17556493366449, -0.11202315195388, -0.18823009262115, -0.04060034127000,  0.05477720428674,  0.04788665548180,  0.04704409688120, -0.02217936801134 }
};

static const Float_t ABButter[9][2*BUTTER_ORDER + 1] = {
    {0.98621192462708, -1.97223372919527, -1.97242384925416,  0.97261396931306,  0.98621192462708 },
    {0.98500175787242, -1.96977855582618, -1.97000351574484,  0.97022847566350,  0.98500175787242 },
    {0.97938932735214, -1.95835380975398, -1.95877865470428,  0.95920349965459,  0.97938932735214 },
    {0.97531843204928, -1.95002759149878, -1.95063686409857,  0.95124613669835,  0.97531843204928 },
    {0.97316523498161, -1.94561023566527, -1.94633046996323,  0.94705070426118,  0.97316523498161 },
    {0.96454515552826, -1.92783286977036, -1.92909031105652,  0.93034775234268,  0.96454515552826 },
    {0.96009142950541, -1.91858953033784, -1.92018285901082,  0.92177618768381,  0.96009142950541 },
    {0.95856916599601, -1.91542108074780, -1.91713833199203,  0.91885558323625,  0.95856916599601 },
    {0.94597685600279, -1.88903307939452, -1.89195371200558,  0.89487434461664,  0.94597685600279 }
};


#ifdef WIN32
#ifndef __GNUC__
#pragma warning ( default : 4305 )
#endif
#endif

// When calling these filter procedures, make sure that ip[-order] and op[-order] point to real data!

// If your compiler complains that "'operation on 'output' may be undefined", you can
// either ignore the warnings or uncomment the three "y" lines (and comment out the indicated line)

static void
filterYule (const Float_t* input, Float_t* output, size_t nSamples, const Float_t* kernel)
{

    while (nSamples--) {
       *output =  1e-10  /* 1e-10 is a hack to avoid slowdown because of denormals */
         + input [0]  * kernel[0]
         - output[-1] * kernel[1]
         + input [-1] * kernel[2]
         - output[-2] * kernel[3]
         + input [-2] * kernel[4]
         - output[-3] * kernel[5]
         + input [-3] * kernel[6]
         - output[-4] * kernel[7]
         + input [-4] * kernel[8]
         - output[-5] * kernel[9]
         + input [-5] * kernel[10]
         - output[-6] * kernel[11]
         + input [-6] * kernel[12]
         - output[-7] * kernel[13]
         + input [-7] * kernel[14]
         - output[-8] * kernel[15]
         + input [-8] * kernel[16]
         - output[-9] * kernel[17]
         + input [-9] * kernel[18]
         - output[-10]* kernel[19]
         + input [-10]* kernel[20];
        ++output;
        ++input;
    }
}

static void
filterButter (const Float_t* input, Float_t* output, size_t nSamples, const Float_t* kernel)
{

    while (nSamples--) {
        *output =
           input [0]  * kernel[0]
         - output[-1] * kernel[1]
         + input [-1] * kernel[2]
         - output[-2] * kernel[3]
         + input [-2] * kernel[4];
        ++output;
        ++input;
    }
}


// returns a INIT_GAIN_ANALYSIS_OK if successful, INIT_GAIN_ANALYSIS_ERROR if not

static int
ResetSampleFrequency ( long samplefreq ) {
    int  i;

    // zero out initial values
    for ( i = 0; i < MAX_ORDER; i++ )
        linprebuf[i] = lstepbuf[i] = loutbuf[i] = rinprebuf[i] = rstepbuf[i] = routbuf[i] = 0.;

    switch ( (int)(samplefreq) ) {
        case 48000: freqindex = 0; break;
        case 44100: freqindex = 1; break;
        case 32000: freqindex = 2; break;
        case 24000: freqindex = 3; break;
        case 22050: freqindex = 4; break;
        case 16000: freqindex = 5; break;
        case 12000: freqindex = 6; break;
        case 11025: freqindex = 7; break;
        case  8000: freqindex = 8; break;
        default:    return INIT_GAIN_ANALYSIS_ERROR;
    }

    sampleWindow = (int) ceil (samplefreq * RMS_WINDOW_TIME);

    lsum         = 0.;
    rsum         = 0.;
    totsamp      = 0;

    memset ( A, 0, sizeof(A) );

    return INIT_GAIN_ANALYSIS_OK;
}

int
gain_init_analysis ( long samplefreq )
{
    if (ResetSampleFrequency(samplefreq) != INIT_GAIN_ANALYSIS_OK) {
        return INIT_GAIN_ANALYSIS_ERROR;
    }

    linpre       = linprebuf + MAX_ORDER;
    rinpre       = rinprebuf + MAX_ORDER;
    lstep        = lstepbuf  + MAX_ORDER;
    rstep        = rstepbuf  + MAX_ORDER;
    lout         = loutbuf   + MAX_ORDER;
    rout         = routbuf   + MAX_ORDER;

    memset ( B, 0, sizeof(B) );
	memset ( C, 0, sizeof(C) );
	chap_void = 1;

    return INIT_GAIN_ANALYSIS_OK;
}

// returns GAIN_ANALYSIS_OK if successful, GAIN_ANALYSIS_ERROR if not

static __inline double fsqr(const double d)
{  return d*d;
}

int
gain_analyze_samples ( const Float_t* left_samples, const Float_t* right_samples, size_t num_samples, int num_channels )
{
    const Float_t*  curleft;
    const Float_t*  curright;
    long            batchsamples;
    long            cursamples;
    long            cursamplepos;
    int             i;

    if ( num_samples == 0 )
        return GAIN_ANALYSIS_OK;

    cursamplepos = 0;
    batchsamples = (long)num_samples;

    switch ( num_channels) {
    case  1: right_samples = left_samples;
    case  2: break;
    default: return GAIN_ANALYSIS_ERROR;
    }

    if ( num_samples < MAX_ORDER ) {
        memcpy ( linprebuf + MAX_ORDER, left_samples , num_samples * sizeof(Float_t) );
        memcpy ( rinprebuf + MAX_ORDER, right_samples, num_samples * sizeof(Float_t) );
    }
    else {
        memcpy ( linprebuf + MAX_ORDER, left_samples,  MAX_ORDER   * sizeof(Float_t) );
        memcpy ( rinprebuf + MAX_ORDER, right_samples, MAX_ORDER   * sizeof(Float_t) );
    }

    while ( batchsamples > 0 ) {
        cursamples = batchsamples > sampleWindow-totsamp  ?  sampleWindow - totsamp  :  batchsamples;
        if ( cursamplepos < MAX_ORDER ) {
            curleft  = linpre+cursamplepos;
            curright = rinpre+cursamplepos;
            if (cursamples > MAX_ORDER - cursamplepos )
                cursamples = MAX_ORDER - cursamplepos;
        }
        else {
            curleft  = left_samples  + cursamplepos;
            curright = right_samples + cursamplepos;
        }

        YULE_FILTER ( curleft , lstep + totsamp, cursamples, ABYule[freqindex]);
        YULE_FILTER ( curright, rstep + totsamp, cursamples, ABYule[freqindex]);

        BUTTER_FILTER ( lstep + totsamp, lout + totsamp, cursamples, ABButter[freqindex]);
        BUTTER_FILTER ( rstep + totsamp, rout + totsamp, cursamples, ABButter[freqindex]);

        curleft = lout + totsamp;                   // Get the squared values
        curright = rout + totsamp;

        i = cursamples % 16;
        while (i--)
        {   lsum += fsqr(*curleft++);
            rsum += fsqr(*curright++);
        }
        i = cursamples / 16;
        while (i--)
        {   lsum += fsqr(curleft[0])
                  + fsqr(curleft[1])
                  + fsqr(curleft[2])
                  + fsqr(curleft[3])
                  + fsqr(curleft[4])
                  + fsqr(curleft[5])
                  + fsqr(curleft[6])
                  + fsqr(curleft[7])
                  + fsqr(curleft[8])
                  + fsqr(curleft[9])
                  + fsqr(curleft[10])
                  + fsqr(curleft[11])
                  + fsqr(curleft[12])
                  + fsqr(curleft[13])
                  + fsqr(curleft[14])
                  + fsqr(curleft[15]);
            curleft += 16;
            rsum += fsqr(curright[0])
                  + fsqr(curright[1])
                  + fsqr(curright[2])
                  + fsqr(curright[3])
                  + fsqr(curright[4])
                  + fsqr(curright[5])
                  + fsqr(curright[6])
                  + fsqr(curright[7])
                  + fsqr(curright[8])
                  + fsqr(curright[9])
                  + fsqr(curright[10])
                  + fsqr(curright[11])
                  + fsqr(curright[12])
                  + fsqr(curright[13])
                  + fsqr(curright[14])
                  + fsqr(curright[15]);
            curright += 16;
        }

        batchsamples -= cursamples;
        cursamplepos += cursamples;
        totsamp      += cursamples;
        if ( totsamp == sampleWindow ) {  // Get the Root Mean Square (RMS) for this set of samples
            double  val  = STEPS_per_dB * 10. * log10 ( (lsum+rsum) / totsamp * 0.5 + 1.e-37 );
            int     ival = (int) val;
            if ( ival <                     0 ) ival = 0;
			if ( ival >= ANALYZE_SIZE ) ival = ANALYZE_SIZE - 1;
            A [ival]++;
            lsum = rsum = 0.;
            memmove ( loutbuf , loutbuf  + totsamp, MAX_ORDER * sizeof(Float_t) );
            memmove ( routbuf , routbuf  + totsamp, MAX_ORDER * sizeof(Float_t) );
            memmove ( lstepbuf, lstepbuf + totsamp, MAX_ORDER * sizeof(Float_t) );
            memmove ( rstepbuf, rstepbuf + totsamp, MAX_ORDER * sizeof(Float_t) );
            totsamp = 0;
        }
        if ( totsamp > sampleWindow )   // somehow I really screwed up: Error in programming! Contact author about totsamp > sampleWindow
            return GAIN_ANALYSIS_ERROR;
    }
    if ( num_samples < MAX_ORDER ) {
        memmove ( linprebuf,                           linprebuf + num_samples, (MAX_ORDER-num_samples) * sizeof(Float_t) );
        memmove ( rinprebuf,                           rinprebuf + num_samples, (MAX_ORDER-num_samples) * sizeof(Float_t) );
        memcpy  ( linprebuf + MAX_ORDER - num_samples, left_samples,          num_samples             * sizeof(Float_t) );
        memcpy  ( rinprebuf + MAX_ORDER - num_samples, right_samples,         num_samples             * sizeof(Float_t) );
    }
    else {
        memcpy  ( linprebuf, left_samples  + num_samples - MAX_ORDER, MAX_ORDER * sizeof(Float_t) );
        memcpy  ( rinprebuf, right_samples + num_samples - MAX_ORDER, MAX_ORDER * sizeof(Float_t) );
    }
	chap_void = 0;

    return GAIN_ANALYSIS_OK;
}


static Float_t
analyzeResult ( Uint32_t* Array, size_t len )
{
    Uint32_t  elems;
    Int32_t   upper;
    size_t    i;

    elems = 0;
    for ( i = 0; i < len; i++ )
        elems += Array[i];
    if ( elems == 0 )
        return GAIN_NOT_ENOUGH_SAMPLES;

    upper = (Int32_t) ceil (elems * (1. - RMS_PERCENTILE));
    for ( i = len; i-- > 0; ) {
        if ( (upper -= Array[i]) <= 0 )
            break;
    }

    return (Float_t)i / (Float_t)STEPS_per_dB;
}


Float_t
gain_get_chapter ( void )
{
    Float_t  retval;
    int    i;

	retval = analyzeResult ( A, ANALYZE_SIZE );

	for ( i = 0; i < ANALYZE_SIZE; i++ ) {
        B[i] += A[i];
        A[i]  = 0;
    }

    for ( i = 0; i < MAX_ORDER; i++ )
        linprebuf[i] = lstepbuf[i] = loutbuf[i] = rinprebuf[i] = rstepbuf[i] = routbuf[i] = 0.f;

    totsamp = 0;
    lsum    = rsum = 0.;
	chap_void = 1;
    return retval;
}

Float_t
gain_get_title ( void )
{
	Float_t  retval;
	int    i;
	
	if (chap_void == 0)
		gain_get_chapter();
	
	retval = analyzeResult ( B, ANALYZE_SIZE );
	
	for ( i = 0; i < ANALYZE_SIZE; i++ ) {
		C[i] += B[i];
		B[i]  = 0;
	}
	
	return retval;
}


Float_t
gain_get_album ( void )
{
	return analyzeResult ( C, ANALYZE_SIZE );
}

/* end of gain_analysis.c */
