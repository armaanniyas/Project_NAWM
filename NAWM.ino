#include <Adafruit_CircuitPlayground.h>
#include <PulseSensorPlayground.h>
#define LATENCY 5

PulseSensorPlayground pulseSensor;

// Intialise variables

bool firstTime = true;
double lastx, lasty, lastz;
int movementCount = 0;
double totalChange = 0;
double prevTotalChange = 0;
bool isAsleep = false;
int lightSleepCount = 0;
int deepSleepCount = 0;
int REMSleepCount = 0;
int aboveRHR = 0;
int belowRHR = 0;

class user{
  private:
  String name;
  int age;
  String gender;
  bool calibrated;
  double RHR;
  double deepSleepThreshold;
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

    double getRHR(){
      return RHR;
    }

    double getDeepSleepThreshold(){
      return deepSleepThreshold;
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

      deepSleepThreshold = RHR * 0.8;
    }
    bool isCalibrated(){
      return calibrated;
    }
};

user testUser("Amy", 25,"F");

bool movement(double x, double y, double z) {
  if (firstTime) {
    lastx = x;
    lasty = y;
    lastz = z;
    firstTime = false; // Update firstTime here after initialization
    return false;
  } else {
    double xchange = abs(x - lastx);
    double ychange = abs(y - lasty);
    double zchange = abs(z - lastz);

    prevTotalChange = totalChange;

    totalChange = sqrt(pow(xchange, 2) + pow(ychange, 2) + pow(zchange, 2));

    if((totalChange > 8) && (prevTotalChange > 8)){
      movementCount = movementCount + 1;
      if(movementCount == 10){
        movementCount = 0;
        return true;
      }else{
        return false;
      }
    }else{
      movementCount = 0;
      return false;
    }

    // Update the last recorded values
    lastx = x;
    lasty = y;
    lastz = z;
  }
}

void sleepMetrics(){
  
}

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
  bool hasMovement = movement(CircuitPlayground.motionX(), CircuitPlayground.motionY(), CircuitPlayground.motionZ());

  if ((bpm > 0) && (bpm < 200)) {
    if(isAsleep){
      if(bpm < testUser.getDeepSleepThreshold()){
        deepSleepCount = deepSleepCount + 1;
        aboveRHR = 0;
      }else if(bpm < testUser.getRHR()){
        lightSleepCount = lightSleepCount + 1;
        aboveRHR = 0;
      }else if(!hasMovement){
        REMSleepCount = REMSleepCount + 1;
        aboveRHR = 0;
      }else{
        aboveRHR = aboveRHR + 1;
      }
      if(aboveRHR > LATENCY){
        isAsleep = false;
        Serial.println("Waking detected.");
        sleepMetrics();
        aboveRHR = 0;
      }
    }else{
      if (bpm < testUser.getRHR()){
      belowRHR = belowRHR + 1;
    }else{
      belowRHR = 0;
    }
    if((belowRHR > LATENCY) && (!hasMovement)){
      isAsleep = true;
      Serial.println("Sleep detected.");
      belowRHR = 0;
      lightSleepCount = LATENCY;
    }
    }
  }
  Serial.print("BPM: ");
  Serial.println(bpm);
  if(((lightSleepCount + deepSleepCount + REMSleepCount) != 0) && (isAsleep)){
    Serial.print("Light Sleep Percentage: ");
    Serial.println((double)lightSleepCount/(lightSleepCount + deepSleepCount + REMSleepCount));
    Serial.print("Deep Sleep Percentage: ");
    Serial.println((double)deepSleepCount/(lightSleepCount + deepSleepCount + REMSleepCount));
    Serial.print("REM Sleep Percentage: ");
    Serial.println((double)REMSleepCount/(lightSleepCount + deepSleepCount + REMSleepCount));
  }

  delay(1000);
}
