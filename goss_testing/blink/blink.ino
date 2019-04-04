#include <TimeLib.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
//    This works to set the time ...
    setTime(10,43,00,02,9,2019);
    
  pinMode(2, OUTPUT);
  Serial.println("setup complete.");

}

void loop() {
  // put your main code here, to run repeatedly:

  //digitalWrite(2, HIGH);
  Serial.println((String)"Hello "+hour()+' '+minute()+' '+second()+' '+"World");
  Serial.println((String)"HH:"+hour()+" MM:" +minute()+" SS:"+second());
  delay(5000);
  
  //digitalWrite(2, LOW);
  
  //delay(60000);

}
