//
//  LowPassFilter.h
//
//  Created by Saul Laufer on 10/28/16.
//
//

#ifndef LOWPASSFILTER_H_INCLUDED
#define LOWPASSFILTER_H_INCLUDED

#include <stdio.h>

//==============================================================================
/**
 */
class SaulBWFilter
{
public:
    SaulBWFilter(double sampleRate);
    ~SaulBWFilter();
    
    //==============================================================================
    void createFilter(double cutoffFreq, double qFactor, int dbPerOct);
    void createFilter(double cutOffFreq);
    void createHighTransformFilter(double cutoffFreq, int qFactor);
    
    void processSamples(float* samplePtr, int numSamples);
    void processSingleSampleRaw(float sample);
    
    void reset();
    //==============================================================================

private:
    float coefs[5];
    
    double sampleRate;
    int slope;
    
    const double pi = 3.1415926535897932384626433832795;
    
    double xv[3];
    double yv[3];
    
    //==============================================================================
    void processSamplesInternal(float* samplePtr, int numSamples);

protected:
    //==============================================================================
    float v1, v2;
};

#endif
