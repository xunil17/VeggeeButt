#include <Servo.h>
#include <BasicLinearAlgebra.h>

// for vive
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define Vive1PIN 19 // the signal from the sensor
#define Vive2PIN 2 // the signal from the sensor
#define DEG_PER_US 0.0216 // equal to (180 deg) / (8333 us)
#define DEG_TO_RADIAN 0.0174533 // pi/180
#define LIGHTHOUSEHEIGHT 3.5 // in feet
#define LIGHTHOUSEANGLE 30 // in degrees

// structure to store the sensor data
typedef struct {
  unsigned long changeTime[11];
  double horzAng;
  double vertAng;
  int useMe;
  int collected;
} viveSensor;

// variables for the sensor data and filter
volatile viveSensor V1, V2;
double xPos1, yPos1, xPos2, yPos2;
double xOld1 = 0, yOld1 = 0, xFilt1 = 0, yFilt1 = 0;
double xOld2 = 0, yOld2 = 0, xFilt2 = 0, yFilt2 = 0;

using namespace BLA;

enum Block {
  Cube,
  Cylinder,
  None
};

#define photo1 A8
#define laser1 23

#define photo2 A6
#define laser2 21

#define photo3 A4
#define laser3 19

#define ir_pin A2

#define led 13

#define lmotor_pwm 9
#define lmotor_dir 3

#define rmotor_pwm 4
#define rmotor_dir 5

#define gripper_pwm 6
Servo gripper;

#define button_front1 2
#define button_front2 7
#define button_gripper1 8
#define button_gripper2 10

#define Vive1PIN 15
#define Vive2PIN 14

#define scan_threshold 100

typedef struct Point_ {
  float x;
  float y;
} Point;

typedef struct State_ {
  float x;
  float y;
  float heading;
} State;


//typedef struct state State;

// global instance representing Robot state
volatile State CurrState = {0, 0, 0};

void print_Point(Point p) {
  Serial.print("x: ");
  Serial.print(p.x);
  Serial.print(", y: ");
  Serial.println(p.y);
};

void print_CurrState() {
  Serial.println("STATE:");
  Serial.print("\t");
  Serial.print(CurrState.x);
  Serial.print(CurrState.y);
  Serial.print("\theading: ");
  Serial.println(CurrState.heading);
};

// directional notation is relative to facing the board
//   landscape when the BLUE circle is on the LEFT
// calibration globals:
const Point BL = { -83, -81};
const Point BR = {65, -68};
const Point TL = { -87, 65};
const Point TR = {56, 70};
const float val = 72;
const Point bl = { -val, -val};
const Point br = {val, -val};
const Point tl = { -val, val};
const Point tr = {val, val};


const float heading_threshold = 1.0; // room for error for correcting heading
float middle;

volatile bool hit = false;

