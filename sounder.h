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
   */
  void play(const std::function<double(double)> fleft, const std::function<double(double)> fright, const double start, const double stop, const int sampleRate = 44100);
 
/** 
   * Plays the function f from time start to time stop out of the speakers.  Optional aguments are 
   * sampleRate (default 44100Hz i.e. CD quality).
   */
  void play(const std::function<double(double)> f, const double start, const double stop, const int sampleRate = 44100);
  
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
  
}

#endif	/* SOUNDER_H */

