//Names: Nathaniel, Spencer, Evan
//Purpose: Create a mini swamp coole with the arduino
//CPE 301


//Beginning of temp and humidity code
#include <SimpleDHT.h>

// for DHT11, 
//      VCC: 3.3V through 5.2V
//      GND: GND
//      DATA: 2 for example
/* ******************* GLOBAL CONSTANTS ********************** */
// DHT11 data pin
const int pinDHT11 = 2;
SimpleDHT11 dht11;       //instantiates the SimpleDHT11 Object class to variable dht11

/* *******************************************************
 *                         Setup
 ********************************************************* */
void setup() {
  Serial.begin(9600);
}

/* *******************************************************
 *                      Void Loop
 ********************************************************* */
void loop() {
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read with raw sample data.
  //Local variables
  float temperatureC = 0.0;
  float temperatureF = 0.0;
  float humidity = 0.0;
  //Local data array
  byte data[40] = {0};
  if (dht11.read2(pinDHT11, &temperatureC, &humidity, data)) {
    Serial.print("Read DHT11 failed");
    return;
  }
  
  Serial.print("Sample RAW Bits: ");
  for (int i = 0; i < 40; i++) {
    Serial.print((int)data[i]);
    if (i > 0 && ((i + 1) % 4) == 0) {
      Serial.print(' ');
    }
  }
  Serial.println("");
  //°F = °C x 9/5 + 32.
  temperatureF = temperatureC * 9.0/5.0 + 32.0;
  Serial.print("Sample OK: ");
  Serial.print(temperatureC); Serial.print("°C, ");
  Serial.print(temperatureF); Serial.print("°F, ");
  Serial.print(humidity); Serial.println("%");
  
  // DHT11 sampling rate is 1HZ.
  delay(1000);
}
//end of temp and humidity code
