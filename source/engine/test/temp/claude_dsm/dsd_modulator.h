/*
 * dsd_modulator.h -- PCM (double, [-1,+1]) -> 1-bit DSD delta-sigma modulator
 *
 * IMPORTANT ASSUMPTION: pcm_in must already be sampled at the TARGET DSD
 * rate (2.8224 MHz for DSD64, 5.6448 MHz for DSD128, etc. -- 44.1kHz-family
 * multiples). This module performs only noise-shaped 1-bit requantization;
 * it does not perform the sample-rate interpolation from a base PCM rate
 * (e.g. 44.1/48/96/192 kHz) up to the DSD rate. That interpolation is a
 * separate, conventional multi-stage FIR/polyphase upsampling problem and
 * is expected to happen upstream of this modulator, exactly as it does in
 * real SACD/DSD encoding pipelines (interpolator stage -> noise shaper
 * stage, kept as two independent blocks).
 *
 * See the accompanying design notes for the noise-shaping theory, the
 * NTF/loop-filter derivation, and references.
 */
#ifndef DSD_MODULATOR_H
#define DSD_MODULATOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DSD_RATE_64 = 0,   /* 64  x 44.1kHz =  2,822,400 Hz */
    DSD_RATE_128,      /* 128 x 44.1kHz =  5,644,800 Hz */
    DSD_RATE_256,      /* 256 x 44.1kHz = 11,289,600 Hz */
    DSD_RATE_512,      /* 512 x 44.1kHz = 22,579,200 Hz */
    DSD_RATE_1024,     /* 1024x 44.1kHz = 45,158,400 Hz */
    DSD_RATE_COUNT
} dsd_rate_t;

#define DSD_MAX_ORDER 8

typedef struct {
    dsd_rate_t rate;
    int        order;              /* NTF/loop-filter order for this rate   */
    double     in_hist[DSD_MAX_ORDER]; /* in_hist[0]=in[n-1], in_hist[1]=in[n-2], ... */
    double     y_hist[DSD_MAX_ORDER];  /* y_hist[0]=w[n-1],  y_hist[1]=w[n-2],  ...   */
    uint32_t   rng_state;          /* xorshift32 state, quantizer dither     */
    double     dither_amplitude;   /* peak TPDF dither added at the quantizer */
    unsigned   bit_accum;          /* partial output byte being packed       */
    int        bit_count;          /* number of valid bits in bit_accum (0-7)*/
} dsd_modulator_t;

/* Returns the DSD sample rate in Hz for a given rate enum (44.1kHz family). */
double dsd_rate_to_hz(dsd_rate_t rate);

/* Initializes (or resets) a modulator instance for the given DSD rate.
 * dither_seed seeds the internal PRNG used for quantizer dither (pass any
 * nonzero value; use different seeds for independent channels so their
 * dither/idle-tone patterns don't correlate). Pass dither_seed==0 to
 * disable dither entirely (not recommended for production use: see notes
 * on idle tones in the design writeup). */
void dsd_modulator_init(dsd_modulator_t *mod, dsd_rate_t rate, uint32_t dither_seed);

/* Processes num_samples PCM input samples (already at the modulator's DSD
 * rate, values expected in [-1,+1] -- see dsd_max_input_level() for the
 * per-rate headroom this design was verified stable up to) and appends
 * packed 1-bit DSD output (MSB-first within each byte) to out.
 *
 * out must have room for at least dsd_output_bytes_for(mod->bit_count,
 * num_samples) bytes (see helper below). Bit packing state persists across
 * calls, so num_samples need not be a multiple of 8; call
 * dsd_modulator_flush() once at end-of-stream to emit any final partial
 * byte (padded with zero bits).
 *
 * Returns the number of whole bytes appended to out. */
size_t dsd_modulator_process(dsd_modulator_t *mod, const double *pcm_in,
                              size_t num_samples, uint8_t *out);

/* Flushes any partially-filled trailing byte (zero-padded in the low bits)
 * to out. Returns 0 or 1 (the number of bytes written). Safe to call even
 * if there is no pending partial byte (returns 0). */
size_t dsd_modulator_flush(dsd_modulator_t *mod, uint8_t *out);

/* Upper bound on output bytes dsd_modulator_process() can write for
 * num_samples inputs, given bits_pending bits already buffered
 * (mod->bit_count before the call). Use to size the output buffer. */
size_t dsd_output_bytes_for(int bits_pending, size_t num_samples);

/* Recommended maximum |pcm_in| peak level for guaranteed loop stability at
 * this DSD rate, as verified by closed-loop time-domain stress testing
 * (near-full-scale tones, DC steps, band-limited square waves) -- see
 * design notes. Program material should be normalized/limited to at most
 * this level before modulation. Values are conservative (include ~1dB of
 * additional margin beyond the empirically-found stability boundary). */
double dsd_max_input_level(dsd_rate_t rate);

/* One-shot convenience wrapper: equivalent to init + process + flush using
 * a fresh modulator state. dsd_out must have room for
 * dsd_output_bytes_for(0, num_samples) bytes. Returns bytes written. */
size_t pcm_to_dsd(const double *pcm_in, size_t num_samples, uint8_t *dsd_out,
                   dsd_rate_t rate, uint32_t dither_seed);

#ifdef __cplusplus
}
#endif
#endif /* DSD_MODULATOR_H */
