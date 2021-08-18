#include <Arduino.h>
#include <sys/time.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncElegantOTA.h>

#include "webserver.h"

void loop_webserver_task(void *p_webserver) {
    WebServer *webserver = (WebServer *) p_webserver;
    while(true) {
        webserver->loop();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

WebServer::WebServer(Config *config, State *state) {
    SERIAL_PRINTLN("Setup WebServer");
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
    SERIAL_PRINT("WebServer::loop - SSID: ");
    SERIAL_PRINT(WiFi.SSID());
    SERIAL_PRINT(" - IP: ");
    SERIAL_PRINTLN(WiFi.localIP());
    // TODO
    if (!this->m_initialized) {
        this->init();
        return;
    }

    this->check_ntp();
    if (!m_mqttClient.connected()){
        this->init_mqtt();
        return;
    }

    this->send_data();
    m_mqttClient.loop();
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
        this->init_web_server();
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
        SERIAL_PRINTLN(".");
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
        SERIAL_PRINT('.');
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

void WebServer::init_web_server() {

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

    this->m_webserver->on("/save_setup", HTTP_POST, [](AsyncWebServerRequest* request) {}, nullptr, save_setup);
    this->m_webserver->on("/set_value", HTTP_POST, [](AsyncWebServerRequest* request) {}, nullptr,
        [&](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
            String bodyContent = GetBodyContent(data, len);
            set_value(bodyContent);
            request->send(200, "text/plain", bodyContent);
        });

    this->m_webserver->on("/get_time", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200,"text/plain",get_time());
    });

    this->m_webserver->on("/status.json", HTTP_GET, [&](AsyncWebServerRequest* request) {
        request->send(200, "application/json", get_status());
        });

    this->m_webserver->onNotFound([](AsyncWebServerRequest *request) {
        if (SPIFFS.exists(request->url())) {
            request->send(SPIFFS, request->url());
        }
        else {
            request->send(400, "text/plain", "404 Not found");
        }
        });

    AsyncElegantOTA.begin(this->m_webserver);    // Start ElegantOTA
    this->m_webserver->begin();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("");
    Serial.print("Callback - ");
    Serial.print(topic);
    Serial.println("");
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println("");
}

void WebServer::init_mqtt() {
    if(!m_state->is_mqtt_set){
        m_mqttClient.setClient(m_wifiClient);
        //TODO create a configurable variable in config for server & port
        m_mqttClient.setServer("192.168.1.130", 1883);
        m_mqttClient.setCallback(mqtt_callback);
        m_state->is_mqtt_set = true;
    }
    reconnect_mqtt();
}
void WebServer::reconnect_mqtt() {
    //while (!m_mqttClient.connected()) {
    if(!m_mqttClient.connected()) {
        Serial.println("Reconnecting to MQTT Broker..");
        if (m_mqttClient.connect(m_config->get_id().c_str())) {
            Serial.println("Connected.");
            // subscribe to topics
            m_mqttClient.subscribe( String(m_config->get_id()+"/commands/#").c_str() );
        }

    }
}

void WebServer::check_ntp() {
    if(this->m_state->is_clock_set) return;

    configTime( m_config->get_ntp_gmt_offset()      * 60 * 60,
                m_config->get_ntp_daylight_offset() * 60 * 60,
                m_config->get_ntp_server().c_str());
    this->m_state->is_clock_set = true;
}

void WebServer::send_data() {
    String topic = m_config->get_id();
    bool all_temps_valids = true;
    int i = 0;
    for (Sensor* s : this->m_config->sensors) {
        if (s->is_temperature()) {
            if (s->is_valid()) {
                m_mqttClient.publish(String(topic + "/sensors/temp/" + String(i)).c_str(), String(s->get_value(),5).c_str() );
            }else{
                all_temps_valids = false;
            }
        }else{
            m_mqttClient.publish(String(topic + "/sensors/" + s->get_type_string() + "/" + String(i)).c_str(), String(s->get_value()).c_str());
        }
        i++;
    }
    if(all_temps_valids){
        m_mqttClient.publish(String(topic + "/sensors/avgTemp").c_str(), String(m_state->get_avg_temperature(),5).c_str());
    }

}

void WebServer::set_default_time() {
    tm time_info;
    time_info.tm_year = 2021 - 1900;
    time_info.tm_mon = 06;
    time_info.tm_mday = 16;
    time_info.tm_hour = 14;
    time_info.tm_min = 10;
    time_info.tm_sec = 10;
    time_t t = mktime(&time_info);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, nullptr);
    // TODO: Set this variable on check_ntp
    //this->m_state->is_clock_set = true;

}

