#include "api_http.h"
#include "status.h"

#include "time.h"

extern Status state;

void Api_http::init(){
    const char *ssid = "DIGIFIBRA-YEbb";
    const char *password = "xs23gYehFD";
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

//    if (MDNS.begin("esp32")) {
//      Serial.println("MDNS responder started");
//    }

    SPIFFS.begin();


    m_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });


    m_server.on("/avgtemp", HTTP_GET, [&](AsyncWebServerRequest *request){
      request->send(200, "text/plain", String(state.avg_temperature,2));
    });

    m_server.onNotFound([](AsyncWebServerRequest *request) {      
      if (SPIFFS.exists(request->url())  ){
        request->send(SPIFFS, request->url());
      }else{
        request->send(400, "text/plain", "404 Not found");
      }              
    });

    m_server.begin();
    Serial.println("HTTP server started");

    setClock();  
}

// Replaces placeholder in html
String processor(const String& var){
  //Serial.println(var);
  if(var == "STATE"){
    return "state found";
  }
  return String();
}

void printLocalTime()
{
  char temp[32];
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");  
}

// Setting clock just to be sure...
void setClock() {
  const char* ntpServer1 = "pool.ntp.org";
  const char* ntpServer2 = "time.nist.gov";
  const long  gmtOffset_sec = 3600;
  const int   daylightOffset_sec = 3600;


  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1,ntpServer2);
  
  printLocalTime();
}
