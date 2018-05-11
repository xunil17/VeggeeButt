//retrieves if the three lasers in the front detect any
#define laser laser1
#define photo photo1

float scan() {
  int high;
  int low;
  float average = 0;
  for (int i = 0; i < 5; i++) {
    digitalWrite(laser, HIGH);
    delay(30);
    high = analogRead(photo);
    digitalWrite(laser, LOW);
    delay(30);
    low = analogRead(photo);
    average += (high - low);
  }

  average /= 5.0;

  return average;
}

void front_bumper() {
  if((digitalRead(button_front1) == LOW) && (digitalRead(button_front2) == LOW)) {
    digitalWrite(led, HIGH);
    move_left_motor(0);
    move_right_motor(0);
    delay(1000);
  } else {
    digitalWrite(led, LOW);
  }
}

void gripper_bumper() {
  if((digitalRead(button_gripper1) == LOW) && (digitalRead(button_gripper1) == LOW)) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}



