//#include <Matrix.h>
#include <BasicLinearAlgebra.h>

using namespace BLA;

typedef struct Point_ {
  float x;
  float y;
} Point;

// directional notation is relative to facing the board
//   landscape when the BLUE circle is on the LEFT
// calibration globals:
const Point BL = {-83,-81};
const Point BR = {65,-68};
const Point TL = {-87,65};
const Point TR = {56,70};
const float val = 72;
const Point bl = {-val, -val};
const Point br = {val, -val};
const Point tl = {-val, val};
const Point tr = {val, val};

void setup() {
  Serial.begin(9600);

}

void loop() {
  compute_vive_transformation();
  delay(10000);

}

// BL, BR, TL, TR

void compute_vive_transformation() {

  BLA::Matrix<8,8> A= {
    bl.x, bl.y, 1, 0, 0, 0, -BL.x * bl.x, -BL.x * bl.y,
    0, 0, 0, bl.x, bl.y, 1, -BL.y * bl.x, -BL.y * bl.y,
    br.x, br.y, 1, 0, 0, 0, -BR.x * br.x, -BR.x * br.y,
    0, 0, 0, br.x, br.y, 1, -BR.y * br.x, -BR.y * br.y,
    tl.x, tl.y, 1, 0, 0, 0, -TL.x * tl.x, -TL.x * tl.y,
    0, 0, 0, tl.x, tl.y, 1, -TL.y * tl.x, -TL.y * tl.y,
    tr.x, tr.y, 1, 0, 0, 0, -TR.x * tr.x, -TR.x * tr.y,
    0, 0, 0, tr.x, tr.y, 1, -TR.y * tr.x, -TR.y * tr.y};
  
  BLA::Matrix<8,1> B = {
    BL.x,
    BL.y,
    BR.x,
    BR.y,
    TL.x,
    TL.y,
    TR.x,
    TR.y};

    BLA::Matrix<8,8> A_inv = A.Inverse();
    BLA::Matrix<8,1> sol = A_inv * B;
    
    Serial << "A: " << sol << '\n';
  

}
