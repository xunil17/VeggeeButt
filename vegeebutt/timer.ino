#define stop_early 1000
#define competition_length 5000 // 3 * 60 * 1000 = 180000

void check_time() {
  unsigned long current_time = millis();
  if (current_time >= competition_length || competition_length - current_time > stop_early) {
    stop_everything();
    sleep_forever();
  }
}

void stop_everything() {
  stop_robot();
  gripper_stop();
  delay(100);
}

void sleep_forever() {
  while(1) {}
}

