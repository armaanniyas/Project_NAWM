#include <Adafruit_CircuitPlayground.h>

int soundCount = 0, lightCount = 0, lowTempCount = 0, highTempCount = 0;

bool soundCheck(){
  CircuitPlayground.mic.soundPressureLevel(10) > 60.0 ? true : false;
}

bool lightCheck(){
  CircuitPlayground.lightSensor() > 10.0 ? true : false;
}

bool lowTempCheck(){
  CircuitPlayground.temperature() < 18.0 ? true : false;
}

bool highTempCheck(){
  CircuitPlayground.temperature() > 25.0 ? true : false;
}

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize Circuit Playground
  CircuitPlayground.begin();
}

void loop() {

  if(soundCheck) soundCount++;
  if(lightCheck) lightCount++;
  if(lowTempCheck) lowTempCount++;
  if(highTempCheck) highTempCount++;
  
  if(soundCount > 10) Serial.println("Your sleeping environment is too loud!");
  if(lightCount > 10) Serial.println("Your sleeping environment is too bright!");
  if(lowTempCount > 10) Serial.println("Your sleeping environment is too cold!");
  if(highTempCount > 10) Serial.println("Your sleeping environment is too hot!");
  

  
  delay(1000);
}