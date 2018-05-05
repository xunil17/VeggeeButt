#include <Servo.h>
#include <BasicLinearAlgebra.h>

using namespace BLA;

#define photo1 A8
#define laser1 23

#define photo2 A6
#define laser2 21

#define photo3 A4
#define laser3 19

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

#define vive1 14
#define vive2 15

typedef struct Point_ {
  float x;
  float y;
} Point;

typedef struct State_ {
  float x;
  float y;
  float heading;
} State;

float right;

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

void setup() {
  pinMode(laser1, OUTPUT);
  pinMode(photo1, INPUT);
  pinMode(laser2, OUTPUT);
  pinMode(photo2, INPUT);
  pinMode(laser3, OUTPUT);
  pinMode(photo3, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  pinMode(lmotor_pwm, OUTPUT);
  pinMode(lmotor_dir, OUTPUT);

  pinMode(rmotor_pwm, OUTPUT);
  pinMode(rmotor_dir, OUTPUT);

  pinMode(button_front1, INPUT);
  pinMode(button_front2, INPUT);

  pinMode(button_gripper1, INPUT);
  pinMode(button_gripper2, INPUT);


  gripper.attach(gripper_pwm);
  Serial.begin(9800);
}

void loop() {
  //  test_eric();
  test_sean();

}

void test_eric() { // test eric's stuff
  compute_transformed_coordinates({ -10, 0});
}


void test_sean() { // test sean's stuff

  right = scan();
  move_left_motor(50);
  move_right_motor(-50);
  if (right >= 100) {
    while ((digitalRead(button_front1) == LOW) || (digitalRead(button_front2) == LOW)) {
      move_left_motor(200);
      move_right_motor(200);
    }
    move_left_motor(0);
    move_right_motor(0);
    delay(3000);
  }

}
