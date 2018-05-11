#define min_dist 110
#define max_dist 300

#define cylinder_grip_val 210
#define cube_grip_val 270
//turning the screw left makes the claw close
//turing the screw right makes the claw open

//0 to 89 is open gripper
//90 is stop gripper
//91 to 180 is close gripper

//110 dist is max open
//around 600 is max close

//moves gripper until at dist
void move_gripper(int dist) {
  if (dist > 600 || dist < min_dist) {
    gripper_stop();
  } else {
    float dif = get_dist() - dist;
    unsigned long start_time = millis();
    while (abs(dif) > 4 && millis() - start_time < 4000) {
      if (dif > 0) { //positive means we need to open gripper
        gripper_loosen();
      } else {
        gripper_tighten(); //close gripper
      }
      dif = get_dist() - dist;
      
    }
    gripper_stop();
  }
}

void gripper_tighten() {
  gripper.write(98);
}

void gripper_loosen() {
  gripper.write(82);
}

void gripper_stop() {
  gripper.write(90);
}

void gripper_tighten_briefly() {
  gripper_tighten();
  delay(200);
  gripper_stop();
}

// grip and identify what is being gripped
Block grab_and_identify() {
  open_gripper_max();
  delay(1000);

  Block ret = None;
  CurrState.holding = ret;

  if (gripper_gripped()) {
    // something is wrong... return None I guess...
    return ret;
  }

  bool gripped = false;

  unsigned long start_time = millis();
  while (1) {
    gripper_tighten();
    if (gripper_gripped()){ // exit if gripped
      gripped = true;
      break;
    } else if (get_dist() > max_dist || millis() - start_time > 4000) { 
      // protective measures. exit if closing too much or took too much time closing.
      break;
    } 
  }

  gripper_stop();

  if (!gripped) {
    return ret; // None
  }

  if (get_dist() <= cylinder_grip_val + 10) {
    ret = Cylinder;
  } else {
    ret = Cube;
  }

  CurrState.holding = ret; // update state
  return ret;
}

bool gripper_gripped() {
  return digitalRead(button_gripper1) == LOW || digitalRead(button_gripper2) == LOW;
}

//opens gripper to max width
void open_gripper_max() {
  CurrState.holding = None;
  move_gripper(min_dist); //move gripper to this position
}

float get_dist() {
  int i;
  float avg = 0;
  for (i = 0; i < 5; i++) {
    avg += analogRead(ir_pin);
  }
  return avg/5;
}

