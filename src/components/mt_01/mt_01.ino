#include <dht_nonblocking.h>
#define MT_SENSOR_TYPE DHT_TYPE_11

static const int MT_01_PIN = A0;
DHT_nonblocking mt_sensor(MT_01_PIN, MT_SENSOR_TYPE)

void setup()
{
  Serial.begin(9600);
}

static bool measure_soil_moisture(float *moisture)
{
  static unsigned long measurement_timestamp = millis();

  /* Measure once every four seconds */
  if(millis() - measurement_timestamp > 3000ul)
  {
      if(mt_sensor.measure(moisture) == true)
      {
          measurement_timestamp = millis();
          return(true);
      }
  }

  return(false);
}
void loop()
{
  float moisture;
  moisture = analogRead(MT_01_PIN);
  moisture = map(moisture, 550, 0, 0, 100);

  if(measure_soil_moisture(&moisture) == true)
  {
    Serial.print("Moisture = ");
    Serial.print(moisture);
    Serial.println("%");
  }
}
