/*
 * dsd_modulator.c -- PCM (double) -> 1-bit DSD delta-sigma modulator
 *
 * Architecture: single-loop ("interpolative") delta-sigma modulator.
 *
 *   w[n] = L(z) * (x[n] - v[n])      loop filter, applied to (input - fed
 *                                     back quantized output)
 *   v[n] = sign(w[n] + dither[n])    1-bit quantizer with TPDF dither
 *
 * L(z) is derived from the desired Noise Transfer Function NTF(z) via
 *   NTF(z) = 1 / (1 + L(z))   =>   L(z) = (1 - NTF(z)) / NTF(z)
 *
 * NTF(z) is a rational, monic (NTF(z->infinity)=1) IIR high-pass transfer
 * function of order N (5 poles/zeros for order 5, etc.), which guarantees
 * L(z) is strictly proper (no z^0 feedthrough term) and therefore
 * realizable without an algebraic loop: at sample n we only ever need
 * v[n-1], v[n-2], ... (already known), never v[n] itself.
 *
 * See dsd_modulator.h and the accompanying design notes for the full NTF
 * synthesis methodology (inverse-Chebyshev pole placement + Chao/Snelgrove
 * -Sedra optimized zero spreading, Lee's-rule out-of-band gain, and
 * closed-loop time-domain stability verification).
 */
#include "dsd_modulator.h"
#include <string.h>

/* ---- Per-rate loop-filter coefficients -----------------------------------
 *
 * Each rate stores L(z)'s numerator/denominator as compact arrays of
 * length `order`: b[k] multiplies in_hist[k] (= in[n-1-k]) and a[k]
 * multiplies y_hist[k] (= w[n-1-k]), k = 0 .. order-1. (The implicit b0=0
 * and a0=1 terms are why these arrays start at what would be index 1 of
 * the full order+1 NTF-derived polynomials.)
 *
 * Recursion:
 *   w[n] = sum_k b[k]*in_hist[k]  -  sum_k a[k]*y_hist[k]
 *
 * Design parameters (order, out-of-band NTF gain, verified max stable
 * input amplitude) are documented per rate below and in dsd_max_input_level().
 */

/* DSD64 (2,822,400 Hz): order 8, OOB gain 1.476, max stable amplitude 0.510 */
static const double DSD64_B[8] = {
     0.77768777836567, -5.14225082352806, 14.59910421942007, -23.06722376602455,
    21.90551453146337, -12.50177339809122, 3.97007607918067, -0.54113439186735
};
static const double DSD64_A[8] = {
    -7.99603577050550, 27.97621953408770, -55.94055619985439, 69.92074487254449,
   -55.94055619985438, 27.97621953408770, -7.99603577050550, 1.00000000000000
};

/* DSD128 (5,644,800 Hz): order 6, OOB gain 1.534, max stable amplitude 0.482 */
static const double DSD128_B[6] = {
     0.85322292771034, -3.90928328928330, 7.19765860500578, -6.65390648118472,
     3.08746131898673, -0.57507420429147
};
static const double DSD128_A[6] = {
    -5.99925663789685, 14.99702668973517, -19.99554010367283, 14.99702668973517,
    -5.99925663789686, 1.00000000000000
};

/* DSD256 (11,289,600 Hz): order 6, OOB gain 1.153, max stable amplitude 0.849 */
static const double DSD256_B[6] = {
     0.28486252984180, -1.38398770093388, 2.69095262458139, -2.61734242282095,
     1.27347307239101, -0.24795795852306
};
static const double DSD256_A[6] = {
    -5.99981415515693, 14.99925662926232, -19.99888494821072, 14.99925662926232,
    -5.99981415515693, 1.00000000000000
};

/* DSD512 (22,579,200 Hz): order 5, OOB gain 1.178, max stable amplitude 0.801 */
static const double DSD512_B[5] = {
     0.32768697101076, -1.25768151000048, 1.81216216879685, -1.16171424086315,
     0.27955572445218
};
static const double DSD512_A[5] = {
    -4.99997676924971, 9.99993030780908, -9.99993030780908, 4.99997676924971,
    -1.00000000000000
};

/* DSD1024 (45,158,400 Hz): order 4, OOB gain 1.129, max stable amplitude 0.904 */
static const double DSD1024_B[4] = {
     0.24302655137938, -0.69998245745760, 0.67288583150718, -0.21586338031892
};
static const double DSD1024_A[4] = {
    -3.99999225640866, 5.99998451282481, -3.99999225640866, 1.00000000000000
};

typedef struct {
    double         hz;
    int            order;
    const double  *b;
    const double  *a;
    double         max_input_level; /* documented safe peak, incl. margin */
} dsd_rate_info_t;

static const dsd_rate_info_t RATE_INFO[DSD_RATE_COUNT] = {
    /* rate         hz            order  b            a            max_input */
    { 2822400.0,     8, DSD64_B,   DSD64_A,   0.454 },
    { 5644800.0,     6, DSD128_B,  DSD128_A,  0.430 },
    {11289600.0,     6, DSD256_B,  DSD256_A,  0.757 },
    {22579200.0,     5, DSD512_B,  DSD512_A,  0.714 },
    {45158400.0,     4, DSD1024_B, DSD1024_A, 0.806 },
};

