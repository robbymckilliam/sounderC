/* 
 * File:   sounder.h
 * Author: Robby McKilliam
 *
 * Created on 22 March 2014, 10:07 AM
 */

#ifndef SOUNDER_H
#define	SOUNDER_H

#include <functional>
#include <vector>

namespace sounder {

    static constexpr double pi = 3.14159265359;

    /** 
     * Plays the function fleft out of the left speaker and fright out of the right speaker from time 
     * start to time stop out of the speakers.  Optional argument is sampleRate (default 44100Hz i.e. CD quality).
     * Buffers samples of the function first. This is reliable but will consume lots of memory for 
     * playbacks of long duration.  Use playUnbuffered in this case.
     */
    void play(const std::function<double(double) > fleft, const std::function<double(double) > fright, const double start, const double stop, const int sampleRate = 44100);

    /** 
     * Plays the function f from time start to time stop out of the speakers.  Optional arguments are 
     * sampleRate (default 44100Hz i.e. CD quality).  Buffers samples of the function first.  This is reliable but
     * will consume lots of memory for playbacks of long duration.  Use playUnbuffered in this case.
     */
    void play(const std::function<double(double) > f, const double start, const double stop, const int sampleRate = 44100);
    
    /** 
     * Plays sequence of stereo samples fleft and fright out of the speakers.  fleft are samples for left
     * speaker and fright are samples for right speakers.  The number of left and right samples must be
     * the same, that is fleft.size() must equal fright.size().
     * An optional argument is sampleRate (default 44100Hz i.e. CD quality)
     */
    void playSamples(const std::vector<double>& fleft, const std::vector<double>& fright, const int sampleRate = 44100);

    /** 
     * Plays sequence of samples f out of the speakers.  Mono output to both left and right speakers..
     * An optional argument is sampleRate (default 44100Hz i.e. CD quality)
     */
    void playSamples(const std::vector<double>& f, const int sampleRate = 44100);
    
    /** 
     * Plays the function fleft out of the left speaker and fright out of the right speaker from time 
     * start to time stop out of the speakers.  Optional argument is sampleRate (default 44100Hz i.e. CD quality).
     * The functions fleft and fright must be cheap or the computer will not be able to keep up with the soundcard.
     * Buffer underrun will likely occur. You can use the function play to alleviate this problem at 
     * the expense of memory.
     */
    void playUnbuffered(const std::function<double(double) > fleft, const std::function<double(double) > fright, const double start, const double stop, const int sampleRate = 44100);

    /** 
     * Plays the function f from time start to time stop out of the speakers.  Optional arguments are 
     * sampleRate (default 44100Hz i.e. CD quality).
     * The function f must be cheap or the computer will not be able to keep up with the soundcard.
     * Buffer underrun will likely occur. You can use the function play to alleviate this problem at 
     * the expense of memory
     */
    void playUnbuffered(const std::function<double(double) > f, const double start, const double stop, const int sampleRate = 44100);


    namespace util {

        /** Returns the (centered) fractional part of t */
        double fracpart(double t);

        /** 
         * Return a Sherpard tone (sonic barbers pole) at time t.
         * f0 : center frequency where amplitude is largest (default f0=220Hz)
         * R : exponential frequency decay rate. Value of 4 means it takes 4 seconds for the frequency to half (default 15)
         * h : spacing between frequencies in octaves, Value of 1 meas an octave spacing (default 1/Pi)
         * The signal constructed will have period T=Rh
         */
        double shepard(double t, double f0 = 220, double R = 15, double h = 1.0 / pi);

        /** 
         * Record samples of a function into a buffer of std::vector<double> from time start to
         * time stop and a given sample rate.
         */
        std::vector<double> buffer(const std::function<double(double) > f, const double start, const double stop, const int sampleRate);

    }

}

#endif	/* SOUNDER_H */

