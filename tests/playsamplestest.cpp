/* 
 * File:   playsamplestest.cpp
 * Author: harprobey
 *
 * Created on 22/03/2014, 4:13:04 PM
 */

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <functional>
#include "sounder.h"

using namespace std;
using namespace sounder;

bool testPlaySamplesMono() {
    const int sampleRate = 44100;
    const double T = 1.0/sampleRate; //the sampling period
    const double duration = 2.0; //two seconds
    const int numsamples = (int) floor(sampleRate * duration);
    cout << endl;
    std::vector<int> freqs = {200, 400, 1000, 2000};
    for (int hz : freqs) {
        const auto f = [hz] (double t) { return sin(hz * pi * t); };
        vector<double> y;
        for (int n = 0; n < numsamples; n++) y.push_back(f(n * T));
        cout << "Playing a " << hz << "Hz sinusoid" << endl;
        playSamples(y, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlaySamplesStereo() {
    const int sampleRate = 44100;
    const double T = 1.0/sampleRate; //the sampling period
    const double duration = 2.0; //two seconds
    const int numsamples = (int) floor(sampleRate * duration);
    cout << endl;
    std::vector<int> freqs = {200, 400, 1000, 2000};
    for (int hz : freqs) {
        const auto fl = [hz] (double t) { return sin(hz * pi * t); };
        const auto fr = [hz] (double t) { return sin((hz+100) * pi * t); };
        vector<double> yl;
        for (int n = 0; n < numsamples; n++) yl.push_back(fl(n * T));
        vector<double> yr;
        for (int n = 0; n < numsamples; n++) yr.push_back(fr(n * T));
        cout << "Playing a " << hz << "Hz sinusoid on left and " << (hz + 100) << "Hz sinusoid on right" << endl;
        playSamples(yl, yr, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlayFunctionMono() {
    const int sampleRate = 44100;
    const double T = 1.0/sampleRate; //the sampling period
    const double start = 0.0, stop = 2.0;
    cout << endl;
    std::vector<int> freqs = {200, 400, 1000, 2000};
    for (int hz : freqs) {
        auto f = [hz] (double t) { return sin(hz * pi * t); };
        cout << "Playing a " << hz << "Hz sinusoid" << endl;
        play(f, start, stop, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlayFunctionStereo() {
    const int sampleRate = 44100;
    const double T = 1.0/sampleRate; //the sampling period
    const double start = 0.0, stop = 2.0;
    cout << endl;
    std::vector<int> freqs = {200, 400, 1000, 2000};
    for (int hz : freqs) {
        auto fl = [hz] (double t) { return sin(hz * pi * t); };
        auto fr = [hz] (double t) { return sin((hz+100) * pi * t); };
        cout << "Playing a " << hz << "Hz sinusoid on left and " << (hz + 100) << "Hz sinusoid on right" << endl;
        play(fl, fr, start, stop, sampleRate);
    }
    cout << " ... ";
    return true;
}

void runtest(string name, function<bool()> test) {
    cout << name << " ... ";
    if (!test()) cout << "FAIL" << endl;
    else cout << "pass" << endl;
}

int main(int argc, char** argv) {
    runtest("Playing sample mono", testPlaySamplesMono);
    runtest("Playing function mono", testPlayFunctionMono);
    runtest("Playing sample stereo", testPlaySamplesStereo);
    runtest("Playing function stereo", testPlaySamplesStereo);
    return (EXIT_SUCCESS);
}

