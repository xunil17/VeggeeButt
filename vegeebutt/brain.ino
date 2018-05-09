
void plan() {
  Serial.println("Entering plan()");
  find_and_go_to_block(); // look for and get to the block
  
//  Block block_type = grab_and_identify();
//  if (block_type == None) {
//    
//  } else if (block_type == Cylinder) {
//    Serial.println("Cylinder!");
//  } else {
//    Serial.println("Cube!");
//  }
}

void find_and_go_to_block() {
  Serial.println("Entering find_and_go_to_block()");
  open_gripper_max(); // reset gripper before trying to find other stuff
  turn_right(); // turn right by default
  if (block_seen()) {
    delay(50);
    stop_robot();
    delay(500);

    // move toward block unless it moves out of sight
    do {
      move_forward(120);
      
      // stop when block is within our grasp
      if (hit) {
        stop_robot();
        hit = false;
        Block block_type = grab_and_identify();
        handle_block(block_type);
        return;
      }
    } while(block_seen());
  }
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
    return;
  }
}

void handle_cylinder() {
  move_forward(10);
  move_gripper(220); // tighten grip on slipper cylinder
  turn_robot(100);
  delay(2000);
}

void handle_cube() {
  turn_robot(-50);
  delay(2000);
}

//bool front_bumped() {
//  Serial.print(bumped_left());
//  Serial.print(" ");
//  Serial.println(bumped_right());
//  return bumped_left() || bumped_right();
//}
//
//bool bumped_left() {
//  return digitalRead(button_front1) == HIGH;
//}
//
//bool bumped_right() {
//  return digitalRead(button_front2) == HIGH;
//}

bool block_seen() {
  float scan_result = scan();
  return scan_result >= scan_threshold;
}

