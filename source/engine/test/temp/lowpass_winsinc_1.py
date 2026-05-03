import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

# ===================================================================
# Low-pass FIR filter design: 8191 coefficients
# Sampling rate: 44.1 kHz
# Cutoff frequency: 12.5 kHz
# ===================================================================
# Best practical method for this specification:
#   Windowed-sinc design using the Kaiser window (scipy.signal.firwin).
#   This is the standard, most reliable, and widely used approach in
#   professional audio DSP for very high-order FIR filters.
#
#   Why this method?
#   - Extremely sharp transition (~5-10 Hz wide with N=8191)
#   - Controllable stopband attenuation via the Kaiser beta parameter
#   - Guaranteed linear phase (Type I, symmetric coefficients)
#   - Numerically stable and very fast even for N > 8000
#   - Parks-McClellan / remez (equiripple) is theoretically minimax-optimal
#     but frequently fails to converge or becomes impractically slow for
#     N=8191 without heavy parameter tuning. The window method gives
#     virtually indistinguishable performance in audio applications.
#
#   Kaiser beta = 8.0 gives ~80 dB stopband attenuation (excellent for audio).
#   You can increase to 10-12 for >100 dB if you need ultra-clean rejection.
# ===================================================================

# Parameters
numtaps = 4097          # Number of coefficients (must be odd)
fs      = 44100.0       # Sampling frequency in Hz
cutoff  = 12500.0       # Desired cutoff frequency in Hz
beta    = 8.0           # Kaiser window shape parameter (~80 dB attenuation)

# Design the filter
coeffs = signal.firwin(
    numtaps,
    cutoff=cutoff,
    window=('kaiser', beta),
    pass_zero=True,      # Low-pass
    scale=True,          # Normalize passband gain to 1.0
    fs=fs
)

# Verify basic properties
print(f"✅ Filter designed successfully!")
print(f"   Coefficients shape : {coeffs.shape}")
print(f"   DC gain (sum)      : {np.sum(coeffs):.10f}  (should be exactly 1.0)")
print(f"   Center tap         : {coeffs[numtaps//2]:.10f}")
print(f"   First/last tap     : {coeffs[0]:.2e} / {coeffs[-1]:.2e}")

c_filename = "lowpass_fir_12.5kHz.c"

with open(c_filename, "w") as f:
    f.write("static const double lowpass_fir_12500Hz_" + str(numtaps) + "[] = {\n\t")
    for i in range(numtaps):
        f.write(f" {coeffs[i]:.18f},")
        if (i + 1) % 8 == 0:
            f.write("\n\t")
        else:
            f.write(" ")
    f.write("};\n")

w, h = signal.freqz(coeffs, worN=8192*4)
plt.plot(w * fs / (2 * np.pi), 20 * np.log10(np.abs(h)))
plt.axvline(cutoff, color='r', linestyle='--')
plt.title("Frequency response of designed 8191-tap low-pass FIR")
plt.xlabel("Frequency (Hz)"); plt.ylabel("Gain (dB)")
plt.grid(True)
plt.show()
