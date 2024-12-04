#include <Adafruit_CircuitPlayground.h>
#include <PulseSensorPlayground.h>

PulseSensorPlayground pulseSensor;

class user{
  private:
  String name;
  int age;
  String gender;
  bool calibrated;
  int RHR;
  public:
    user(String n, int a, String g){
      name = n;
      age = a;
      gender = g;
      calibrated = false;
    }

    String getName(){
      return name;
    }

    void calibrate(){
      Serial.println("Please enter your Resting Heart Rate (RHR):");
      while (Serial.available() == 0) {
        // Wait for user input
      }

      RHR = Serial.parseInt();
      calibrated = true;  // Mark the user as calibrated
      Serial.print("Your Resting Heart Rate is: ");
      Serial.println(RHR);
      calibrated = true;
    }
    bool isCalibrated(){
      return calibrated;
    }
};

user testUser("Amy", 25,"F");

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  pulseSensor.analogInput(9);
  pulseSensor.setThreshold(500);
  pulseSensor.begin();

  if (pulseSensor.begin()) {
    Serial.println("Pulse Sensor Initialized Successfully");
  } else {
    Serial.println("Pulse Sensor Initialization Failed");
  }
};

void loop() {

  if(testUser.isCalibrated() == false){
    testUser.calibrate();
    Serial.print("Calibration complete. Welcome to NAWM, ");
    Serial.print(testUser.getName());
    Serial.println("!");
  }
  
  double bpm = pulseSensor.getBeatsPerMinute();

  if (bpm > 0) {
    Serial.print("BPM: ");
    Serial.println(bpm);
  } else {
    Serial.println("No beat detected");
  }

  delay(1000);
}
