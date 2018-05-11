#define EINTMAX 360

void plan() {
  Serial.println("Entering plan()");
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
//  open_gripper_max(); // reset gripper before trying to find other stuff
  if (CurrState.dir == Left) {
    turn_left();
  } else {
    turn_right();
  }
  
  if (block_seen()) {
    swap_dir();
    stop_robot();
    delay(250);

    // move toward block unless it moves out of sight
    do {
      update_vive(); // update vive periodically for accurate coordinates
      move_forward(110);
      
      // stop when block is within our grasp
      if (hit) {
        stop_robot();
        hit = false;
        move_forward(30);
        delay(200);
        stop_robot();
        Block block_type = grab_and_identify();
        handle_block(block_type);
        return;
      }
    } while(block_seen());
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
  open_gripper_max();
  back_up();
}

void find_and_go_to_goal() {
  find_and_go_to_target(get_closest_goal());
}

void find_and_go_to_dumpster() {
  find_and_go_to_target(get_closest_dumpster());
}

void handle_block(Block type) {
  Serial.println("Entering handle_block()");
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
  if (TeamType == Cylinder) {
    find_and_go_to_goal();
  } else {
    find_and_go_to_dumpster();
  }
}

void handle_cube() {
  if (TeamType == Cube) {
    find_and_go_to_goal();
  } else {
    find_and_go_to_dumpster();
  }
}

void go_to_target(Point target) {
  float min_u = 70;
  if (CurrState.holding == Cylinder) {
    min_u = 120;
  }
  int vel = 70;
  float u;
  float kp = 0.5;
  float target_heading = get_heading_toward(target);
  float heading_diff = get_heading_difference(target_heading);
  float dist = get_distance_between_points(target,{CurrState.x,CurrState.y});
  while (abs(dist) > 0.6) {
    update_vive();
    u = kp * heading_diff;
    if(abs(heading_diff) > 15) {
      turn_to_target(target);
    } else if (heading_diff <= 0) { //turn left a little, spin left motor faster than right motor
      move_left_motor(vel+u);
      move_right_motor(vel-u);
    } else if (heading_diff > 0) { //turn right a little, spin right motor faster than left motor
      move_left_motor(vel-u);
      move_right_motor(vel+u);
    }
//    turn_robot((int)u);
//    print_CurrState();

    
//    Serial.print(eint);
//    Serial.print(dist);
//    Serial.print(" ");
//    Serial.print(CurrState.heading);
//    Serial.print(" ");
//    Serial.print(heading_diff);
//    Serial.print(" ");
//    Serial.print(target_heading);
//    Serial.print(" ");
//    Serial.println(u);
    target_heading = get_heading_toward(target);
    heading_diff = get_heading_difference(target_heading);
    dist = get_distance_between_points(target,{CurrState.x,CurrState.y});
  }
  stop_robot();
  delay(1000);
}

void turn_to_target(Point target) {
  float min_u = 30;
  if (CurrState.holding == Cylinder) {
    min_u = 50;
  }
  float u;
//  float eint = 0;
  float kp = 0.4;
//  float ki = 0.2;
  float target_heading = get_heading_toward(target);
  float heading_diff = get_heading_difference(target_heading);
  while (abs(heading_diff) > 5) {
    update_vive();
//    eint = eint + heading_diff; //error sum
//    if (eint > EINTMAX) {
//      eint = EINTMAX;
//    } else if (eint < -EINTMAX) {
//      eint = -EINTMAX;
//    }
////    u = kp * heading_diff + ki * eint;
    u = kp * heading_diff;
    if (u < 0) {
      u -= min_u;
    } else {
      u += min_u;
    }
    turn_robot((int)u);
//    print_CurrState();

    
//    Serial.print(eint);
////    Serial.print(" ");
    Serial.print(CurrState.heading);
    Serial.print(" ");
    Serial.print(heading_diff);
    Serial.print(" ");
    Serial.print(target_heading);
    Serial.print(" ");
    Serial.println(u);
    target_heading = get_heading_toward(target);
    heading_diff = get_heading_difference(target_heading);
  }
}

bool block_seen() {
  float scan_result = scan();
  return scan_result >= scan_threshold;
}