/* Peak TPDF dither amplitude injected at the quantizer, as a fraction of
 * the loop filter's typical internal signal swing. Purpose: decorrelate
 * deterministic limit cycles / idle tones that a noiseless 1-bit quantizer
 * produces on near-silent or highly periodic input (well known in ΔΣ
 * literature, e.g. Norsworthy/Schreier/Temes ch.2 and Reiss 2008 JAES).
 * This value is intentionally small: enough to randomize bit-pattern
 * periodicity, not enough to measurably raise the shaped noise floor.
 * Tune per application if needed (0.0 disables dither entirely). */
#define DSD_DITHER_SCALE 0.02

double dsd_rate_to_hz(dsd_rate_t rate)
{
    if (rate < 0 || rate >= DSD_RATE_COUNT) return 0.0;
    return RATE_INFO[rate].hz;
}

double dsd_max_input_level(dsd_rate_t rate)
{
    if (rate < 0 || rate >= DSD_RATE_COUNT) return 0.0;
    return RATE_INFO[rate].max_input_level;
}

void dsd_modulator_init(dsd_modulator_t *mod, dsd_rate_t rate, uint32_t dither_seed)
{
    memset(mod, 0, sizeof(*mod));
    mod->rate = rate;
    mod->order = RATE_INFO[rate].order;
    mod->rng_state = dither_seed ? dither_seed : 1u; /* xorshift32 needs nonzero state */
    mod->dither_amplitude = dither_seed ? DSD_DITHER_SCALE : 0.0;
    mod->bit_accum = 0;
    mod->bit_count = 0;
}

/* xorshift32 PRNG -- fast, deterministic, no external dependency. */
static uint32_t xorshift32(uint32_t *state)
{
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/* Returns a triangular-PDF random value in [-1, +1) by summing two
 * independent uniform variates (standard TPDF construction). */
static double tpdf_sample(uint32_t *state)
{
    double u1 = (double)(xorshift32(state) >> 8) * (1.0 / 16777216.0); /* [0,1) */
    double u2 = (double)(xorshift32(state) >> 8) * (1.0 / 16777216.0);
    return (u1 - u2); /* triangular, range (-1,1), zero mean */
}

size_t dsd_output_bytes_for(int bits_pending, size_t num_samples)
{
    size_t total_bits = (size_t)bits_pending + num_samples;
    return total_bits / 8; /* whole bytes only; see dsd_modulator_flush for the remainder */
}

size_t dsd_modulator_process(dsd_modulator_t *mod, const double *pcm_in,
                              size_t num_samples, uint8_t *out)
{
    const int order = mod->order;
    const dsd_rate_info_t *info = &RATE_INFO[mod->rate];
    const double *b = info->b;
    const double *a = info->a;
    double in_hist[DSD_MAX_ORDER];
    double y_hist[DSD_MAX_ORDER];
    unsigned bit_accum = mod->bit_accum;
    int bit_count = mod->bit_count;
    size_t out_pos = 0;
    size_t n;
    int k;

    memcpy(in_hist, mod->in_hist, sizeof(double) * order);
    memcpy(y_hist, mod->y_hist, sizeof(double) * order);

    for (n = 0; n < num_samples; n++) {
        double x = pcm_in[n];
        double w = 0.0;
        double dither, v, in_new;
        unsigned bit;

        for (k = 0; k < order; k++) {
            w += b[k] * in_hist[k] - a[k] * y_hist[k];
        }

        dither = mod->dither_amplitude != 0.0
                     ? mod->dither_amplitude * tpdf_sample(&mod->rng_state)
                     : 0.0;

        bit = (w + dither >= 0.0) ? 1u : 0u;
        v = bit ? 1.0 : -1.0;
        in_new = x - v;

        /* shift delay lines (order is small: 4-8 taps, a loop is simplest
         * and the branch predicts perfectly since `order` is loop-invariant) */
        for (k = order - 1; k > 0; k--) {
            in_hist[k] = in_hist[k - 1];
            y_hist[k] = y_hist[k - 1];
        }
        in_hist[0] = in_new;
        y_hist[0] = w;

        bit_accum = (bit_accum << 1) | bit;
        bit_count++;
        if (bit_count == 8) {
            out[out_pos++] = (uint8_t)bit_accum;
            bit_accum = 0;
            bit_count = 0;
        }
    }

    memcpy(mod->in_hist, in_hist, sizeof(double) * order);
    memcpy(mod->y_hist, y_hist, sizeof(double) * order);
    mod->bit_accum = bit_accum;
    mod->bit_count = bit_count;

    return out_pos;
}

size_t dsd_modulator_flush(dsd_modulator_t *mod, uint8_t *out)
{
    if (mod->bit_count == 0) return 0;
    out[0] = (uint8_t)(mod->bit_accum << (8 - mod->bit_count));
    mod->bit_accum = 0;
    mod->bit_count = 0;
    return 1;
}

size_t pcm_to_dsd(const double *pcm_in, size_t num_samples, uint8_t *dsd_out,
                   dsd_rate_t rate, uint32_t dither_seed)
{
    dsd_modulator_t mod;
    size_t bytes;

    dsd_modulator_init(&mod, rate, dither_seed);
    bytes = dsd_modulator_process(&mod, pcm_in, num_samples, dsd_out);
    bytes += dsd_modulator_flush(&mod, dsd_out + bytes);
    return bytes;
}
