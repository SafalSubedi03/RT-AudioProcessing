import numpy as np
import wave

# Parameters
duration = 20            # seconds
sample_rate = 44100      # Hz
frequency = 440          # Hz (A4 note)
amplitude = 32767        # Max for 16-bit PCM

# Generate time array
t = np.linspace(0, duration, int(sample_rate * duration), endpoint=False)

# Generate a simple melody: sum of 2 sine waves
signal = 0.5 * np.sin(2 * np.pi * frequency * t) + 0.5 * np.sin(2 * np.pi * 660 * t)
signal = (signal / np.max(np.abs(signal)) * amplitude).astype(np.int16)  # Normalize and convert to 16-bit

# Save as WAV
with wave.open("generated_music.wav", "w") as wav_file:
    wav_file.setnchannels(1)          # Mono
    wav_file.setsampwidth(2)          # 16-bit PCM
    wav_file.setframerate(sample_rate)
    wav_file.writeframes(signal.tobytes())

print("generated_music.wav created successfully!")
