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

    tm time_info;
    char time_str[20];  // 2021-05-21 12:32:45

    SERIAL_PRINT("Display::loop");

    if (this->m_state->is_clock_set) {
        getLocalTime(&time_info);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &time_info);
        SERIAL_PRINT(" - Time: ");
        SERIAL_PRINT(time_str);
        SERIAL_PRINTLN("");
    } else {
        SERIAL_PRINTLN("Display::Clock is not set");
    }
}
