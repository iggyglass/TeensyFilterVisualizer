#include <Arduino.h>
#include <ST7735_t3.h>
#include <Audio.h>
#include <SPI.h>

#include <MovingAvg.hpp>
#include "utils.h"
#include "hardware.h"
#include "FilterVis.h"

#define FFT_SIZE 1024 // either 256 or 1024
#define FFT_TYPE CONCAT(AudioAnalyzeFFT, FFT_SIZE)

#define AVG_SIZE 10

#define SCALE_Y 1.2

// Audio stuff
FFT_TYPE fft;
AudioInputAnalog audioIn(AUDIO_IN_PIN);
AudioConnection patch(audioIn, fft);

// Display stuff
ST7735_t3 disp = ST7735_t3(TFT_CS, TFT_DC, TFT_RST);

FilterVis<AVG_SIZE> filt1 = FilterVis<AVG_SIZE>(FILT1_VR1_PIN, FILT1_VR2_PIN, tft::color565(0, 0, 255), tft::color565(0, 128, 250));
FilterVis<AVG_SIZE> filt2 = FilterVis<AVG_SIZE>(FILT2_VR1_PIN, FILT2_VR2_PIN, tft::color565(255, 0, 0), tft::color565(231, 85, 74));

constexpr int scaleX = (FFT_SIZE / 2) / TFT_WIDTH;

// Prototypes
int readFFTY(int bin);
int readFFTY(int binStart, int binEnd);

inline int16_t freqToPos(float hz);

void renderFFT();
void renderFreq(float f, tft::color_t color);

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

    filt1.RenderFilter(renderFreq);
    filt2.RenderFilter(renderFreq);
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

void renderFreq(float f, tft::color_t color)
{
    int16_t pos = freqToPos(f);

    disp.drawFastHLine(0, pos, TFT_HEIGHT, color);
}

int readFFTY(int bin)
{
    float n = fft.read(bin) * SCALE_Y;
    return (int)(n * TFT_HEIGHT);
}

int readFFTY(int binStart, int binEnd)
{
    float n = fft.read(binStart, binEnd) * SCALE_Y;
    return (int)(n * TFT_HEIGHT);
}

inline int16_t freqToPos(float hz)
{
    // f_n = n * (F_sample / N_fft)
    return (int16_t)((hz * FFT_SIZE) / 44100.0) / scaleX;
}
