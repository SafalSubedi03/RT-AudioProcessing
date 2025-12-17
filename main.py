import sounddevice as sd
import soundfile as sf
import numpy as np
from NLMS import compute_y, updatewk  


fs = 44100
channels = 1
blocksize = 512  # adjust for lower latency if needed

# Load far-end signal
far_end_speaker_audio, Fs = sf.read("un.wav")
far_end_speaker_audio_size = len(far_end_speaker_audio)


near_end_speaker_audio = np.zeros(far_end_speaker_audio_size)

# NLMS parameters
M = 22      # filter length
mu = 0.5    # step size
wk = np.zeros(M)


current_frame_start = 0


def audio_callback(indata, outdata, frames, time, status):
    global current_frame_start, wk, near_end_speaker_audio

    indata = indata[:, 0]  # first channel of microphone input

    if status:
        print(status)

    # Determine chunk boundaries
    end_index = min(current_frame_start + frames, far_end_speaker_audio_size)
    current_chunk_size = end_index - current_frame_start

    # Reference signal (far-end)
    ref = far_end_speaker_audio[current_frame_start:end_index]

    # Play far-end on speaker
    outdata[:current_chunk_size, 0] = ref
    if current_chunk_size < frames:
        outdata[current_chunk_size:, 0] = 0  # zero-pad last chunk

    # Arrays for predicted signal and error (near-end)
    y = np.zeros(current_chunk_size)
    en = np.zeros(current_chunk_size)

    # Process each sample in the chunk
    for n in range(current_chunk_size):
        
        y[n] = compute_y(wk, ref, n)
        en[n] = indata[n] - y[n]
        wk = updatewk(wk, ref, n, mu, en[n])

  
    near_end_speaker_audio[current_frame_start:end_index] = en
    current_frame_start += current_chunk_size


print("Mic → speaker running. Press Ctrl+C to stop.")

try:
    with sd.Stream(
            samplerate=fs,
            channels=channels,
            blocksize=blocksize,
            callback=audio_callback):
        while current_frame_start < far_end_speaker_audio_size:
            sd.sleep(50)  # keep loop responsive
except KeyboardInterrupt:
    print("\nStream stopped manually.")

# Save the near-end audio
sf.write("vn.wav", near_end_speaker_audio, Fs)
print("Near-end audio saved to vn.wav")
