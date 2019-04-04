#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

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

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
IPAddress ip(192, 168, 1, 75);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup()
{
  Serial.begin(9600);
  //ethernet setup begin
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  //ethernet setup end
  
  //greenbox setup begin
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
  //greenbox setup end
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
;
  float humidity;

  /* Measure temperature and humidity.  If true, measurement is available. */
  if (measure_environment(&temperature, &humidity) == true)
  {
    //if (temperature >= 24.0)
    if (((temperature*9/5)+32) >= 85.0)
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
    //Serial.print(temperature, 1);
    Serial.print(((temperature * 9/5) + 32), 1);
    //Serial.print(temp_f, 1);
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

  //ethernet test section begin
   // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          /*for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");  
          }*/
          client.print("T = ");
          client.print(((temperature * 9/5) + 32), 1);
          client.print(" deg. C, H = ");
          client.print(humidity, 1);
          client.println("%");
  
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  //ethernet test section end
}