String get_time()
{
    tm time_info;
    char time_str[20];  // 2021-05-21 12:32:45

    getLocalTime(&time_info);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &time_info);
    return time_str;
}

String WebServer::get_status()
{
    StaticJsonDocument<2048> doc;

    JsonObject root = doc.to<JsonObject>();

    root["mode"] = m_config->get_mode_string();
    root["handler"] = m_config->get_handler_string();

    root["reference_temp"] = m_config->get_temperature_reference();
    root["avg_temp"] = m_state->get_avg_temperature();

    JsonArray sensors = root.createNestedArray("sensors");
    for (auto sensor : m_config->sensors) {
        JsonObject obj = sensors.createNestedObject();
        obj["type"] = sensor->get_type_string();
        if(sensor->get_type() == SENSOR_TYPE::DOOR){
            obj["value"] = (sensor->get_value() >0 ) ? "Opened" : "Closed";
        }else{
            obj["value"] = sensor->get_value();
        }

    }

    JsonArray actuators = root.createNestedArray("actuators");
    {
        auto act = m_config->cold;
        if (act != nullptr) {
            JsonObject obj = actuators.createNestedObject();
            obj["name"] = "cold";
            obj["type"] = act->get_type();
            obj["value"] = act->get_value();
        }
    }
    {
        auto act = m_config->heat;
        if (act != nullptr) {
            JsonObject obj = actuators.createNestedObject();
            obj["name"] = "heat";
            obj["type"] = act->get_type();
            obj["value"] = act->get_value();
        }
    }
    {
        auto act = m_config->light;
        if (act != nullptr) {
            JsonObject obj = actuators.createNestedObject();
            obj["name"] = "light";
            obj["type"] = act->get_type();
            obj["value"] = act->get_value();
        }
    }
    {
        auto act = m_config->fan;
        if (act != nullptr) {
            JsonObject obj = actuators.createNestedObject();
            obj["name"] = "fan";
            obj["type"] = act->get_type();
            obj["value"] = act->get_value();
        }
    }

    String json;
    serializeJson(doc, json);

    return json;
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
        SERIAL_PRINTLN("Failed to open file for writing");
        return;
    }
    if (file.print(bodyContent)) {
        SERIAL_PRINTLN("File written");
    } else {
        SERIAL_PRINTLN("Write failed");
    }

    file.close();

    request->send(200, "text/plain", "OK");

    delay(1000);
    ESP.restart();
}

void WebServer::set_value(String json) {
    StaticJsonDocument<128> doc;
    DeserializationError error;


    error = deserializeJson(doc, json);
    if (error || !doc.containsKey("name") || !doc.containsKey("value")) {
        SERIAL_PRINTLN("Error in parameters");
        return;
    }
    String name = doc["name"].as<const char*>();
    float value = doc["value"].as<float>();

    SERIAL_PRINTLN("Change value of " + name + " to " + String(value));

    if(name == "heat"){
        m_config->heat->set_value(value);
    } else if (name == "cold") {
        m_config->cold->set_value(value);
    } else if (name == "fan") {
        m_config->fan->set_value(value);
    } else if (name == "light") {
        m_config->light->set_value(value);
    }

}
