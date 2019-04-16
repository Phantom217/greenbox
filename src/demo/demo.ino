#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int LAMP_GREEN_PIN = 2;
static const int LAMP_HEAT_PIN =  3;
static const int FAN_01_PIN = 4;
static const int DHT_SENSOR_PIN = 5;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
static const unsigned long LAMP_TIMER = 10000;
unsigned long lamp_timer_start = 0;
bool green_on = false;
bool heat_on = false;
bool fan_on = false;
bool lamp_timer_active = false;

void setup()
{
  Serial.begin(9600);

  pinMode(LAMP_GREEN_PIN, OUTPUT);
  pinMode(LAMP_HEAT_PIN, OUTPUT);
  pinMode(FAN_01_PIN, OUTPUT);

  digitalWrite(LAMP_GREEN_PIN, LOW); /* somehow this turns it on, not sure why */
  green_on = true;
  digitalWrite(LAMP_HEAT_PIN, HIGH); /* turns the lamp off */
  heat_on = false;
  digitalWrite(FAN_01_PIN, HIGH); /* same case as the above two */
  fan_on = false;

  lamp_timer_start = millis();
  lamp_timer_active = true;
}


/**
 *  Poll for a measurement, keeping the state machine alive.  Returns
 *  1true if a measurement is available.
 */
static bool measure_environment(float *temperature, float *humidity)
{
  static unsigned long measurement_timestamp = millis();

  /* Measure once every four seconds. */
  if (millis() - measurement_timestamp > 3000ul)
  {
    if (dht_sensor.measure(temperature, humidity) == true)
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
  if (measure_environment(&temperature, &humidity) == true)
  {
    if (temperature >= 24.0)
    {
      digitalWrite(FAN_01_PIN, LOW);
      fan_on = true;
      Serial.println("Fan ON");
    }

    else
    {
      digitalWrite(FAN_01_PIN, HIGH);
      fan_on = false;
      Serial.println("Fan OFF");
    }

    Serial.print("T = ");
    Serial.print(temperature, 1);
    Serial.print(" deg. C, H = ");
    Serial.print(humidity, 1);
    Serial.println("%");
  }

  if (lamp_timer_active && (millis() - lamp_timer_start) >= LAMP_TIMER)
  {
    lamp_timer_start += LAMP_TIMER;
    green_on = !green_on;
    heat_on = !heat_on;

    if (green_on && !heat_on)
    {
      digitalWrite(LAMP_GREEN_PIN, LOW);
      digitalWrite(LAMP_HEAT_PIN, HIGH);
      Serial.println("Green Lamp ON, Heat Lamp OFF");
    }

    else if (heat_on && !green_on)
    {
      digitalWrite(LAMP_HEAT_PIN, LOW);
      digitalWrite(LAMP_GREEN_PIN, HIGH);
      Serial.println("Heat Lamp ON, Green Lamp OFF");
    }

    else
    {
      Serial.println("Well, this is awkward... write better code!");
    }
  }
}
