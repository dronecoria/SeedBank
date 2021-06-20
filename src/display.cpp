#include "display.h"
#include "sensor.h"

/*
void loop_display_task(void *p_display) {
    Display *display = (Display *) p_display;
    while(true) {
        display->loop();
        delay(1000);
    }
}
*/

Display::Display(Config *config, State *state) {
    Serial.println("Setup Display");
    this->m_config = config;
    this->m_state = state;

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

void Display::loop() {

    struct tm   time_info;
    char        time_str[20];  // 2021-05-21 12:32:45

    Serial.print("Display::loop");
    if (this->m_state->is_clock_set) {
        getLocalTime(&time_info);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &time_info);
        Serial.print(" - Time: ");
        Serial.print(time_str);
        Serial.println("");
    } else {
        Serial.println("Display::Clock is not set");
    }
}
