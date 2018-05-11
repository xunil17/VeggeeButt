#include <Servo.h>
#include <BasicLinearAlgebra.h>

// for vive
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define DEG_PER_US 0.0216 // equal to (180 deg) / (8333 us)
#define DEG_TO_RADIAN 0.0174533 // pi/180
#define LIGHTHOUSEHEIGHT 3.5 // in feet
#define LIGHTHOUSEANGLE 33 // in degrees

// structure to store the sensor data
typedef struct {
  unsigned long changeTime[11];
  float horzAng;
  float vertAng;
  int useMe;
  int collected;
} viveSensor;

// variables for the sensor data and filter
volatile viveSensor V1, V2;
volatile float xPos1, yPos1, xPos2, yPos2;
volatile float xOld1 = 0, yOld1 = 0, xFilt1 = 0, yFilt1 = 0;
volatile float xOld2 = 0, yOld2 = 0, xFilt2 = 0, yFilt2 = 0;

using namespace BLA;

//enum Plan {
//  LookForBlock,
//  GoToBlock,
//  GrabAndIdentifyBlock,
//  LookForGoal,
//  GoToGoal,
//  DropBlock,
//  LookForReset,
//  GoToReset
//};

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

enum Block {
  Cube,
  Cylinder,
  None
};

typedef struct Point_ {
  float x;
  float y;
} Point;

typedef struct State_ {
  float x;
  float y;
  float heading;
  Block holding;
} State;


//typedef struct state State;

// global instance representing Robot state
volatile State CurrState = {0, 0, 0, None};

void print_Point(Point p) {
  Serial.print("x: ");
  Serial.print(p.x);
  Serial.print(", y: ");
  Serial.println(p.y);
};

void print_CurrState() {
  Serial.print("CurrState:");
  Serial.print("x,y,heading,holding: ");
  Serial.print(CurrState.x);
  Serial.print(", ");
  Serial.print(CurrState.y);
  Serial.print(", ");
  Serial.print(CurrState.heading);
  Serial.print(", ");
  Serial.println(Block_to_string(CurrState.holding));
};

String Block_to_string(Block b) {
  if (b == Cylinder) {
    return "Cylinder";
  } else if (b == Cube) {
    return "Cube";
  } else {
    return "None";
  }
}

const Block TeamType = Cylinder;

// directional notation is relative to facing the board
//   landscape when the BLUE circle is on the LEFT
// calibration globals:
const Point BLCAL = { 0.84, -4.75};
const Point BRCAL = { 15.98, -3.3};
const Point TLCAL = { 0.72, 4.74 };
const Point TRCAL = { 15.30, 4.23 };

const Point BL = { 0, 0 };
const Point BR = { BRCAL.x - BLCAL.x, BRCAL.y - BLCAL.y };
const Point TL = { TLCAL.x - BLCAL.x, TLCAL.y - BLCAL.y };
const Point TR = { TRCAL.x - BLCAL.x, TRCAL.y - BLCAL.y };
const float valx = 12;
const float valy = 6;
const Point bl = { 0, 0 };
const Point br = { valx, 0 };
const Point tl = { 0, valy};
const Point tr = { valx, valy };

const BLA::Matrix<8,8> A= {
  bl.x, bl.y, 1, 0, 0, 0, -BL.x * bl.x, -BL.x * bl.y,
  0, 0, 0, bl.x, bl.y, 1, -BL.y * bl.x, -BL.y * bl.y,
  br.x, br.y, 1, 0, 0, 0, -BR.x * br.x, -BR.x * br.y,
  0, 0, 0, br.x, br.y, 1, -BR.y * br.x, -BR.y * br.y,
  tl.x, tl.y, 1, 0, 0, 0, -TL.x * tl.x, -TL.x * tl.y,
  0, 0, 0, tl.x, tl.y, 1, -TL.y * tl.x, -TL.y * tl.y,
  tr.x, tr.y, 1, 0, 0, 0, -TR.x * tr.x, -TR.x * tr.y,
  0, 0, 0, tr.x, tr.y, 1, -TR.y * tr.x, -TR.y * tr.y
};

const BLA::Matrix<8,1> B = {
  BL.x,
  BL.y,
  BR.x,
  BR.y,
  TL.x,
  TL.y,
  TR.x,
  TR.y
};

const float heading_threshold = 1.0; // room for error for correcting heading
float middle;

volatile bool hit = false;

const Point Goal1 = {9.67, -0.05};

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
  open_gripper_max();

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
  test_eric();
//  test_sean();
}

void ISR_button() {
  stop_robot();
  hit = true;
}

void calibrate_routine() {
  update_vive();
}

void test_eric() { 
//  plan();

//  update_vive();
//  print_Point(read_front());
  Serial.println(Block_to_string(grab_and_identify()));
//  back_up();
//  delay(5000);
  delay(100);
}


void test_sean() {
  if(!gripper_gripped() && get_dist() < 290) {
    gripper.write(98);
  } else {
    open_gripper_max();
    delay(5000);
  }
  Serial.print(digitalRead(button_gripper1));
  Serial.println(digitalRead(button_gripper2));

}


