#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#define LED 3

static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );


void setup( )
{
  Serial.begin( 9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}



/**
 *  Poll for a measurement, keeping the state machine alive.  Returns
 *  1true if a measurement is available.
 */
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



void loop( )
{
  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If true, measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    if (temperature == 23.0)
    {
      digitalWrite(LED, HIGH);
    }
    else
    {
      digitalWrite(LED, LOW);
    }
    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );
  }
}
