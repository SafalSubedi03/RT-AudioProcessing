#include <iostream>
#include <fstream>
#include <vector>
#include <portaudio.h>

using namespace std;

// WAV file header structure
struct WAVHeader {
    char riff[4] = {'R','I','F','F'};
    uint32_t chunkSize;
    char wave[4] = {'W','A','V','E'};
    char fmt[4] = {'f','m','t',' '};
    uint32_t subChunk1Size = 16;
    uint16_t audioFormat = 1; // PCM
    uint16_t numChannels = 1;
    uint32_t sampleRate = 44100;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample = 16;
    char data[4] = {'d','a','t','a'};
    uint32_t dataSize;
};

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << endl;
        return 1;
    }

    PaStream* stream;
    PaStreamParameters inputParams;
    inputParams.device = Pa_GetDefaultInputDevice();
    if (inputParams.device == paNoDevice) {
        cerr << "No default input device." << endl;
        return 1;
    }

    inputParams.channelCount = 1;          // mono
    inputParams.sampleFormat = paInt16;    // 16-bit PCM
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    int sampleRate = 44100;
    int seconds = 5;                        // record duration
    int totalFrames = sampleRate * seconds;
    vector<int16_t> recordedSamples(totalFrames);

    err = Pa_OpenStream(&stream, &inputParams, nullptr, sampleRate, paFramesPerBufferUnspecified, paClipOff, nullptr, nullptr);
    if (err != paNoError) {
        cerr << "Failed to open stream: " << Pa_GetErrorText(err) << endl;
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        cerr << "Failed to start stream: " << Pa_GetErrorText(err) << endl;
        return 1;
    }

    cout << "Recording for " << seconds << " seconds..." << endl;
    err = Pa_ReadStream(stream, recordedSamples.data(), totalFrames);
    if (err != paNoError) {
        cerr << "Failed to read stream: " << Pa_GetErrorText(err) << endl;
        return 1;
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    cout << "Recording finished!" << endl;

    // Prepare WAV header
    WAVHeader header;
    header.numChannels = 1;
    header.sampleRate = sampleRate;
    header.bitsPerSample = 16;
    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;
    header.dataSize = recordedSamples.size() * sizeof(int16_t);
    header.chunkSize = 36 + header.dataSize;

    // Save to file
    ofstream outFile("recorded.wav", ios::binary);
    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outFile.write(reinterpret_cast<char*>(recordedSamples.data()), recordedSamples.size() * sizeof(int16_t));
    outFile.close();

    cout << "Saved as recorded.wav" << endl;
    return 0;
}
