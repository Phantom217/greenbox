#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int LAMP_GREEN_PIN = 2;
static const int LAMP_RED_PIN =  3;
static const int FAN_01_PIN = 4;
static const int DHT_SENSOR_PIN = 5;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
static const unsigned long LAMP_TIMER = 10000;
unsigned long timer_start_lamp = 0;
bool green_on = false;
bool red_on = false;
bool fan_on = false;
bool timer_active_lamp = false;

void setup()
{
  Serial.begin(9600);

  pinMode(LAMP_GREEN_PIN, OUTPUT);
  pinMode(LAMP_RED_PIN, OUTPUT);
  pinMode(FAN_01_PIN, OUTPUT);

  digitalWrite(LAMP_GREEN_PIN, LOW);
  green_on = false;
  digitalWrite(LAMP_RED_PIN, LOW);
  red_on = false;
  digitalWrite(FAN_01_PIN, LOW);
  fan_on = false;

  timer_start_lamp = millis();
  timer_active_lamp = true;
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
    if (temperature >= 25.0)
    {
      digitalWrite(FAN_01_PIN, HIGH);
      fan_on = true;
      Serial.println("Fan ON");
    }

    else
    {
      digitalWrite(FAN_01_PIN, LOW);
      fan_on = false;
      Serial.println("Fan OFF");
    }

    Serial.print("T = ");
    Serial.print(temperature, 1);
    Serial.print(" deg. C, H = ");
    Serial.print(humidity, 1);
    Serial.println("%");
  }

  if (timer_active_lamp && (millis() - timer_start_lamp) >= LAMP_TIMER)
  {
    timer_start_lamp += LAMP_TIMER;
    green_on = !green_on;
    red_on = !red_on;

    if (green_on && !red_on)
    {
      digitalWrite(LAMP_GREEN_PIN, HIGH);
      digitalWrite(LAMP_RED_PIN, LOW);
      Serial.println("Green Lamp ON, Heat Lamp OFF")
    }

    else if (red_on && !green_on)
    {
      digitalWrite(LAMP_RED_PIN, HIGH);
      digitalWrite(LAMP_GREEN_PIN, LOW);
      Serial.println("Heat Lamp ON, Green Lamp OFF")
    }

    else
    {
      Serial.println("Well, this is awkward...");
    }
  }
}
