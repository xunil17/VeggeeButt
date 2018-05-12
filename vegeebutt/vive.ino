// BL, BR, TL, TR
#define stuck_distance_threshold 0.2
#define stuck_time_threshold 10000

Point read_front() {
  return {xFilt2, yFilt2};
}

Point read_back() {
  return {xFilt1, yFilt1};
}

void update_vive() {
  if (V1.useMe == 1) {
    V1.useMe = 0;
    // calculate the position and filter it
    xPos1 = tan((V1.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN) * LIGHTHOUSEHEIGHT;
    yPos1 = LIGHTHOUSEHEIGHT / cos((V1.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN)  * tan((V1.horzAng - 90.0) * DEG_TO_RADIAN);
    xFilt1 = xOld1 * 0.5 + xPos1 * 0.5; // filter
    yFilt1 = yOld1 * 0.5 + yPos1 * 0.5; // filter
    xOld1 = xFilt1; // remember for next loop
    yOld1 = yFilt1; // remember for next loop
  }

  if (V2.useMe == 1) {
    V2.useMe = 0;

    // calculate the position and filter it
    xPos2 = tan((V2.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN) * LIGHTHOUSEHEIGHT;
    yPos2 = LIGHTHOUSEHEIGHT / cos((V2.vertAng - LIGHTHOUSEANGLE) * DEG_TO_RADIAN)  * tan((V2.horzAng - 90.0) * DEG_TO_RADIAN);
    xFilt2 = xOld2 * 0.5 + xPos2 * 0.5; // filter
    yFilt2 = yOld2 * 0.5 + yPos2 * 0.5; // filter
    xOld2 = xFilt2; // remember for next loop
    yOld2 = yFilt2; // remember for next loop
  }

  // unstuck ourselves
  if (mode == Run) {
    if (get_distance_between_points({CurrState.x, CurrState.y}, {PreviousCheckpoint.x, PreviousCheckpoint.y}) > stuck_distance_threshold) {
      PreviousCheckpoint.x = CurrState.x;
      PreviousCheckpoint.y = CurrState.y;
      PreviousCheckpoint.t = millis();
    } else if (millis() - PreviousCheckpoint.t > stuck_time_threshold) {
      stop_everything();
      open_gripper_max();
      if (random(2) == 0) {
        turn_right();
      } else {
        turn_left();
      }
      delay(random(300, 800));
      open_gripper_max();
      stop_robot();
      CurrState.resetting = true;
    }
  }


  // update our Current State
  Point back = read_back();
  Point front = read_front();

  CurrState.x = front.x;
  CurrState.y = front.y;

  CurrState.heading = get_heading_between_points(back, front);
}

// the sensor interrupts
void ISRVive1() {
  // get the time the interrupt occured
  unsigned long mic = micros();
  int i;

  // shift the time into the buffer
  for (i = 0; i < 10; i++) {
    V1.changeTime[i] = V1.changeTime[i + 1];
  }
  V1.changeTime[10] = mic;

  // if the buffer is full
  if (V1.collected < 11) {
    V1.collected++;
  }
  else {
    // if the times match the waveform pattern
    if ((V1.changeTime[1] - V1.changeTime[0] > 7000) && (V1.changeTime[3] - V1.changeTime[2] > 7000) && (V1.changeTime[6] - V1.changeTime[5] < 50) && (V1.changeTime[10] - V1.changeTime[9] < 50)) {
      V1.horzAng = (V1.changeTime[5] - V1.changeTime[4]) * DEG_PER_US;
      V1.vertAng = (V1.changeTime[9] - V1.changeTime[8]) * DEG_PER_US;
      V1.useMe = 1;
    }
  }
}
void ISRVive2() {
  // get the time the interrupt occured
  unsigned long mic = micros();
  int i;

  // shift the time into the buffer
  for (i = 0; i < 10; i++) {
    V2.changeTime[i] = V2.changeTime[i + 1];
  }
  V2.changeTime[10] = mic;

  // if the buffer is full
  if (V2.collected < 11) {
    V2.collected++;
  }
  else {
    // if the times match the waveform pattern
    if ((V2.changeTime[1] - V2.changeTime[0] > 7000) && (V2.changeTime[3] - V2.changeTime[2] > 7000) && (V2.changeTime[6] - V2.changeTime[5] < 50) && (V2.changeTime[10] - V2.changeTime[9] < 50)) {
      V2.horzAng = (V2.changeTime[5] - V2.changeTime[4]) * DEG_PER_US;
      V2.vertAng = (V2.changeTime[9] - V2.changeTime[8]) * DEG_PER_US;
      V2.useMe = 1;
    }
  }
}
