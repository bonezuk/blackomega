import scipy.signal as signal
import numpy as np
import os
import matplotlib.pyplot as plt

numtaps = 8193
beta = 8.0          # Kaiser beta: 8 gives ~80–90+ dB attenuation with very narrow transition
                    # (increase to 10–12 for even more stopband rejection if desired)

# Generate high-quality linear-phase low-pass FIR filter
# (cutoff exactly at 0.5 * Nyquist → identical coeffs for both 88.2 kHz and 96 kHz)
h = signal.firwin(numtaps, cutoff=0.25, window=('kaiser', beta), pass_zero=True, scale=True)

print("DC gain (should be ≈1):", np.sum(h))
print(f"{numtaps}-tap coefficients ready for both 88.2 kHz and 96 kHz pipelines.")

# Save coefficients in the exact format you use
c_filename = "lowpass_half_8193.txt"
if os.path.exists(c_filename):
    os.remove(c_filename)

with open(c_filename, "w") as f:
    f.write(f"{numtaps}\n")
    for i in range(numtaps):
        f.write(f"{h[i]:.18f}\n")

print(f"✅ Coefficients saved to {c_filename}")

# Plot frequency response (example at 88.2 kHz)
fs = 88200.0
cutoff = fs / 4.0

w, h_resp = signal.freqz(h, worN=4097*4)
freq = w * fs / (2 * np.pi)

plt.plot(freq, 20 * np.log10(np.abs(h_resp)))
plt.axvline(cutoff, color='r', linestyle='--', label=f'Design cutoff ({cutoff/1000:.2f} kHz)')
plt.title(f"Frequency response of designed {numtaps}-tap low-pass FIR (Kaiser β={beta})")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Gain (dB)")
plt.xlim(0, fs/2)
plt.ylim(-150, 5)
plt.grid(True)
plt.legend()
plt.show()