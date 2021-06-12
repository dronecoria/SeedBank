#include <Arduino.h>
#include <SPIFFS.h>

#include "config.h"
#include "state.h"
#include "display.h"
#include "webserver.h"

Config *config;
State *state;
Display *display;
WebServer *webserver;


void setup() {
    // Initialize hardware
    Serial.begin(115200);
    Serial.println("Main::Boot");

    Serial.println("Main::Setup SPIFFS");
    SPIFFS.begin();

    // Read configuration from Flash
    config = new Config();

    // Create the Initial State based on configuration
    state = new State(config);

    // Lauch Display task
    display = new Display(config, state);

    // Lauch WebServer task
    webserver = new WebServer(config, state);
}

void loop() {

    // Loop de prueba
    struct tm   time_info;
    char        time_str[20];  // 2021-05-21 12:32:45

    Serial.println("Main::loop START");
    if (state->is_clock_set()) {
        Serial.println("Main::loop getLocalTime");
        getLocalTime(&time_info);
        Serial.println("Main::loop strftime");
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &time_info);
        Serial.print("Main::Time ");
        Serial.println(time_str);
    } else {
        Serial.println("Main::Clock is not set");
    }
    Serial.println("Main::loop delay");
    delay(1000);
    Serial.println("Main::loop END");
}
