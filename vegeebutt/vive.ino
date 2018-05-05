// BL, BR, TL, TR
//
//void compute_vive_transformation() {
//
//  float a[8][8] = {
//    {bl.x, bl.y, 1, 0, 0, 0, -BL.x * bl.x, -BL.x * bl.y},
//    {0, 0, 0, bl.x, bl.y, 1, -BL.y * bl.x, -BL.y * bl.y},
//    {br.x, br.y, 1, 0, 0, 0, -BR.x * br.x, -BR.x * br.y},
//    {0, 0, 0, br.x, br.y, 1, -BR.y * br.x, -BR.y * br.y},
//    {tl.x, tl.y, 1, 0, 0, 0, -TL.x * tl.x, -TL.x * tl.y},
//    {0, 0, 0, tl.x, tl.y, 1, -TL.y * tl.x, -TL.y * tl.y},
//    {tr.x, tr.y, 1, 0, 0, 0, -TR.x * tr.x, -TR.x * tr.y},
//    {0, 0, 0, tr.x, tr.y, 1, -TR.y * tr.x, -TR.y * tr.y},
//  };
//  
//  float b[8][1] = {
//    {BL.x},
//    {BL.y},
//    {BR.x},
//    {BR.y},
//    {TL.x},
//    {TL.y},
//    {TR.x},
//    {TR.y},
//  };
//  
//  Matrix<float> A(8,8,(float*)a);
//  Matrix<float> B(8,1,(float*)b);
//  Matrix<float> sol = Matrix<float>::inv(A) * B;
//  Matrix<float>::inv(A).show(); // inv doesn't seem to work
//
//}
