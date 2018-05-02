%%capital letters represent data collected at four corners
X1 = -83;
Y1 = -81;

X2 = 65;
Y2 = -68;

X3 = -87;
Y3 = 65;

X4 = 56;
Y4 = 70;

%%lowercase letters represent ideal location
x1 = -72;
x2 = 72;
x3 = -72;
x4 = 72;

y1 = -72;
y2 = -72;
y3 = 72;
y4 = 72;

%%A is the left side of the equation
A = [x1 y1 1 0 0 0 -X1*x1 -X1*y1;
 0 0 0 x1 y1 1 -Y1*x1 -Y1*y1;
 x2 y2 1 0 0 0 -X2*x2 -X2*y2;
 0 0 0 x2 y2 1 -Y2*x2 -Y2*y2;
 x3 y3 1 0 0 0 -X3*x3 -X3*y3;
 0 0 0 x3 y3 1 -Y3*x3 -Y3*y3;
 x4 y4 1 0 0 0 -X4*x4 -X4*y4;
 0 0 0 x4 y4 1 -Y4*x4 -Y4*y4];
 
%B is right of the equation
B = [X1, Y1, X2, Y2, X3, Y3, X4, Y4]';

%solve for solution 
sol = (A^-1)*B;

%reformat solution into new and invert it
new = [sol(1) sol(2) sol(3);
       sol(4) sol(5) sol(6);
       sol(7) sol(8) 1]^-1;

%input is the weird quadrilaterial that we get
input = [20;50;1];

%output is coordinates of new system
out = new*input;

%have to transform from 3-d to 2-d system
x_out = out(1)/out(3)
y_out = out(2)/out(3)