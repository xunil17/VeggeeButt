
void plan() {
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
        if (block_type == None) {
          
        } else if (block_type == Cylinder) {
          Serial.println("Cylinder!");
        } else {
          Serial.println("Cube!");
        }
        return;
      }
    } while(block_seen());
  }
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

