#define EINTMAX 360

//drives the left motor forward or backward (-255 to 255)
void move_left_motor(int vel) {
  if (vel > 0) {
    analogWrite(lmotor_pwm, vel);
    digitalWrite(lmotor_dir, HIGH);
  } else {
    analogWrite(lmotor_pwm, -vel);
    digitalWrite(lmotor_dir, LOW);
  }
}

//drives the right motor forward or backward (-255 to 255)
void move_right_motor(int vel) {
  if (vel > 0) {
    analogWrite(rmotor_pwm, vel);
    digitalWrite(rmotor_dir, LOW);
  } else {
    analogWrite(rmotor_pwm, -vel);
    digitalWrite(rmotor_dir, HIGH);
  }
}

/*  THESE FUNCTIONS MAY BE USELESS
  //turns robot left (0 to 255)
  void turn_left(unsigned int spd) {
  move_left_motor(spd);
  move_right_motor(-spd);
  }

  //turns robot right (0 to 255)
  void turn_right(unsigned int spd) {
  move_left_motor(-spd);
  move_right_motor(spd);
  }*/

//turns the robot (-255 to 0) is turn left and (0 to 255) is turn right
void turn_robot(int vel) {
  move_left_motor(vel);
  move_right_motor(-vel);
}

void turn_to_heading(float target_heading) {
  float u;
  float eint = 0;
  float kp = 0.829;
  float ki = 0.2;
  float heading_diff = get_heading_difference(target_heading);
  while (heading_diff > 3) {
    eint = eint + heading_diff; //error sum
    if (eint > EINTMAX) {
      eint = EINTMAX;
    } else if (eint < -EINTMAX) {
      eint = -EINTMAX;
    }
    u = kp * heading_diff + ki * eint;
    turn_robot((int)u);
    heading_diff = get_heading_difference(target_heading);
  }
}


//0 to 90 is open gripper
//90 to 180 is close gripper

//moves gripper until at dist
void move_gripper(int dist) {
  if(dist > 600 || dist < 80) {
    gripper.write(90);
  } else {
    int dif = get_dist() - dist;
    while(abs(dif) > 5) {
      if(dif > 0) { //positive means we need to open gripper
        gripper.write(80);
      } else {
        gripper.write(100);
      }
      dif = get_dist() - dist;
      Serial.println(dif);
    }
    gripper.write(90);
  }
}

int grab_and_identify() {
//  open_gripper_max();
  move_gripper(200);
  
}

//opens gripper to max width
void open_gripper_max() {
  move_gripper(90); //move gripper to this position
}

int get_dist() {
  return analogRead(ir_pin);
}


