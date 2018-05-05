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

//#define button1 32
//#define button2 31
//#define button3 30
//#define button4 29

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
volatile State CurrState = {0,0,0};

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
const Point BL = {-83,-81};
const Point BR = {65,-68};
const Point TL = {-87,65};
const Point TR = {56,70};
const float val = 72;
const Point bl = {-val, -val};
const Point br = {val, -val};
const Point tl = {-val, val};
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


  gripper.attach(gripper_pwm);
  Serial.begin(9800);
}

void loop() {
//  test_eric();
 test_sean();
} 

void test_eric() { // test eric's stuff
<<<<<<< HEAD

//  for(int i = 0; i < 360; i++) {
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.println(get_heading_difference(i));
//  }
//  compute_vive_transformation();
=======
  compute_transformed_coordinates({-10, 0});
>>>>>>> 0f55afd0cb657b1cb27d688b1fd8ecb9b557f21b
}


void test_sean() { // test sean's stuff

  move_left_motor(-200);
  move_right_motor(-200);
//  middle = scan();
//  gripper.write(100);
//  left(250);
//  right(250);
  
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
