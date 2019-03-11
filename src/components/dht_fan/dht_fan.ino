#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#define DC_ENABLE 5
#define DIRA 3
#define DIRB 4


static const int DHT_SENSOR_PIN = 2;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
float demo_var = 20.0;


void setup()
{
  pinMode(DC_ENABLE, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  Serial.begin(9600);
//  pinMode(LED, OUTPUT);
//  digitalWrite(LED, LOW);
}


/**
 *  Poll for a measurement, keeping the state machine alive.  Returns
 *  1true if a measurement is available.
 */
static bool measure_environment(float *temperature, float *humidity)
{
  static unsigned long measurement_timestamp = millis();

  /* Measure once every four seconds. */
  if(millis() - measurement_timestamp > 3000ul)
  {
    if(dht_sensor.measure(temperature, humidity) == true)
    {
      measurement_timestamp = millis();
      return(true);
    }
  }

  return(false);
}


void loop()
{
  float temperature;
  float humidity;

  /* Measure temperature and humidity.  If true, measurement is available. */
  if(measure_environment(&temperature, &humidity) == true)
  {
    if (demo_var >= 25.0) /* 25.0 is just a placeholder for the desired temp to turn on fan */
    {
      if (25.0 <= (25.0 + 1)) /* idea is if the temp is greater than a certain amount more than the highest desired temp to turn fan on full else medium speed while over */
      {
        Serial.println("PWM ON: full speed");
        analogWrite(DC_ENABLE, 255);
        digitalWrite(DIRA, HIGH);
        digitalWrite(DIRB, LOW);
      }

      /* need to test this statement */
      else
      {
        Serial.println("PWM ON: mid speed");
        analogWrite(DC_ENABLE, 128);
        digitalWrite(DIRA, HIGH);
        digitalWrite(DIRB, LOW);
      }
    }

    else
    {
      Serial.println("PWM off");
      digitalWrite(DC_ENABLE, LOW);
    }

    Serial.print("T = ");
    Serial.print(temperature, 1);
    Serial.print(" deg. C, H = ");
    Serial.print(humidity, 1);
    Serial.println("%");
    Serial.print("Demo_var = ");
    Serial.println(demo_var, 1);

    /* for demo purposes only */
    if (demo_var <= 20.0)
      demo_var = 23.0;
    else if (demo_var >= 26.0)
      demo_var = 20.0;
    else
      demo_var++;
  }
}