void setup() {
  pinMode(laser1, OUTPUT);
  pinMode(photo1, INPUT);
  pinMode(laser2, OUTPUT);
  pinMode(photo2, INPUT);
  pinMode(laser3, OUTPUT);
  pinMode(photo3, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  pinMode(ir_pin, INPUT);

  pinMode(lmotor_pwm, OUTPUT);
  pinMode(lmotor_dir, OUTPUT);

  pinMode(rmotor_pwm, OUTPUT);
  pinMode(rmotor_dir, OUTPUT);

  pinMode(button_front1, INPUT_PULLUP);
  pinMode(button_front2, INPUT_PULLUP);

  pinMode(button_gripper1, INPUT);
  pinMode(button_gripper2, INPUT);


  attachInterrupt(digitalPinToInterrupt(button_front1), ISR_button, RISING);
  attachInterrupt(digitalPinToInterrupt(button_front2), ISR_button, RISING);

  gripper.attach(gripper_pwm);
//  open_gripper_max();

    // vive
  V1.horzAng = 0;
  V1.vertAng = 0;
  V1.useMe = 0;
  V1.collected = 0;
  pinMode(Vive1PIN, INPUT); // to read the sensor
  attachInterrupt(digitalPinToInterrupt(Vive1PIN), ISRVive1, CHANGE);
  V2.horzAng = 0;
  V2.vertAng = 0;
  V2.useMe = 0;
  V2.collected = 0;
  pinMode(Vive2PIN, INPUT); // to read the sensor
  attachInterrupt(digitalPinToInterrupt(Vive2PIN), ISRVive2, CHANGE);

  Serial.begin(9800);
}

void loop() {
  static int i = 0; // keep track of time
  char message[30];

//  Serial.println(V2.useMe);
  if (V1.useMe == 1) {
    V1.useMe = 0;
    // calculate the position and filter it
    xPos1 = tan((V1.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN) * LIGHTHOUSEHEIGHT;
    yPos1 = LIGHTHOUSEHEIGHT / cos((V1.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN)  * tan((V1.horzAng - 90.0) * DEG_TO_RADIAN);
    xFilt1 = xOld1 * 0.5 + xPos1 * 0.5; // filter
    yFilt1 = yOld1 * 0.5 + yPos1 * 0.5; // filter
    xOld1 = xFilt1; // remember for next loop
    yOld1 = yFilt1; // remember for next loop
//
//    Serial.print(xFilt1);
//    Serial.print(" ");
//    Serial.println(yFilt1);
  }

    if (V2.useMe == 1) {
    V2.useMe = 0;

    // calculate the position and filter it
    xPos2 = tan((V2.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN) * LIGHTHOUSEHEIGHT;
    yPos2 = LIGHTHOUSEHEIGHT / cos((V2.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN)  * tan((V2.horzAng - 90.0) * DEG_TO_RADIAN);
    xFilt2 = xOld2 * 0.5 + xPos2 * 0.5; // filter
    yFilt2 = yOld2 * 0.5 + yPos2 * 0.5; // filter
    xOld2 = xFilt2; // remember for next loop
    yOld2 = yFilt2; // remember for next loop

    Serial.print(xFilt2);
    Serial.print(" ");
    Serial.println(yFilt2);
  }
  delay(100);
//  plan();

//  scan();
}

void ISR_button() {
  stop_robot();
  hit = true;
}

void test_eric() { // test eric's stuff
  Serial.print("Cylinder, Cube, None: ");
  Serial.print(Cylinder);
  Serial.print(", ");
  Serial.print(Cube);
  Serial.print(", ");
  Serial.println(None);
  Serial.print("grab and identify: ");
  Serial.println(grab_and_identify());
}


void test_sean() { // test sean's stuff

//  right = scan();
//  move_left_motor(25);
//  move_right_motor(-25);
//  if (right >= 100) { //if it sees block
//    while ((digitalRead(button_front1) == LOW) && (digitalRead(button_front2) == LOW)) {
//      move_left_motor(150);
//      move_right_motor(150);
//    }
//    move_left_motor(0);
//    move_right_motor(0);
//    delay(3000);
//  }

}

// the sensor interrupts
void ISRVive1() {
  // get the time the interrupt occured
  unsigned long mic = micros();
  int i;

  // shift the time into the buffer
  for (i = 0; i < 10; i++) {
    V1.changeTime[i] = V1.changeTime[i + 1];
  }
  V1.changeTime[10] = mic;

  // if the buffer is full
  if (V1.collected < 11) {
    V1.collected++;
  }
  else {
    // if the times match the waveform pattern
    if ((V1.changeTime[1] - V1.changeTime[0] > 7000) && (V1.changeTime[3] - V1.changeTime[2] > 7000) && (V1.changeTime[6] - V1.changeTime[5] < 50) && (V1.changeTime[10] - V1.changeTime[9] < 50)) {
      V1.horzAng = (V1.changeTime[5] - V1.changeTime[4]) * DEG_PER_US;
      V1.vertAng = (V1.changeTime[9] - V1.changeTime[8]) * DEG_PER_US;
      V1.useMe = 1;
    }
  }
}
void ISRVive2() {
  // get the time the interrupt occured
  unsigned long mic = micros();
  int i;

  // shift the time into the buffer
  for (i = 0; i < 10; i++) {
    V2.changeTime[i] = V2.changeTime[i + 1];
  }
  V2.changeTime[10] = mic;

  // if the buffer is full
  if (V2.collected < 11) {
    V2.collected++;
  }
  else {
    // if the times match the waveform pattern
    if ((V2.changeTime[1] - V2.changeTime[0] > 7000) && (V2.changeTime[3] - V2.changeTime[2] > 7000) && (V2.changeTime[6] - V2.changeTime[5] < 50) && (V2.changeTime[10] - V2.changeTime[9] < 50)) {
      V2.horzAng = (V2.changeTime[5] - V2.changeTime[4]) * DEG_PER_US;
      V2.vertAng = (V2.changeTime[9] - V2.changeTime[8]) * DEG_PER_US;
      V2.useMe = 1;
    }
  }
}
