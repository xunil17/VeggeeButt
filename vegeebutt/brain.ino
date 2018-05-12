#define EINTMAX 1000

#define heading_diff_threshold 5

void plan() {
  CurrState.resetting = false;
  find_and_go_to_block(); // look for and get to the block
}

void find_and_go_to_block() {
  if (CurrState.turn_direction == Left) {
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
      check_time();
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

void swap_dir() {
  if (CurrState.turn_direction == Left) {
    CurrState.turn_direction = Right;
  } else {
    CurrState.turn_direction = Left;
  }
}

bool check_boundary_and_maybe_reset() {
  if (CurrState.resetting) {
    return true;
  }

  if (!within_boundary()) {
    CurrState.resetting = true;
    go_to_center();
    return true;
  }

  return false;
}

void go_to_center() {
  find_and_go_to_target(get_closest_center());
}

void go_to_center_after_dumpster() {
  // for dumpsters, switch which center to go to
  if (CurrState.last_center == Left) {
    CurrState.last_center = Right;
    find_and_go_to_target(CR);
  } else {
    CurrState.last_center = Left;
    find_and_go_to_target(CL);
  }
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
  go_to_center_after_dumpster();
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
  int vel = 70;
  if (CurrState.holding == Cylinder) {
    vel = 70;
  } else if (CurrState.holding == Cube) {
    vel = 70;
  }
  float u;
  float kp = 0.5;
  float target_heading = get_heading_toward(target);
  float heading_diff = get_heading_difference(target_heading);
  float dist = get_distance_between_points(target, {CurrState.x, CurrState.y});
  while (abs(dist) > 0.6) {
    update_vive();
    check_time();

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
  float u;
  float kp = 0.5;
  float min_u = 35;
  if (CurrState.holding == Cylinder) {
    kp = 0.7;
    min_u = 50;
  }
  float target_heading = get_heading_toward(target);
  float heading_diff = get_heading_difference(target_heading);
  while (abs(heading_diff) > heading_diff_threshold) {
    update_vive();
    check_time();
    
    u = kp * heading_diff;

    if (u < 0) {
      u = u - min_u;
    } else {
      u = u + min_u;
    }
    turn_robot((int)u);
    target_heading = get_heading_toward(target);
    heading_diff = get_heading_difference(target_heading);
  }
}


bool block_seen() {
  float scan_result = scan();
  return scan_result >= scan_threshold;
}
