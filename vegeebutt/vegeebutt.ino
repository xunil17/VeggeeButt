#include <Servo.h>

#define photo1 A8
#define laser1 23

#define photo2 A6
#define laser2 21

#define photo3 A4
#define laser3 19

#define led 13

#define lmotor_pwm 2
#define lmotor_dir 3

#define rmotor_pwm 4
#define rmotor_dir 5

#define gripper_pwm 6
Servo gripper;

#define button1 32
#define button2 31
#define button3 30
#define button4 29

typedef struct Loc_ {
  float x;
  float y;
} Loc; // Location

void printLoc(Loc loc) {
  Serial.print("x: ");
  Serial.print(loc.x);
  Serial.print(", y: ");
  Serial.println(loc.y);
};

typedef struct State_ {
  Loc location;
  float heading;
} State;

//typedef struct state State;

// global instance representing Robot state
State VState = {{0, 0}, 79};

void printVState() {
  Serial.println("STATE:");
  Serial.print("\t");
  printLoc(VState.location);
  Serial.print("\theading: ");
  Serial.println(VState.heading);
};

// directional notation is relative to facing the board
//   landscape when the BLUE circle is on the LEFT
// calibration globals:
const Loc bottom_left = {0,0};
const Loc bottom_right = {0,0};
const Loc top_left = {0,0};
const Loc top_right = {0,0};
const float heading_threshold = 1.0; // room for error for correcting heading

float middle;

void setup() {
  pinMode(laser1, OUTPUT);
  pinMode(photo1, INPUT);
  pinMode(laser2, OUTPUT);
  pinMode(photo2, INPUT);
  pinMode(laser3, OUTPUT);
  pinMode(photo3, INPUT);
  pinMode(led, OUTPUT);

  pinMode(lmotor_pwm, OUTPUT);
  pinMode(lmotor_dir, OUTPUT);

  pinMode(rmotor_pwm, OUTPUT);
  pinMode(rmotor_dir, OUTPUT);

  gripper.attach(gripper_pwm);
  Serial.begin(9800);
}

void loop() {
//  test_eric();
//  test_sean();
} 

void test_eric() { // test eric's stuff
//  printVState();
  for(int i = 0; i < 360; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(get_heading_difference(i));
  }
}

void test_sean() { // test sean's stuff
  middle = scan();
  gripper.write(100);
  left(250);
  right(250);
  
//  middle = scan();
//  if(middle >= 30) {
//    left(0);
//    right(0);
//  }
//  else {
//    left(20);
//    right(-20);
//  }
}

void left(int vel) {
  if (vel > 0) {
    analogWrite(lmotor_pwm, vel);
    digitalWrite(lmotor_dir, LOW);
  } else {
    analogWrite(lmotor_pwm, -vel);
    digitalWrite(lmotor_dir, HIGH);
  }
}

void right(int vel) {
  if (vel > 0) {
    analogWrite(rmotor_pwm, vel);
    digitalWrite(rmotor_dir, LOW);
  } else {
    analogWrite(rmotor_pwm, -vel);
    digitalWrite(rmotor_dir, HIGH);
  }
}

float scan() {

  int high[3];
  int low[3];
  float average[3];
  average[0] = 0;
  average[1] = 0;
  average[2] = 0;
  for (int i = 0; i < 5; i++) {
    digitalWrite(laser1, HIGH);
    digitalWrite(laser2, HIGH);
    digitalWrite(laser3, HIGH);
    delay(30);
    high[0] = analogRead(photo1);
    high[1] = analogRead(photo2);
    high[2] = analogRead(photo3);
    digitalWrite(laser1, LOW);
    digitalWrite(laser2, LOW);
    digitalWrite(laser3, LOW);
    delay(30);
    low[0] = analogRead(photo1);
    low[1] = analogRead(photo2);
    low[2] = analogRead(photo3);
    //    Serial.print(high);
    //    Serial.print(" ");
    //    Serial.println(low);
    average[0] += (high[0] - low[0]);
    average[1] += (high[1] - low[1]);
    average[2] += (high[2] - low[2]);
  }
  average[0] = average[0] / 5.0;
  average[1] = average[1] / 5.0;
  average[2] = average[2] / 5.0;
  Serial.print(average[0]);
  Serial.print(' ');
  Serial.print(average[1]);
  Serial.print(' ');
  Serial.println(average[2]);
//  Serial.print(' ');
  
  if (average[0] >= 100) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
  return average[1];
}

