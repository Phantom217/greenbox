#include <dht.h>
static const int MT_01_PIN = A0;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    float moisture;
    moisture = analogRead(MT_01_PIN);
    moisture = map(moisture, 550, 0, 0, 100);

    Serial.print("Moisture : ");
    Serial.print(moisture);
    Serial.println("%");
    delay(1000);
}
