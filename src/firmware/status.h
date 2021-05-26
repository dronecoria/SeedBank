#ifndef STATUS_H
#define STATUS_H

#define ERROR_READING -9999

#include "config.h"
#include <vector>

enum class DISPLAY_STATUS { NORMAL = 0, INFO, OFF };
enum class TRANSITION_TYPE { COLD, HEAT };
class Status {
public:
  // sensors last lecture
  std::vector<float> temperatures;
  bool door;

  // calculated
  float avg_temperature;

  bool inTransition = false;
  TRANSITION_TYPE trasitionType;
  float lastTempReferencia = ERROR_READING;

  int temporizador = 0;// milisegundos que quedan de la accion en curso
  int bloqueoSeguridad = 0;// milisegundos que quedan de bloqueo

  Status();

  void init(Config* config);

  void update(void);

  Config* getConfig();

  DISPLAY_STATUS getDisplayStatus();
  void nextDisplayStatus();

private:
  Config* m_config;
  DISPLAY_STATUS m_display_status;
};

#endif