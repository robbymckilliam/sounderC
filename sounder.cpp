#include <stdio.h>
#include <iostream>
#include <math.h>
#include <portaudio.h>
#include "sounder.h"

namespace sounder {

    /** 
     * The number of sample per buffer.  Set to 2048, which at CD quality (44100Hz) sampling 
     * (the default) corresponds to approximately 46 milliseconds.  This will approximately be
     * the delay for audio.  Not a problem for anything I'm currently doing.
     */
    static constexpr int buffer_size = 2048;

    /// Holds a stero left, right pair
    struct stereoSample {
        const double left;
        const double right;
        stereoSample(const double l, const double r) : left(l), right(r) {
        };
    };

    /// Abstract class for returning stereo signals 
    class playData {
    public:
        const int sampleRate;
        playData(int sampleRate) : sampleRate(sampleRate) { };
        virtual stereoSample nextSample() = 0;
        virtual bool finished() = 0;
    };

    /// Class for holding a function to play and the current time
    class playFunctionData : public playData {
    public:
        const double stop;
        ///sample period
        const double T;

        playFunctionData(const std::function<stereoSample(double)> fleft, const double start, const double stop, const int sampleRate) :
        f(fleft), t(start), stop(stop), T(1.0 / sampleRate), playData(sampleRate) { };

        virtual stereoSample nextSample() {
            if (finished()) return stereoSample(0.0, 0.0);
            t += T;
            return f(t - sampleRate);
        };

        virtual bool finished() { return t >= stop; }

    protected:
        const std::function<stereoSample(double)> f;
        double t;
    };

    /// Class contains iterators to sampled audio data
    class playSamplesData : public playData {
    public:

        playSamplesData(const std::vector<stereoSample>& f, const int sampleRate) :
        current(f.begin()), end(f.end()), playData(sampleRate) { };

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
    static int playCallback(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData) {
        playData *data = (playData*) userData;
        float *out = (float*) outputBuffer;
        for (int i = 0; i < framesPerBuffer; i++) {
            stereoSample s = data->nextSample();
            *out++ = s.left; /* left */
            *out++ = s.right; /* right */
        }
        return data->finished() ? paComplete : paContinue;
    }

    static void throwerror(std::string e) {
        Pa_Terminate();
        throw e;
    }

    /** Plays stereo data out the speakers */
    void playStereoData(playData* d) {
        if (paNoError != Pa_Initialize()) throwerror("Port audio fail to initialise."); //initialise port audio

        //setup port audio output device
        PaStreamParameters outputParameters;
        outputParameters.device = Pa_GetDefaultOutputDevice(); //get default port audio output device
        if (outputParameters.device == paNoDevice) throwerror("Error: No default output device.");
        outputParameters.channelCount = 2; // stereo output 
        outputParameters.sampleFormat = paFloat32; // 32 bit floating point output 
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;

        PaStream *stream;
        Pa_OpenStream(
                &stream,
                NULL, /* no input */
                &outputParameters,
                d->sampleRate,
                buffer_size,
                paClipOff, /* we won't output out of range samples so don't bother clipping them */
                playCallback,
                d);

        if (paNoError != Pa_StartStream(stream)) throwerror("Port audio stream failed to start.");

        //sleep until stream is finished (up to 200 millisecond delay at end)
        while (Pa_IsStreamActive(stream)) Pa_Sleep(200);

        if (paNoError != Pa_StopStream(stream)) throwerror("Port audio stream failed to stop.");

        //were finished close the stream and terminate port audio
        if (paNoError != Pa_CloseStream(stream)) throwerror("Port audio fail to initialise.");
        Pa_Terminate();
    }

    void playSamples(const std::vector<double>& fleft, const std::vector<double>& fright, const int sampleRate) {
        if (fleft.size() != fright.size()) throw "Must have the same number of left and right samples";
        int N = fleft.size(); //the number of samples
        std::vector<stereoSample> f;
        f.reserve(N);
        for (int n = 0; n < N; n++) f.push_back(stereoSample(fleft[n], fright[n]));
        playData* pd = new playSamplesData(f, sampleRate);
        playStereoData(pd);
        delete pd;
    }

    void playSamples(const std::vector<double>& f, const int sampleRate) {
        playSamples(f, f, sampleRate);
    }

    void play(const std::function<double(double) > fleft, const std::function<double(double) > fright, const double start, const double stop, const int sampleRate) {
        std::vector<double> fl = util::buffer(fleft, start, stop, sampleRate);
        std::vector<double> fr = util::buffer(fright, start, stop, sampleRate);
        playSamples(fl, fr, sampleRate);
    }

    void play(const std::function<double(double) > f, const double start, const double stop, const int sampleRate) {
        std::vector<double> f_ = util::buffer(f, start, stop, sampleRate);
        playSamples(f_, sampleRate);
    }

    void playUnbuffered(const std::function<double(double) > fleft, const std::function<double(double) > fright, const double start, const double stop, const int sampleRate) {
        auto f = [fleft, fright] (double t) { return stereoSample(fleft(t), fright(t)); };
        playFunctionData* pd = new playFunctionData(f, start, stop, sampleRate);
        playStereoData(pd);
        delete pd;
    }

    void playUnbuffered(const std::function<double(double) > f, const double start, const double stop, const int sampleRate) {
        auto f_ = [f] (double t) { 
            double v = f(t);
            return stereoSample(v, v); 
        };
        playFunctionData* pd = new playFunctionData(f_, start, stop, sampleRate);
        playStereoData(pd);
        delete pd;
    }

    namespace util {

        double fracpart(double t) {
            return t - round(t);
        }

        double shepard(double t, double f0, double R, double h) {
            double B = R; //kernel bandwidth, how fast the high and low frequencies are attenuated (fixed to R)
            auto s = [ = ] (double t_){return exp(-t_ * t_ / B) * sin(2 * pi * f0 * pow(2, -t_ / R) * t_) / sqrt(pi * B);}; //chirp pulse
            double T = R*h; //period
            int kmin = (int) floor(-10 * B / T); //sum out to 10 standard deviations in positive and negative direction
            int kmax = (int) ceil(10 * B / T);
            double tp = T * fracpart(t / T); //get equivalent time inside fundamental period
            double out = 0.0;
            for (int k = kmin; k <= kmax; k++) out += s(tp - T * k) * T / 1.5;
            return out;
        }

        std::vector<double> buffer(const std::function<double(double) > f, const double start, const double stop, const int sampleRate) {
            const double T = 1.0 / sampleRate; //sample period
            int N = (int) floor((stop - start) * sampleRate); //size of buffer
            std::vector<double> buf;
            buf.reserve(N);
            for (double t = start; t <= stop; t += T) buf.push_back(f(t));
            return buf;
        }

    }

}
