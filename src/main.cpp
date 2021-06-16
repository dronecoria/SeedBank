#include <Arduino.h>
#include <SPIFFS.h>

#include "config.h"
#include "state.h"
#include "display.h"
#include "webserver.h"
#include "decisor.h"

Config *config;
State *state;
Display *display;
WebServer *webserver;
Decisor *decisor;


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

    // Lauch Decisor task
    decisor = new Decisor(config, state);
}

void loop() {

    // Loop de prueba
    Serial.println("Main::loop START");
    display->loop();
    delay(1000);
    Serial.println("Main::loop delay");
    Serial.println("Main::loop END");
}
