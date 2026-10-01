#include "dsd_modulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
    dsd_modulator_t m;
    const int n = 65536;
    double *x = malloc((size_t)n * sizeof(double));
    uint8_t *y = malloc((size_t)n / 8 + 8);
    int ones = 0, bits = 0;
    size_t nb;
    int i;
    double amp = 0.25;
    double fs = 64.0 * 44100.0;
    double f  = 1000.0;

    if (dsd_modulator_init(&m, DSD_RATE_64) != 0) {
        fprintf(stderr, "init failed\n");
        return 1;
    }
    for (i = 0; i < n; i++)
        x[i] = amp * sin(2.0 * 3.14159265358979323846 * f * (double)i / fs);

    nb = dsd_modulator_process(&m, x, (size_t)n, y);
    nb += dsd_modulator_flush(&m, y + nb);

    for (i = 0; i < (int)nb; i++) {
        unsigned b;
        for (b = 0; b < 8; b++) {
            if (y[i] & (1u << (7 - b)))
                ones++;
            bits++;
        }
    }
    printf("bytes=%zu bits=%d ones=%d density=%.4f (expect ~0.5 + amp/2 = %.4f)\n",
           nb, bits, ones, (double)ones / (double)bits, 0.5 + amp / 2.0);

    /* DC test */
    dsd_modulator_reset(&m);
    for (i = 0; i < n; i++)
        x[i] = 0.3;
    ones = bits = 0;
    nb = dsd_modulator_process(&m, x, (size_t)n, y);
    nb += dsd_modulator_flush(&m, y + nb);
    for (i = 0; i < (int)nb; i++) {
        unsigned b;
        for (b = 0; b < 8; b++) {
            if (y[i] & (1u << (7 - b)))
                ones++;
            bits++;
        }
    }
    printf("DC 0.3  density=%.4f (expect ~0.65)\n", (double)ones / (double)bits);

    /* rate table smoke test */
    {
        dsd_rate_t rates[] = {
            DSD_RATE_64, DSD_RATE_128, DSD_RATE_256,
            DSD_RATE_512, DSD_RATE_1024
        };
        int r;
        for (r = 0; r < 5; r++) {
            if (dsd_modulator_init(&m, rates[r]) != 0) {
                fprintf(stderr, "init %d failed\n", (int)rates[r]);
                return 1;
            }
            nb = dsd_modulator_process(&m, x, 1024, y);
            printf("rate DSD%d order=%d bytes=%zu\n",
                   (int)rates[r], m.order, nb);
        }
    }
    free(x);
    free(y);
    return 0;
}
