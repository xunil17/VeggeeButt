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
#define Vive2PIN 17 // formerly 14

#define scan_threshold 100

#define button_isr_delay 200

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
  Direction turn_direction;
  Block holding;
  bool resetting;
  Direction last_center;
} State;

typedef struct LocationRecording_ {
  float x;
  float y;
  unsigned long t;
} LocationRecording;

const Block TeamType = Cube;

// global instance representing Robot state
volatile State CurrState = {0, 0, 0, Right, None, false, TeamType == Cube ? Left : Right};
volatile LocationRecording PreviousCheckpoint = {0, 0, 0};

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
  Serial.print(Direction_to_string(CurrState.turn_direction));
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

// directional notation is relative to facing the board
//   landscape when the center BLUE circle is on the LEFT
// calibration globals:
const Point BLCAL = { 1.28, -2.37};
const Point BRCAL = { 8.68, -1.00};
const Point TLCAL = { 1.2, 2.68 };
const Point TRCAL = { 8.60, 2.12 };

// dumpsters
//const Point DBOT = { (BLCAL.x + BRCAL.x) / 2, BLCAL.y > BRCAL.y ? BLCAL.y : BRCAL.y };
//const Point DTOP = { (TLCAL.x + TRCAL.x) / 2, TLCAL.y < TRCAL.y ? TLCAL.y : TRCAL.y };
const Point DBOT = { 5.50, -1.63 };
const Point DTOP = { 5.43, 2.10 };

// centers
// 1/3 and 2/3 distance between left and right averaged sides
//const Point CL = { ((BLCAL.x + TLCAL.x) / 2) + ((((BRCAL.x - BLCAL.x) + (TRCAL.x - TLCAL.x)) / 2) / 3), (BLCAL.y + TLCAL.y) / 2 };
//const Point CR = { ((BLCAL.x + TLCAL.x) / 2) + (((BRCAL.x - BLCAL.x) + (TRCAL.x - TLCAL.x)) / 3), (BRCAL.y + TRCAL.y) / 2 };
const Point CL = { 2.35, 0.3 };
const Point CR = { 7.83, 0.55 };

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

//const Mode mode = Test;
//const Mode mode = Calibrate;
const Mode mode = Run;

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
  delayMicroseconds(button_isr_delay);
  if(digitalRead(button_front1) == HIGH) {
    hit = true;
//    Serial.println("hit");
  }
}

void ISR_button_2() {
  delayMicroseconds(button_isr_delay);
  if(digitalRead(button_front2) == HIGH) {
    hit = true;
//    Serial.println("hit");
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
  test_within_boundary();
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
  print_Point(CL);
  print_Point(CR);
  print_Point(read_front());
}

void test_vive() {
  int i = 0;
  for(i = 0; i < 150; i++) {
    move_left_motor(i);
    move_right_motor(i);
    delay(200);
    update_vive();
    Serial.println(i);
    print_Point(read_front());
  }
}

void test_grip_and_identify() {
  Block b = grab_and_identify();
  Serial.println(Block_to_string(b));
}
