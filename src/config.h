#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

#include "actuators.h"
#include "sensors.h"

class Config {
public:
  char* name;
  bool active;
  std::vector<float> temperatureTable;

  //TODO automatizar y eliminar condiciones
  float condiciones_margen1;
  float condiciones_margen2;
  float condiciones_margen4;
  float condiciones_histeresisCalentar_umbralBajo;
  float condiciones_histeresisCalentar_umbralAlto;
  float condiciones_histeresisCalentar_duracionBajoUmbralBajo;
  float condiciones_histeresisCalentar_duracionBajoUmbralAlto;
  float condiciones_histeresisCalentar_duracionSobreUmbralAlto;
  float condiciones_histeresisEnfriar_umbralBajo;
  float condiciones_histeresisEnfriar_umbralAlto;
  float condiciones_histeresisEnfriar_duracionBajoUmbralBajo;
  float condiciones_histeresisEnfriar_duracionBajoUmbralAlto;
  float condiciones_histeresisEnfriar_duracionSobreUmbralAlto;
  float condiciones_intervaloMuestreo;
  float condiciones_intervaloAlmacenamiento;
  float condiciones_intervaloDeSeguridad;

  // sensors
  std::vector<Sensor*> sensors;

  // relays
  Relay* cold;
  Relay* heat;
  Relay* fan;
  Relay* light;

  Config();
  ~Config();

  void init(void);

private:
};

#endif
