import numpy as np 
import soundfile as sf

yn ,Fs = sf.read("yn.wav")
print(yn[:100])