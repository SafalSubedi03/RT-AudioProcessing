#ifndef READDATA_H
#define READDATA_H

#include <fstream>
#include <cstdint>

class ReadData {
private:
    unsigned int totalSamples;   // total number of samples (16-bit PCM)
    int16_t* data;               // buffer for samples
    std::ifstream& in;           // reference to opened wav file

public:
    // Constructor
    ReadData(std::ifstream& inF);

    // Read samples starting from a given index
    int16_t* read(unsigned long int starting_sample,unsigned long num_samples);

    unsigned int getTotalSamples() const { return totalSamples;}

    
    // Destructor
    ~ReadData();


};

#endif // READDATA_H
