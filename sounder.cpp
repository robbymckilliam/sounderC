#include <stdio.h>
#include <iostream>
#include <math.h>
#include <portaudio.h>
#include "sounder.h"

namespace sounder {

 /** Struct contains iterators to sampled audio data*/
struct playSamplesData
{
    std::vector<double>::const_iterator current;
    const std::vector<double>::const_iterator end;
    
    playSamplesData(std::vector<double>::const_iterator current, const std::vector<double>::const_iterator end) : 
    current(current), end(end) {}
};

/** 
 * Callback for the playSamples function.  Reads from iterators in playSamplesData struct
 * and parse them to the port audio outputBuffer.
 */
static int playSamplesCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    playSamplesData *data = (playSamplesData*)userData;
    float *out = (float*)outputBuffer;
    int finished = 0;
     for( int i=0; i<framesPerBuffer; i++ )
    {
         float right_sample = *(data->current);
         float left_sample = right_sample; //actually just mono!
        *out++ = right_sample;  /* left */
        *out++ = left_sample;  /* right */
        
        if(data->current == data->end) {
            finished = 1;
            break;
        }
        else data->current++;
     }
    return finished;
}

static void throwerror(std::string e){
    Pa_Terminate();
    throw e;
}
    
void playSamples(const std::vector<double>& f, const int sampleRate){
    if( paNoError != Pa_Initialize() ) throwerror("Port audio fail to initialise."); //initialise port audio
    
    //setup port audio output device
    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice(); //get default port audio output device
    if(outputParameters.device == paNoDevice) throwerror("Error: No default output device.");
    outputParameters.channelCount = 2;       // stereo output 
    outputParameters.sampleFormat = paFloat32; // 32 bit floating point output 
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    //struct containing iterators to samples to play
    playSamplesData data(f.begin(), f.end());
    
    PaStream *stream;
    Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sampleRate,
              paFramesPerBufferUnspecified,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              playSamplesCallback,
              &data );
    
    if(paNoError != Pa_StartStream(stream) ) throwerror("Port audio stream failed to start.");
    
    //sleep for a sufficient time for all audio to finish (half a second added to ensure this)
    double duration = (f.size()/sampleRate + 0.5);
    Pa_Sleep( 1000 * duration);
    
    if(paNoError != Pa_StopStream(stream) ) throwerror("Port audio stream failed to stop.");
   
    //were finished close the stream and terminate port audio
    if( paNoError != Pa_CloseStream(stream) ) throwerror("Port audio fail to initialise.");
    Pa_Terminate();
    
}

}