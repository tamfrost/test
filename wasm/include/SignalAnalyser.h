#define _USE_MATH_DEFINES
#include <fftw3.h>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <complex>
#include <vector>
#include <deque>

using namespace std::complex_literals;

#ifndef NSG_SIGNALANALYSER_H
#define NSG_SIGNALANALYSER_H

#define Nfft 1024
#define POWER_REF 0

enum ModulationType{AM, LSB, USB, FM, CW, NONE};

class SignalAnalyser {

public:
    SignalAnalyser(int sampleRate, double output_gain, double gs_zero, float threshold, float knee, float ratio, float attack, float release);
    ~SignalAnalyser();
    // static std::string getCompilationTime();
    // static std::string getCommitHash();
    float getScaleFactor();
    void initWindow();
    void initBlackmanHarris();
    void fft(int nSamples, uint32_t sampleInPtr, uint32_t sampleOutPtr);
    void ifft(int nSamples, uint32_t sampleInPtr, uint32_t sampleOutPtr);
    void shiftFrequency(int nSamples, double fqShift, uint32_t sampleInPtr, uint32_t sampleOutPtr);
    double demodulate(int chunkNo, int modulation, int nSamples, bool useAGC, uint32_t sampleInPtr, uint32_t audioSampleOutPtr, uint32_t audioSampleOutAgcPtr, uint32_t iqSampleOutPtr);
    void AGC(int N, float* samples, float fs, float T, float W, float R, float Ta, float Tr, float* retSamples);

    static std::string getCompilationTime() {
        return std::string(__DATE__) + " " + std::string(__TIME__);
    }

    static std::string getCommitHash() {
        return std::string(COMMIT_HASH);
    }


private:
    int bufferDepth;
    int sampleCount = 0;
    int sampleRate;
    float threshold;
    float knee;
    float ratio;
    float attack;
    float release;
    std::vector<std::complex<double>>* dataBuffer;
    float window[Nfft];
    float scaleFactor = 0;
    double dcOffset = 0;
    double maxAverage = 1;
    double alphaOffset = 0.5;
    double gain = 0;
    double gainSmooth = 0;
    double* gaincomputer(int N, double* xdb, double T, double W, double R);
    double gs_zero = -30.0;
    double OUTPUT_GAIN = 0.25;
    fftw_plan fftwPlanForward, fftwPlanBackward;
    double fftSamplesIn[Nfft*2];
    double (*fftIqSamplesIn)[2];
    fftw_complex fftIqSamplesOut[Nfft];

};


#endif //NSG_SIGNALANALYSER_H
