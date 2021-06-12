#include "display_ssd1306.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void u8g2_prepare()
{
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.enableUTF8Print();
}

void u8g2_printInfo()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_timB10_tf);
    u8g2.setCursor(0, 0);
    /*
    u8g2.print("1: ");
    u8g2.print(temperatures[0]);
    u8g2.print(" ºC");
    u8g2.setCursor(0, 12);
    u8g2.print("2: ");
    u8g2.print(temperatures[1]);
    u8g2.print(" ºC");
    u8g2.setCursor(0, 24);
    u8g2.print("H: ");
    u8g2.print(hdc_temperature);
    u8g2.print(" ºC ");
    u8g2.print(hdc_humidity);
    u8g2.print("%");
    u8g2.setCursor(0, 36);
    u8g2.print(WiFi.localIP());
    u8g2.print(" ");
    u8g2.print(ssid);
    u8g2.setCursor(0, 48);
    u8g2.print(time_str);
    */
    u8g2.print("Hello world!");
    u8g2.sendBuffer();
}

void loop_display(void* parameter) {
    // Setup
    Serial.print(" - Configure OLED display ... ");
    u8g2.begin();
    u8g2_prepare();
    Serial.println("OK");

    // Loop
    for (;;) {
        u8g2_printInfo();
        delay(1000);
    }
}
