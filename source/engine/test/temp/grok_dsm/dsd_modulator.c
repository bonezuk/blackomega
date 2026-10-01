/*
 * dsd_modulator.c
 *
 * Single-bit CRFB delta-sigma modulator for DSD64…DSD1024.
 *
 * -------------------------------------------------------------------------
 * NTF / IIR design notes
 * -------------------------------------------------------------------------
 *
 * Linear model of a single-loop 1-bit modulator:
 *
 *     Y(z) = STF(z) X(z) + NTF(z) E(z)
 *
 * with NTF(z) = 1 / (1 + H(z)) and STF(z) ≈ 1 when a unity feed-forward
 * path is present.  H(z) is the loop filter (a cascade of integrators
 * with local resonator feedbacks).
 *
 * Causality of the loop requires NTF(∞) = 1, i.e. the first sample of
 * the NTF impulse response is identically 1.  Stability of a 1-bit
 * quantiser is governed by Lee's criterion
 *
 *     ||NTF||_∞  ≤  1.5     (≈ +3.5 dB out-of-band gain)
 *
 * which is the design constraint used by Schreier's synthesizeNTF and
 * by Kenney & Carley's CLANS optimiser.
 *
 * Zero placement
 *   A pure differentiator NTF = (1 − z^{-1})^N puts every zero at DC.
 *   Spreading the zeros across the audio band (0…20 kHz) as the roots
 *   of a Chebyshev polynomial of the second kind (Schreier 1993, also
 *   Inverse-Chebyshev high-pass) reduces in-band noise power by
 *
 *       ~3.5 dB (N=2) … ~18 dB (N=5) … ~28 dB (N=7)
 *
 *   relative to all-DC zeros (Dunn & Sandler / Schreier tables).
 *   In the CRFB topology those zeros are realised by resonator gains
 *
 *       g_i  ≈  (2 sin(ω_i / 2))²   ≈  ω_i²    for ω_i ≪ 1
 *
 *   so g scales as 1/R² when the sample rate (oversampling ratio R)
 *   is doubled and the analog notch frequencies are held constant.
 *
 * Pole placement
 *   Poles of the NTF are placed as a Butterworth high-pass (maximally
 *   flat stop-band of the *noise*, i.e. maximally flat out-of-band
 *   NTF gain) and then radially scaled until ||NTF||_∞ = 1.5.
 *   CLANS additionally optimises the time-domain noise gain.
 *
 * Order versus DSD rate (audio band 0–20 kHz)
 *   DSD64  (R = 64)  : 7th/8th-order needed for ≥120 dB in-band SQNR
 *                      (Philips SACD literature used 5th-order ≈ 118 dB;
 *                       software encoders go higher).
 *   DSD128 (R = 128) : 7th-order
 *   DSD256 (R = 256) : 6th-order
 *   DSD512 (R = 512) : 5th-order
 *   DSD1024(R = 1024): 5th-order (OSR is so large that order-4 already
 *                      exceeds 24-bit in-band; 5th keeps ultrasonic
 *                      noise from rising too close to the audio band)
 *
 * These choices follow the SoX-DSD / sox_ng recommendations and the
 * SACD-era papers:
 *
 *   Reefman & Janssen, "Signal processing for Direct Stream Digital",
 *     Philips / AES, 2002  (5th-order SDM + integrator clipping)
 *   Reefman & Janssen, "Enhanced Sigma Delta Structures for SACD",
 *     2002  (SDPC pre-correction; idle-tone suppression)
 *   Harpe, "Trellis-type Sigma Delta Modulators for SACD", 2003
 *   Schreier, Delta-Sigma Toolbox (synthesizeNTF / clans)
 *   Norsworthy, Schreier, Temes, "Delta-Sigma Data Converters", IEEE 1997
 *
 * The numerical (a, g) sets for R = 64, 128, 256 are the published
 * CLANS realisations used by mansr/sox sdm.c.  Sets for R = 512 and
 * 1024 are obtained from the R = 256 prototypes by scaling resonator
 * gains g' = g / (R_new/R_ref)², which keeps the analog NTF zeros
 * fixed.  Feedback taps a are reused; at these OSRs the Lee margin
 * only grows.
 *
 * Topology (CRFB — Cascade of Resonators with distributed FeedBack)
 *   Standard Schreier form plus a direct input feed-forward so that
 *   STF ≈ 1 + O(z^{-N}).  One-step state update:
 *
 *     d0     = s0 − g0 s1 + x − y
 *     v      = x  + a0 d0
 *     di     = si + s{i-1} − gi s{i+1}     i = 1 … N−2
 *     v     += ai di
 *     d{N-1} = s{N-1} + s{N-2}
 *     v     += a{N-1} d{N-1}
 *     y      = sign(v)          ∈ {−1, +1}
 *     s  ← d                    (with integrator clipping)
 *
 * Integrator clipping (Reefman/Janssen) prevents the high-order loop
 * from latching up on full-scale or DC-heavy material.  A soft input
 * limiter keeps the modulation index inside the stable region
 * (|x| ≲ 0.6 … 0.75 depending on order).
 */

