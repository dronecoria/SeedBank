#include "display_st7789.h"
#include "api_http.h"
#include "status.h"

extern Status state;

void loop_display_st7789(void* parameter) {
  Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

  // if the display has CS pin try with SPI_MODE0
   //tft.init(240, 240, SPI_MODE2);
  tft.init(128, 96, SPI_MODE2);

  tft.setRotation(0);
  tft.fillScreen(ST77XX_MAGENTA);
  delay(1500);
  tft.fillScreen(ST77XX_BLACK);

  for (;;) {

    if (state.avg_temperature == ERROR_READING) {
      tft.fillScreen(ST77XX_GREEN);
      delay(500);
      continue;
    }

    tft.setCursor(0, 0);
    // tft.setTextWrap(true);
    // tft.invertDisplay(false);
    // tft.fillScreen(ST77XX_BLACK);
    switch (state.getDisplayStatus()) {
    case DISPLAY_STATUS::NORMAL: {
      tft.setTextSize(3);
      tft.setTextColor(ST77XX_RED, ST77XX_BLACK);

      //tft.print(String(state.avg_temperature, 2));
      for (auto t : state.temperatures) {
        tft.print(String(t, 2));
        tft.print("\n");
      }

      int positionX = 5;
      int positionY = 64;
      if (state.getConfig()->light->isActive()) {
        tft.fillRect(positionX, positionY, 32, 32, ST77XX_BLACK);
        tft.drawBitmap(positionX, positionY, light, 32, 32, ST77XX_YELLOW);
        positionX += 32 + 5;
      }
      if (state.getConfig()->fan->isActive()) {
        tft.fillRect(positionX, positionY, 32, 32, ST77XX_BLACK);
        tft.drawBitmap(positionX, positionY, fan, 32, 32, ST77XX_GREEN);
        positionX += 32 + 5;
      }
      if (state.getConfig()->cold->isActive()) {
        tft.fillRect(positionX, positionY, 32, 32, ST77XX_BLACK);
        tft.drawBitmap(positionX, positionY, snow, 32, 32, ST77XX_CYAN);
        positionX += 32 + 5;
      }
      if (state.getConfig()->heat->isActive()) {
        tft.fillRect(positionX, positionY, 32, 32, ST77XX_BLACK);
        tft.drawBitmap(positionX, positionY, fire, 32, 32, ST77XX_RED);
        positionX += 32 + 5;
      }
      for (; positionX < 200; positionX += 37) {
        tft.fillRect(positionX, positionY, 32, 32, ST77XX_BLACK);
      }
    } break;
    case DISPLAY_STATUS::INFO:
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_BLACK, ST77XX_YELLOW);
      tft.print(WiFi.SSID());
      tft.print(" ");
      tft.print(WiFi.getHostname());
      tft.print("\n");
      tft.print(WiFi.networkID().toString());
      tft.print("\n");

      tft.setTextSize(2);
      tft.setTextColor(ST77XX_BLACK, ST77XX_YELLOW);
      tft.print(WiFi.localIP().toString());
      tft.print("\n");

      tft.print(WiFi.macAddress());
      tft.print("\n");

      display_time(tft);
      break;
    case DISPLAY_STATUS::OFF:
      // tft.invertDisplay(false);
      tft.fillScreen(ST77XX_BLACK);
      break;
    }
    delay(100);
  }
}

void display_time(Adafruit_ST7789& tft) {

  char h[80];
  char m[80];
  char s[80];
  char p[80];

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  strftime(h, 80, "%I", &timeinfo);
  strftime(m, 80, "%M", &timeinfo);
  strftime(s, 80, "%S", &timeinfo);
  strftime(p, 80, "%p", &timeinfo);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.print(h);
  tft.print(":");
  tft.print(m);
  tft.print(":");
  tft.print(s);
  tft.print(" ");
  tft.print(p);
  tft.print("\n");
}
