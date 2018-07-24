//
//  LowPassFilter.cpp
//
//  Created by Saul Laufer on 10/28/16.
//
//

#include "SaulBWFilter.h"
#include "Math.h"

//==============================================================================
SaulBWFilter::SaulBWFilter(double sampleRate)
{
    this.sampleRate = sampleRate;
}
SaulBWFilter::~SaulBWFilter(){}
//==============================================================================
void SaulBWFilter::createFilter(double sampleRate, double cutoffFreq)
{
    createFilter(sampleRate, cutoffFreq, 0.5, 24);
}

// for pure butterworth, qFactor should be 0.5

// NOTE:: dbPerOct MUST be divisible by 12
// theoretically can be anything but must 12 and 24 are the only realistic ones
// phase response gets super fucked up after 24

void SaulBWFilter::createFilter(double sampleRate, double cutoffFreq, double qFactor, int dbPerOct)
{
    if (dbPerOct == 24)
    {
        slope = 2;
    }
    else
    {
        slope = 1;
    }
    
    const double n = 1.0 / tan (2.0 * pi * cutoffFreq / sampleRate);
    const double nSquared = n * n;
    
    double b0 = 1.0 / (1.0 + 1.0 / qFactor * n + nSquared);
    double b1 = b0 * 2.0;
    double b2 = b0;
    double a1 = b0 * 2.0 * (1.0 - nSquared);
    double a2 = b0 * (1.0 - 1.0 / qFactor * n + nSquared);
    
    coefs[0] = (float) b0;
    coefs[1] = (float) b1;
    coefs[2] = (float) b2;
    coefs[3] = (float) a1;
    coefs[4] = (float) a2;
}

//H(z) inversion for a funky high pass variation
//slope limited to 12db/oct
void SaulBWFilter::createHighTransformFilter(double sampleRate, double cutoffFreq, int qFactor)
{
    const double n = 1.0 / atan (2.0 * pi * cutoffFreq / sampleRate);
    const double nSquared = n * n;
    
    double b0 = 1.0 / (1.0 + 1.0 / qFactor * nSquared + n);
    double b1 = b0 * 4.0;
    double b2 = b0;
    double a1 = b0 * 4.0 * (1.0 + nSquared);
    double a2 = b0 * (1.0 - 1.0 / (qFactor * nSquared + n));
    
    coefs[0] = (float) b0;
    coefs[1] = (float) b1;
    coefs[2] = (float) b2;
    coefs[3] = (float) a1;
    coefs[4] = (float) a2;
}


void SaulBWFilter::processSamples(float *samplePtr, int numSamples)
{
    for (int i = 0; i < slope; i++)
    {
        processSamplesInternal(samplePtr, numSamples);
    }
}

void SaulBWFilter::processSingleSampleRaw(float sample)
{
    processSamplesInternal(&samplePtr, 1);
}

void SaulBWFilter::reset()
{
        v1 = v2 = 0;
}

void SaulBWFilter::processSamplesInternal(float *samplePtr, int numSamples)
{
    const float c0 = coefs[0];
    const float c1 = coefs[1];
    const float c2 = coefs[2];
    const float c3 = coefs[3];
    const float c4 = coefs[4];
    
    float lv1 = v1;
    float lv2 = v2;
    
    for (int i = 0; i < numSamples; ++i)
    {
        const float in = samplePtr[i];
        const float out = c0 * in + lv1;
        samplePtr[i] = out;
        
        lv1 = c1 * in - c3 * out + lv2;
        lv2 = c2 * in - c4 * out;
    }
    
    if (! (lv1 < -1.0e-8f || lv1 > 1.0e-8f))
    {
        v1 = lv1;
    }

    if (! (lv2 < -1.0e-8f || lv2 > 1.0e-8f))
    {
        v2 = lv2;
    }
}
