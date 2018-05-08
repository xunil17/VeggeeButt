// Functions for calculations related to headings and coordinates.


// This function returns the difference between the target_heading
// and VBot's current heading. Negative means the target_heading is to
// the left and positive means the target_heading is to the right.
// Returns the difference with the smallest magnitude.
float get_heading_difference(float target_heading) {
  float headf = target_heading;
  float headi = CurrState.heading;
  float left = -1;
  float right = -1;
  
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

// get heading relative to current heading to face target
float get_heading_toward(Point target) {
  return 90.0 - rad_to_deg(atan((target.y - CurrState.y) / (target.x - CurrState.x)));
}

float rad_to_deg(float rad) {
  return rad * 180 / M_PI;
}

float deg_to_rad(float deg) {
  return deg * M_PI / 180;
}
