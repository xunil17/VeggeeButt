


void plan() {
  turn_right();
  if (block_seen()) {
    delay(50);
    stop_robot();
    delay(500);
    while(block_seen()) {
      move_forward(150);
    }
  }




  
  delay(50);
}

bool block_seen() {
  float scan_result = scan();
  return scan_result >= scan_threshold;
}

