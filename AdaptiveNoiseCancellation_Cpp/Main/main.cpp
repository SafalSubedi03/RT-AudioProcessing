// This file will contain the code to do real time audio processing
#include<iostream>
#include<portaudio.h>
#include<cstring>


#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512


using namespace std;
static void checkError(PaError err){
    if(err != paNoError){
        cout<<"Got Error:- "<<Pa_GetErrorText(err)<<endl;
        exit(EXIT_FAILURE);
    }
}


static int patestCallback(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags, void* userData
){
    return 0;
}

int main(){
    PaError err;
    err = Pa_Initialize();
    checkError(err);

    int numDevices = Pa_GetDeviceCount();
    cout<<"Found "<<numDevices<<" devices."<<endl;
    if(numDevices < 0){
        cout<<"Error Getting Device Count"<<endl;
        exit(EXIT_FAILURE);
    }
    const PaDeviceInfo* deviceInfo;
    // for(int i = 0;i < numDevices;i++){
    //     deviceInfo = Pa_GetDeviceInfo(i);
    //     cout<<"Device "<<i+1<<endl;
    //     cout<<"     name:- "<<deviceInfo->name<<endl;
    //     cout<<"     maxInputChannel:- "<<deviceInfo->maxInputChannels<<endl;
    //     cout<<"     maxOutputChannels:- "<<deviceInfo->maxOutputChannels<<endl;
    //     cout<<"     defaultSampleRate:- "<<deviceInfo->defaultSampleRate<<endl;
    // }
    int indevice = 0;
    int outdevice = 2;
    int duration = 5; //in s

    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;

    deviceInfo = Pa_GetDeviceInfo(indevice);
    memset(&inputParameters, 0,sizeof(inputParameters));
    inputParameters.channelCount = 2;
    inputParameters.device = indevice;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;

    deviceInfo = Pa_GetDeviceInfo(outdevice);
    memset(&outputParameters, 0,sizeof(outputParameters));
    outputParameters.channelCount = 2;
    outputParameters.device = outdevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = deviceInfo->defaultHighOutputLatency;

    PaStream* stream;
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        patestCallback,
        NULL
    );
    checkError(err);

    err = Pa_StartStream(stream);
    checkError(err);

    cout<<"---RECORDING AUDIO FOR "<<duration<<" SECONDS---"<<endl;
    Pa_Sleep(duration * 1000);

    err = Pa_StopStream(stream);
    checkError(err);
    err = Pa_CloseStream(stream);
    checkError(err);


    err = Pa_Terminate();
    checkError(err);
    return EXIT_SUCCESS;


}
