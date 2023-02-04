

#include "HX711.h" //You must have this library in your arduino library folder
#include <NimBLEDevice.h>
#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "HX711.h"
#include "EEPROM.h"

#include <ArduinoBleOTA.h>

HX711 scale3;
bool reset_values = false;
int userbuttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 2500;
bool ota_flag = false;
// Change this calibrati on factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor3 = -37640; //-106600 worked for my 40Kg max scale setup  144050
#define ON_OFF_RESET_PIN 4
//=============================================================================================
//                         SETUP
//=============================================================================================
void OTA_UPDATE()
{
  ArduinoBleOTA.begin("ArduinoBleOTA", InternalStorage);

  ota_flag = true;

  while (ota_flag == 1)
  {
#if defined(BLE_PULL_REQUIRED)
    BLE.poll();
#endif
    ArduinoBleOTA.pull();
  }
}
void IRAM_ATTR ON_OFF_RESET() // changes made
{
  reset_values = true;
}
void setup()
{
  scale3.begin(32, 33);
  Serial.begin(115200);
  Serial.println("HX711 Calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a,s,d,f to increase calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press z,x,c,v to decrease calibration factor by 10,100,1000,10000 respectively");
  Serial.println("Press t for tare");
  scale3.set_scale();
  scale3.tare(); // Reset the scale to 0

  long zero_factor3 = scale3.read_average(); // Get a baseline reading
  Serial.print("Zero factor3: ");            // This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor3);
  pinMode(ON_OFF_RESET_PIN, INPUT); // changes made
  attachInterrupt(ON_OFF_RESET_PIN, ON_OFF_RESET, FALLING);
}

//=============================================================================================
//                         LOOP
//=============================================================================================
void loop()
{
 
 

 

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor3 += 10;
    else if (temp == '-' || temp == 'z')
      calibration_factor3 -= 10;
    else if (temp == 's')
      calibration_factor3 += 100;
    else if (temp == 'x')
      calibration_factor3 -= 100;
    else if (temp == 'd')
      calibration_factor3 += 1000;
    else if (temp == 'c')
      calibration_factor3 -= 1000;
    else if (temp == 'f')
      calibration_factor3 += 10000;
    else if (temp == 'v')
      calibration_factor3 -= 10000;
    else if (temp == 'g')
      calibration_factor3 += 1;
    else if (temp == 'b')
      calibration_factor3 -= 1;
    else if (temp == 't')

      scale3.tare();
  }
  int button_value = digitalRead(ON_OFF_RESET_PIN);
  // Serial.print("button value ");
 //  Serial.println(button_value);
  // Serial.print("user_button state : ");
 //  Serial.println(userbuttonState);

  if (button_value != lastButtonState)
  {
    Serial.println("button presssed");
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
 //   Serial.println("button presssed 2.5 sec");
     if (button_value != userbuttonState)
     {
//      Serial.println("button ota ");
      userbuttonState = button_value;
      if (userbuttonState == LOW)
      {

        //   BLEDevice::deinit(true);

        Serial.println("OTA MODE ENTER");

        OTA_UPDATE(); // OTA update function
      }
    }
  }
  lastButtonState = button_value;

   scale3.set_scale(calibration_factor3); // Adjust to this calibration factor

  Serial.print("Reading3: ");
  Serial.print(scale3.get_units(), 2);
  Serial.print(" kg"); // Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor3: ");
  Serial.print(calibration_factor3);
  Serial.println();
  delay(1000);
  // Reset the scale to zero
}
