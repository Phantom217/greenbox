//#include <TimeLib.h>

//int temp_humidty_snsr =A0;
//int sensor_value=0;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//    This works to set the time ...
    //setTime(10,43,00,02,9,2019);
    
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
 
  Serial.println("setup complete.");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("high sent to pin 2");
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  //Serial.println((String)"Hello "+hour()+' '+minute()+' '+second()+' '+"World");
  //Serial.println((String)"HH:"+hour()+" MM:" +minute()+" SS:"+second());
  delay(10000);
  Serial.println("low sent to pin 2");
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  delay(10000);
  //sensor_value=analogRead(A0);
  //Serial.println("temp is "+sensor_value);

}