#include "dsd_modulator.h"

#include <math.h>
#include <string.h>

typedef struct {
    int         order;
    dsd_rate_t  rate;
    double      a[DSD_MOD_MAX_ORDER];
    double      g[DSD_MOD_MAX_ORDER];
    double      clip;
    double      in_limit;
} ntf_proto_t;

/* ---- published CLANS coefficients (SoX-DSD / Schreier-CLANS) ---- */

static const ntf_proto_t k_dsd64_clans7 = {
    /* DSD64, 7th-order CLANS.  Best quality that stays stable with
     * integrator clipping on typical music.  ~120 dB+ in-band. */
    7, DSD_RATE_64,
    { 1.30828743581024e+00,
      6.14252690035661e-01,
      1.30284958810903e-01,
      1.31280998331490e-02,
      4.80497172614556e-04,
      1.28747977598542e-07,
     -1.01500259908072e-06 },
    { 0.0,
      3.96825873999969e-04,
      0.0,
      1.32436089566069e-03,
      0.0,
      2.16898568341885e-03 },
    4.0, 0.55
};

static const ntf_proto_t k_dsd128_clans7 = {
    7, DSD_RATE_128,
    { 8.98180853333862e-01,
      3.27985497323439e-01,
      6.38803466871112e-02,
      7.18262647412857e-03,
      4.51845004995476e-04,
      1.49685651672331e-05,
      4.22554681245302e-08 },
    { 0.0,
      9.92163123766340e-05,
      0.0,
      3.31199917300393e-04,
      0.0,
      5.42540771343282e-04 },
    4.0, 0.60
};

static const ntf_proto_t k_dsd256_clans6 = {
    6, DSD_RATE_256,
    { 9.97000121097967e-01,
      3.46002867430604e-01,
      5.74352078895161e-02,
      4.96197900435677e-03,
      2.16319301330580e-04,
      3.45938007947910e-06 },
    { 8.57500543083848e-06,
      0.0,
      6.58398680532347e-05,
      0.0,
      1.30939362595793e-04,
      0.0 },
    4.0, 0.65
};

/* DSD256 5th-order CLANS — prototype for 512 / 1024 (g scaled). */
static const ntf_proto_t k_dsd256_clans5 = {
    5, DSD_RATE_256,
    { 1.10212073518628e+00,
      4.33447134954244e-01,
      7.17865111532609e-02,
      4.48367825425951e-03,
      8.60861641068938e-05 },
    { 0.0,
      4.36651951230006e-05,
      0.0,
      1.23660417994961e-04 },
    4.0, 0.70
};

static void load_proto(dsd_modulator_t *m, const ntf_proto_t *p, double g_scale)
{
    int i;
    memset(m, 0, sizeof(*m));
    m->order    = p->order;
    m->rate     = p->rate;
    m->clip     = p->clip;
    m->in_limit = p->in_limit;
    for (i = 0; i < p->order; i++) {
        m->a[i] = p->a[i];
        m->g[i] = p->g[i] * g_scale;
    }
}

