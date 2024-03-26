#ifndef __PINS_H
#define __PINS_H

// TFT Pins
#define TFT_MISO 12
#define TFT_MOSI 11  // A12
#define TFT_SCK 13  // A13
#define TFT_DC 9 
#define TFT_CS 10  
#define TFT_RST 8

// ADC Pins
#define AUDIO_IN_PIN 16 // A2

// Filter Pot Pins
#define FILT1_VR2_PIN 15 // A1
#define FILT1_VR1_PIN 14 // A0
#define FILT2_VR2_PIN 18 // A4
#define FILT2_VR1_PIN 17 // A3

// Filter Component Values
#define FILT_VR1_VAL 100e3
#define FILT_VR2_VAL 10e3
#define FILT_R7_VAL 1e3
#define FILT_R4_VAL 1e3
#define FILT_C2_VAL 10e-6

// TFT Dimensions
#define TFT_WIDTH 160
#define TFT_HEIGHT 128

#endif
