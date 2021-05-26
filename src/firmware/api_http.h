#ifndef API_HTTP_H
#define API_HTTP_H

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

class Api_http {
public:
  Api_http();
  void init();

  void update();

private:
  AsyncWebServer m_server;
};

String processor(const String& var);

// time
void printLocalTime();
void setClock();

#endif
