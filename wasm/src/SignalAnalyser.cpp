#include <cstring>
#include <math.h>
#include "SignalAnalyser.h"

using namespace std::complex_literals;

SignalAnalyser::SignalAnalyser(int sampleRate, double output_gain, double gs_zero, float threshold, float knee, float ratio, float attack, float release) {

    fftIqSamplesIn = reinterpret_cast<double (&)[Nfft][2]>(fftSamplesIn);
    fftwPlanForward = fftw_plan_dft_1d(Nfft,fftIqSamplesIn,fftIqSamplesOut,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwPlanBackward = fftw_plan_dft_1d(Nfft,fftIqSamplesIn,fftIqSamplesOut,FFTW_BACKWARD,FFTW_ESTIMATE);
    initWindow();
    this->sampleRate = sampleRate;
    this->OUTPUT_GAIN = output_gain;
    this->gs_zero = gs_zero;
    this->threshold = threshold;
    this->knee = knee;
    this->ratio = ratio;
    this->attack = attack;
    this->release = release;


}

void SignalAnalyser::initWindow() {
    initBlackmanHarris();
}

void SignalAnalyser::initBlackmanHarris() {
    const float a0 = 0.35875f;
    const float a1 = 0.48829f;
    const float a2 = 0.14128f;
    const float a3 = 0.01168f;
    for (int i = 0; i < Nfft; i++) {
        window[i] = a0 - (a1 *cosf((2.0f*M_PI*i)/(Nfft-1))) + (a2 *cosf((4.0f*M_PI*i)/(Nfft-1))) - (a3 *cosf((6.0f*M_PI*i)/(Nfft-1)));
        this->scaleFactor+=window[i]*window[i];
    }
}

SignalAnalyser::~SignalAnalyser() {

    fftw_destroy_plan(fftwPlanForward);
    fftw_destroy_plan(fftwPlanBackward);
    fftw_cleanup();
}

// static std::string SignalAnalyser::getCompilationTime() {
//     return std::string(__DATE__) + " " + std::string(__TIME__);
// }

// static std::string SignalAnalyser::getCommitHash() {
//     return std::string(COMMIT_HASH);
// }

float SignalAnalyser::getScaleFactor() {
    return this->scaleFactor;
}

void SignalAnalyser::ifft(int nSamples, uint32_t sampleInPtr, uint32_t sampleOutPtr) {
    float* sampleIn = reinterpret_cast<float*>(sampleInPtr);
    std::copy(sampleIn, sampleIn+(nSamples * 2), fftSamplesIn);
    float* sampleOut = reinterpret_cast<float*>(sampleOutPtr);
    fftw_execute(fftwPlanBackward);
    for (int i=0; i<nSamples; i++) {
        sampleOut[i] = sqrt(fftIqSamplesOut[i][0]*fftIqSamplesOut[i][0] + fftIqSamplesOut[i][1]*fftIqSamplesOut[i][1]);
    }
//    fft(nSamples, sampleInPtr, sampleOutPtr);
}


void SignalAnalyser::shiftFrequency(int nSamples, double fqShift, uint32_t sampleInPtr, uint32_t sampleOutPtr) {
    auto* sampleIn = reinterpret_cast<float*>(sampleInPtr);
    auto* sampleOut = reinterpret_cast<float*>(sampleOutPtr);
    //for (int i=0; i<=nSamples-2; i+=2) {
    for (int i=0; i<=nSamples; i+=2) {
        double tshift = (nSamples/2)/fqShift;
        double phase = (i/2)*2*M_PI/tshift;
        double shift_sin = sin(phase);
        double shift_cos = cos(phase);
        float iSample = sampleIn[i];
        float qSample = sampleIn[i+1];
        sampleOut[i] = iSample*shift_cos - qSample*shift_sin;
        sampleOut[i+1] = iSample*shift_sin + qSample*shift_cos;
    }
}


void SignalAnalyser::fft(int nSamples, uint32_t sampleInPtr, uint32_t sampleOutPtr) {
    //float* sampleIn = reinterpret_cast<float*>(sampleInPtr);

    float (&sampleIn)[Nfft][2] = *reinterpret_cast<float (*)[Nfft][2]>(sampleInPtr);

    //std::copy(sampleIn, sampleIn+(nSamples * 2), fftSamplesIn);
    for(int i=0; i <Nfft; i++) {
        fftIqSamplesIn[i][0] = sampleIn[i][0] * window[i];
        fftIqSamplesIn[i][1] = sampleIn[i][1] * window[i];
    }
    float* sampleOut = reinterpret_cast<float*>(sampleOutPtr);
    fftw_execute(fftwPlanForward);
    for (int i=0; i<Nfft; i++) {
        //sampleOut[i] = sqrt(fftIqSamplesOut[i][0]*fftIqSamplesOut[i][0] + fftIqSamplesOut[i][1]*fftIqSamplesOut[i][1]);
        sampleOut[i] = 10*log10(((fftIqSamplesOut[i][0]*fftIqSamplesOut[i][0] + fftIqSamplesOut[i][1]*fftIqSamplesOut[i][1])+1)/(this->scaleFactor))-165;
    }
}



/* Reference: "Digital Dynamic Range Compressor Design - A Tutorial and Analysis"
*            Dmitrios Giannoulis, Michael Massber, and Joshua D. Reiss,
*            Journal of Audio Engineering Soc. Vol 60. N0. 6, 2012 June.
*/
void SignalAnalyser::AGC(int N, float* samples, float fs, float T, float W, float R, float Ta, float Tr, float* retSamples) {

    // Convert to dB
    double xdb[N];
    double EPSDB = 1e-12; // used to avoid zeros in dB calculations
    for (int i=0;  i < N; i++ ) {
        xdb[i] = 20.0*log10(abs((double)samples[i])+EPSDB);
    }

    double* xsc = gaincomputer(N, xdb, T, W, R);

    // Smooth gain
    double Ka = exp(-log(9.0) / (fs * Ta));
    double Kr = exp(-log(9.0) / (fs * Tr));
    double gc[N];
    double gs[N];

    for (int k=0; k< N; k++) {
        gc[k] = xsc[k] - xdb[k];
    }

    for (int k=0; k<N; k++) {
        if (gc[k] > this->gs_zero)
            gs[k] = Ka*this->gs_zero + (1.0 - Ka)*gc[k];
        else
            gs[k] = Kr*this->gs_zero + (1.0 - Kr)*gc[k];
        this->gs_zero = gs[k];
    }

    // Calculate output samples
    double normgain[1];
    normgain[0] = 0.0;
    double* M = gaincomputer(1, normgain, T, W, R);
//    double retSamples[N];
    for (int k=0; k<N; k++) {
        retSamples[k] = (float)(OUTPUT_GAIN*samples[k]*pow(10.0, ((gs[k] - M[0])/20.0)));
    }
//    return retSamples;
}

double* SignalAnalyser::gaincomputer(int N, double* xdb, double T, double W, double R)
{
    // Gain computer
    double xsc[N];
    if (W > 0.0) {
        for (int k=0; k<N; k++ ) {
            if (xdb[k] < (T-W/2.0)) {
                xsc[k] = xdb[k];
            }
            else if ((xdb[k] < (T-W/2.0)) && (xdb[k] <= (T+W/2.0))) {
                xsc[k] = xdb[k] + ((1/R-1)*pow((xdb[k] - T + W/2.0), 2.0))/(2.0*W);
            }
            else {
                xsc[k] = T + (xdb[k]-T)/R;
            }
        }
    }
    else {
        for (int k=0; k<N; k++ ) {
            if (xdb[k] < T)
                xsc[k] =  xdb[k];
            else
                xsc[k] = T + (xdb[k]-T)/R;
        }
    }
    return xsc;
}

double SignalAnalyser::demodulate(int chunkNo, int modulation, int nSamples, bool useAGC, uint32_t sampleInPtr, uint32_t audioSampleOutPtr, uint32_t audioSampleOutAgcPtr, uint32_t iqSampleOutPtr) {

    auto* sampleIn = reinterpret_cast<float*>(sampleInPtr);
    auto* audioSampleOut = reinterpret_cast<float*>(audioSampleOutPtr);
    auto* audioSampleOutAgc = reinterpret_cast<float*>(audioSampleOutAgcPtr);
    auto* iqSampleOut = reinterpret_cast<float*>(iqSampleOutPtr);

    //double chunkMax = 0;
    //double chunkMaxRaw = 0;
    double sampleSum = 0;
    double sampleSumRaw = 0;
    //for (int i=0; i<=nSamples*2 - 2; i+=2) {

    float audioSampleOutTmp[nSamples];

    for (int i=0; i<=nSamples*2; i+=2) {

        float iSample = sampleIn[i];
        float qSample = sampleIn[i+1];

        iqSampleOut[i] = iSample;
        iqSampleOut[i+1] = qSample;

        float rawSampleValue = modulation == ModulationType::AM ? sqrt(iSample*iSample + qSample*qSample) : iSample;
        //audioSampleOutTmp[i/2] = rawSampleValue - dcOffset;
        audioSampleOutTmp[i/2] = rawSampleValue;
        //sampleSumRaw += rawSampleValue;
        //sampleSum += audioSampleOutTmp[i/2];

//        if (audioSampleOut[i/2] > chunkMax) chunkMax = audioSampleOut[i/2];
//        if ((rawSampleValue - dcOffset) > chunkMaxRaw) chunkMaxRaw = rawSampleValue - dcOffset;

        //normalize level (AGC)
//        gainSmooth = 0.001*gain + 0.999*gainSmooth; //exponential smoothing of the gain since it updates only every chunk
        //audioSampleOut[i/2] *= gain;

        //silence the first chunk to detect initial levels
        //if (chunkNo == 0) {
        //    audioSampleOutTmp[i/2] = 0;
        //}

        //softer start
        if (chunkNo == 0 && i < 100) {
            audioSampleOutTmp[i/2] = audioSampleOutTmp[i/2] * i/100;
        }

        audioSampleOut[i/2] = 0.0001 * audioSampleOutTmp[i/2];

        sampleCount++;
    }

    //estimate levels during the first chunk
    //if (chunkNo == 0) {
    //    dcOffset = sampleSumRaw/nSamples;
    //}

    // Update levels (exponential smoothing, sort of a moving average, https://en.wikipedia.org/wiki/Exponential_smoothing)
    // It is simply a weighted average between the old value and the new. The more weight to the old value the slower change.
    // tau = -T/(ln(1-alpha) where T is the time between chunks and tau is the time constant
    //dcOffset = (alphaOffset * sampleSumRaw/nSamples) + (1.0 - alphaOffset) * dcOffset;
    //maxAverage = (alphaAGC * chunkMaxRaw) + (1.0 - alphaAGC) * maxAverage;

    //gain = 1/maxAverage;

    // printf("%10f %10f %10f %10f\n", this->knee, this->ratio, this->attack, this->release);

//     //TODO a more advanced AGC that did not work well when first tried but should be looked at further
     //AGC(nSamples, audioSampleOutTmp, this->sampleRate, -20.0, 0.0, 10.0, 6.0, 0.0005, audioSampleOut);

    AGC(nSamples, audioSampleOutTmp, this->sampleRate, this->threshold, this->knee, this->ratio, this->attack, this->release, audioSampleOutAgc);

    return gainSmooth;

}
