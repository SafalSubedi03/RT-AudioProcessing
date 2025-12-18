// This file will be used to learn manipulation of wav file using cpp without any third party library

#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;

//RIFF Chunk
const string chunk_id = "RIFF";
const string chunk_size = "----";
const string format = "WAVE";

// fmt Chunk
const string sub_chunk1_id = "fmt ";
const int sub_chunk1_size = 16;
const int audio_format = 1;
const int num_channels = 2;
const int sample_rate = 44100;
const int byte_rate = sample_rate * num_channels * (sub_chunk1_size / 8);
const int block_align = num_channels * (sub_chunk1_size/ 8);
const int bits_per_sample = 16;

// data chunk
const string sub_chunk2_id = "data";
const string sub_chunk2_size = "----";

//helper function to write header into the file with correct data size
void write_as_byte(ofstream &file,int value, int byte_size){
    //write method takes 1byte pointer only
    file.write(reinterpret_cast<const char*>(&value), byte_size);
}

// Audio file parameters
const int frequency = 900;
const int max_amplitude = 32767;
const int duration = 2; //in s



int main(){

    ofstream wav;
    wav.open("test.wav",ios::binary);

    if(wav.is_open()){
        wav<<chunk_id;
        wav<<chunk_size;
        wav<<format;

        wav<<sub_chunk1_id;
        write_as_byte(wav,sub_chunk1_size,4);
        write_as_byte(wav,audio_format,2);
        write_as_byte(wav,num_channels,2);
        write_as_byte(wav,sample_rate,4);
        write_as_byte(wav,byte_rate,4);
        write_as_byte(wav,block_align,2);
        write_as_byte(wav,bits_per_sample,2);

        wav <<sub_chunk2_id;
        wav<<sub_chunk2_size;

        int start_audio = wav.tellp();

        for (int i =0;i<duration *sample_rate;i++){
            double amplitude = (i / duration * sample_rate) * max_amplitude;
            double value = sin((2*3.14*frequency*i)/sample_rate);

            double channel1 = amplitude * value;
            double channel2 = (max_amplitude-amplitude) * value;
            // Write data values in each channel
            write_as_byte(wav , channel1, 4);
            write_as_byte(wav , channel2, 4);

            int end_audio = wav.tellp();
            //Store the size of the audio data in sub_chunk2_size
            wav.seekp(start_audio-4);
            write_as_byte(wav,end_audio-start_audio,4);

            // Store the chunkSize
            wav.seekp(4,ios::beg);
            write_as_byte(wav,end_audio-start_audio+sub_chunk1_size+16+4,4);
            // write_as_byte(wav,end_audio-8,4);

        }
    }


    wav.close();
    return 0;
}