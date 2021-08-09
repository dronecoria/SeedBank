#include "decisor.h"

void loop_decisor_task(void *p_decisor) {
    Decisor *decisor = (Decisor *) p_decisor;
    unsigned long currentTime = millis();
    unsigned long previousTime = 0;
    while(true) {
        currentTime = millis();
        decisor->loop(currentTime - previousTime);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        previousTime = currentTime;
    }
}

Decisor::Decisor(Config *config, State *state) {
    SERIAL_PRINTLN("Setup Decisor");
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

void Decisor::loop(float deltatime) {
    m_state->sensors_update();
    m_handler->run(deltatime);
}

void Decisor::select_handler(HANDLER_TYPE handler) {
    if(m_handler != nullptr){
        delete m_handler;
    }
    switch(handler){
        case HANDLER_TYPE::BASIC:
            m_handler = new Handler_basic(m_config, m_state);
            break;
        case HANDLER_TYPE::PID:
            m_handler = new Handler_pid(m_config, m_state);
            break;
        case HANDLER_TYPE::TEST:
        default:
            m_handler = new Handler_test(m_config, m_state);
            break;
    }
}
