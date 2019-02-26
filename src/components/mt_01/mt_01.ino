#include <dht.h>
static const int MT_01_PIN = A0;

void setup()
{
    Serial.begin(9600);
}

static bool measure_soil_moisture(float moisture)
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

    Serial.print("Moisture : ");
    Serial.print(moisture);
    Serial.println("%");
    delay(1000);
}
