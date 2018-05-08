//retrieves if the three lasers in the front detected any a
float scan() {
  int high[3];
  int low[3];
  float average[3];
  average[0] = 0;
  average[1] = 0;
  average[2] = 0;
  for (int i = 0; i < 5; i++) {
    digitalWrite(laser1, HIGH);
    digitalWrite(laser2, HIGH);
    digitalWrite(laser3, HIGH);
    delay(30);
    high[0] = analogRead(photo1);
    high[1] = analogRead(photo2);
    high[2] = analogRead(photo3);
    digitalWrite(laser1, LOW);
    digitalWrite(laser2, LOW);
    digitalWrite(laser3, LOW);
    delay(30);
    low[0] = analogRead(photo1);
    low[1] = analogRead(photo2);
    low[2] = analogRead(photo3);
    //    Serial.print(high);
    //    Serial.print(" ");
    //    Serial.println(low);
    average[0] += (high[0] - low[0]);
    average[1] += (high[1] - low[1]);
    average[2] += (high[2] - low[2]);
  }
  average[0] = average[0] / 5.0;
  average[1] = average[1] / 5.0;
  average[2] = average[2] / 5.0;
//  Serial.print(average[0]);
//  Serial.print(' ');
//  Serial.print(average[1]);
//  Serial.print(' ');
//  Serial.println(average[2]);
//  Serial.print(' ');
  
  if (average[2] >= 100) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
  return average[2];
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



