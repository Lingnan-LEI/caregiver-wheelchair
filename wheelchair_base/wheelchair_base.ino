#include <Servo.h>

#define NUM_SENSORS 4
#define NUM_READINGS 10
#define CALIBRATION_TIME 500
#define SENSOR_INTERVAL 50
#define L_MOTOR_PIN 8
#define R_MOTOR_PIN 9
#define PWM_MIN 1000
#define PWM_MID 1500
#define PWM_MAX 2000
#define HIST_GAP 5

int currentTime;
int hallSensorPin[NUM_SENSORS] = {A0, A1, A2, A3};
int sensorAverage[NUM_SENSORS];
int sensorCal[NUM_SENSORS];
int sensorTotal[NUM_SENSORS];
int sensorReadings[NUM_SENSORS][NUM_READINGS];
int readIndex = 0;

Servo leftMotor;
Servo rightMotor;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(hallSensorPin[i], INPUT_PULLUP);
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
    sensorCal[i] = sensorAverage[i];
    Serial.println(sensorCal[i]);
  }

  leftMotor.attach(L_MOTOR_PIN);
  rightMotor.attach(R_MOTOR_PIN);

}

void loop() {
  if (millis() - currentTime > SENSOR_INTERVAL) {
    currentTime = millis();
    readSensors();

    if (sensorAverage[1] - sensorAverage[0] >= HIST_GAP) {
      int leftMotorRead = map(sensorAverage[1], 0, 1024, PWM_MID, PWM_MAX);
      leftMotor.writeMicroseconds(leftMotorRead);
      Serial.print(leftMotorRead);
      Serial.print(", ");
      
    }
    else if (sensorAverage[0] - sensorAverage[1] >= HIST_GAP) {
      int leftMotorRead = map(sensorAverage[0], 0, 1024, PWM_MID, PWM_MIN);
      leftMotor.writeMicroseconds(leftMotorRead);
      Serial.print(leftMotorRead);
      Serial.print(", ");
    }

    else {
      leftMotor.writeMicroseconds(PWM_MID);
      Serial.print(PWM_MID);
      Serial.print(", ");
      }
    
    if (sensorAverage[3] - sensorAverage[2] >= HIST_GAP) {
      int rightMotorRead = map(sensorAverage[3], 0, 1024, PWM_MID, PWM_MAX);
      rightMotor.writeMicroseconds(rightMotorRead);
      Serial.print(rightMotorRead);
      Serial.println();
    }
    else if (sensorAverage[2] - sensorAverage[3] >= HIST_GAP) {
      int rightMotorRead = map(sensorAverage[2], 0, 1024, PWM_MID, PWM_MIN);
      rightMotor.writeMicroseconds(rightMotorRead);
      Serial.print(rightMotorRead);
      Serial.println();
    }

    else {
      rightMotor.writeMicroseconds(PWM_MID);
      Serial.print(PWM_MID);
      Serial.println();
      }
    
  }
  delay(250);
}

void readSensors() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorTotal[i] = sensorTotal[i] - sensorReadings[i][readIndex];
    sensorReadings[i][readIndex] = analogRead(hallSensorPin[i]);
    sensorTotal[i] = sensorTotal[i] + sensorReadings[i][readIndex];
    sensorAverage[i] = (sensorTotal[i] / NUM_READINGS) - sensorCal[i];
    //Serial.print(sensorAverage[i]);
    //Serial.print(" , ");
  }
  readIndex = readIndex + 1;
  if (readIndex >= NUM_READINGS) {
    readIndex = 0;
  }
  //Serial.println();
}
