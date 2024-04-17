#ifndef __FILTER_VIS_H
#define __FILTER_VIS_H

#include <Arduino.h>
#include <MovingAvg.hpp>

#include "hardware.h"
#include "utils.h"

template<uint8_t size>
class FilterVis
{
private:
    MovingAvg<size> vr1Avg = {};
    MovingAvg<size> vr2Avg = {};

    tft::color_t centerColor;
    tft::color_t bwColor;

    uint8_t vr1Pin;
    uint8_t vr2Pin;

private:
    static inline float calcNaturalFreq(float vr2)
    {
        return 1.0 / (2.0 * PI * (vr2 + FILT_R7_VAL) * FILT_C2_VAL);
    }

    static inline float calcBW(float vr1, float f0)
    {
        float q = (vr1 + FILT_R4_VAL) / FILT_R6_VAL;
        return f0 / q;
    }

public:
    FilterVis(uint8_t vr1Pin, uint8_t vr2Pin, tft::color_t centerColor, tft::color_t bwColor)
    {
        this->vr1Pin = vr1Pin;
        this->vr2Pin = vr2Pin;
        this->centerColor = centerColor;
        this->bwColor = bwColor;
    }

    void RenderFilter(void (*renderFreq)(float, tft::color_t))
    {
        vr1Avg.Sample((analogRead(vr1Pin) / 1024.0) * FILT_VR1_VAL);
        vr2Avg.Sample((analogRead(vr2Pin) / 1024.0) * FILT_VR2_VAL);

        float vr1 = vr1Avg.Get();
        float vr2 = vr2Avg.Get();

        float f0 = calcNaturalFreq(vr2);
        float hbw = calcBW(vr1, f0) / 2;

        renderFreq(f0, centerColor);
        renderFreq(f0 + hbw, bwColor);
        renderFreq(f0 - hbw, bwColor);
    }
};

#endif
