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
  return get_closest_point(DTOP, DBOT);
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

bool within_boundary() {
  Point c = {CurrState.x, CurrState.y};
  Point closest_b = get_closest_point(BLCAL, BRCAL);
  Point closest_t = get_closest_point(TLCAL, TRCAL);
  Point closest = get_closest_point(closest_b, closest_t);

  bool xokay = false;
  bool yokay = false;

  // check y
  // if closer to bottom, current y needs to be greater than closest.y
  if (closest == BLCAL || closest == BRCAL) {
    yokay = c.y + y_boundary_offset > closest.y;
  } else {
    yokay = c.y - y_boundary_offset < closest.y;
  }

  // check x
  // if closer to left, current x needs to be greater than closest.x
  if (closest == BLCAL || closest == TLCAL) {
    xokay = c.x + x_boundary_offset > closest.x;
  } else {
    xokay = c.x - x_boundary_offset < closest.x;
  }

  return xokay && yokay;
}

Point get_closest_calibrated_point() {
  Point calibrated_points[] = { BLCAL, BRCAL, TLCAL, TRCAL, DBOT, DTOP };
  int cpsize = 6;
  for (int i = 0; i < cpsize; i++) {
    
  }
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
