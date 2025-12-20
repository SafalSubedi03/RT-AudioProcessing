#include <iostream>
#include <fstream>
#include <portaudio.h>
#include <csignal>
#include <algorithm>
#include "../include/ReadWavFile.h"
#include "../include/anclogic.h"
#include "../include/WriteWavFile.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512

using namespace std;

// Stop flag for Ctrl+C
static volatile bool keepRunning = true;
void intHandler(int) { keepRunning = false; }

// ANC parameters
const unsigned short int M = 128;
float wk[M] = {0};      // global adaptive filter
float mu = 0.2f;        // learning rate

// File tracking
static unsigned long fileIndex = 0;

// WAV streams
ifstream in("un.wav", ios::binary);
ReadData rd(in);
ofstream outfile("out.wav", ios::binary);
WriteWavFile wd(outfile);


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

    float eBuffer[FRAMES_PER_BUFFER] = {0};

    for (unsigned long i = M; i < framesToRead; i++) {
        // Play reference signal
        out[i] = uData[i];

        // Compute y[n] = wk * u[n-k]
        float y = 0.0f;
        for (unsigned short k = 0; k < M; k++)
            y += wk[k] * uData[i - k];

        // Get microphone sample d[n]
        float d = (inMic != nullptr) ? static_cast<float>(inMic[i]) : 0.0f;

        // Compute error signal e[n]
        eBuffer[i] = d - y;

        // Update filter coefficients
        float input_power = 1e-6f;
        for (unsigned short k = 0; k < M; k++)
            input_power += uData[i - k] * uData[i - k];

        for (unsigned short k = 0; k < M; k++)
            wk[k] += (mu / input_power) * eBuffer[i] * uData[i - k];
    }

    // Convert error signal to int16_t for WAV
    int16_t eInt[FRAMES_PER_BUFFER] = {0};
    for (unsigned long i = 0; i < framesToRead; i++)
        eInt[i] = static_cast<int16_t>(clampp(eBuffer[i], -32768.0f, 32767.0f));

    // Write error signal to WAV
    wd.writeData(eInt, framesToRead, fileIndex);

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

    // Update WAV header with recorded samples
    wd.updateHeader(fileIndex * sizeof(int16_t));
    outfile.close();
    in.close();

    cout << "ANC finished. Total samples: " << fileIndex << endl;
    for (int i = 0; i < M; i++)
        cout << "w[" << i << "] = " << wk[i] << endl;

    return 0;
}
