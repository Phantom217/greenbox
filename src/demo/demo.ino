#include <dht.h>
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#include <SPI.h>
#include <Ethernet.h>

static const int LAMP_HEAT_PIN =  3;
static const int FAN_01_PIN = 4;
static const int DHT_SENSOR_PIN = 5;
static const int MT_01_PIN = A0;
DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
String STATUS_HEAT = "Heat OFF";
String STATUS_FAN = "Fan OFF";

byte mac[] = { 0x00, 0xAA, 0xBB,0xCC, 0xDE, 0x02 };

IPAddress ip(192,168,1,75);

EthernetServer server(80);

void setup()
{
  Serial.begin(9600);

  while (!Serial)
  {
    ;
  }

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server located at: ");
  Serial.println(Ethernet.localIP());

  pinMode(LAMP_HEAT_PIN, OUTPUT);
  pinMode(FAN_01_PIN, OUTPUT);

  digitalWrite(LAMP_HEAT_PIN, HIGH); /* turns the lamp off */
  digitalWrite(FAN_01_PIN, HIGH); /* same case as the above */
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
  float moisture;

  temp_f = ((temperature * 9/5) + 32);
  moisture = analogRead(MT_01_PIN);
  moisture = map(moisture, 550, 0, 0, 100);

  /* Measure temperature and humidity.  If true, measurement is available. */
  if (measure_environment(&temperature, &humidity) == true)
  {
    if (temperature >= 32.0)
    {
      digitalWrite(FAN_01_PIN, LOW); /*turn fan on */
      STATUS_FAN = "Fan ON";
      Serial.println("Fan ON");
    }
    if (temperature < 29.4)
    {
      digitalWrite(FAN_01_PIN, HIGH); /*turn fan off */
      STATUS_FAN = "Fan OFF";
      Serial.println("Fan OFF");
    }
    if (temperature > 29.4)
    {
      digitalWrite(LAMP_HEAT_PIN, HIGH); /* heat lamp off */
      STATUS_HEAT = "Heat OFF";
      Serial.println("Heat OFF: Temp > 85 deg. F");
    }
    if (temperature <= 26.6)
    {
      digitalWrite(LAMP_HEAT_PIN, LOW); /* heat lamp on */
      STATUS_HEAT = "Heat ON";
      Serial.println("Heat ON: Temp < 80 deg. F");
    }

    Serial.print("T = ");
    Serial.print(temp_f, 1);
    Serial.print(" deg. F, H = ");
    Serial.print(humidity, 1);
    Serial.print("%, Moisture = ");
    Serial.print(moisture, 1);
    Serial.println("%");
  }

 EthernetClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          client.print("T = ");
          client.print(temp_f, 1);
          client.print(" deg. F, H = ");
          client.print(humidity, 1);
          client.print("%, Moisture = ");
          client.print(moisture, 1);
          client.println("%");

          client.println("<br>");
          client.print(STATUS_HEAT);
          client.print("----");
          client.print(STATUS_FAN);

          client.println("</html>");
          break;
        }
        if (c == '\n')
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disconnected");
  }
}
