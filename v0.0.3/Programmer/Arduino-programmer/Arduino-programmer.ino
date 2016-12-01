#include <SoftwareSerial.h>
const byte rxPin = 3;
const byte txPin = 2;

SoftwareSerial vSerial(rxPin, txPin);

void setup( ){
  Serial.begin(9600);
  vSerial.begin(9600);
}

void loop() {
  if( vSerial.available())
    Serial.write( vSerial.read());
  if( Serial.available())
    vSerial.write( Serial.read());
}
