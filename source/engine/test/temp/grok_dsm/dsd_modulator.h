/*
 * dsd_modulator.h — single-channel PCM (double, −1…+1) to packed DSD
 *
 * Input samples are assumed to already be at the target DSD sample rate
 * (interpolation / SRC is outside this module).  Each input sample produces
 * one 1-bit DSD sample.  Eight consecutive bits are packed MSB-first into
 * one output byte (bit 7 = earliest sample), matching DSF / DSDIFF
 * convention.
 *
 * NTF / loop-filter coefficients are rate-dependent CRFB realisations of
 * Schreier / CLANS designs used in SACD-era and SoX-DSD literature.
 */
#ifndef DSD_MODULATOR_H
#define DSD_MODULATOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DSD_MOD_MAX_ORDER 8

typedef enum {
    DSD_RATE_64   = 64,    /*  2.8224 MHz  (64  × 44.1 kHz) */
    DSD_RATE_128  = 128,   /*  5.6448 MHz                  */
    DSD_RATE_256  = 256,   /* 11.2896 MHz                  */
    DSD_RATE_512  = 512,   /* 22.5792 MHz                  */
    DSD_RATE_1024 = 1024   /* 45.1584 MHz                  */
} dsd_rate_t;

typedef struct dsd_modulator {
    int      order;
    dsd_rate_t rate;
    double   a[DSD_MOD_MAX_ORDER];   /* distributed feedback (CRFB) */
    double   g[DSD_MOD_MAX_ORDER];   /* resonator gains (NTF zeros) */
    double   state[DSD_MOD_MAX_ORDER];
    double   clip;                   /* integrator clip level       */
    double   in_limit;               /* soft input limiter          */
    uint8_t  bit_acc;                /* pending packed bits         */
    int      bit_count;              /* 0…7 bits already in acc     */
} dsd_modulator_t;

/*
 * Initialise (or re-initialise) a modulator for the given DSD rate.
 * Returns 0 on success, −1 if the rate is not supported.
 */
int  dsd_modulator_init(dsd_modulator_t *m, dsd_rate_t rate);

/* Clear integrator state and any leftover packed bits. */
void dsd_modulator_reset(dsd_modulator_t *m);

/*
 * Convert n_in PCM samples to packed DSD.
 *
 * dsd_out must hold at least (pending_bits + n_in) / 8 bytes.
 * Leftover bits (when the total is not a multiple of 8) stay in the
 * context and are emitted on a later call, or by dsd_modulator_flush().
 *
 * Returns the number of bytes written to dsd_out.
 */
size_t dsd_modulator_process(dsd_modulator_t *m,
                             const double    *pcm,
                             size_t           n_in,
                             uint8_t         *dsd_out);

/*
 * Emit a final byte if any bits remain, padding unused LSBs with
 * a 01010101… idle pattern (zero-mean).  Returns 0 or 1.
 */
size_t dsd_modulator_flush(dsd_modulator_t *m, uint8_t *dsd_out);

#ifdef __cplusplus
}
#endif

#endif /* DSD_MODULATOR_H */
