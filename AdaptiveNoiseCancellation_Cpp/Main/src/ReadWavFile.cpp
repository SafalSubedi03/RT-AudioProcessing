#include "../include/ReadWavFile.h"
#include <iostream>

using namespace std;

ReadData::ReadData(ifstream& inF)
    : totalSamples(0), data(nullptr), in(inF)
{
    // Move to data-size field in WAV header
    in.seekg(40, ios::beg);

    uint32_t dataBytes = 0;
    in.read(reinterpret_cast<char*>(&dataBytes), 4);

    // 16-bit PCM → 2 bytes per sample
    totalSamples = dataBytes / 2;
}

int16_t* ReadData::read(unsigned long int starting_sample, unsigned long num_samples)
{
    if (starting_sample + num_samples > totalSamples) {
        cout << "Invalid data request" << endl;
        return nullptr;
    }

    // Reallocate buffer
    delete[] data;
    data = new int16_t[num_samples];

    // Seek to PCM data start (44 bytes header)
    in.seekg(44 + starting_sample * 2, ios::beg);

    // Read PCM samples
    in.read(reinterpret_cast<char*>(data), num_samples * 2);

    return data;
}

ReadData::~ReadData()
{
    delete[] data;
}
