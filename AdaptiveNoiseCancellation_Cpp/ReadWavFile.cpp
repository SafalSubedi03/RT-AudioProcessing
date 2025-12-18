#include <iostream>
#include <fstream>
#include<vector>

using namespace std;

class readFile {
private:
    ifstream& wav;
    unsigned int sampling_freq;
    unsigned short channels;
    unsigned short audio_format;
    unsigned int subchunk2_size;
    unsigned short int block_align;
    unsigned short int bits_per_sample;


public:
    readFile(ifstream& wav_stream) : wav(wav_stream) {}

    void readInfo() {
        // Sampling frequency (4 bytes at byte offset 24)
        wav.seekg(24, ios::beg);
        wav.read(reinterpret_cast<char*>(&sampling_freq), 4);

        // Number of channels (2 bytes at byte offset 22)
        wav.seekg(22, ios::beg);
        wav.read(reinterpret_cast<char*>(&channels), 2);

        // Audio format (2 bytes at byte offset 20)
        wav.seekg(20, ios::beg);
        wav.read(reinterpret_cast<char*>(&audio_format), 2);

        //Read the data size
        wav.seekg(40,ios::beg);
        wav.read(reinterpret_cast<char*>(&subchunk2_size),4);

        //Read number of bytes for one sample
        wav.seekg(32,ios::beg);
        wav.read(reinterpret_cast<char*>(&block_align),2);

        wav.seekg(34,ios::beg);
        wav.read(reinterpret_cast<char*>(&bits_per_sample),2);
    }

    unsigned int get_sampling_freq() { return sampling_freq; }
    unsigned short get_channel_numbers() { return channels; }
    unsigned short get_block_align() { return block_align; }

    string get_audio_format() {
        if (audio_format == 1) return "PCM";
        else if (audio_format == 3) return "IEEE float";
        else return "Other";
    }



    //Assuming the data is PCM 16 bit format
    std::vector<int16_t> readData(unsigned  int number_of_samples,unsigned short int starting_sample){
        const short int bytes_per_sample = bits_per_sample/8;
        
        if(subchunk2_size < number_of_samples * bytes_per_sample){
            cout<<"Inadequate data present"<<endl;
            return {};
        }

        std::vector<int16_t> samples(number_of_samples);
        wav.seekg(44 + bytes_per_sample * starting_sample,ios::beg);

        for(int i = 0; i<number_of_samples;i++){
            wav.read(reinterpret_cast<char*>(&samples[i]),bytes_per_sample);
        }
        
        return samples;
   
    }
};

int main() {
    ifstream wav("test.wav", ios::binary);
    if (!wav) {
        cerr << "Cannot open WAV file!\n";
        return 1;
    }

    readFile rf(wav);
    rf.readInfo();

    cout << "Sampling Freq: " << rf.get_sampling_freq()
         << "  Number of Channels: " << rf.get_channel_numbers()
         << "  Audio Format: " << rf.get_audio_format() << endl;

    
    unsigned int number_of_samples = 441;
    unsigned short int starting_sample = 0;
    
    std::vector<int16_t> data = rf.readData(number_of_samples,starting_sample);
    cout<<"[";
    for(int i = 0;i<data.size();i++){
        cout<<data[i]<<",";
    }
    
    cout<<"]";

    
    return 0;
}
