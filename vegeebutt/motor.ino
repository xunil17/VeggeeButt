#define turn_speed 35


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

void move_forward(int vel) {
  move_left_motor(vel);
  move_right_motor(vel);
}

void move_backward(int vel) {
  move_forward(-vel);
}

void turn_left() {
  turn_robot(-turn_speed);
}

void turn_right() {
  turn_robot(turn_speed);
}

//turns the robot (-255 to 0) is turn left and (0 to 255) is turn right
void turn_robot(int vel) {
  move_left_motor(vel);
  move_right_motor(-vel);
}

void stop_robot() {
  move_left_motor(0);
  move_right_motor(0);
}
