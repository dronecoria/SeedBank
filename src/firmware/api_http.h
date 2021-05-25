#ifndef API_HTTP_H
#define API_HTTP_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

//#include <ESPmDNS.h>


class Api_http
{
public:
  Api_http():m_server(80){};
  void init();

  void update();

private:
  AsyncWebServer m_server;
};

String processor(const String& var);

//time
void printLocalTime();
void setClock();


#endif
