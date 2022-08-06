#include <M5Stack.h>
#include "WHILL.h"
#include "Wire.h"

#define JOY_ADDR 0x52

uint8_t x_data = 0;
uint8_t y_data = 0;
uint8_t button_data = 0;
char data[100] = {0};

const uint8_t NEUTRAL = 120;
const int8_t MAX_TH = 100;
const int8_t MIN_TH = 10;

WHILL whill(&Serial2);

bool is_power_on = false;
uint8_t text_font = 0;

void setup()
{
  Serial.begin(115200);
  // Power ON Stabilizing...
  delay(500);
  M5.begin();
  M5.Lcd.clear();
  //disable the speak noise
  dacWrite(25, 0);

  Wire.begin(21, 22, 400000);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setTextFont(text_font);
}

int8_t convertJoystick(uint8_t ext_joy)
{
  int8_t conv_joy = ext_joy - NEUTRAL;

  if(conv_joy >= 0){
    if(abs(conv_joy) < MIN_TH) return 0;
    conv_joy -= MIN_TH; //offset;
    if(abs(conv_joy) >= MAX_TH) return MAX_TH;
    return conv_joy;
  }else{
    if(abs(conv_joy) < MIN_TH) return 0;
    conv_joy += MIN_TH; //offset;
    if(abs(conv_joy) >= MAX_TH) return -MAX_TH;
    return conv_joy;
  }
}

void loop() {
  M5.update();
  //M5.Lcd.setCursor(0, 0);
  //M5.Lcd.clear(BLACK);

  // put your main code here, to run repeatedly:
  Wire.requestFrom(JOY_ADDR, 3);
  if (Wire.available()) {
    x_data = Wire.read();
    y_data = Wire.read();
    int8_t conv_y = convertJoystick(y_data);
    int8_t conv_x = convertJoystick(x_data);
    button_data = Wire.read();

    if(M5.BtnA.wasPressed()){
      String power_str = String("Power:");
      if(is_power_on){
        whill.setPower(false);
        is_power_on = false;
        power_str += String("OFF");
      }else{
        whill.setPower(true);
        delay(100);
        whill.setPower(true);
        is_power_on = true;
        power_str += String("ON ");
      }
      M5.Lcd.setCursor(0, 180);
      M5.Lcd.print(power_str);
    }else{
      whill.setJoystick(conv_x, conv_y);
      sprintf(data, "x:%d y:%d button:%d\n", conv_x, conv_y, button_data);
      Serial.print(data);
      String x_str = String("X:") + String(conv_x) + String("    ");
      String y_str = String("Y:") + String(conv_y) + String("    ");

      int16_t interval_h = 35;
      uint8_t line_cnt = 0;
      //int16_t text_height = M5.Lcd.fontHeight(text_font);
      M5.Lcd.drawString("Joystick", 0, interval_h * line_cnt); line_cnt++;
      M5.Lcd.drawString(x_str, 0, interval_h * line_cnt); line_cnt++;
      M5.Lcd.drawString(y_str, 0, interval_h * line_cnt); line_cnt++;
    }
  }
  delay(10);
}
