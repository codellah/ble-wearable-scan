#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#define COUNT_LOW 0
#define COUNT_HIGH 8888

#define TIMER_WIDTH 16

#include "esp32-hal-ledc.h"  // from Motor script

const int PIN = 2;        // BLE pin
const int CUTOFF = -50;   // Looks at range to cutoff BLE scan
const byte led_gpio = 4;  // pin for motor
int servo_pos = 4000;
int scanTime = 2;  // In seconds
BLEScan* pBLEScan;

int average_rssi = CUTOFF;

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");
  pinMode(PIN, OUTPUT);

  BLEDevice::init("");
  ledcSetup(1, 50, TIMER_WIDTH);
  ledcAttachPin(led_gpio, 1);

  pBLEScan = BLEDevice::getScan();  // create new scan

  pBLEScan->setActiveScan(
      true);  // active scan uses more power, but get results faster
  pBLEScan->setInterval(100);  // Scan every 100 msec
  pBLEScan->setWindow(99);     // less or equal setInterval value
}

void loop() {
  /*Activating Bluetooth search*/
  BLEScanResults results = pBLEScan->start(1);
  bool found_device = false;

  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);
        String dev = device.getAddress().toString().c_str();

   // Serial.printf("Device %s, Signal Strength: %d, address %s", device.getName(),
                  device.getRSSI();
                  //device.getAddress().toString().c_str();
                  Serial.println(dev);
    if (dev == "Device Name") {
      //                     ^^^^^^^^^^^^^^^^^^^^^^^^ Replace this string with
      //                     the name of advertising device name
Serial.println("Success");
      found_device = true;

      // Keep a running average of RSSI value from 1 recent scan.
      int rssi = device.getRSSI();
      average_rssi = (average_rssi + rssi) / 2;
      break;
    }
  }

  // If the target device is not found, then force the average_rssi to be below
  // the CUTOFF and stop.
  if (!found_device) {
    average_rssi = CUTOFF - 1;
  }
  Serial.printf("Current average_rssi is: %d", average_rssi);

  // delete results fromBLEScan buffer to release memory
  pBLEScan->clearResults();

  /*Activating the motor and LED light*/
  digitalWrite(PIN, average_rssi > CUTOFF ? HIGH : LOW);
  if (average_rssi > CUTOFF) {
    ledcWrite(1, 720);  // Rotates motor when device is detected
    delay(250);
  } else {
    ledcWrite(1, 0);
    delay(250);
  }
}
