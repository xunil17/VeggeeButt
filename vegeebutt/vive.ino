// BL, BR, TL, TR
Point compute_transformed_coordinates(Point input) {

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
    TR.y
  };

  BLA::Matrix<8,8> A_inv = A.Inverse();
  BLA::Matrix<8,1> sol = A_inv * B;
  BLA::Matrix<3,3> new_sol = {
    sol(0,0), sol(1,0), sol(2,0), sol(3,0), sol(4,0), sol(5,0), sol(6,0), sol(7,0), 1
  };

  //save this matrix so we don't have to calculate it every time
  BLA::Matrix<3,3> new_sol_inv = new_sol.Inverse();

  BLA::Matrix<3,1> input_mat = {
    input.x, input.y, 1
  };

  BLA::Matrix<3,1> out = new_sol_inv * input_mat;

  float x_out = out(0,0) / out(2,0);
  float y_out = out(1,0) / out(2,0);

  Serial << "Transformed: " << input.x << ", " << input.y << " -> " << x_out << ", " << y_out << '\n';

  return {x_out, y_out};
}
