
//
//// TODO fix
//void adjust_heading(Loc target) {
//  float headf = get_heading(target);
//  float headi = VState.heading
//  float left;
//  float right;
//  
//  if (abs(headf - headi) <= heading_threshold) {
//    return;
//  } else if (headf > headi) {
//    left = 360 - headf + headi;
//    right = headf - headi;
//  } else if (headf < headi) {
//    left = headi - headf;
//    right = 360 - headi + headf;
//  }
//  
//  if (left < right) {
////    turn_left(left);
//  } else {
////    turn_right(right);
//  }
//}


// This function returns the difference between the target_heading
// and VBot's current heading. Negative means the target_heading is to
// the left and positive means the target_heading is to the right.
// Returns the difference with the smallest magnitude.
float get_heading_difference(float target_heading) {
  float headf = target_heading;
  float headi = VState.heading;
  float left, right;
  
  if (headf > headi) {
    left = 360 - headf + headi;
    right = headf - headi;
  } else if (headf < headi) {
    left = headi - headf;
    right = 360 - headi + headf;
  }

  if (left < right) {
    return -left;
  } else {
    return right;
  }
}

float get_heading_toward(Loc target) {
  return 90.0 - rad_to_deg(atan((target.y - VState.location.y) / (target.x - VState.location.x)));
}

float rad_to_deg(float rad) {
  return rad * 180 / M_PI;
}

float deg_to_rad(float deg) {
  return deg * M_PI / 180;
}
