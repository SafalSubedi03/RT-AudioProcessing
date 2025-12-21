# Adaptive Noise Cancellation (ANC) in C++

## Project Overview

This project implements **Adaptive Noise Cancellation (ANC)** using **C++** with **PortAudio** for real-time audio input/output. It also includes a **Python implementation** for reference and testing.

The system takes a reference audio signal (`un.wav`), plays it in real-time, records the environment using a microphone, and outputs a WAV file (`out.wav`) containing the **error signal**, i.e., the recorded audio with the reference signal suppressed.

The core of the ANC algorithm is an **adaptive filter** using the **Normalized Least Mean Squares (NLMS)** method to iteratively cancel the noise.

---

## Features

* Real-time audio playback and recording using **PortAudio**.
* Adaptive filtering using **NLMS algorithm**.
* Output is a `.wav` file (`out.wav`) in **16-bit PCM** format with the reference signal suppressed.
* Global handling of filter coefficients and buffers for real-time processing.
* Works with single-channel audio at **44.1 kHz** sample rate.
* Graceful termination on `Ctrl+C`.

---

## Requirements

* **C++17 or higher**
* **PortAudio library** (required for real-time audio playback and recording; x64 for Windows recommended)
* Standard C++ libraries: `<iostream>`, `<fstream>`, `<csignal>`, `<algorithm>`
* Input WAV file (`un.wav`) in the project directory

Optional for Python comparison/testing:

* `numpy`
* `sounddevice`
* `soundfile`

---

## Notes

* The project assumes **single-channel audio**.
* The output file length is automatically truncated to be divisible by `FRAMES_PER_BUFFER`.
* The filter adapts in real-time; initial outputs may contain residual noise.
* The project can be extended to **multi-channel audio** or **more advanced adaptive filters**.

---

## Usage

### C++

This project is set up for **easy building and execution in Visual Studio Code** using the Microsoft Visual Studio (MSVS) compiler:

1. **Building the project**

   * Press `Ctrl + Shift + B` in VS Code.
   * This will **automatically build `main.cpp`** using MSVS.
   * All generated **build files and the executable (`.exe`)** are placed in the `build` folder.

2. **Batch file automation**

   * A **batch file** is included in the project.
   * The batch file runs the same commands as defined in `tasks.json`, compiling `main.cpp`, linking the **PortAudio library**, and copying necessary DLLs to the build folder.
   * You can run this batch file manually if needed to perform the build outside of VS Code.

3. **Running the program**

   * Ensure your input file `un.wav` is present in the project directory.
   * Run the generated `.exe` from the `build` folder.
   * The program will play the reference signal, record the microphone input, and generate `out.wav` containing the error signal.
   * Press `Ctrl + C` to stop real-time processing safely.

### Python

* Install the required Python libraries using:

  ```bash
  pip install -r requirements.txt
  ```
* Run the Python script directly.
* The script will process the reference signal, perform adaptive noise cancellation, and generate the output WAV file.

---

