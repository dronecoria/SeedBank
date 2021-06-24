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
    float t_avg = this->m_state->get_avg_temperature();
    float t_reference = m_config->get_temperature_reference();

    float t_diff = 0;
    int num_sensors = 0;
    for (auto s : this->m_config->sensors) {
        float t = s->get_last_value();
        t_diff += abs(t - t_avg);
        num_sensors++;
    }
    //t_diff /= num_sensors;

    Serial.print("Decisor::loop");
    Serial.print(" - Avg Temp: ");
    Serial.print(t_avg);
    Serial.print(" - All: ");
    this->m_state->print_all_temperatures();

    Serial.print("   REF: ");
    Serial.print(t_reference);

    Serial.print("   DIFF: ");
    Serial.print(t_diff);

    Serial.println("");



    //basic decissor
    if(t_reference < t_avg){
        if (this->m_config->cold != nullptr) {
            this->m_config->cold->enable();
        }
    }
    if (t_reference > t_avg) {
        if (this->m_config->heat != nullptr) {
            this->m_config->heat->enable();
        }
    }
    //if(t_diff > 0.3f){
        if (this->m_config->fan != nullptr) {
            this->m_config->fan->set_value( t_diff );
        }
    //}
    // if (this->m_config->light != nullptr) {
    //     this->m_config->light->enable();
    // }
}
