#include "config.h"
#include "status.h"
#include "display.h"

#include "api_http.h"

Api_http api_http;
Status state;

TaskHandle_t Task_display;

void IRAM_ATTR Ext_INT1_ISR() 
{
  state.nextDisplayStatus();
  Serial.print(F("PRESSED BUTTON"));
}

void setup(void) {

  Serial.begin(9600);
  Serial.print(F("Init"));

  Config *config = new Config();
  config->init();

  state.init(config);

  xTaskCreatePinnedToCore(
    loop_display,  /* Function to implement the task */
    "Display",       /* Name of the task */
    10000,         /* Stack size in words */
    NULL,          /* Task input parameter */
    0,             /* Priority of the task */
    &Task_display, /* Task handle. */
    0);            /* Core where the task should run */


  api_http.init();

  // botton
  int pin_botton = 19;
  pinMode(pin_botton, INPUT);
  attachInterrupt(pin_botton, Ext_INT1_ISR, RISING);
}


void loop() {
  state.update();

//timetable

  delay(500); 
}
