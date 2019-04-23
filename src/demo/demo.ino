#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#include <SPI.h>
#include <Ethernet.h>

static const int LAMP_HEAT_PIN =  3;
static const int FAN_01_PIN = 4;
static const int DHT_SENSOR_PIN = 5;
static const int SOIL_PIN = A0;
int moisture_val = 0;

DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

String STATUS_HEAT = "Heat OFF";
String STATUS_FAN = "Fan OFF";
String STATUS_SOIL = "Soil: Too Dry";

boolean moist_soil = false;

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
  float humidity;
<<<<<<< HEAD
  float moisture;

  temp_f = ((temperature * 9/5) + 32);
  moisture = analogRead(MT_01_PIN);
  moisture = map(moisture, 550, 0, 0, 100);
=======
>>>>>>> add parameters for soil sensor

  /* Measure temperature and humidity.  If true, measurement is available. */
  if (measure_environment(&temperature, &humidity) == true)
  {
    if (((temperature*9/5)+32) >= 90.0)
    {
      digitalWrite(FAN_01_PIN, LOW); /*turn fan on */
      STATUS_FAN = "Fan ON";
      Serial.println("Fan ON");
    }
    if (((temperature*9/5)+32) < 85.0)
    {
      digitalWrite(FAN_01_PIN, HIGH); /*turn fan off */
      STATUS_FAN = "Fan OFF";
      Serial.println("Fan OFF");
    }
    if (((temperature*9/5)+32) > 85.0)
    {
      digitalWrite(LAMP_HEAT_PIN, HIGH); /* heat lamp off */
      STATUS_HEAT = "Heat OFF";
      Serial.println("Heat OFF: Temp > 85 deg. F");
    }
    if (((temperature*9/5)+32) <= 80.0)
    {
      digitalWrite(LAMP_HEAT_PIN, LOW); /* heat lamp on */
      STATUS_HEAT = "Heat ON";
      Serial.println("Heat ON: Temp < 80 deg. F");
    }

    moisture_val = analogRead(SOIL_PIN);

    if (moisture_val > 350 && moisture_val < 700)
    {
      moist_soil = true;
      STATUS_SOIL = "Soil: Okay";
    }
    else if (moisture_val > 700)
    {
      moist_soil = false;
      STATUS_SOIL = "Soil: Too Dry";
    }
    else
    {
      moist_soil = false;
      STATUS_SOIL = "Soil: Too Much Water";
    }

    Serial.print("T = ");
    Serial.print(((temperature*9/5)+32), 1);
    Serial.print(" deg. F, H = ");
    Serial.print(humidity, 1);
    Serial.print("%, Moisture = ");
    Serial.println(moisture_val, 1);
    Serial.println(STATUS_SOIL);
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
          client.println("Refresh: 4");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          client.print("T = ");
          client.print(((temperature*9/5)+32), 1);
          client.print(" deg. F, H = ");
          client.print(humidity, 1);
          client.print("%, ");
          client.println(STATUS_SOIL);

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
