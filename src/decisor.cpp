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
    float t = this->m_state->get_avg_temperature();
    float t_reference = m_config->get_temperature_reference();


    Serial.print("Decisor::loop");
    Serial.print(" - Avg Temp: ");
    Serial.print(t);
    Serial.print(" - All: ");
    this->m_state->print_all_temperatures();

    Serial.print("   REF: ");
    Serial.print(t_reference);

    Serial.println("");
}
