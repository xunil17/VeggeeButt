//Point compute_center() {
//  Point front = read_front();
//  Point back = read_back();
//  return {(front.x-back.x) / 2 + (back.x), (front.y-back.y) / 2 + (back.y)};
//}

//float compute_heading() {
//  Point front = read_front();
//  Point back = read_back();
//  
//}

Point read_front() {
  return {xFilt2, yFilt2};
}

Point read_back() {
  return {xFilt1, yFilt1};
}

// BL, BR, TL, TR

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

//Point compute_transformed_coordinates(Point input) {
//
//  BLA::Matrix<8,8> A= {
//    bl.x, bl.y, 1, 0, 0, 0, -BL.x * bl.x, -BL.x * bl.y,
//    0, 0, 0, bl.x, bl.y, 1, -BL.y * bl.x, -BL.y * bl.y,
//    br.x, br.y, 1, 0, 0, 0, -BR.x * br.x, -BR.x * br.y,
//    0, 0, 0, br.x, br.y, 1, -BR.y * br.x, -BR.y * br.y,
//    tl.x, tl.y, 1, 0, 0, 0, -TL.x * tl.x, -TL.x * tl.y,
//    0, 0, 0, tl.x, tl.y, 1, -TL.y * tl.x, -TL.y * tl.y,
//    tr.x, tr.y, 1, 0, 0, 0, -TR.x * tr.x, -TR.x * tr.y,
//    0, 0, 0, tr.x, tr.y, 1, -TR.y * tr.x, -TR.y * tr.y};
//  
//  BLA::Matrix<8,1> B = {
//    BL.x,
//    BL.y,
//    BR.x,
//    BR.y,
//    TL.x,
//    TL.y,
//    TR.x,
//    TR.y
//  };
//
//  BLA::Matrix<8,8> A_inv = A.Inverse();
//  BLA::Matrix<8,1> sol = A_inv * B;
//  BLA::Matrix<3,3> new_sol = {
//    sol(0,0), sol(1,0), sol(2,0), sol(3,0), sol(4,0), sol(5,0), sol(6,0), sol(7,0), 1
//  };
//
//  //save this matrix so we don't have to calculate it every time
//  BLA::Matrix<3,3> new_sol_inv = new_sol.Inverse();
//
//  BLA::Matrix<3,1> input_mat = {
//    input.x - BLCAL.x, input.y - BLCAL.y, 1
//  };
//
//  BLA::Matrix<3,1> out = new_sol_inv * input_mat;
//
//  float x_out = out(0,0) / out(2,0);
//  float y_out = out(1,0) / out(2,0);
//
//  Serial << "Transformed: " << input.x << ", " << input.y << " -> " << x_out << ", " << y_out << '\n';
//
//  return {x_out, y_out};
//}
