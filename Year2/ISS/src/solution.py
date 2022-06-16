
import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.io.wavfile import write
from scipy import signal
import cmath
import statistics as st

samples1, wf1 = wavfile.read('./frame_audio/off_ramec.wav')
samples2, wf2 = wavfile.read('./frame_audio/on_ramec.wav')

# ustrednenie a normalizacia useku bez ruska
wf1 = wf1 - np.mean(wf1)
wf1 /= np.abs(wf1).max()

# ustrednenie a normalizacia useku s ruskom
wf2 = wf2 - np.mean(wf2)
wf2 /= np.abs(wf2).max()

# polia na uchovanie ramcov, pre 99 ramcov, 1 zahodime
frame_off = np.zeros(shape=(99, 320))
frame_on = np.zeros(shape=(99, 320))

samp_counter = 0
start_sample = 0
end_sample = 320
for i in range(99):
    idx = 0
    for j in range(start_sample, end_sample):
        frame_off[i, idx] = wf1[j]
        frame_on[i, idx] = wf2[j]
        samp_counter = j
        idx += 1

    start_sample = samp_counter - 159
    end_sample = start_sample + 320

# ramec ma dlzku 20 ms
frame_len = 0.02
time = np.linspace(0., frame_len, frame_off.shape[1])
time *= 1000

plt.plot(time, frame_off[0], label = "Rámec 0 bez masky")
plt.plot(time, frame_on[0], label = "Rámec 0 s maskou")
plt.title("Graf zvolených rámcov")
plt.xlabel("Čas [ms]")
plt.ylabel("Y")
plt.legend()
plt.show()

# centrálne klipkovanie
tmp_clip_off = np.zeros(shape=(99, 320))
tmp_clip_on = np.zeros(shape=(99, 320))
for i in range(99):
    pos_max_off = 0.7*np.abs(frame_off[i]).max()
    neg_max_off = pos_max_off * -1
    pos_max_on = 0.7 * np.abs(frame_on[i]).max()
    neg_max_on = pos_max_on * -1
    for j in range(320):
        if frame_off[i, j] > pos_max_off:
            tmp_clip_off[i, j] = 1.0
        elif frame_off[i, j] < neg_max_off:
            tmp_clip_off[i, j] = -1.0
        else:
            tmp_clip_off[i, j] = 0.0

        if frame_on[i, j] > pos_max_on:
            tmp_clip_on[i, j] = 1.0
        elif frame_on[i, j] < neg_max_on:
            tmp_clip_on[i, j] = -1.0
        else:
            tmp_clip_on[i, j] = 0.0

x_asis = np.linspace(0, 319, 320)
plt.plot(x_asis, tmp_clip_on[16])
plt.title("Klipkovanie rámca")
plt.xlabel("Vzorky")
plt.ylabel("Y")
plt.show()

# autokoleracia
tmp_corel_off = np.zeros(shape=(99, 320))
tmp_corel_on = np.zeros(shape=(99, 320))
for i in range(99):
    for j in range(0, 320):
        sum_cor = 0
        sum_cor2 = 0
        for k in range(0, 320 - j):
            mul_cor = tmp_clip_off[i, k] * tmp_clip_off[i, k + j]
            sum_cor += mul_cor
            mul_cor2 = tmp_clip_on[i, k] * tmp_clip_on[i, k + j]
            sum_cor2 += mul_cor2

        tmp_corel_off[i, j] = sum_cor
        tmp_corel_on[i, j] = sum_cor2

lag = np.argmax(tmp_corel_on[16, 80:320]) + 80
lag_value = np.max(tmp_corel_on[16, 80:320])
plt.plot(x_asis, tmp_corel_on[16])
plt.plot(lag, lag_value, marker='o', markerfacecolor='red', markersize=12, label="lag")
plt.vlines(x=lag, ymin=0, ymax=lag_value, colors='r')
plt.axvline(x=79, c='k', label="Prah")
plt.title("Autokorelácia rámca s rúškom")
plt.xlabel("Vzorky")
plt.ylabel("Y")
plt.legend()
plt.show()

# urcenie zakladnej frekvencie
tmp_freq_off = np.zeros(99)
tmp_freq_on = np.zeros(99)
FS = 16000
for i in range(99):
    # prah ma hodnotu 80 vzorku(200 Hz), berieme indexy vyssie ako 79
    max_lag_off = np.argmax(tmp_corel_off[i, 80:320]) + 80
    f_off = FS/max_lag_off
    max_lag_on = np.argmax(tmp_corel_on[i, 80:320]) + 80
    f_on = FS/max_lag_on

    tmp_freq_off[i] = f_off
    tmp_freq_on[i] = f_on

x_asis = np.linspace(0, 98, 99)
plt.plot(x_asis, tmp_freq_off, label = "Bez rúška")
plt.plot(x_asis, tmp_freq_on, label = "S rúškom")
plt.title("Základná frekvencia")
plt.xlabel("Rámce")
plt.ylabel("f0")
plt.legend()
plt.show()
print(np.mean(tmp_freq_off), np.mean(tmp_freq_on))
print(st.variance(tmp_freq_off), st.variance(tmp_freq_on))

