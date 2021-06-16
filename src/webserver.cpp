#include <Arduino.h>
#include <sys/time.h>
#include "webserver.h"

void loop_webserver_task(void *p_webserver) {
    WebServer *webserver = (WebServer *) p_webserver;
    while(true) {
        webserver->loop();
        delay(1000);
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

void WebServer::loop() {
    Serial.println("WebServer::loop");
    // TODO
    if (!this->m_initialized) {
        this->init();
    } else {
        this->check_ntp();
        this->send_data();
    }
}

void WebServer::init() {
    if (!this->m_state->is_clock_set) {
        this->set_default_time();
    }
    if (this->m_config->get_wifi_ssid() == nullptr) {
        this->init_access_point();
    } else {
        this->init_wifi_client();
    }
    if (this->m_initialized) {
        this->init_server();
    }
}

void WebServer::init_access_point() {
}

void WebServer::init_wifi_client() {
}

void WebServer::init_server() {
}

void WebServer::check_ntp() {
}

void WebServer::send_data() {
}

void WebServer::set_default_time() {
    struct tm tm;
    tm.tm_year = 2021 - 1900;
    tm.tm_mon = 06;
    tm.tm_mday = 16;
    tm.tm_hour = 14;
    tm.tm_min = 10;
    tm.tm_sec = 10;
    time_t t = mktime(&tm);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);
    // TODO: Set this variable on check_ntp
    this->m_state->is_clock_set = true;

}
