#include "HX711.h"
#include <Sabertooth.h>

const int DOUT=8;
const int CLK=9;
const int POT=A0;

const double STRAIN_DEADBAND_MIN = -20000;
const double STRAIN_DEADBAND_MAX = 20000;

const double POT_DEADBAND_MIN = 500;
const double POT_DEADBAND_MAX = 600 ;

const double STRAIN_MIN = -100000;
const double STRAIN_MAX = 100000;

const double POT_MIN = 0;
const double POT_MAX = 1024;

const double ST_INPUT_MIN = -35;
const double ST_INPUT_MID = 0;
const double ST_INPUT_MAX = 80;

const double TURN_MIN = -35;
const double TURN_MID = 0;
const double TURN_MAX = 80;


HX711 forward_scale;
Sabertooth ST(128, Serial);

void setup() {
  Serial.begin(9600);
  //Serial1.begin(9600);
  //ST.autobaud();
  forward_scale.begin(DOUT, CLK);
  forward_scale.tare();
  

}

void loop() {
  
  double forward_speed;
  double turn_speed;
  double forward_com = forward_scale.get_value();
  double turn_com = analogRead(POT);
  delay(10);
  turn_com = analogRead(POT);

  if (forward_com > STRAIN_DEADBAND_MIN && forward_com < STRAIN_DEADBAND_MAX) {
    forward_speed = 0;
  }
  else if (forward_com <= STRAIN_DEADBAND_MIN) {
    forward_speed = map(forward_com, STRAIN_MIN, STRAIN_DEADBAND_MIN, ST_INPUT_MIN, ST_INPUT_MID);
  }
  else if (forward_com >= STRAIN_DEADBAND_MAX) {
   forward_speed = map(forward_com, STRAIN_DEADBAND_MAX, STRAIN_MAX , ST_INPUT_MID, ST_INPUT_MAX);
  }
  if (turn_com > POT_DEADBAND_MIN && turn_com < POT_DEADBAND_MAX) {
    turn_speed = 0;
    
  }
  else if (turn_com <= POT_DEADBAND_MIN) {
    turn_speed = map(turn_com, POT_MIN, POT_DEADBAND_MIN, TURN_MIN, TURN_MID);
    
  }
  else if (turn_com >= POT_DEADBAND_MAX) {
   turn_speed = map(turn_com, POT_DEADBAND_MAX, POT_MAX , TURN_MID, TURN_MAX);
   
  }  
  double left_mot = forward_speed;
  double right_mot = forward_speed;

  if (forward_speed >= 0 && turn_speed > 0) {
    left_mot = left_mot + turn_speed;
  
  }
  else if (forward_speed >= 0 && turn_speed < 0) {
    right_mot = right_mot - turn_speed;
   
  }
  else if (forward_speed < 0 && turn_speed > 0) {
    left_mot = left_mot - turn_speed;

  }
  else if (forward_speed < 0 && turn_speed < 0) {
    right_mot = right_mot + turn_speed;

  }
  //ST.motor(LEFT, left_mot);
  //ST.motor(RIGHT, right_mot);
  Serial.print(left_mot);
  Serial.print(", ");
  Serial.println(right_mot);
  delay(50);

}
