#pragma once

#include <cmath>

#if defined(_WIN32) && !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

#ifndef pi
#define  pi 3.1415926535897932384626433832795
#endif

#ifndef sqrt2
#define sqrt2 (2.0 * 0.707106781186547524401)
#endif

#ifndef sqrt2over2
#define  sqrt2over2  0.707106781186547524401
#endif

struct Filter {
    enum EType {
        None = 0,
        FirstOrderHighPass,
        FirstOrderLowPass,
        SecondOrderButterworthHighPass,
        SecondOrderButterworthLowPass,
    };

    void init(EType type, float freqCutoff_Hz, float sampleRate) {
        m_type = type;
        switch (type) {
            case EType::None:
                {
                }
                break;
            case EType::FirstOrderHighPass:
                {
                    calculateCoefficientsFirstOrderHighPass(freqCutoff_Hz, sampleRate);
                }
                break;
            case EType::FirstOrderLowPass:
                {
                    calculateCoefficientsFirstOrderLowPass(freqCutoff_Hz, sampleRate);
                }
                break;
            case EType::SecondOrderButterworthHighPass:
                {
                    calculateCoefficientsSecondOrderButterworthHighPass(freqCutoff_Hz, sampleRate);
                }
                break;
            case EType::SecondOrderButterworthLowPass:
                {
                    calculateCoefficientsSecondOrderButterworthLowPass(freqCutoff_Hz, sampleRate);
                }
                break;
        };
    }

    void process(float * samples, int n) {
        switch (m_type) {
            case EType::None:
                {
                }
                break;
            case EType::FirstOrderHighPass:
            case EType::FirstOrderLowPass:
            case EType::SecondOrderButterworthHighPass:
            case EType::SecondOrderButterworthLowPass:
                {
                    for (int i = 0; i < n; ++i) {
                        samples[i] = filterBiquad(samples[i]);
                    }
                }
                break;
        }
    }

private:
    void calculateCoefficientsFirstOrderHighPass(int fc, int fs) {
        float th = 2.0 * pi * fc / fs;
        float g = cos(th) / (1.0 + sin(th));
        this->a0 = (1.0 + g) / 2.0;
        this->a1 = -((1.0 + g) / 2.0);
        this->a2 = 0.0;
        this->b1 = -g;
        this->b2 = 0.0;
    }

    void calculateCoefficientsFirstOrderLowPass(int fc, int fs) {
        float th = 2.0 * pi * fc / fs;
        float g = cos(th) / (1.0 + sin(th));
        this->a0 = (1.0 - g) / 2.0;
        this->a1 = (1.0 - g) / 2.0;
        this->a2 = 0.0;
        this->b1 = -g;
        this->b2 = 0.0;
    }

    void calculateCoefficientsSecondOrderButterworthHighPass(int fc, int fs) {
        float c = tan(pi*fc / fs);
        this->a0 = 1.0 / (1.0 + sqrt2*c + pow(c, 2.0));
        this->a1 = -2.0 * this->a0;
        this->a2 = this->a0;
        this->b1 = 2.0 * this->a0*(pow(c, 2.0) - 1.0);
        this->b2 = this->a0 * (1.0 - sqrt2*c + pow(c, 2.0));
    }

    void calculateCoefficientsSecondOrderButterworthLowPass(int fc, int fs) {
        float c = 1.0 / tan(pi*fc / fs);
        this->a0 = 1.0 / (1.0 + sqrt2*c + pow(c, 2.0));
        this->a1 = 2.0 * this->a0;
        this->a2 = this->a0;
        this->b1 = 2.0 * this->a0*(1.0 - pow(c, 2.0));
        this->b2 = this->a0 * (1.0 - sqrt2*c + pow(c, 2.0));
    }

    float filterBiquad(float sample) {
        float xn = sample;
        float yn =
            this->a0*xn + this->a1*this->xnz1 + this->a2*this->xnz2 -
            this->b1*this->ynz1 - this->b2*this->xnz2;

        this->xnz2 = this->xnz1;
        this->xnz1 = xn;
        this->xnz2 = this->ynz1;
        this->ynz1 = yn;

        return yn;
    }

    EType m_type;

    float a0;
    float a1;
    float a2;
    float b1;
    float b2;

    float xnz1;
    float xnz2;
    float ynz1;
};
