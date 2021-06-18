#include <sys/time.h>
#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "webserver.h"

//TODO eliminar html_preprocessor y pasar los datos por js
extern Config *config;

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
    if (this->m_config->get_mode() == MODE::SETUP || this->m_config->get_wifi_ssid() == nullptr || this->m_config->get_wifi_password() == nullptr) {
        this->init_access_point();
    } else {
        this->init_wifi_client();
    }
    if (this->m_initialized) {
        this->init_server();
    }
}

void WebServer::init_access_point() {
    IPAddress ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);

    const char* ssid = "SeedBank Setup";

    WiFi.mode(WIFI_AP);
    while (!WiFi.softAP(ssid))
    {
        Serial.println(".");
        delay(1000);
    }
    WiFi.softAPConfig(ip, gateway, subnet);
    this->m_initialized = true;
    m_wifi_mode = WIFI_MODE::ACCESS_POINT;
}

void WebServer::init_wifi_client() {
    int tries = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->m_config->get_wifi_ssid(), this->m_config->get_wifi_password());
    while (WiFi.status() != WL_CONNECTED || tries < 10)
    {
        delay(1000);
        Serial.print('.');
        tries++;
    }
    if (tries >= 10) {
        this->init_access_point();
    }
    else {
        this->m_initialized = true;
        m_wifi_mode = WIFI_MODE::CLIENT;
    }
}

String html_preprocessor(const String& var) {
    if (var == "MODE")           return String((int)config->get_mode());
    if (var == "WIFI_SSID")      return config->get_wifi_ssid();
    if (var == "WIFI_PASSWORD")  return config->get_wifi_password();
    if (var == "NTP_SERVER")     return config->get_ntp_server();
    if (var == "NTP_GMT_OFFSET")     return String((long int)config->get_ntp_gmt_offset());
    if (var == "MODE_SELECT_LIST") {
        String select = "<select>";
        select += "</select>";
        return select;
    }

    return String();
}


void WebServer::init_server() {

    if (this->m_webserver != nullptr) {
        // destroy previous if exist
        this->m_webserver->end();
        delete this->m_webserver;
    }
    this->m_webserver = new AsyncWebServer(80);


    // routes

    String home_page = (this->m_wifi_mode == WIFI_MODE::ACCESS_POINT) ? "/setup.html" : "/index.html";
    this->m_webserver->on("/", HTTP_GET, [home_page](AsyncWebServerRequest *request) {
        request->send(SPIFFS, home_page, String(), false, html_preprocessor);
        });

    this->m_webserver->on("/save_setup", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, save_setup);

    this->m_webserver->onNotFound([](AsyncWebServerRequest *request) {
        if (SPIFFS.exists(request->url())) {
            request->send(SPIFFS, request->url(), String(), false, html_preprocessor);
        }
        else {
            request->send(400, "text/plain", "404 Not found");
        }
        });

    this->m_webserver->begin();

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


String GetBodyContent(uint8_t *data, size_t len)
{
    String content = "";
    for (size_t i = 0; i < len; i++) {
        content.concat((char)data[i]);
    }
    return content;
}

void save_setup(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    String bodyContent = GetBodyContent(data, len);

    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(bodyContent)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }

    file.close();

    request->send(200, "text/plain", "OK");

    delay(1000);
    ESP.restart();
}
