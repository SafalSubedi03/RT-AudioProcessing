import sounddevice as sd
import numpy as np
import wave

SAMPLE_RATE = 44100
DURATION = 15
CHANNELS = 1
OUTPUT_FILE = "un.wav"

print("Recording...")
audio_data = sd.rec(int(DURATION * SAMPLE_RATE),
                    samplerate=SAMPLE_RATE,
                    channels=CHANNELS,
                    dtype='float32')  # float [-1,1]
sd.wait()
print("Recording finished.")

# Normalize to full scale
max_val = np.max(np.abs(audio_data))
if max_val > 0:
    audio_data = audio_data / max_val  # now in [-1,1]

# Convert to int16 for WAV
audio_int16 = (audio_data * 32767).astype(np.int16)

with wave.open(OUTPUT_FILE, 'wb') as wf:
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(2)  # 16-bit
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(audio_int16.tobytes())

print(f"Saved to {OUTPUT_FILE}")
