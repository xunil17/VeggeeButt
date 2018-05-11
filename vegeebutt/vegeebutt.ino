#include <Servo.h>

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

enum Mode {
  Run,
  Calibrate,
  Test
};

enum Direction {
  Left,
  Right
};

typedef struct Point_ {
  float x;
  float y;
} Point;

bool operator==(const Point& p1, const Point& p2) {
  return p1.x == p2.x && p1.y == p2.y;
}

typedef struct State_ {
  float x;
  float y;
  float heading;
  Direction dir;
  Block holding;
  bool resetting;
} State;

// global instance representing Robot state
volatile State CurrState = {0, 0, 0, Right, None, false};

void print_Point(Point p) {
  Serial.print("x: ");
  Serial.print(p.x);
  Serial.print(", y: ");
  Serial.println(p.y);
};

void print_CurrState() {
  Serial.print("CurrState ");
  Serial.print("(x,y,heading,direction,holding,lifetime): ");
  Serial.print(CurrState.x);
  Serial.print(", ");
  Serial.print(CurrState.y);
  Serial.print(", ");
  Serial.print(CurrState.heading);
  Serial.print(", ");
  Serial.print(Direction_to_string(CurrState.dir));
  Serial.print(", ");
  Serial.print(Block_to_string(CurrState.holding));
  Serial.print(", ");
  Serial.println(CurrState.resetting);
};

String Block_to_string(Block b) {
  if (b == Cylinder) {
    return "Cylinder";
  } else if (b == Cube) {
    return "Cube";
  } else {
    return "None";
  }
};

String Direction_to_string(Direction dir) {
  if (dir == Left) {
    return "Left";
  } else {
    return "Right";
  }
};

const Block TeamType = Cube;

// directional notation is relative to facing the board
//   landscape when the center BLUE circle is on the LEFT
// calibration globals:
const Point BLCAL = { 1.33, -2.31};
const Point BRCAL = { 15, -2.5};
const Point TLCAL = { 1.36, 2.89 };
const Point TRCAL = { 14.1, 2.9 };

// dumpsters
const Point DBOT = { (BLCAL.x + BRCAL.x) / 2, BLCAL.y > BRCAL.y ? BLCAL.y : BRCAL.y };
const Point DTOP = { (TLCAL.x + TRCAL.x) / 2, TLCAL.y < TRCAL.y ? TLCAL.y : TRCAL.y };

// centers
//const Point CL =
//const Point CR =

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


  attachInterrupt(digitalPinToInterrupt(button_front1), ISR_button_1, RISING);
  attachInterrupt(digitalPinToInterrupt(button_front2), ISR_button_2, RISING);

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

const Mode mode = Test;

void loop() {
  if (mode == Calibrate) {
    calibrate_routine();
    delay(100);
  } else if (mode == Test) {
    update_vive();
    test();
    delay(100);
  } else {
    go();
  }
}

void ISR_button_1() {
  delayMicroseconds(10000);
  if(digitalRead(button_front1) == HIGH) {
    stop_robot();
    hit = true;
  } else {
    hit = false;
  }

}

void ISR_button_2() {
  delayMicroseconds(10000);
  if(digitalRead(button_front2) == HIGH) {
    stop_robot();
    hit = true;
  } else {
    hit = false;
  }
}

void calibrate_routine() {
  update_vive();
  print_Point(read_front());
}

void go() {
  plan();
  check_time();
}

// instead of commenting and uncommenting, just write new functions starting
// with `test_` if you think you will reuse them.
void test() {
  turn_to_target(BRCAL);
  delay(500);
}

void test_get_closest_goal() {
  Point goal = get_closest_goal();
  print_Point(goal);
  print_Point(read_front());
  Serial.println();
  delay(250);
}

void test_within_boundary() {
  Serial.println(within_boundary());
  delay(100);
}

void test_bumper() {
  stop_robot();
  delay(100);
  Serial.print(digitalRead(button_front1));
  Serial.println(digitalRead(button_front2));
}

void test_print_dumpsters() {
  print_Point(DTOP);
  print_Point(DBOT);
}
