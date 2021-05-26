#include "api_http.h"
#include "config.h"
#include "display.h"
#include "status.h"
#include <SPIFFS.h>

Api_http api_http;
Status state;

int pin_botton = 19;
void IRAM_ATTR Ext_INT1_ISR() {
	static unsigned long last_interrupt_time = 0;
	unsigned long interrupt_time = millis();
	if (interrupt_time - last_interrupt_time > 200) {
		state.nextDisplayStatus();
	}
	last_interrupt_time = interrupt_time;
}

void setup(void) {
	Serial.begin(9600);
	Serial.print(F("Init"));

	SPIFFS.begin();

	Config* config = new Config();
	config->init();

	state.init(config);

	TaskHandle_t Task_display;

	xTaskCreatePinnedToCore(loop_display,  /* Function to implement the task */
		"Display",     /* Name of the task */
		10000,         /* Stack size in words */
		NULL,          /* Task input parameter */
		0,             /* Priority of the task */
		&Task_display, /* Task handle. */
		0);            /* Core where the task should run */

	api_http.init();

	// botton
	pinMode(pin_botton, INPUT);
	attachInterrupt(pin_botton, Ext_INT1_ISR, RISING);
}

void loop() {
	state.update();

	// TODO timetable
	// test
	// if (state.temperatures[0] > 24) {
	//   state.getConfig()->cold->enable();
	// } else {
	//   state.getConfig()->cold->disable();
	// }
	// if (state.temperatures[1] > 24) {
	//   state.getConfig()->light->enable();
	// } else {
	//   state.getConfig()->light->enable();
	// }
	timetable();

	int interval = 500;
	if (state.temporizador > 0) {
		state.temporizador -= interval;
		if (state.temporizador <= 0) {
			//apagar frio o calor
			//TODO falta variable para ver cual
			state.getConfig()->cold->disable();
			state.getConfig()->heat->disable();
			state.bloqueoSeguridad = state.getConfig()->condiciones_intervaloDeSeguridad * 1000;
			state.temporizador = 0;
		}
		Serial.printf("\nTemporizador %d", state.temporizador);
	}
	if (state.bloqueoSeguridad > 0) {
		state.bloqueoSeguridad -= interval;
		if (state.bloqueoSeguridad <= 0) {
			state.bloqueoSeguridad = 0;
		}
		Serial.printf("\nBloqueoSeguridad %d", state.bloqueoSeguridad);
	}
	delay(interval);
}

int minutosTranscurridosDia()
{
	char h[80];
	char m[80];
	char s[80];
	char p[80];

	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		Serial.println("Failed to obtain time");
		return -1;
	}

	strftime(h, 80, "%I", &timeinfo);
	strftime(m, 80, "%M", &timeinfo);

	return atoi(h) * 60 + atoi(m);
}

