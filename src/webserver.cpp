#include <Arduino.h>
#include <sys/time.h>
#include <WiFi.h>
#include <SPIFFS.h>
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
    Serial.print("WebServer::loop - SSID: ");
    Serial.print(WiFi.SSID());
    Serial.print(" - IP: ");
    Serial.println(WiFi.localIP());
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
    WiFi.begin(this->m_config->get_wifi_ssid().c_str(), this->m_config->get_wifi_password().c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print('.');
        if(tries >= 10) break;
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
        request->send(SPIFFS, home_page);
        });

    this->m_webserver->on("/save_setup", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, save_setup);

    this->m_webserver->on("/get_time", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200,"text/plain",get_time());
    });

    this->m_webserver->onNotFound([](AsyncWebServerRequest *request) {
        if (SPIFFS.exists(request->url())) {
            request->send(SPIFFS, request->url());
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

String get_time()
{
    struct tm   time_info;
    char        time_str[20];  // 2021-05-21 12:32:45

    getLocalTime(&time_info);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &time_info);
    return time_str;
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

    File file = SPIFFS.open(CONFIG_FILENAME, FILE_WRITE);
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
