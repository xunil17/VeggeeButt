#define min_dist 90
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
    int i;
    gripper.write(90);
  } else {
    float dif = get_dist() - dist;
    while (abs(dif) > 1) {
      if (dif > 0) { //positive means we need to open gripper
        gripper.write(82);
        //Serial.println("open gripper");
      } else {
        gripper.write(98); //close gripper
        //Serial.println("close gripper");
      }
      dif = get_dist() - dist;
//      Serial.println(dif);
      //      Serial.println(dif);
    }
    gripper.write(90);
  }
}

Block grab_and_identify() {
  open_gripper_max();
  delay(1000);
  move_gripper(210);
  if(gripper_gripped()) {
    return Cylinder;
  } 

  move_gripper(300);
  if(gripper_gripped()) {
    return Cube;
  }

  return None;
}

bool gripper_gripped() {
  return digitalRead(button_gripper1) == LOW || digitalRead(button_gripper2) == LOW;
}

//opens gripper to max width
void open_gripper_max() {
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

