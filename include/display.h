#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "state.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_MOSI 13  // Data out     labeled sda
#define TFT_MISO 12  // Data in      not connected
#define TFT_SCLK 14  // Clock out    labeled sck
#define TFT_RST 26 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 27
#define TFT_CS -1

class Display {
public:
    TaskHandle_t task;
    Display(Config *config, State *state);
    void loop();

private:
    Config *m_config;
    State *m_state;

    void screen_ip();

    Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
};

#endif
