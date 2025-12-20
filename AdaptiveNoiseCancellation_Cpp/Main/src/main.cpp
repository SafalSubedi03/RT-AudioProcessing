#include <iostream>
#include <fstream>
#include <portaudio.h>
#include <csignal>
#include <algorithm>
#include "../include/ReadWavFile.h"
#include "../include/anclogic.h"
#include "../include/WriteWavFile.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 1024
#define ALPHA 25 // sound amplification factor to store in the output file
#define OALPHA 2 // sound amplification factor for output playback
using namespace std;

// Stop flag for Ctrl+C
static volatile bool keepRunning = true;
void intHandler(int) { keepRunning = false; }

// ANC parameters
const unsigned short int M = 30;
float wk[M] = {0};      // global adaptive filter
float mu = 0.2f;        // learning rate

// File tracking
static unsigned long fileIndex = 0;

// WAV streams
ifstream in("un.wav", ios::binary);
ReadData rd(in);
ofstream outfile("out.wav", ios::binary);
WriteWavFile wd(outfile);

// Global buffer for storing all error samples
int16_t* globalEBuffer = nullptr;

template<typename T>
T clampp(T value, T minVal, T maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

// PortAudio callback
static int audioCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo*,
                         PaStreamCallbackFlags,
                         void*)
{
    int16_t* out = (int16_t*)outputBuffer;
    const int16_t* inMic = (const int16_t*)inputBuffer;

    unsigned long totalSamples = rd.getTotalSamples();
    unsigned long framesToRead = framesPerBuffer;
    if (fileIndex + framesPerBuffer > totalSamples)
        framesToRead = totalSamples - fileIndex;

    int16_t* uData = rd.read(fileIndex, framesToRead);
    if (!uData) { // EOF
        for (unsigned long i = 0; i < framesPerBuffer; i++)
            out[i] = 0;
        return paComplete;
    }

    // Allocate dynamic array for this block
    int16_t* eBlock = new int16_t[framesToRead](); // zero-initialized

    for (unsigned long i = M; i < framesToRead; i++) {
        // Play reference signal
        out[i] = OALPHA*uData[i];

        // Compute y[n] = wk * u[n-k]
        float y = 0.0f;
        for (unsigned short k = 0; k < M; k++)
            y += wk[k] * (uData[i - k] / 32768.0f);

        // Get microphone sample d[n]
        float d = (inMic != nullptr) ? (inMic[i] / 32768.0f) : 0.0f;

        // Compute error signal e[n]
        float e = d - y;

        // Store in dynamic array (scale & clamp)
        eBlock[i] = (int16_t)(clampp(e, -1.0f, 1.0f) * ALPHA * 32767.0f);

        // Update filter coefficients
        float input_power = 1e-6f;
        for (unsigned short k = 0; k < M; k++){
            float temp = uData[i - k] / 32768.0f;
            input_power += temp * temp;
        }
        for (unsigned short k = 0; k < M; k++)
            wk[k] += (mu / input_power) * e * (uData[i - k]/32768.0f);
    }

    // Allocate global buffer if first callback
    if (globalEBuffer == nullptr) {
        globalEBuffer = new int16_t[rd.getTotalSamples()]();
    }

    // Append this block to global buffer
    for (unsigned long i = 0; i < framesToRead; i++)
        globalEBuffer[fileIndex + i] = eBlock[i];

    delete[] eBlock;

    fileIndex += framesToRead;
    return (fileIndex >= totalSamples) ? paComplete : paContinue;
}

// Error checker
static void checkError(PaError err) {
    if (err != paNoError) {
        cout << "PortAudio Error: " << Pa_GetErrorText(err) << endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    signal(SIGINT, intHandler); // handle Ctrl+C
    wd.writeHeader(0);           // placeholder WAV header

    PaError err = Pa_Initialize();
    checkError(err);

    int outDevice = Pa_GetDefaultOutputDevice();
    int inDevice = Pa_GetDefaultInputDevice();
    if (outDevice == paNoDevice || inDevice == paNoDevice) {
        cout << "No default input/output device found!" << endl;
        return -1;
    }

    const PaDeviceInfo* deviceInfo;

    // Output parameters
    PaStreamParameters outputParameters;
    deviceInfo = Pa_GetDeviceInfo(outDevice);
    outputParameters.device = outDevice;
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    // Input parameters
    PaStreamParameters inputParameters;
    deviceInfo = Pa_GetDeviceInfo(inDevice);
    inputParameters.device = inDevice;
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    PaStream* stream;
    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters,
                        SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff,
                        audioCallback, NULL);
    checkError(err);

    err = Pa_StartStream(stream);
    checkError(err);

    cout << "Playing un.wav and performing ANC... Press Ctrl+C to stop." << endl;
    while (Pa_IsStreamActive(stream) == 1 && keepRunning) {
        Pa_Sleep(100);
    }

    err = Pa_StopStream(stream);
    checkError(err);
    err = Pa_CloseStream(stream);
    checkError(err);
    Pa_Terminate();

    // Write the full error signal to WAV once
    if (globalEBuffer != nullptr) {
        wd.writeData(globalEBuffer, fileIndex, 0);
        wd.updateHeader(fileIndex * sizeof(int16_t));
        delete[] globalEBuffer;
    }

    outfile.close();
    in.close();

    cout << "ANC finished. Total samples: " << fileIndex << endl;
    for (int i = 0; i < M; i++)
        cout << "w[" << i << "] = " << wk[i] << endl;

    return 0;
}
