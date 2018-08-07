/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341 Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <ILI9341_t3.h>
#include <SD.h>
#include <SPI.h>


#include "ArduinoJson.h"
#include "Bounce.h"

#include "Misc.h"
#include "Controls.h"
//#include "RotaryEncoder.h"
#include "Screens.h"
#include "Preset.h"

/////////////////////
// SD CARD
/////////////////////
File file;
#define SDCARD_CS 3

/////////////////////
// TOUCH
/////////////////////
// The STMPE610 uses hardware SPI on the shield, and #8
constexpr unsigned STMPE_CS = 6;
constexpr unsigned STMPE_IRQ = 2;

/////////////////////
// TFT DISPLAY
/////////////////////
// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC  9
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

/////////////////////
// CONTROLS
/////////////////////
Controls controls(1,1);

char jsonTextBuffer[1024];
StaticJsonBuffer<1024> jsonBuffer;
JsonObject *jsonObj;


constexpr unsigned PRESET_ID_INDEX = 6;
char presetFilename[] = "PRESET0.JSN";
PresetArray *presetArray = nullptr;
unsigned activePreset = 0;
unsigned selectedPreset = 0;

void setup(void) {

  Serial.begin(57600);
  while (!Serial) {}
  pinMode(23,INPUT);

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, 1); // disable the SD CARD
  pinMode(STMPE_CS, OUTPUT);
  digitalWrite(STMPE_CS, 1); // disable the Touch interface
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, 1); // disable the TFT interface

  // Setup the Controls.
  controls.addRotary(19, 22, SWAP, 3);
  controls.addSwitch(23, 10);
  controls.addTouch(STMPE_CS, STMPE_IRQ, tft.height(), tft.width());

  Serial.println("Creating Preset Array");
  presetArray = createPresetArray();
  //createDefaultPresets(presetArray, 2, 2);

  // Check the SD Card
  if (!SD.begin(SDCARD_CS)) {
    Serial.println("SD Card init seems to fail");
  } else {
      for (unsigned i=0; i<MAX_PRESETS; i++) {
        presetFilename[PRESET_ID_INDEX] = i + 0x30;
        file = SD.open(presetFilename);
        if (!file) {
          //Serial.println(String("Can't open ") + presetFilename);
        } else {
          // Read the file contents
          Serial.println(String("Processing ") + presetFilename);
          size_t availableBytes = file.available();
          Serial.println(String("Reading ") + availableBytes + String(" bytes"));
          if (availableBytes > 0) {
            file.read(jsonTextBuffer, availableBytes);
            jsonObj = &jsonBuffer.parseObject(jsonTextBuffer);
              if (!jsonObj->success()) {
                Serial.println("Parsing JSON object failed");
              } else {
                Preset newPreset;
                jsonToPreset(*jsonObj, newPreset);
                addToVector(*presetArray, newPreset, i);
              }
          }
          
        }
      }
  }
  digitalWrite(SDCARD_CS,1);
  
  tft.begin();
  tft.setRotation(3);
  Serial.println(String("Height is ") + tft.height() + String(", width is ") + tft.width());  
  
  Serial.println("FINISHED: setup()");

  DrawPresetNavigation(tft, presetArray, activePreset, selectedPreset);

//  while (true) {
//    StringEdit(tft, (*presetArray)[0].name, knob0, sw0);
//  }

}


void loop()
{
  controls.getTouchPoint();

  int knobAdjust = controls.getRotaryAdjustUnit(0);
  if (knobAdjust != 0) {
    selectedPreset = adjustWithWrap(selectedPreset, knobAdjust, presetArray->size()-1);
    DrawPresetNavigation(tft, presetArray, activePreset, selectedPreset);
    Serial.println(String("Knob adjusted by ") + knobAdjust + String(", selectedPreset is now ") + selectedPreset);
  }

  if (controls.isSwitchToggled(0)) {
    Serial.println(String("Setting activePreset to ") + selectedPreset);
    activePreset = selectedPreset;
    DrawPresetNavigation(tft, presetArray, activePreset, selectedPreset);
  }

  delay(100);

}
