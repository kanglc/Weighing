// Project: Weighing
// Company: Circular Unite
// By: Kang Liat Chuan (Nov 2023), with some help from cGPT
// Description: Weigh recycled material, upload to app (Bluetooth) and send to cloud
//              This project uses 3 example codes initially.
//              1. An example from BluetoothSerial (SerialToSerialBT),
//              2. HX711 calibration, and
//              3. HX711 measurement, from https://randomnerdtutorials.com/esp32-load-cell-hx711/

#include <stdio.h>
#include <Arduino.h>
#include "HX711.h"
#include "soc/rtc.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
HX711 scale;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;


void setup() {
  Serial.begin(115200);
  SerialBT.begin("Weighing"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t"); Serial.println(scale.read()); // print a raw reading from the ADC
  Serial.print("read average: \t\t"); Serial.println(scale.read_average(20)); // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t"); Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight (not set yet)
  Serial.print("get units: \t\t"); Serial.println(scale.get_units(5), 1); // print the average of 5 readings from the ADC minus tare weight (not set) divided
                                                                          // by the SCALE parameter (not set yet)

  //scale.set_scale(INSERT YOUR CALIBRATION FACTOR);
  scale.set_scale(429.1165);  // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0
  Serial.println("Scale setup done.");

  /*
  Serial.println("After setting up the scale:");
  Serial.print("read: \t\t"); Serial.println(scale.read()); // print a raw reading from the ADC
  Serial.print("read average: \t\t"); Serial.println(scale.read_average(20)); // print the average of 20 readings from the ADC
  Serial.print("get value: \t\t"); Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight, set with tare()
  Serial.print("get units: \t\t"); Serial.println(scale.get_units(5), 1); // print the average of 5 readings from the ADC minus tare weight, divided
                                                                          // by the SCALE parameter set with set_scale
  Serial.println("Readings:");
  */
}

void loop() {
  /*
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
  delay(20)
  */

  // get the weight
  float myFloat = scale.get_units();
  uint16_t myInt = (uint16_t)myFloat;
  char myString[5];
  convertIntToString(myInt, myString);

  // Send weight to BT
  SerialBT.write((uint8_t*)myString, sizeof(myString));

  // show in serial terminal
  Serial.print("one reading:\t"); Serial.println(myFloat, 1);
  //Serial.print("\t| average:\t"); Serial.println(scale.get_units(10), 5);

  scale.power_down();             // put the ADC in sleep mode
  delay(1000);
  scale.power_up();
}

void convertIntToString(int num, char* str) {
    sprintf(str, "%d", num);
}


//// Calibrating the load cell. Calibration factor = (reading)/(known weight)
//#include <Arduino.h>
//#include "soc/rtc.h"
//#include "HX711.h"
//
//// HX711 circuit wiring
//const int LOADCELL_DOUT_PIN = 16;
//const int LOADCELL_SCK_PIN = 4;
//
//HX711 scale;
//
//void setup() {
//  Serial.begin(115200);
//  //rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
//  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
//}
//
//void loop() {
//
//  if (scale.is_ready()) {
//    scale.set_scale();    
//    Serial.println("Tare... remove any weights from the scale.");
//    delay(5000);
//    scale.tare();
//    Serial.println("Tare done...");
//    Serial.print("Place a known weight on the scale...");
//    delay(5000);
//    long reading = scale.get_units(10);
//    Serial.print("Result: ");
//    Serial.println(reading);
//  } 
//  else {
//    Serial.println("HX711 not found.");
//  }
//  delay(1000);
//}
