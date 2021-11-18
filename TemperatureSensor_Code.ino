//THIS PROGRAM USES PWM PIN 2
//Temperature and Humidity Sensor

//NOTE: dht_nonblocking library must be included to run this code
//Link to dht_nonblocking library:  https://github.com/olewolf/DHT_nonblocking

#include <dht_nonblocking.h>

#include <Adafruit_Sensor.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );


void setup( )
{
  Serial.begin( 9600);
}


void loop( )
{
  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    Serial.print( "Temperature = " );
    float op1 = temperature * 1.8;
    float op2 = op1 + 32;
    Serial.print(op2, 1 );
    Serial.print( " deg. F, Humidity = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
  }
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
