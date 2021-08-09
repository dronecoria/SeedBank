#include <Arduino.h>
#include <SPIFFS.h>

#include "defines.h"
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
#ifdef DEBUG_SERIAL
    Serial.begin(115200);
#endif
    SERIAL_PRINTLN("Main::Boot");

    SERIAL_PRINTLN("Main::Setup SPIFFS");
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
    display->loop();
    delay(1000);
}
