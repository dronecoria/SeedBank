#include "decisor.h"

void loop_decisor_task(void *p_decisor) {
    Decisor *decisor = (Decisor *) p_decisor;
    while(true) {
        decisor->loop();
        delay(1000);
    }
}

Decisor::Decisor(Config *config, State *state) {
    Serial.println("Setup Decisor");
    this->m_config = config;
    this->m_state = state;

    xTaskCreatePinnedToCore(
        loop_decisor_task,    // Function to implement the task
        "Decisor",            // Name of the task
        10000,                // Stack size in words
        (void *) this,        // Task input parameter
        0,                    // Priority of the task
        &this->task,          // Task handle.
        1);                   // Core where the task should run

}

void Decisor::loop() {
    Serial.println("Decisor::loop");
    // TODO
}
