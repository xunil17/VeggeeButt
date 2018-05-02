float heading_threshold = 1.0;

// directional notation is relative to facing the board landscape when the BLUE circle is on the LEFT
// calibration globals:
struct xy bottom_left = {0,0};
struct xy bottom_right = {0,0};
struct xy top_left = {0,0};
struct xy top_right = {0,0};

// TODO fix
void adjust_heading(float headi, struct xy current, struct xy target) {
  float heading_threshold = 1.0;
  float headf = get_heading(current, target);
  float left;
  float right;
  
  if (abs(headf - headi) <= heading_threshold) {
    return;
  } else if (headf > headi) {
    left = 360 - headf + headi;
    right = headf - headi;
  } else if (headf < headi) {
    left = headi - headf;
    right = 360 - headi + headf;
  }
  
  if (left < right) {
//    turn_left(left);
  } else {
//    turn_right(right);
  }
}

float get_heading(struct xy current, struct xy target) {
  return 90.0 - rad_to_deg(atan((target.y - current.y) / (target.x - current.x)));
}

float rad_to_deg(float rad) {
  return rad * 180 / M_PI;
}

float deg_to_rad(float deg) {
  return deg * M_PI / 180;
}
