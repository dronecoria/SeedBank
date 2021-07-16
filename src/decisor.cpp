#include "decisor.h"

void loop_decisor_task(void *p_decisor) {
    Decisor *decisor = (Decisor *) p_decisor;
    while(true) {
        decisor->loop();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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

    select_handler(m_config->get_handler());
}

void Decisor::loop() {
    m_state->sensors_update();

    m_handler();



    // info
    float t_avg = m_state->get_avg_temperature();
    float t_reference = m_config->get_temperature_reference();

    // float t_diff = 0;
    // int num_sensors = 0;
    // for (auto s : this->m_config->sensors) {
    //     float t = s->get_value();
    //     if (t != TEMP_ERROR_READING){
    //         t_diff += abs(t - t_avg);
    //         num_sensors++;
    //     }
    // }
    float t_max_diff = 0;
    for (auto s : m_config->sensors) {
        if (s->is_temperature()) {
            float t = s->get_value();
            t_max_diff = max(t_max_diff, abs(t - t_avg));
        }
    }
    //t_diff /= num_sensors;




    Serial.print("Decisor::loop");
    Serial.print(" - Avg Temp: ");
    Serial.print(t_avg);
    Serial.print(" - All: ");
    this->m_state->print_all_temperatures();

    Serial.print("   REF: ");
    Serial.print(t_reference);

    Serial.print("   MAX DIFF: ");
    Serial.print(t_max_diff);

    Serial.println("");

}

void Decisor::set_handler(std::function<void()> handler) {
    m_handler = handler;
}

void Decisor::select_handler(HANDLER handler) {
    switch(handler){
        case HANDLER::BASIC:
            set_handler([&]() {
                float t_avg = m_state->get_avg_temperature();
                float t_reference = m_config->get_temperature_reference();

                float t_max_diff = 0;
                for (auto s : m_config->sensors) {
                    float t = s->get_value();
                    t_max_diff = max(t_max_diff, abs(t - t_avg));
                }

                if (t_reference < t_avg) {
                    if (m_config->cold != nullptr) {
                        m_config->cold->enable();
                    }
                }
                if (t_reference > t_avg) {
                    if (m_config->heat != nullptr) {
                        m_config->heat->enable();
                    }
                }
                if (t_max_diff > 0.25f) {
                    if (m_config->fan != nullptr) {
                        //this->m_config->fan->set_value(t_diff);
                        m_config->fan->enable();
                    }
                }
                if (m_config->light != nullptr) {
                    m_config->light->set_value(t_max_diff);
                }
                });
            break;
        case HANDLER::PID:
            set_handler([](){});
            break;
        case HANDLER::TEST:
        default:
            set_handler([&](){});
            break;
    }
}
