#include "../include/WriteWavFile.h"

using namespace std;

void WriteWavFile::write_as_byte(ofstream &file, int value, int byte_size){
    file.write(reinterpret_cast<const char*>(&value), byte_size);
}

// Constructor: store file reference
WriteWavFile::WriteWavFile(ofstream& outFile) : wav(outFile) {}

// Write initial WAV header
void WriteWavFile::writeHeader(int total_data_bytes){
    const int chunk_size = 36 + total_data_bytes;
    const int sub_chunk2_size = total_data_bytes;

    wav << chunk_id;
    write_as_byte(wav, chunk_size, 4);
    wav << format;

    wav << sub_chunk1_id;
    write_as_byte(wav, sub_chunk1_size, 4);
    write_as_byte(wav, audio_format, 2);
    write_as_byte(wav, num_channels, 2); // MONO
    write_as_byte(wav, sample_rate, 4);
    write_as_byte(wav, byte_rate, 4);
    write_as_byte(wav, block_align, 2);
    write_as_byte(wav, bits_per_sample, 2);

    wav << sub_chunk2_id;
    write_as_byte(wav, sub_chunk2_size, 4);
}

// Write PCM data starting at specific sample index
void WriteWavFile::writeData(int16_t* data, unsigned long num_samples, unsigned long int start_index){
    wav.seekp(44 + start_index * sizeof(int16_t), ios::beg);
    wav.write(reinterpret_cast<const char*>(data), num_samples * sizeof(int16_t));
}

// Update WAV header after recording
void WriteWavFile::updateHeader(int total_data_bytes){
    // Update chunk size
    wav.seekp(4, ios::beg);
    write_as_byte(wav, 36 + total_data_bytes, 4);

    // Update subchunk2 size
    wav.seekp(40, ios::beg);
    write_as_byte(wav, total_data_bytes, 4);
}