# implementacia DFT
fft_off = np.zeros(shape=(99, 1024), dtype=complex)
fft_on = np.zeros(shape=(99, 1024), dtype=complex)
spect_off = np.zeros(shape=(99, 1024))
spect_on = np.zeros(shape=(99, 1024))


def dft(x, N):
    a = np.zeros(N, dtype=complex)
    for k in range(N):
        sum = 0
        for n in range(N):
            sum += x[n] * cmath.exp(-1j*2 * cmath.pi * k * n * (1/N))
        a[k] = sum
    return a


def dft_prepare():
    # rozsirime kazdy ramec nulami aby sme mali 1024 vzorkov
    for i in range(99):
        pad_off = frame_off[i]
        pad_on = frame_on[i]
        pad_off = np.pad(pad_off, (0, 704), 'constant')
        pad_on = np.pad(pad_on, (0, 704), 'constant')
        fft_off[i] = dft(pad_off, 1024)
        fft_on[i] = dft(pad_on, 1024)


for i in range(99):
    fft_off[i] = np.fft.fft(frame_off[i], n=1024)
    fft_on[i] = np.fft.fft(frame_on[i], n=1024)
    for j in range(1024):
        spect_off[i, j] = 10 * np.log10(np.abs((fft_off[i, j]**2) + 1e-20))
        spect_on[i, j] = 10 * np.log10(np.abs((fft_on[i, j]**2) + 1e-20))


plt.imshow(np.transpose(spect_off[:, 0:512]), extent=[0.0, 1.0, 0, 8000], origin='lower', interpolation='none', aspect='auto')
plt.colorbar()
plt.title("Spektogram bez rúška")
plt.xlabel("Čas")
plt.ylabel("Frekvencia")
plt.show()

plt.imshow(np.transpose(spect_on[:, 0:512]), extent=[0.0, 1.0, 0, 8000], origin='lower', interpolation='none', aspect='auto')
plt.colorbar()
plt.title("Spektogram s rúškom")
plt.xlabel("Čas")
plt.ylabel("Frekvencia")
plt.show()

#frekvencna charakteristika
freqch = np.zeros(shape=(99, 1024), dtype=complex)
for i in range(99):
    ws, freqch[i] = signal.freqz(fft_on[i], fft_off[i], worN=1024)

aver_freq = np.average(np.abs(freqch), axis=0)
log_freq = np.zeros(1024)
for i in range(1024):
    log_freq[i] = 10 * np.log10(np.abs(1e-20 + (aver_freq[i] ** 2)))

x_asis = np.linspace(0, 8000, 512)
plt.plot(x_asis, log_freq[0:512])
plt.title("Frekvencná charakteristika rúška")
plt.xlabel("Frekvencia")
plt.ylabel("Spektrálna hustota výkonu")
plt.show()

#implementacia IDFT
def idft(x, N):
    a = np.zeros(N, dtype=complex)
    for k in range(N):
        sum = 0
        for n in range(N):
            sum += x[n] * cmath.exp(1j*2 * cmath.pi * k * n * (1/N))
        a[k] = sum/N
    return a


#IDFT
inverse = np.fft.ifft(aver_freq[0:512], n=1024)[0:512]
x_asis = np.linspace(0, 511, 512)
plt.plot(x_asis, inverse.real)
plt.title("Impulzná odozva rúška")
plt.xlabel("n")
plt.ylabel("h[n]")
plt.show()

#filtracia
fs_sent, maskoff_sent = wavfile.read('../audio/maskoff_sentance.wav')
fs_sent2, maskon_sent = wavfile.read('../audio/maskon_sentance.wav')
fs_tone, maskoff_tone = wavfile.read('../audio/maskoff_tone.wav')
fs_tone2, maskon_tone = wavfile.read('../audio/maskon_tone.wav')

simulated_sent = signal.lfilter(b=inverse.real, a=1, x=maskoff_sent)
simulated_tone = signal.lfilter(b=inverse.real, a=1, x=maskoff_tone)

x_tone = np.linspace(0, maskoff_tone.shape[0]/fs_tone, maskoff_tone.shape[0])
x_tone2 = np.linspace(0, maskon_tone.shape[0]/fs_tone2, maskon_tone.shape[0])
x_sent = np.linspace(0, maskoff_sent.shape[0]/fs_sent, maskoff_sent.shape[0])
x_sent2 = np.linspace(0, maskon_sent.shape[0]/fs_sent2, maskon_sent.shape[0])

plt.plot(x_tone, simulated_tone, label="Simulovany ton")
plt.plot(x_tone2, maskon_tone, label="Originalny ton")
plt.title("Graf simulovaneho a originalneho tonu")
plt.xlabel("Čas [s]")
plt.legend()
plt.show()

plt.plot(x_sent, simulated_sent, label="Simulovana veta")
plt.plot(x_sent2, maskon_sent, label="Originalna veta")
plt.title("Graf simulovanej a originalnej vety")
plt.xlabel("Čas [s]")
plt.legend()
plt.show()

write("sim_maskon_tone.wav", 16000, simulated_tone)
write("sim_maskon_sentence.wav", 16000, simulated_sent)
