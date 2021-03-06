/* 
 * File:   playsamplestest.cpp
 * Author: Robby McKilliam
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

const int sampleRate = 44100;
const double T = 1.0/sampleRate; //the sampling period
const std::vector<int> freqs = {100, 200, 500, 1000};

bool testPlayMono() {
    const double start = 0.0, stop = 1.0;
    cout << endl;
    for (int hz : freqs) {
        auto f = [hz] (double t) { return sin(hz * 2 * pi * t); };
        cout << "Playing a " << hz << "Hz sinusoid" << endl;
        play(f, start, stop, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlayStereo() {
    const double start = 0.0, stop = 1.0;
    cout << endl;
    for (int hz : freqs) {
        auto fl = [hz] (double t) { return sin(hz * 2 * pi * t); };
        auto fr = [hz] (double t) { return sin((2*hz) * 2 * pi * t); };
        cout << "Playing a " << hz << "Hz sinusoid on left and " << (2*hz) << "Hz sinusoid on right" << endl;
        play(fl, fr, start, stop, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlayUnbufferedMono() {
    const double start = 0.0, stop = 1.0;
    cout << endl;
    for (int hz : freqs) {
        auto f = [hz] (double t) { return sin(hz * 2 * pi * t); };
        cout << "Playing a " << hz << "Hz sinusoid" << endl;
        playUnbuffered(f, start, stop, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlayUnbufferedStereo() {
    const double start = 0.0, stop = 1.0;
    cout << endl;
    for (int hz : freqs) {
        auto fl = [hz] (double t) { return sin(hz * 2 * pi * t); };
        auto fr = [hz] (double t) { return sin((2*hz) * 2 * pi * t); };
        cout << "Playing a " << hz << "Hz sinusoid on left and " << (2*hz) << "Hz sinusoid on right" << endl;
        playUnbuffered(fl, fr, start, stop, sampleRate);
    }
    cout << " ... ";
    return true;
}

bool testPlayShepardTone() {
    const double start = 0.0, stop = 10.0;
    double f0 = 220, R = 15, h = 1.0 / pi; //parameters for Shepard tone
    double T = R*h; //period of Shepard tone
    auto shep = [=] (double t) { return util::shepard(t,f0,R,h); };
    std::vector<double> buf = util::buffer(shep, 0, T, sampleRate); //buffer a single period
    auto f = [&buf] (double t) { //function retrieves from buffer periodically
        int i = ((int) floor(t*sampleRate)) % buf.size(); 
        return buf[i];
    };
    playUnbuffered(f, start, stop, sampleRate); //playing Shepard tone directly unbuffered might not be possible depending on your computer.
    return true;
}

void runtest(string name, function<bool()> test) {
    cout << name << " ... ";
    if (!test()) cout << "FAIL" << endl;
    else cout << "pass" << endl;
}

int main(int argc, char** argv) {
    runtest("Playing mono", testPlayMono);
    runtest("Playing stereo", testPlayStereo);
    runtest("Playing unbuffered mono", testPlayUnbufferedMono);
    runtest("Playing unbuffered stereo", testPlayUnbufferedStereo);
    runtest("Playing a shepard tone for 10 seconds", testPlayShepardTone);
    return (EXIT_SUCCESS);
}

