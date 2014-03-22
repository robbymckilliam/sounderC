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

bool testPlaySamples() {
    const int sampleRate = 44100;
    const double T = 1.0/sampleRate; //the sampling period
    const double duration = 2.0; //two seconds
    const int numsamples = (int) floor(sampleRate * duration);
    const auto f = [] (double t) { return sin(300 * pi * t); };
    vector<double> y;
    for(int n = 0; n < numsamples; n++) y.push_back(f(n*T));
    playSamples(y,sampleRate);
    return true;
}

void runtest(string name, function<bool()> test) {
    cout << name << " ... ";
    if (!test()) cout << "FAIL" << endl;
    else cout << "pass" << endl;
}

int main(int argc, char** argv) {
    runtest("Play samples ", testPlaySamples);
    return (EXIT_SUCCESS);
}