int dsd_modulator_init(dsd_modulator_t *m, dsd_rate_t rate)
{
    if (!m)
        return -1;

    switch (rate) {
    case DSD_RATE_64:
        load_proto(m, &k_dsd64_clans7, 1.0);
        m->rate = DSD_RATE_64;
        return 0;
    case DSD_RATE_128:
        load_proto(m, &k_dsd128_clans7, 1.0);
        m->rate = DSD_RATE_128;
        return 0;
    case DSD_RATE_256:
        load_proto(m, &k_dsd256_clans6, 1.0);
        m->rate = DSD_RATE_256;
        return 0;
    case DSD_RATE_512:
        /* analog zeros held; g ∝ 1/R²  →  scale from R=256 by 1/4 */
        load_proto(m, &k_dsd256_clans5, 0.25);
        m->rate     = DSD_RATE_512;
        m->in_limit = 0.72;
        return 0;
    case DSD_RATE_1024:
        load_proto(m, &k_dsd256_clans5, 0.0625); /* 1/16 */
        m->rate     = DSD_RATE_1024;
        m->in_limit = 0.75;
        return 0;
    default:
        return -1;
    }
}

void dsd_modulator_reset(dsd_modulator_t *m)
{
    if (!m)
        return;
    memset(m->state, 0, sizeof(m->state));
    m->bit_acc   = 0;
    m->bit_count = 0;
}

static double sat(double x, double lim)
{
    if (x >  lim) return  lim;
    if (x < -lim) return -lim;
    return x;
}

/*
 * One CRFB step.  Returns the 1-bit decision as +1.0 or −1.0.
 * `s` is the integrator state (length = order), updated in place.
 */
static double crfb_step(dsd_modulator_t *m, double x)
{
    const int     n = m->order;
    const double *a = m->a;
    const double *g = m->g;
    double       *s = m->state;
    double        d[DSD_MOD_MAX_ORDER];
    double        v;
    double        y;
    int           i;

    x = sat(x, m->in_limit);

    /* First integrator + input summing node (includes −y from *previous*
     * decision: the caller has already subtracted y from s[0] via the
     * update of the previous step).  We fold (x − y) into d[0] after
     * the new decision is known; here we first form the pre-quantiser
     * value using a trial with the implicit last y already in the
     * state.  Equivalent SoX form uses explicit y. */

    /* Reconstruct last y from the last update is unnecessary if we
     * apply feedback here.  Keep last_y implicit: we pass y of *this*
     * sample after deciding it.  Compute loop-filter output assuming
     * current y is applied this step (standard delay-free CRFB with
     * NTF leading coefficient 1). */

    /* Pre-feedback filter state advance with y = 0, then correct. */
    d[0] = s[0] - g[0] * ((n > 1) ? s[1] : 0.0) + x;
    v    = x + a[0] * d[0];

    for (i = 1; i < n - 1; i++) {
        d[i] = s[i] + s[i - 1] - g[i] * s[i + 1];
        v   += a[i] * d[i];
    }
    if (n >= 2) {
        d[n - 1] = s[n - 1] + s[n - 2];
        v       += a[n - 1] * d[n - 1];
    }

    y = (v >= 0.0) ? 1.0 : -1.0;

    /* Apply the 1-bit DAC feedback to the first summing node. */
    d[0] -= y;

    for (i = 0; i < n; i++)
        s[i] = sat(d[i], m->clip);

    return y;
}

size_t dsd_modulator_process(dsd_modulator_t *m,
                             const double    *pcm,
                             size_t           n_in,
                             uint8_t         *dsd_out)
{
    size_t n_out = 0;
    size_t i;

    if (!m || !pcm || !dsd_out)
        return 0;

    for (i = 0; i < n_in; i++) {
        double y = crfb_step(m, pcm[i]);
        /* MSB-first: first sample occupies bit 7. */
        m->bit_acc = (uint8_t)((m->bit_acc << 1) | (y > 0.0 ? 1u : 0u));
        m->bit_count++;
        if (m->bit_count == 8) {
            dsd_out[n_out++] = m->bit_acc;
            m->bit_acc   = 0;
            m->bit_count = 0;
        }
    }
    return n_out;
}

size_t dsd_modulator_flush(dsd_modulator_t *m, uint8_t *dsd_out)
{
    if (!m || !dsd_out || m->bit_count == 0)
        return 0;

    /* Pad unused LSBs with 0/1 idle (alternating), zero-mean. */
    while (m->bit_count < 8) {
        unsigned pad = (unsigned)(m->bit_count & 1);
        m->bit_acc = (uint8_t)((m->bit_acc << 1) | pad);
        m->bit_count++;
    }
    dsd_out[0]   = m->bit_acc;
    m->bit_acc   = 0;
    m->bit_count = 0;
    return 1;
}
