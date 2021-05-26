#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "config.h"

Config::Config() {}

void Config::init(void) {

  delay(1000);
  Serial.println("REad file");
  File file = SPIFFS.open("/config.json");

  if (!file) {
    Serial.println("There was an error opening the config file");
    // TODO errors alerts?
    return;
  }

  StaticJsonDocument<2046> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  name = (char*)malloc(sizeof(char) * strlen(doc["name"]));
  strcpy(name, doc["name"]);
  active = doc["active"];

  for (float repo : doc["tablaTemperaturas"].as<JsonArray>()) {
    temperatureTable.push_back(repo);
  }

  condiciones_margen1 = doc["condiciones"]["margen1"];
  condiciones_margen2 = doc["condiciones"]["margen2"];
  condiciones_margen4 = doc["condiciones"]["margen4"];
  condiciones_histeresisCalentar_umbralBajo = doc["condiciones"]["histeresisCalentar"]["umbralBajo"];
  condiciones_histeresisCalentar_umbralAlto = doc["condiciones"]["histeresisCalentar"]["umbralAlto"];
  condiciones_histeresisCalentar_duracionBajoUmbralBajo = doc["condiciones"]["histeresisCalentar"]["duracionBajoUmbralBajo"];
  condiciones_histeresisCalentar_duracionBajoUmbralAlto = doc["condiciones"]["histeresisCalentar"]["duracionBajoUmbralAlto"];
  condiciones_histeresisCalentar_duracionSobreUmbralAlto = doc["condiciones"]["histeresisCalentar"]["duracionSobreUmbralAlto"];
  condiciones_histeresisEnfriar_umbralBajo = doc["condiciones"]["histeresisEnfriar"]["umbralBajo"];
  condiciones_histeresisEnfriar_umbralAlto = doc["condiciones"]["histeresisEnfriar"]["umbralAlto"];
  condiciones_histeresisEnfriar_duracionBajoUmbralBajo = doc["condiciones"]["histeresisEnfriar"]["duracionBajoUmbralBajo"];
  condiciones_histeresisEnfriar_duracionBajoUmbralAlto = doc["condiciones"]["histeresisEnfriar"]["duracionBajoUmbralAlto"];
  condiciones_histeresisEnfriar_duracionSobreUmbralAlto = doc["condiciones"]["histeresisEnfriar"]["duracionSobreUmbralAlto"];
  condiciones_intervaloMuestreo = doc["condiciones"]["intervaloMuestreo"];
  condiciones_intervaloAlmacenamiento = doc["condiciones"]["intervaloAlmacenamiento"];
  condiciones_intervaloDeSeguridad = doc["condiciones"]["intervaloDeSeguridad"];

  for (JsonObject repo : doc["sensors"].as<JsonArray>()) {
    if (repo["type"] == "bmp280") {
      this->sensors.push_back(new Sensor_BMP280((int)repo["pin"]));
    }
    else if (repo["type"] == "bmdummyp280") {
      this->sensors.push_back(new Sensor_DUMMY((int)repo["pin"]));
    }
  }

  this->cold = (doc["cold"]["type"] == "none") ? nullptr : new Relay((int)doc["cold"]["pin"]);
  this->heat = (doc["heat"]["type"] == "none") ? nullptr : new Relay((int)doc["heat"]["pin"]);
  this->fan = (doc["fan"]["type"] == "none") ? nullptr : new Relay((int)doc["fan"]["pin"]);
  this->light = (doc["light"]["type"] == "none") ? nullptr : new Relay((int)doc["light"]["pin"]);
}

Config::~Config() {
  for (auto sensor : this->sensors) {
    delete sensor;
  }
}