void timetable()
{
	Config* config = state.getConfig();
	int minutosTranscurridosDelDia = minutosTranscurridosDia();
	int minutosDia = 60 * 24;
	int index = ceil((config->temperatureTable.size() * minutosTranscurridosDelDia) / minutosDia) - 1;

	int temperaturaReferencia = config->temperatureTable[index];

	float temperaturaMedia = state.avg_temperature;
	//int numSensores = config->sensors.size();

	Serial.print("\ntemperaturaReferencia:");Serial.print(temperaturaReferencia);
	Serial.print("\ntemperaturaMedia:");Serial.print(temperaturaMedia);


	// controlar si esta en una transicion
	if (!state.inTransition && state.lastTempReferencia != temperaturaReferencia) {
		state.inTransition = true;
		if (state.lastTempReferencia == ERROR_READING) {
			// al iniciar las neveras
			state.inTransition = false;
		}
		else {
			state.trasitionType = (state.lastTempReferencia - temperaturaReferencia > 0) ? TRANSITION_TYPE::COLD : TRANSITION_TYPE::HEAT;
			//log("Inicio transicion " + nevera.trasitionType, false);
		}
		state.lastTempReferencia = temperaturaReferencia;
	}
	/*
		// MEDIDAS SEGURIDAD
		// 2 - check si calor y frio estan encendidos a la vez
		if (Number(reles[nevera.calor]) > 0 && Number(reles[nevera.frio]) > 0) {
			setRele(nevera.calor, 0);
			setRele(nevera.frio, 0);
			desbloqueo(nevera);
			avisoTelegram("ALERTA:\n\nLa nevera " + nevera.nombre + " tiene el FRIO Y el CALOR encendidos a la vez.\n Temperatura Media: " + temperaturaMedia + " \n Temperatura de referencia: " + temperaturaReferencia);
			return;
		}
	*/
	if (temperaturaMedia == ERROR_READING) return;

	/*
	// 1 - Check margenes de seguridad, reles que se han quedado encendidos y se han pasado de la temperatura de referencia
	const margen = 2.0;
	if (Number(reles[nevera.frio]) > 0 && ((temperaturaMedia + margen) < temperaturaReferencia)) {
		setRele(nevera.frio, 0);  //apagar rele
		desbloqueo(nevera);
		avisoTelegram("ALERTA:\n\nLa nevera " + nevera.nombre + " tiene el FRIO encendido con temperatura Media: " + temperaturaMedia + " y la de referencia: " + temperaturaReferencia);
		return;
	}
	if (Number(reles[nevera.calor]) > 0 && ((temperaturaMedia - margen) > temperaturaReferencia)) {
		setRele(nevera.calor, 0);  //apagar rele
		desbloqueo(nevera);
		avisoTelegram("ALERTA:\n\nLa nevera " + nevera.nombre + " tiene el CALOR encendido con temperatura Media: " + temperaturaMedia + " y la de referencia: " + temperaturaReferencia);
		return;
	}


	// 3 - Avisar si la histeresis no se estabiliza
	//     solo si no esta en una transicion horaria
	if (nevera.inTransition) {
		// ver si finaliza una transicion
		if (nevera.trasitionType == = 'frio') {
			if ((temperaturaReferencia + nevera.condiciones.histeresisEnfriar.umbralAlto) > temperaturaMedia) {
				nevera.inTransition = false;
				log("Fin transicion frio");
			}
		}
		if (nevera.trasitionType == = 'calor') {
			if ((temperaturaReferencia - nevera.condiciones.histeresisCalentar.umbralAlto) < temperaturaMedia) {
				nevera.inTransition = false;
				log("Fin transicion calor");
			}
		}
	}
	else {
		const margen = 1.0;
		//avisar de histeresis
		if ((temperaturaReferencia - margen) > temperaturaMedia) {
			if (nevera.lastMessage == null) {
				nevera.lastMessage = "histeresis por frio";
				//avisoTelegram("AVISO:\n\nLa nevera " + nevera.nombre + " supera la histeresis por frio.\n Temperatura Media: " + temperaturaMedia + " \n Temperatura de referencia: " + temperaturaReferencia);
				write_aviso(nevera.nombre + ";histeresis por frio;Temperatura Media: " + temperaturaMedia + "  Temperatura de referencia: " + temperaturaReferencia);

			}
		}
		else if ((temperaturaReferencia + margen) < temperaturaMedia) {
			if (nevera.lastMessage == null) {
				nevera.lastMessage = "histeresis por calor";
				//avisoTelegram("AVISO:\n\nLa nevera " + nevera.nombre + " supera la histeresis por calor.\n Temperatura Media: " + temperaturaMedia + " \n Temperatura de referencia: " + temperaturaReferencia);
				write_aviso(nevera.nombre + ";histeresis por calor;Temperatura Media: " + temperaturaMedia + "  Temperatura de referencia: " + temperaturaReferencia);
			}
		}
		else {
			if (nevera.lastMessage != null) {
				//avisoTelegram("Fin " + nevera.lastMessage +" \n Temperatura Media: " + temperaturaMedia + " \n Temperatura de referencia: " + temperaturaReferencia);
				write_aviso(nevera.nombre + ";Fin " + nevera.lastMessage + ";Temperatura Media: " + temperaturaMedia + "  Temperatura de referencia: " + temperaturaReferencia);
				nevera.lastMessage = null;
			}
		}
	}

*/
	if (state.temporizador > 0) return;

	bool inEvento = false;
	if (state.bloqueoSeguridad <= 0) {
		if (config->cold != nullptr) {
			// Evento B: FRIO
			if (temperaturaMedia - temperaturaReferencia > config->condiciones_margen2) {
				// encender nevera y ventiladores
				Serial.println("encender frio");
				config->cold->enable();
				config->fan->enable();
				inEvento = true;
				float temperaturaABajar = temperaturaMedia - temperaturaReferencia;

				int timeToEnd = 0;
				if (temperaturaABajar < config->condiciones_histeresisEnfriar_umbralBajo) {
					timeToEnd = config->condiciones_histeresisEnfriar_duracionBajoUmbralBajo * 1000;
				}
				else if (temperaturaABajar < config->condiciones_histeresisEnfriar_umbralAlto) {
					timeToEnd = config->condiciones_histeresisEnfriar_duracionBajoUmbralAlto * 1000;
				}
				else {
					timeToEnd = config->condiciones_histeresisEnfriar_duracionSobreUmbralAlto * 1000;
				}
				//log("histeresis frio: " + temperaturaABajar + " encender :" + timeToEnd + " seg", false);
				//clearTimeout(nevera.temporizador);
				// nevera.temporizador = setTimeout(
				// 	apagarFrio,
				// 	timeToEnd ? ? 3000,
				// 	nevera
				// );
				state.temporizador = timeToEnd;

			}
			if (!inEvento && config->heat != nullptr) {
				// Evento C: CALOR
				if (temperaturaMedia - temperaturaReferencia < -config->condiciones_margen2) {
					// encender calor y ventiladores
					Serial.println("encender calor");
					config->heat->enable();
					config->fan->enable();
					inEvento = true;
					float temperaturaASubir = temperaturaReferencia - temperaturaMedia;

					int timeToEnd = 0;
					if (temperaturaASubir < config->condiciones_histeresisCalentar_umbralBajo) {
						timeToEnd = config->condiciones_histeresisCalentar_duracionBajoUmbralBajo * 1000;
					}
					else if (temperaturaASubir < config->condiciones_histeresisCalentar_umbralAlto) {
						timeToEnd = config->condiciones_histeresisCalentar_duracionBajoUmbralAlto * 1000;
					}
					else {
						timeToEnd = config->condiciones_histeresisCalentar_duracionSobreUmbralAlto * 1000;
					}

					//log("histeresis calor: " + temperaturaASubir + " encender :" + timeToEnd + " seg", false);
					// clearTimeout(nevera.temporizador);
					// nevera.temporizador = setTimeout(
					// 	apagarCalor,
					// 	timeToEnd ? ? 3000,
					// 	nevera
					// );
					state.temporizador = timeToEnd;

				}
			}
		}
	}
	else {
		Serial.println("BLOQUEADA");
	}
	// Evento A: activar ventiladores para evitar estratificacion
	if (!inEvento && state.temperatures.size() > 1) {
		float temp1 = state.temperatures[0];
		float temp2 = state.temperatures[1];
		float diff = abs(temp1 - temp2);
		if (diff > config->condiciones_margen1) {
			config->fan->enable();
		}
		else if (diff < config->condiciones_margen4) {
			config->fan->disable();
		}
	}



}