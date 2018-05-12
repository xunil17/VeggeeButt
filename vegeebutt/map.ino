// Functions for calculations related to headings and coordinates.


// larger boundary offset means we can get closer to the edges...
#define x_boundary_offset 0
#define y_boundary_offset 0


Point get_closest_goal() {
  Point g1, g2;
  if (TeamType == Cylinder) {
    g1 = BRCAL;
    g2 = TLCAL;
  } else {
    g1 = BLCAL;
    g2 = TRCAL;
  }

  return get_closest_point(g1, g2);
}

Point get_closest_dumpster() {
//  return get_closest_point(DTOP, DBOT);
  if (CurrState.last_dumpster == Left) {
    CurrState.last_dumpster = Right;
    return DBOT;
  } else {
    CurrState.last_dumpster = Left;
    return DTOP;
  }
}

Point get_closest_center() {
  return get_closest_point(CL, CR);
}

Point get_closest_point(Point p1, Point p2) {
  Point curr = {CurrState.x, CurrState.y};

  if (get_distance_between_points(curr, p1) > get_distance_between_points(curr, p2)) {
    return p2;
  } else {
    return p1;
  }
}

// bool within_boundary() { // using nearest
//   Point c = {CurrState.x, CurrState.y};
//   Point closest = get_closest_calibrated_point();
//   bool xokay = false;
//   bool yokay = false;
//
//   // check y
//   // if closer to bottom, current y needs to be greater than closest.y
//   if (closest == BLCAL || closest == BRCAL || closest == DBOT) {
//     yokay = c.y + y_boundary_offset > closest.y;
//   } else {
//     yokay = c.y - y_boundary_offset < closest.y;
//   }
//
//   // check x
//   // if closer to left, current x needs to be greater than closest.x
//   if (closest == BLCAL || closest == TLCAL) {
//     xokay = c.x + x_boundary_offset > closest.x;
//   } else if (closest == BRCAL || closest == TRCAL) {
//     xokay = c.x - x_boundary_offset < closest.x;
//   } else {
//     xokay = true;
//   }
//
//   return xokay && yokay;
// }

bool within_boundary() { // using interpolation
  Point c = {CurrState.x, CurrState.y};
  Point closest = get_closest_calibrated_point();
  Point L = BLCAL;
  Point M = DBOT;
  Point R = BRCAL;
  if (closest == TLCAL || closest == TRCAL || closest == DTOP) {
    L = TLCAL;
    M = DTOP;
    R = TRCAL;
  }

  if (c.x < L.x || c.x > R.x) {
    return false;
  }

  float yboundary;

  if (c.x < M.x) {
    yboundary = ((M.y - L.y) / (M.x - L.x)) * c.x + L.y;
  } else {
     yboundary = ((R.y - M.y) / (R.x - M.x)) * (c.x - M.x) + M.y ;
  }

  if (L == TLCAL) {
    return c.y < yboundary;
  } else {
    return c.y > yboundary;
  }
}

Point get_closest_calibrated_point() {
  Point curr = {CurrState.x, CurrState.y};
  Point calibrated_points[] = { BLCAL, BRCAL, TLCAL, TRCAL, DBOT, DTOP };
  int cpsize = 6;
  float differences[cpsize];
  for (int i = 0; i < cpsize; i++) {
    differences[i] = get_distance_between_points(curr, calibrated_points[i]);
  }

  int min_index = 0;

  for (int i = 0; i < cpsize; i++) {
    if (differences[i] < differences[min_index]) {
      min_index = i;
    }
  }

  return calibrated_points[min_index];
}

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

float get_distance_between_points(Point target1, Point target2) {
  return sqrt(sq(target1.x-target2.x) + sq(target1.y-target2.y));
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
