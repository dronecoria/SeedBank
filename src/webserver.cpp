#include "webserver.h"

void loop_webserver(WebServer *webserver) {
    Serial.println("WebServer::loop");
    // TODO
    delay(1000);
}

void loop_webserver_task(void *p_webserver) {
    WebServer *webserver = (WebServer *) p_webserver;
    while(true) {
        loop_webserver(webserver);
    }
}

WebServer::WebServer(Config *config, State *state) {
    Serial.println("Setup WebServer");
    this->m_config = config;
    this->m_state = state;

    xTaskCreatePinnedToCore(
        loop_webserver_task,  /* Function to implement the task */
        "WebServer",          /* Name of the task */
        10000,                /* Stack size in words */
        (void *) this,        /* Task input parameter */
        0,                    /* Priority of the task */
        &this->task,          /* Task handle. */
        0);                   /* Core where the task should run */

}


