import scipy.signal as signal
import numpy as np
import os
import matplotlib.pyplot as plt

numtaps = 8192
beta = 8.0          # Kaiser beta: 5–6 ≈ 60 dB, 8–9 ≈ 80–90 dB, 12+ for >100 dB (excellent trade-off)

# Single set of coefficients (identical for both sample rates)
h = signal.firwin(numtaps, cutoff=0.5, window=('kaiser', beta), pass_zero=True, scale=True)

# Optional: save for your DSP engine
print("DC gain (should be ≈1):", np.sum(h))
print("Coefficients ready for both 88.2 kHz and 96 kHz pipelines.")

c_filename = "lowpass_half_8192.txt"
if os.path.exists(c_filename):
    os.remove(c_filename);

with open(c_filename, "w") as f:
    f.write(f"{numtaps}\n")
    for i in range(numtaps):
        f.write(f"{h[i]:.18f}\n")

fs = 44100 * 2
cutoff = fs / 4.0

w, h = signal.freqz(h, worN=8192*4)
plt.plot(w * fs / (2 * np.pi), 20 * np.log10(np.abs(h)))
plt.axvline(cutoff, color='r', linestyle='--')
plt.title("Frequency response of designed 8191-tap low-pass FIR")
plt.xlabel("Frequency (Hz)"); plt.ylabel("Gain (dB)")
plt.grid(True)
plt.show()
