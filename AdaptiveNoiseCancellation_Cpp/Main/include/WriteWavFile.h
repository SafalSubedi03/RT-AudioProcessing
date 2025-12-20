#ifndef WRITEWAVFILE_H
#define WRITEWAVFILE_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

class WriteWavFile {
private:
    unsigned long int totalsample;
    std::ofstream& wav; // reference to output file

    // RIFF Chunk
    const std::string chunk_id = "RIFF";
    const std::string format = "WAVE";

    // fmt Chunk
    const std::string sub_chunk1_id = "fmt ";
    const int sub_chunk1_size = 16;
    const int audio_format = 1;        // PCM
    const int num_channels = 1;        // mono
    const int sample_rate = 44100;
    const int bits_per_sample = 16;
    const int byte_rate = sample_rate * num_channels * (bits_per_sample / 8);
    const int block_align = num_channels * (bits_per_sample / 8);

    // Data chunk
    const std::string sub_chunk2_id = "data";

    void write_as_byte(std::ofstream &file, int value, int byte_size);

public:
    WriteWavFile(std::ofstream& outFile); // only declare
    void writeHeader(int total_data_bytes);
    void writeData(int16_t* data, unsigned long num_samples, unsigned long int start_index);
    void updateHeader(int total_data_bytes);
    
};

#endif
