import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

fs = 96000   # Sample rate, Hz

def plot_response(w, h, title):
    "Utility function to plot response functions"
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(w, 20*np.log10(np.abs(h)))
    ax.set_ylim(-40, 5)
    ax.grid(True)
    ax.set_xlabel('Frequency (Hz)')
    ax.set_ylabel('Gain (dB)')
    ax.set_title(title)

cutoff = 200.0    # Desired cutoff frequency, Hz
trans_width = 200  # Width of transition from pass to stop, Hz
numtaps = 765     # Size of the FIR filter.
taps = signal.remez(numtaps, [0, cutoff, cutoff + trans_width, 0.5*fs],
                    [1, 0], fs=fs)
w, h = signal.freqz(taps, [1], worN=2000, fs=fs)
plot_response(w, h, "Low-pass Filter")
plt.show()

total = 0
count = 0
ostr = "\t\t"
for tap in taps:
    ostr = ostr + " " + str(tap) + ", "
    count += 1
    total += 1
    if count >= 4:
        ostr = ostr + " // " + str(total)
        print(ostr)
        ostr = "\t\t"
        count = 0
ostr = ostr + " // " + str(total)
print(ostr)
