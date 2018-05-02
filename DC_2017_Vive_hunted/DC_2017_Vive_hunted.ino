/*************************************
 * code for the hunted robot
 * as often as possible, read the opponent position from the xbee
 * the UART4 RX pin is 31 (second from the bottom left)
 * the xbee needs 3.3v, gnd, and dout
 * 
 * and read the vive position sensor on pin 24
 * 
*************************************/
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define V1PIN 24 // the signal from front vive sensor
#define V2PIN 25 // signal from back vive sensor
#define DEG_PER_US 0.0216 // equal to (180 deg) / (8333 us)
#define LIGHTHOUSEHEIGHT 6.0 // in feet

// structure to store the sensor data
typedef struct {
  unsigned long changeTime[11];
  double horzAng;
  double vertAng;
  int useMe;
  int collected;
} viveSensor;

// variables for the sensor data and filter
volatile viveSensor V1;
int state = 0;
double xPos, yPos;
double xOld = 0, yOld = 0, xFilt = 0, yFilt = 0;

void setup() {
  Serial.begin(9600); // to talk to the computer
  pinMode(13, OUTPUT); // to blink the led on pin 13
  pinMode(V1PIN, INPUT); // to read the sensor
  pinMode(V2PIN, INPUT);

  // initialize the sensor variables
  V1.horzAng = 0;
  V1.vertAng = 0;
  V1.useMe = 0;
  V1.collected = 0;
  // interrupt on any sensor change
  attachInterrupt(digitalPinToInterrupt(V1PIN), ISRV1, CHANGE);
}

void loop() {
  // if the sensor data is new
  if (V1.useMe == 1) {
    V1.useMe = 0;

    // calculate the position and filter it
    xPos = tan((V1.vertAng - 90.0) * DEG_TO_RAD) * LIGHTHOUSEHEIGHT;
    yPos = tan((V1.horzAng - 90.0) * DEG_TO_RAD) * LIGHTHOUSEHEIGHT;
    xFilt = xOld * 0.5 + xPos * 0.5;
    yFilt = yOld * 0.5 + yPos * 0.5;
    xOld = xFilt;
    yOld = yFilt;
    Serial.print(xFilt);
    Serial.print(" ");
    Serial.println(yFilt);

    // blink the led so you can tell if you are getting sensor data
    digitalWrite(13, state);
    if (state == 1) {
      state = 0;
    }
    else {
      state = 1;
    }
  }
}

// the sensor interrupt
void ISRV1() {
  // get the time the interrupt occured
  unsigned long mic = micros();
  int i;

  // shift the time into the buffer
  for (i = 0; i < 10; i++) {
    V1.changeTime[i] = V1.changeTime[i + 1];
  }
  V1.changeTime[10] = mic;

  // if the buffer is full
  if (V1.collected < 11) {
    V1.collected++;
  }
  else {
    // if the times match the waveform pattern
    if ((V1.changeTime[1] - V1.changeTime[0] > 7000) && (V1.changeTime[3] - V1.changeTime[2] > 7000) && (V1.changeTime[6] - V1.changeTime[5] < 50) && (V1.changeTime[10] - V1.changeTime[9] < 50)) {
      V1.horzAng = (V1.changeTime[5] - V1.changeTime[4]) * DEG_PER_US;
      V1.vertAng = (V1.changeTime[9] - V1.changeTime[8]) * DEG_PER_US;
      V1.useMe = 1;
    }
  }
}



