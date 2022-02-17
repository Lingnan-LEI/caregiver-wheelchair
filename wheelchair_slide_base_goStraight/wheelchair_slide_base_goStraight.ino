
#include <Sabertooth.h>

#define USBCON
#define NUM_SENSORS 2
#define NUM_READINGS 10
#define CALIBRATION_TIME 500
#define SENSOR_INTERVAL 50
#define DEADBAND_MIN 465 //275
#define DEADBAND_MAX 615 //425
#define SLIDE_MIN 0
#define SLIDE_MAX 900
#define ST_INPUT_MIN -60 //-30
#define ST_INPUT_MAX 127 //95
#define ST_INPUT_MID 0
#define TURN_THRESH 50//65


int currentTime;
int slideSensorPin[NUM_SENSORS] = {A0, A1};
int sensorSmoothed[NUM_SENSORS];
int sensorCal[NUM_SENSORS];
int sensorTotal[NUM_SENSORS];
int sensorReadings[NUM_SENSORS][NUM_READINGS];
int readIndex = 0;
int throttle = 0;

Sabertooth ST(128, Serial1);

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  //SabertoothTXPinSerial.begin(9600);
  ST.autobaud();

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(slideSensorPin[i], INPUT);
  }
  int startupTime = millis();
  int currentTime = millis();
  while (currentTime - startupTime < CALIBRATION_TIME) {
    if (millis() - currentTime > SENSOR_INTERVAL) {
      currentTime = millis();
      readSensors();
    }
  }
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorCal[i] = sensorSmoothed[i];
  }

}

void loop() {
  if (millis() - currentTime > SENSOR_INTERVAL) {
    currentTime = millis();
    readSensors();
  }

  if (abs(sensorSmoothed[NUM_SENSORS - 1] - sensorSmoothed[0]) < TURN_THRESH)  {

    int sensorSum = 0;
    for (int i = 0; i < NUM_SENSORS; i++) {
      sensorSum = sensorSum + sensorSmoothed[i];
    }
    int sensorAverage = sensorSum / NUM_SENSORS;

    mapThrottle(sensorAverage);
    for (int i = 0; i < NUM_SENSORS; i++) {
      ST.motor(i + 1, throttle);
      //Serial.print(throttle);
      //Serial.print(", ");
    }
    //Serial.println();
  }

  else {

    for (int i = 0; i < NUM_SENSORS; i++) {
      mapThrottle(sensorSmoothed[i]);
      ST.motor(i + 1, throttle);
      //Serial.print(throttle);
      //Serial.print(", ");
    }
    //Serial.println();
  }
}

void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorTotal[i] = sensorTotal[i] - sensorReadings[i][readIndex];
    sensorReadings[i][readIndex] = analogRead(slideSensorPin[i]);
    delay(10);
    sensorReadings[i][readIndex] = analogRead(slideSensorPin[i]);
    sensorTotal[i] = sensorTotal[i] + sensorReadings[i][readIndex];
    sensorSmoothed[i] = sensorTotal[i] / NUM_READINGS; //Calculate new average
    Serial.print(sensorSmoothed[0]);
    Serial.print(sensorSmoothed[1]);
    Serial.print("  ");
    Serial.println();
  }
  readIndex = readIndex + 1;
  if (readIndex >= NUM_READINGS) {
    readIndex = 0;
  }
}

int mapThrottle(int reading) {

  if (reading == 0) {
    throttle = 0;
  }
  else if (reading > DEADBAND_MIN && reading < DEADBAND_MAX) {
    throttle = 0;
    
  }
  else if (reading < DEADBAND_MIN) {
    throttle = map(reading, SLIDE_MIN, DEADBAND_MIN, ST_INPUT_MIN, ST_INPUT_MID);
  }
  else if (reading > DEADBAND_MAX) {
    throttle = map(reading, DEADBAND_MAX, SLIDE_MAX , ST_INPUT_MID, ST_INPUT_MAX);
    
  }
  return throttle;
}
