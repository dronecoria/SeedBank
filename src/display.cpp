#include "display.h"
#include "sensor.h"

/*
void loop_display_task(void *p_display) {
    Display *display = (Display *) p_display;
    while(true) {
        display->loop();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
*/

Display::Display(Config *config, State *state) {
    SERIAL_PRINTLN("Setup Display");
    this->m_config = config;
    this->m_state = state;

    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI);

    tft.init(240, 240, SPI_MODE2);

    tft.setRotation(0);
    tft.fillScreen(ST77XX_MAGENTA);
    delay(1500);
    tft.fillScreen(ST77XX_BLACK);

    /*
    xTaskCreatePinnedToCore(
        loop_display_task,  // Function to implement the task
        "Display",          // Name of the task
        10000,              // Stack size in words
        (void *) this,      // Task input parameter
        0,                  // Priority of the task
        &this->task,        // Task handle.
        0);                 // Core where the task should run
    */
}

void Display::screen_ip()
{
    tft.setCursor(0, 0);
    tft.setRotation(1);

    tft.setTextWrap(true);
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
    tft.print("\n  ");
    tft.print("\n  ");
    tft.print(WiFi.SSID());
    tft.print("\n  ");
    //tft.print(WiFi.getHostname());
    //tft.print("\n");
    //tft.print(WiFi.networkID().toString());
    tft.print("\n  ");

    tft.print(WiFi.macAddress());

    tft.print("\n\n");
    tft.setTextSize(3);
    tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
    tft.print(WiFi.localIP().toString());
    tft.print("\n");


    tft.setTextSize(4);
    if (m_state->get_avg_temperature() == TEMP_ERROR_READING){
        tft.print("\n  --");
    }else{
        tft.print("\n  ");
        tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    }

    tft.print(String(m_state->get_avg_temperature(), 2));
}

void Display::loop()
{
    if (m_config->get_mode() == MODE::SETUP) {
        screen_ip();
    }else{
        screen_ip();
    }
}
