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

void print_Loc(Loc loc) {
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
State CurrState = {{0, 0}, 79};

void print_CurrState() {
  Serial.println("STATE:");
  Serial.print("\t");
  print_Loc(CurrState.location);
  Serial.print("\theading: ");
  Serial.println(CurrState.heading);
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
//  printCurrState();
  for(int i = 0; i < 360; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(get_heading_difference(i));
  }
}

void test_sean() { // test sean's stuff
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
