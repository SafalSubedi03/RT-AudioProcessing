#include <iostream>
#include "portaudio.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

int main()
{
    PaError err;
    PaStream* stream;

    err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cout << "PortAudio init failed\n";
        return -1;
    }

    PaStreamParameters inputParams;
    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = 1;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(
        &stream,
        &inputParams,
        nullptr,                // no output
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        nullptr,
        nullptr
    );

    if (err != paNoError)
    {
        std::cout << "Failed to open stream\n";
        Pa_Terminate();
        return -1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cout << "Failed to start stream\n";
        return -1;
    }

    std::cout << "Recording... speak into mic (5 seconds)\n";

    float buffer[FRAMES_PER_BUFFER];

    for (int i = 0; i < 5 * SAMPLE_RATE / FRAMES_PER_BUFFER; i++)
    {
        Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    std::cout << "Done.\n";
    return 0;
}
