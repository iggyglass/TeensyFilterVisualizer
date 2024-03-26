#include <Arduino.h>
#include <ST7735_t3.h>
#include <Audio.h>
#include <SPI.h>

#include "MovingAvg.h"
#include "utils.h"
#include "hardware.h"

#define FFT_SIZE 1024 // either 256 or 1024
#define FFT_TYPE CONCAT(AudioAnalyzeFFT, FFT_SIZE)

#define AVG_SIZE 10

// Audio stuff
FFT_TYPE fft;
AudioInputAnalog audioIn(AUDIO_IN_PIN);
AudioConnection patch(audioIn, fft);

// Display stuff
ST7735_t3 disp = ST7735_t3(TFT_CS, TFT_DC, TFT_RST);

tft::color_t filt1CenterColor = tft::color565(0, 0, 255);
tft::color_t filt1BwColor = tft::color565(0, 128, 250);

tft::color_t filt2CenterColor = tft::color565(255, 0, 0);
tft::color_t filt2BwColor = tft::color565(231, 85, 74);

constexpr int scaleX = (FFT_SIZE / 2) / TFT_WIDTH;

// Filter state estimation stuff
MovingAvg<AVG_SIZE> filt1Vr1Avg = {};
MovingAvg<AVG_SIZE> filt1Vr2Avg = {};
MovingAvg<AVG_SIZE> filt2Vr1Avg = {};
MovingAvg<AVG_SIZE> filt2Vr2Avg = {};

// Prototypes
int readFFTY(int bin);
int readFFTY(int binStart, int binEnd);

inline int16_t freqToPos(float hz);
inline float calcNaturalFreq(float vr2);
inline float calcBW(float vr1, float f0);

void renderFFT();
void renderFilter(int pinVr1, int pinVr2, MovingAvg<AVG_SIZE>& vr1Avg, MovingAvg<AVG_SIZE>& vr2Avg, tft::color_t centerColor, tft::color_t bwColor);

// Code
void setup()
{
    AudioMemory(12);

    disp.initR(INITR_BLACKTAB);
    disp.fillScreen(ST7735_BLACK);

    disp.useFrameBuffer(true);
}

void loop()
{
    if (!fft.available()) return;

    disp.fillScreen(ST7735_BLACK);

    renderFilter(FILT1_VR1_PIN, FILT1_VR2_PIN, filt1Vr1Avg, filt1Vr2Avg, filt1CenterColor, filt1BwColor);
    renderFilter(FILT2_VR1_PIN, FILT2_VR2_PIN, filt2Vr1Avg, filt2Vr2Avg, filt2CenterColor, filt2BwColor);
    renderFFT();

    disp.updateScreenAsync();
}

void renderFFT()
{
    int py = readFFTY(0);

    for (int i = 1; i < FFT_SIZE / 2 && i < TFT_WIDTH; i++)
    {
        int y = readFFTY(i * scaleX, i * scaleX + scaleX - 1);

        disp.drawLine(py, i - 1, y, i, ST7735_WHITE);
        py = y;
    }
}

void renderFilter(int pinVr1, int pinVr2, MovingAvg<AVG_SIZE>& vr1Avg, MovingAvg<AVG_SIZE>& vr2Avg, tft::color_t centerColor, tft::color_t bwColor)
{
    vr1Avg.Sample((analogRead(pinVr1) / 1024.0) * FILT_VR1_VAL);
    vr2Avg.Sample((analogRead(pinVr2) / 1024.0) * FILT_VR2_VAL);

    float vr1 = vr1Avg.Get();
    float vr2 = vr2Avg.Get();

    float f0 = calcNaturalFreq(vr2);
    float hbw = calcBW(vr1, f0) / 2;

    int16_t centerX = freqToPos(f0);
    int16_t bwPos = freqToPos(f0 + hbw);
    int16_t bwNeg = freqToPos(f0 - hbw);

    disp.drawFastHLine(0, centerX, TFT_HEIGHT, centerColor);
    disp.drawFastHLine(0, bwPos, TFT_HEIGHT, bwColor);
    disp.drawFastHLine(0, bwNeg, TFT_HEIGHT, bwColor);
}

int readFFTY(int bin)
{
    float n = fft.read(bin);
    return (int)(n * TFT_HEIGHT);
}

int readFFTY(int binStart, int binEnd)
{
    float n = fft.read(binStart, binEnd);
    return (int)(n * TFT_HEIGHT);
}

inline int16_t freqToPos(float hz)
{
    // f_n = n * (F_sample / N_fft)
    return (int16_t)((hz * FFT_SIZE) / 44100.0) / scaleX;
}

inline float calcNaturalFreq(float vr2)
{
    // The extra 1000 is to convert from kHz to Hz
    return 1000.0 / (2.0 * PI * (vr2 + FILT_R7_VAL) * FILT_C2_VAL);
}

inline float calcBW(float vr1, float f0)
{
    float q = (vr1 + FILT_R4_VAL) / 10000.0;
    return f0 / q;
}
