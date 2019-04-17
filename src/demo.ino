#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

/* static const int LAMP_GREEN_PIN = 2; */
static const int LAMP_HEAT_PIN =  3;
static const int FAN_01_PIN = 4;
static const int DHT_SENSOR_PIN = 5;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
/* static const unsigned long LAMP_TIMER = 10000; */
/* unsigned long lamp_timer_start = 0; */
/* bool green_on = false; */
bool heat_on = false;
bool fan_on = false;
/* bool lamp_timer_active = false; */

byte mac[] = { 0x00, 0xAA, 0xBB,0xCC, 0xDE, 0x02 };

char server_name[] = "web.greenbox.net";

EthernetClient client;

void setup()
{
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    while (true);
  }

  Serial.begin(9600);

/*   pinMode(LAMP_GREEN_PIN, OUTPUT); */
  pinMode(LAMP_HEAT_PIN, OUTPUT);
  pinMode(FAN_01_PIN, OUTPUT);

/*   digitalWrite(LAMP_GREEN_PIN, LOW); /* somehow this turns it on, not sure why */ */
/*   green_on = true; */
  digitalWrite(LAMP_HEAT_PIN, HIGH); /* turns the lamp off */
  heat_on = false;
  digitalWrite(FAN_01_PIN, HIGH); /* same case as the above two */
  fan_on = false;

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
  float temp_f;
  float humidity;

  temp_f = (temperature * 9/5) + 32;

  /* Measure temperature and humidity.  If true, measurement is available. */
  if (measure_environment(&temperature, &humidity) == true)
  {
    if (temperature >= 32.0)
    {
      digitalWrite(FAN_01_PIN, LOW); /*turn fan on */
      fan_on = true; /* set flag */
      Serial.println("Fan ON");
    }
    if (temperature < 29.4)
    {
      digitalWrite(FAN_01_PIN, HIGH); /*turn fan off */
      fan_on = false;
      Serial.println("Fan OFF");
    }
    if (temperature > 29.4)
    {
      digitalWrite(LAMP_HEAT_PIN, HIGH); /* heat lamp off */
      heat_on = false;
      Serial.println("Heat OFF: Temp > 85 deg. F");
    }
    if (tempreature <= 26.6)
    {
      digitalWrite(LAMP_HEAT_PIN, LOW); /* heat lamp on */
      heat_on = true;
      Serial.println("Heat ON: Temp < 80 deg. F");
    }

    Serial.print("T = ");
    Serial.print(temp_f, 1);
    Serial.print(" deg. F, H = ");
    Serial.print(humidity, 1);
    Serial.println("%");
  }


}

void sendGET()
{
  if (client.connect(server_name, 80)) /* start client connection, check for connection*/
  {
    Serial.println("connected");
/* edit next 2 lines for reading html from sd card */
    client.println("GET /~shb/arduino.txt HTTP/1.1"); /* download text */
    client.println("Host: web.greenbox.net");
    client.println("Connection: close"); /* close 1.1 persistent connection */
    client.println(); /* end of get request */
  }
  else
  {
    Serial.println("connection failed"); /* error message if no client */
    Serial.println();
  }

  while (client.connected() && !client.available()) delay(1); /* wait for data */
  while (client.connected() || !client.available()) /* connected or data available */
  {
    char c = client.read(); /* get byte from ethernet buffer */
    Serial.print(c); /* print byte to serial monitor */
  }

  Serial.println();
  Serial.println("disconnecting...");
  Serial.println("=====================");
  Serial.println();
  client.stop(); /* stop client */
}
