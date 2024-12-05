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
int interval = 0;
int prevInterval = 0;
int sum = 0;

class user{
  private:
  String name;
  int age;
  String gender;
  bool calibrated;
  double RHR;
  double deepSleepThreshold;
  double sleepTarget;

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

    double getSleepTarget(){
      return sleepTarget;
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

      deepSleepThreshold = RHR * 0.8;

       Serial.println("What is your sleep target (in hours):");
      while (Serial.available() == 0) {
        // Wait for user input
      }

      sleepTarget = Serial.parseInt();
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
      if(movementCount == 1){
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

double calculateHRV(int sum, int count){
  return sqrt(sum / (count-1));
    }

void sleepMetrics(int light, int deep, int REM, double HRV){
  double sleepDuration = (light + deep + REM) / (60*60);
  Serial.print("Good morning ");
  Serial.print(testUser.getName());
  Serial.println("!");
  Serial.print("Last night, you slept for");
  Serial.print(sleepDuration);
  Serial.print("hours.");
  if (sleepDuration < testUser.getSleepTarget()){
    Serial.println("You may feel tired today as you did not hit your sleep target.");
    Serial.print("Sleep debt:");
    Serial.println(sleepDuration - testUser.getSleepTarget());
  }else{
    Serial.print("Well done for hitting your sleep target.");
  }
  Serial.println("----------------------");
  Serial.println("Sleep Stages Breakdown:");
  Serial.print("Light sleep: ");
  Serial.println((double)light/(light + deep + REM)*100);
  Serial.print("Deep sleep: ");
  Serial.println((double)deep/(light + deep + REM)*100);
  Serial.print("REM sleep: ");
  Serial.println((double)REM/(light + deep + REM)*100);
  Serial.println("----------------------");
  Serial.print("Heart rate variability (HRV): ");
  Serial.print(HRV);
  Serial.println("----------------------");
  Serial.println("Analysis: ");
  if((double)REM/(light + deep + REM) < 0.2){
    Serial.println("Your REM sleep is low. We encourage you to avoid caffiene and meals before bed and try a relaxing bedtime routine to help you wind down and maximise restorative sleep.");
  }else{
    Serial.println("You hit optimal REM targets! You should feel physically and mentally ready and pumped for the day ahead.");
  }
  if(HRV >= 60){
    Serial.println("Your HRV is above the baseline. Great work. This indicates better fitness and overall health.");
  }else if(HRV < 35){
    Serial.println("Your HRV is low. This may be a sign of overtraining, lack of sleep, dehydration or stress. Ensure you are getting good sleep, drinking fluids and avoid burnouts. If you need support, speak to a friend.");
  }
  double standardizedHRV = (HRV - 15) / (100 - 15) *100;
  double standardizedLight = 100 - abs(light*100 - 50);
  double standardizedDeep = 100 - 2*(abs(deep*100 - 25));
  double standardizedREM = 100 - 2*(abs(REM*100 - 25));
  double standardizedDuration = 100 - 15 *(abs(sleepDuration - testUser.getSleepTarget()));
  double sleepScore = 0.3*standardizedHRV + 0.2*standardizedLight + 0.2*standardizedDeep + 0.2*standardizedREM + 0.1*standardizedDuration;

  Serial.print("Sleep Performance Score: ");
  Serial.println(sleepScore);

  if(sleepScore < 70){
    Serial.println("Your sleep quality is poor. Work on hitting optimum sleep by improving your sleep habits.");
  }else if(sleepScore >= 90){
    Serial.println("You are acing your sleep. Keep it up.");
  }else{
    Serial.println("You are getting sufficient sleep. Try to hit 100% of your sleep needs for peak performance.");
  }
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
      if (pulseSensor.sawStartOfBeat()) {
        prevInterval = interval;
        interval = pulseSensor.getInterBeatIntervalMs();
        sum = sum + pow(abs(interval - prevInterval), 2);
        }
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
        double HRV = calculateHRV(sum, lightSleepCount + deepSleepCount + REMSleepCount);
        aboveRHR = 0;

        sleepMetrics(lightSleepCount, deepSleepCount, REMSleepCount, HRV);
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
    Serial.println(hasMovement);
    Serial.println(aboveRHR);
  }

  delay(1000);
}
