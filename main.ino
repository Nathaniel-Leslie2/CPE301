#include <dht_nonblocking.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>

//temperature sensor code:
#include <DHT.h>
#include <DHT_U.h>

// Define Pins for LEDs
#define BLUE 53 //Turn on when motor is running.
#define GREEN 51 //This LED is lit until the fan is turned on (idle state)
#define RED 47 //turns on if water level too low, all other LEDs off until water level up
#define YELLOW 49 //remains on until system starts (disable state)
//END OF LED pin definitions

#define DHT_SENSOR_TYPE DHT_TYPE_11

//RTC libraries + initialization
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


Servo myservo;
const int A=A0;

//DC motor code:
//pins D: 5,4,3

//Variable to be passed in from main
int turn_on =0;

//define port E pointers
volatile unsigned char* port_e = (unsigned char*) 0x2E;
volatile unsigned char* ddr_e = (unsigned char*) 0x2D;
volatile unsigned char* pin_e = (unsigned char*) 0x2C;

//define port G pointers
volatile unsigned char* port_g = (unsigned char*) 0x34;
volatile unsigned char* ddr_g = (unsigned char*) 0x33;
volatile unsigned char* pin_g = (unsigned char*) 0x32;
//end of DC motor code

// Check the water level
//Define ADC Register Pointers
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;
//end of water sensor and servo register pointers

//variables for water sensor code.
//had to edit these variables to get the water sensor code to work.
int adc_id = 2;
int HistoryValue = 2;
char printBuffer[128];

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

static const int DHT_SENSOR_PIN = 2; //pin two for temperature sensor
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

void setup( )
{
  Serial.begin( 9600);
  adc_init(); //initialized water and servo controls
  myservo.attach(9);
  myservo.write(90);// move servos to center position -> 90°
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Temperature:");

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(YELLOW, HIGH);

  //error messages for RTC
/*  if (! rtc.begin()) {
   Serial.println("Couldn't find RTC");
   while (1);
 }
 if (! rtc.isrunning()) {
   Serial.println("RTC is NOT running!");
 }*/
}

//integer variables for LEDs
int redValue = 255;
int greenValue = 255;
int blueValue = 255;
int yellowValue = 255;

//How to turn LED on/off
// analogWrite(RED, redValue);

void loop( )
{
 //DC motor setup
  //Set PE5 to output
  *ddr_e |= 0x20;
  
//set PE3 to output
  *ddr_e |= 0x08;
  
//set PG5 to output
  *ddr_g |= 0x20;

    //write a 1 to PE5
*port_e |= 0x20;

//write a 0 to PG5
*port_g &= 0x20;

  //end of DC motor code

  
  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    Serial.print( "Temperature = " );
    float op1 = temperature * 1.8;
    float op2 = op1 + 32; //conversion from C to F
    temperature = op2;
    Serial.print(String(op2));
    Serial.println();
    logTemp(op2);
    Serial.print( "deg. F, Humidity = " );
    Serial.print(String(humidity));
    Serial.println( "%" );
  }
//water sensor loop code:
    int value = adc_read(adc_id); // get adc value
    //red LED water level low loop
    errorLED(value);
    if(((HistoryValue>=value) && ((HistoryValue - value) > 10)) || ((HistoryValue<value) && ((value - HistoryValue) > 10)))
    {
      sprintf(printBuffer,"Water level is %d\n", value);
      Serial.print(printBuffer);
      HistoryValue = value;
    } //end of water sensor loop code

    //servo loops:
  int voltage = adc_read(A);//read voltage from POT
  int angle = voltage/5.7;//Scale down analog input to be between 180 and 0
  myservo.write(angle);// move servos   
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print("Fahrenheit: ");
  lcd.print(String(temperature));

 if(temperature > 75 && value > 150){
  //write a 1 to the enable bit on PE3
  *port_e |= 0x08;
  analogWrite(BLUE, blueValue);
  analogWrite(RED, 0);
  analogWrite(YELLOW, 0);
  analogWrite(GREEN, 0);
  }
  else{
  *port_e &= 0x00;
  analogWrite(BLUE, 0);
  analogWrite(RED, 0);
  analogWrite(YELLOW, 0);
  analogWrite(GREEN, greenValue);
  }

}

void errorLED(int waterLevel){
  if(waterLevel <= 150){
  analogWrite(BLUE, 0);
  analogWrite(RED, redValue);
  analogWrite(YELLOW, 0);
  analogWrite(GREEN, 0);  
  lcd.print("Error!!!");
  lcd.setCursor(0, 1);
  lcd.print("WATER TOO LOW");
  waterLevel = adc_read(adc_id);
  *port_e &= 0x00;
  errorLED(waterLevel);
  }
}

void logTemp(int temp){
//loop to log time for temperature readings
 DateTime now = rtc.now();
 Serial.print("Time log: ");
 Serial.print(now.month(), DEC);
 Serial.print('/');
 Serial.print(now.day(), DEC);
// Serial.print(now.year(), DEC);
// Serial.print(" (");
// Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
 Serial.print(" ");
 Serial.print(now.hour(), DEC);
 Serial.print(':');
 Serial.print(now.minute(), DEC);
 Serial.print(':');
 Serial.print(now.second(), DEC);
 Serial.print(" Temperature is: ");
 Serial.print(temp);
 Serial.println();
 delay(9000);
  
}

static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}
//end of temperature sensor code

//function for water sensor loop and servo loop
void adc_init()
{

  //MUST UPDATE FUNCTION AND SEPERATE THE SERVO AND WATER SENSOR POINTERS.
  // setup the A register
  *my_ADCSRA |= B10000000;
  *my_ADCSRA &= B11110111;
  *my_ADCSRA &= B11011111;
  
  // setup the B register
  *my_ADCSRB &= B11111000;

  // setup the MUX Register
  *my_ADMUX |= (1<<REFS0);
}

//second funciton for water sensor and servo loop:
unsigned int adc_read(unsigned char adc_channel_num)
{
    //MUST UPDATE FUNCTION AND SEPERATE THE SERVO AND WATER SENSOR POINTERS.
  int channel_selector;
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX &= B11100000;

  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= B11110111;

  //Assign correct channel using MUX 5:0
  if (adc_channel_num < 8) {
    *my_ADMUX |= adc_channel_num;
  }
  else if ((adc_channel_num > 7) && (adc_channel_num < 16)) {
     channel_selector = (adc_channel_num - 8);
     *my_ADCSRB |= B00001000;
     *my_ADMUX |= channel_selector;
  }

  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= B01000000;
  
  // wait for the conversion to complete
  while ((*my_ADCSRA & 0x40) != 0);
  
  // return the result in the ADC data register
  return (*my_ADC_DATA & 0x03FF);
}
