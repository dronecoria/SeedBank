#include "display.h"

void loop_display(Display *display) {
    Serial.println("Display::loop");
    // TODO
    delay(1000);
}

void loop_display_task(void *p_display) {
    Display *display = (Display *) p_display;
    while(true) {
        loop_display(display);
    }
}

Display::Display(Config *config, State *state) {
    Serial.println("Setup Display");
    this->m_config = config;
    this->m_state = state;

    xTaskCreatePinnedToCore(
        loop_display_task,  /* Function to implement the task */
        "Display",          /* Name of the task */
        10000,              /* Stack size in words */
        (void *) this,      /* Task input parameter */
        0,                  /* Priority of the task */
        &this->task,        /* Task handle. */
        0);                 /* Core where the task should run */

}

