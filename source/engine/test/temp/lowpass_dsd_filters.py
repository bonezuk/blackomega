import scipy.signal as signal
import numpy as np
import os
import matplotlib.pyplot as plt

def calc_lp_filter(numtaps, coff, filename, fs):
    beta = 8.0
    h = signal.firwin(numtaps, cutoff=coff, window=('kaiser', beta), pass_zero=True, scale=True)
    if os.path.exists(filename):
        os.remove(filename)
    with open(filename, "w") as f:
        f.write(f"{numtaps}\n")
        for i in range(numtaps):
            f.write(f"{h[i]:.18f}\n")
    print(filename, " : DC gain (should be ≈1):", np.sum(h))
    w, h_resp = signal.freqz(h, worN=4097)
    freq = w * fs / (2 * np.pi)
    plt.plot(freq, 20 * np.log10(np.abs(h_resp)))
    plt.axvline(coff, color='r', linestyle='--', label=f'Design cutoff ({coff/1000:.2f} kHz)')
    plt.title(filename)
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Gain (dB)")
    plt.xlim(0, fs/2)
    plt.ylim(-150, 5)
    plt.grid(True)
    plt.legend()
    plt.show()

# e_lpQuarter_DSD2,
#calc_lp_filter(4097, 0.25, "lpQuarter_DSD2.txt", 96000)
# e_lpHalf_DSD4, // 8193
#calc_lp_filter(8193, 0.5, "lpHalf_DSD4.txt", 192000)
# e_lpHalf_DSD8, // 16385
#calc_lp_filter(16385, 0.5, "lpHalf_DSD8.txt", 384000)
# e_lpQuarter_DSD8,
#calc_lp_filter(16385, 0.25, "lpQuarter_DSD8.txt", 384000)
# e_lpQuarter_DSD16, // 32769
#calc_lp_filter(32769, 0.25, "lpQuarter_DSD16.txt", 384000 * 2)
# e_lpQuarter_DSD32, // 65537
#calc_lp_filter(65537, 0.25, "lpQuarter_DSD32.txt", 384000 * 4)
# e_lpQuarter_DSD64, // 131073
#calc_lp_filter(131073, 0.25, "lpQuarter_DSD64.txt", 384000 * 8)
# e_lpQuarter_DSD128, // 262145
#calc_lp_filter(262145, 0.25, "lpQuarter_DSD128.txt", 384000 * 16)
# e_lpQuarter_DSD256, // 524289
#calc_lp_filter(524289, 0.25, "lpQuarter_DSD256.txt", 384000 * 32)
# e_lpQuarter_DSD512, // 1048577
#calc_lp_filter(1048577, 0.25, "lpQuarter_DSD512.txt", 384000 * 64)
# e_lpQuarter_DSD1024, // 2097153
#calc_lp_filter(2097153, 0.25, "lpQuarter_DSD1024.txt", 384000 * 128)

#    e_lpHalf_DSD0_5, // 1025
#calc_lp_filter(1025, 0.5, "lpHalf_DSD0_5.txt", 48000)
#    e_lpHalf_DSD1, // 2049
#calc_lp_filter(2049, 0.5, "lpHalf_DSD1.txt", 24000)

# e_lowPassHalf_8192, // 8193
calc_lp_filter(8192, 0.5, "lowpass_half_8192.txt", 192000)
