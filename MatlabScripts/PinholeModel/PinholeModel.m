clear all; close all; clc

%% Intrinsic parameters
params = load('params.mat').params.Intrinsics;

%% Pinhole
% sp = K[R|t]P

% R0 = [R zeros(3,1)]
% T0 = [zeros(3,3) t]

% sp = K(R0 + T0)P
% sp = K*R0*P + K*T0*P

% K*R0*P results in 3x1 vector
% K  -> 3x3
% R0 -> 3x4
% P  -> 4x1

% K*T0*P also results in 3x1 vector
% K  -> 3x3
% T0 -> 3x4
% P  -> 4x1

% Results in 3x1 vector
% sp - K*R0*P = K*T0*P

% inv(K)*(sp - K*R0*P) = T0*P

% inv(K)*sp - R0*P = T0*P

% Non-square matrices (m-by-n matrices for which m ≠ n) do not have an
% inverse. So T0 and P has no inverse.

% T0*P = [t1 ; t2 ; t3] = t

% Image captured with the camera in the real position (70, 120, 4.5) cm

% Laser1 (X,Y,Z) = (  70,  70, 267 ) cm -> ( 728, 233) px
% Laser2 (X,Y,Z) = (  70, 120, 267 ) cm -> ( 734, 423 ) px
% Laser3 (X,Y,Z) = ( 120,  70, 267 ) cm -> ( 917, 231) px

L1 = [ 700 728;
       700 233;
      2670   1]; % mm px

L2 = [ 700 734;
      1200 423;
      2670   1]; % mm px

L3 = [1200 917;
       700 231;
      2670   1]; % mm px

% Select LASER
Laser = L3;

% CameraPosition
t = [700;1200;45]; % mm

% Real LASER coordinates
X = Laser(1,1);
Y = Laser(2,1);
Z = Laser(3,1);
P = [ X Y Z 1]';

% Image LASER coordinates
u = Laser(1,2);
v = Laser(2,2);
p_im = [u;v;Laser(3,2)];

K = params.K; % Intrinsic matrix [fx, fy in mm and cx, cy in pixel]

a = 0; % Angle Alpha
b = 0; % Angle Beta
g = 0; % Angle Gamma

%% Rotation Matrix 1 (split into pitch, yaw and roll)
% https://en.wikipedia.org/wiki/Rotation_matrix
R = [cos(a) -sin(a) 0;
     sin(a)  cos(a) 0;
          0      0  1] *...
    [ cos(b) 0 sin(b);
          0  1     0;
     -sin(b) 0 cos(b)] *...
     [1     0       0;
      0 cos(g) -sin(g);
      0 sin(g)  cos(g)];
%% Rotation Matrix 2
% https://pt.wikipedia.org/wiki/Matriz_de_rota%C3%A7%C3%A3o#:~:text=Uma%20matriz%20de%20rota%C3%A7%C3%A3o%20%C3%A9,definido%20pelos%20elementos%20da%20matriz%3B
%R = [cos(a)*cos(g)-sin(a)*cos(b)*sin(g) sin(a)*cos(g)+cos(a)*cos(b)*sin(g)  sin(b)*sin(g);
%    -cos(a)*sin(g)-sin(a)*cos(b)*cos(g) -sin(a)*sin(g)+cos(a)*cos(b)*cos(g) sin(b)*cos(g);
%    sin(a)*sin(b)                       -cos(a)*sin(b)                      cos(b)];

%% R0 Matrix
R0 = [R zeros(3,1)];

%% If you want to check s values:
%X = K*[R t]*P;
%s = [X(1)/p_im(1) X(2)/p_im(2) X(3)/p_im(3)];
%disp("s = " + s);

%% Estimating s
X = K*[R t]*P;
s_avg = X(3)/p_im(3);

%% Estimating Position
disp("t = " + (inv(K)*s_avg*p_im - R0*P));

disp("camera = " + t);