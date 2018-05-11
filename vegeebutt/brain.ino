#define EINTMAX 1000

#define heading_diff_threshold 5

void plan() {
  CurrState.resetting = false;
  find_and_go_to_block(); // look for and get to the block
}

void swap_dir() {
  if (CurrState.dir == Left) {
    CurrState.dir = Right;
  } else {
    CurrState.dir = Left;
  }
}

void find_and_go_to_block() {
  if (CurrState.dir == Left) {
    turn_left();
  } else {
    turn_right();
  }

  if (block_seen()) {
    swap_dir();
    stop_robot();

    // move toward block unless it moves out of sight
    do {
      update_vive(); // update vive periodically for accurate coordinates
      if (check_boundary_and_maybe_reset()) {
        return;
      }

      move_forward(110);

      // stop when block is within our grasp
      if (hit && block_seen()) {
        stop_robot();
        hit = false;
        move_forward(30);
        delay(200);
        stop_robot();
        Block block_type = grab_and_identify();
        handle_block(block_type);
        return;
      } else if (hit) {
        hit = false;
      }
    } while (block_seen());
  }
}

bool check_boundary_and_maybe_reset() {
  if (CurrState.resetting) {
    //    hit = false;
    return true;
  }

  if (!within_boundary()) {
    CurrState.resetting = true;
    //    hit = false;
    go_to_center();
    return true;
  }

  return false;
}

void go_to_center() {
  find_and_go_to_target({7, 0});
}

void back_up() {
  move_backward(100);
  delay(800);
  stop_robot();
}

void find_and_go_to_target(Point target) {
  go_to_target(target);
  stop_robot();
}

void find_and_go_to_goal() {
  find_and_go_to_target(get_closest_goal());
  open_gripper_max();
  back_up();
  go_to_center();
}

void find_and_go_to_dumpster() {
  find_and_go_to_target(get_closest_dumpster());
  open_gripper_max();
  back_up();
  go_to_center();
}

void handle_block(Block type) {

  if (type == Cylinder) {
    Serial.println("Found Cylinder!");
    handle_cylinder();
  } else if (type == Cube) {
    Serial.println("Found Cube!");
    handle_cube();
  } else {
    Serial.println("Found None.");
    open_gripper_max();
    return;
  }
}

void handle_cylinder() {
  gripper_tighten_briefly();

  go_to_center();

  if (TeamType == Cylinder) {
    find_and_go_to_goal();
  } else {
    find_and_go_to_dumpster();
  }
}

void handle_cube() {
  gripper_tighten_briefly();

  go_to_center();

  if (TeamType == Cube) {
    find_and_go_to_goal();
  } else {
    find_and_go_to_dumpster();
  }
}

void go_to_target(Point target) {
  int vel = 110;
  if (CurrState.holding == Cylinder) {
    vel = 200;
  } else if (CurrState.holding == Cube) {
    vel = 140;
  }
  float u;
  float kp = 0.5;
  float target_heading = get_heading_toward(target);
  float heading_diff = get_heading_difference(target_heading);
  float dist = get_distance_between_points(target, {CurrState.x, CurrState.y});
  while (abs(dist) > 0.6) {
    update_vive();

    u = kp * heading_diff;
    if (abs(heading_diff) > 15) {
      turn_to_target(target);
    } else if (heading_diff <= 0) { //turn left a little, spin left motor faster than right motor
      move_left_motor(vel + u);
      move_right_motor(vel - u);
    } else if (heading_diff > 0) { //turn right a little, spin right motor faster than left motor
      move_left_motor(vel - u);
      move_right_motor(vel + u);
    }

    target_heading = get_heading_toward(target);
    heading_diff = get_heading_difference(target_heading);
    dist = get_distance_between_points(target, {CurrState.x, CurrState.y});
  }
  stop_robot();
  delay(1000);
}

void turn_to_target(Point target) {
  float kp = 0.5;
  float ki = 0.07;
  float kd = 0.2;
  float error_integral, edot;
//  if (CurrState.holding == Cylinder) {
//    float kp = 0.7;
//    float ki = 0.07;
//    float kd = 0.2;
//  }
  float u;
  float target_heading = get_heading_toward(target);
  float heading_diff = get_heading_difference(target_heading);
  float previous = heading_diff;
  int asize = 5;
  bool heading_diffs_within[asize];
  initialize_within(heading_diffs_within, asize, abs(heading_diff) < heading_diff_threshold);
  while (!all_within(heading_diffs_within, asize)) {
    update_vive();
    
    error_integral = error_integral + heading_diff; //error sum
    edot = heading_diff - previous;
    if (error_integral > EINTMAX) {
      error_integral = EINTMAX;
    } else if (error_integral < -EINTMAX) {
      error_integral = -EINTMAX;
    }

//    u = kp * heading_diff + ki * error_integral + kd * edot;
    u = kp * heading_diff + ki * error_integral;

    //    if (u < 0) {
    //      u = u - min_u;
    //    } else {
    //      u = u + min_u;
    //    }
    Serial.print(heading_diff);
    Serial.print(" ");
    Serial.print(u);
    Serial.print(" ");
    Serial.print(kp * heading_diff);
    Serial.print(" ");
    Serial.print(ki * error_integral);
    Serial.print(" ");
    Serial.println(kd * edot);
//    print_array(heading_diffs_within, asize);
//    Serial.print(" ");
//    Serial.println(error_integral);
    turn_robot((int)u);
    target_heading = get_heading_toward(target);
    previous = heading_diff;
    heading_diff = get_heading_difference(target_heading);
    update_and_rotate(heading_diffs_within, asize, heading_diff);
  }
}

void update_and_rotate(bool within[], int asize, float heading_diff) {
  for (int i = asize - 1; i > 0; i--) {
    within[i] = within[i-1];
  }

  within[0] = abs(heading_diff) < heading_diff_threshold;
}

void initialize_within(bool within[], int asize, bool first) {
  within[0] = first;
  for (int i = 1; i < asize; i++) {
    within[i] = false;
  }
}

bool all_within(bool within[], int asize) {
  for (int i = 0; i < asize; i++) {
    if (!within[i]) {
      return false;
    }
  }

  return true;
}

void print_array(bool within[], int asize) {
  Serial.print("[");
  for (int i = 0; i < asize; i++) {
    Serial.print(within[i]);
    Serial.print(", ");
  }

  Serial.println("]");
}

bool block_seen() {
  float scan_result = scan();
  return scan_result >= scan_threshold;
}
