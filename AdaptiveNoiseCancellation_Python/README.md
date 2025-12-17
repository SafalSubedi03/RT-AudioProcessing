## **Project Overview**

This project demonstrates a **real-time Active Noise Cancellation (ANC) system** implemented in Python using the **Normalized Least Mean Squares (NLMS) adaptive filter**.

The system works as follows:

1. A **far-end signal** (pre-recorded audio) is played on your computer speakers.
2. The **microphone simultaneously captures the resulting sound**, which includes the far-end audio and any additional near-end voice (e.g., your own voice).
3. The NLMS adaptive filter predicts the far-end audio component in real-time (`y[n]`) and subtracts it from the microphone input (`indata`).
4. The remaining signal (`en[n]`) primarily contains the **near-end voice**, effectively canceling the far-end playback.
5. The filtered near-end signal is saved to a `.wav` file (`vn.wav`) for further use or analysis.

Essentially, this system lets you **record only your own voice** while playing back another audio source, using adaptive filtering.

---

## **Features**

* Real-time audio processing with **Python SoundDevice** and **SoundFile**.
* Adaptive filter implemented using **NLMS algorithm**.
* Configurable filter length (`M`) and step size (`mu`) to control adaptation speed.
* Handles streaming in small chunks (`blocksize`) to minimize latency.

---

## **Installation**

Install all dependencies using the provided `requirements.txt` file:

```bash
pip install -r requirements.txt
```



## **Usage**

1. Place your far-end audio file (e.g., `un.wav`) in the same folder as the script.
2. Run the Python script:

```bash
python main.py
```

3. Speak while the far-end audio plays.
4. Stop the script with `Ctrl+C`.
5. The resulting near-end audio (your voice) will be saved as `vn.wav`.

---

## **Important Notes**

* **Filter length limitation:**
  Due to Python’s processing speed, the filter length (`M`) is limited to around **20 taps**. Longer filters provide better cancellation but may **slow down processing**, leading to audio glitches or increased latency.
* **Step size (`mu`)** affects convergence speed: higher values adapt faster but can cause instability; lower values are more stable but slower.

---

## **Future Work**

* Implement the ANC system in **C++** for **faster processing**, allowing longer filter lengths and improved noise cancellation efficiency.
* Explore **multi-channel microphone setups** for enhanced performance.
* Integrate with **real-time hardware platforms** for practical ANC applications (headphones, smart devices).

---



