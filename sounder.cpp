#include <stdio.h>
#include <iostream>
#include <math.h>
#include <portaudio.h>
#include "sounder.h"

namespace sounder {
    
/** Holds a stero left, right pair */
    struct stereoSample {
        const double left;
        const double right;
        stereoSample(const double l, const double r) : left(l), right(r) { };
    };

    /** Abstract class for returning stero signals */
    class playData {
    public:
        virtual stereoSample nextSample() = 0;
        virtual bool finished() = 0;
    };

    /** Struct for holding a function to play and the current time*/
    class playFunctionData : public playData {
    public:
        const double stop;
        const int sampleRate;

        playFunctionData(const std::function<stereoSample(double)> f, const double start, const double stop, const int sampleRate=44100) : 
        f(f), t(start), stop(stop), sampleRate(sampleRate) { };
        
        virtual stereoSample nextSample() {
            if( finished() ) return stereoSample(0.0,0.0);
            t += sampleRate;
            return f(t - sampleRate);
        };
        
        virtual bool finished() { return t >= stop; }
        
    protected:
        const std::function<stereoSample(double)> f;
        double t;
    };

/** Struct contains iterators to sampled audio data*/
    class playSamplesData : public playData {
    public:

        playSamplesData(const std::vector<stereoSample>& f) : current(f.begin()), end(f.end()) {}

        virtual stereoSample nextSample() {
            if (finished()) return stereoSample(0.0, 0.0);
            stereoSample s = *current;
            current++;
            return s;
        };

        virtual bool finished() { return current == end; }
        
    protected:
        std::vector<stereoSample>::const_iterator current;
        const std::vector<stereoSample>::const_iterator end;
    };

/** 
 * Callback for the playSamples function.  Reads from iterators in playSamplesData struct
 * and parse them to the port audio outputBuffer.
 */
static int playCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    playData *data = (playData*)userData;
    float *out = (float*)outputBuffer;
     for( int i=0; i<framesPerBuffer; i++ )
    {
        stereoSample s = data->nextSample();
        *out++ = s.left;  /* left */
        *out++ = s.right;  /* right */
     }
    return data->finished();
}

static void throwerror(std::string e){
    Pa_Terminate();
    throw e;
}
  
void playStereoSamples(const std::vector<stereoSample>& f, const int sampleRate) {
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
    playSamplesData data(f);
    
    PaStream *stream;
    Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sampleRate,
              paFramesPerBufferUnspecified,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              playCallback,
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

void playSamples(const std::vector<double>& fleft, const std::vector<double>& fright, const int sampleRate) {
    if( fleft.size() != fright.size() ) throw "Must have the same number of left and right samples";
    int N = fleft.size(); //the number of samples
    std::vector<stereoSample> f;
    f.reserve(N);
    for(int n = 0; n < N; n++) f.push_back(stereoSample(fleft[n], fright[n]));
    playStereoSamples(f,sampleRate);
}

void playSamples(const std::vector<double>& f, const int sampleRate) {
    playSamples(f,f,sampleRate);
}

}