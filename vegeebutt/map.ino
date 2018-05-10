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
  
  if (headf >= headi) {
    left = 360 - headf + headi;
    right = headf - headi;
  } else if (headf < headi) {
    left = headi - headf;
    right = 360 - headi + headf;
  }

//  Serial.print("initial, target, left, right: ");
//  Serial.print(headi);
//  Serial.print(" ");
//  Serial.print(headf);
//  Serial.print(" ");
//  Serial.print(left);
//  Serial.print(" ");
//  Serial.println(right);

  if (left < right) {
    return -left;
  } else {
    return right;
  }
}

float get_heading_between_points(Point p1, Point p2) {
  float heading = 90 - rad_to_deg(atan((p1.y - p2.y) / (p1.x - p2.x)));
  if (p2.x <= p1.x) {
    heading += 180;
  }

  if (heading >= 360) {
    heading -= 360;
  }

  return heading;
}

// get heading relative to current heading to face target
float get_heading_toward(Point target) {
  return get_heading_between_points({CurrState.x, CurrState.y}, target);
}

float rad_to_deg(float rad) {
  return rad * 180 / M_PI;
}

float deg_to_rad(float deg) {
  return deg * M_PI / 180;
}
