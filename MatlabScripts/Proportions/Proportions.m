clear; close all; clc;

% With Distortion
% Laser1 (X,Y,Z) = (  70,  70, 267 ) cm -> ( 728, 233) px
% Laser2 (X,Y,Z) = (  70, 120, 267 ) cm -> ( 734, 423 ) px
% Laser3 (X,Y,Z) = ( 120,  70, 267 ) cm -> ( 917, 231) px

% Corrected Distortion
% Laser1 (X,Y,Z) = (  70,  70, 267 ) cm -> ( 728, 231) px
% Laser2 (X,Y,Z) = (  70, 120, 267 ) cm -> ( 733, 423 ) px
% Laser3 (X,Y,Z) = ( 120,  70, 267 ) cm -> ( 923, 226) px

L1real = [ 70;
           70;
          267];

L2real = [ 70;
          120;
          267];

L3real = [120;
           70;
          267];

L1px = [728;
        231;
          1];

L2px = [733;
        423;
          1];

L3px = [923;
        226;
         1];

t = [70;120;4.5]; % Camera Position

params = load('params.mat').params.Intrinsics;

Cx = params.PrincipalPoint(1);
Cy = params.PrincipalPoint(2);
C = [Cx; Cy; 1]; % Principal Point
cameraAOV = pi*(65/180); %Camera angle of view in radians

imagesize = params.ImageSize;

% Distance between 2 anchors in real coordinates and pixels
d2preal = PDistance(L1real, L2real);
d2px = PDistance(L1px, L2px);

% Ratio between real system and pixels
cm_by_px = d2preal/d2px;

% Captured width size in real coordinates
realsize = imagesize*cm_by_px;

% Pythagoras
h = (realsize(2)/(2*tan(cameraAOV/2)));
disp("h = " + h);
disp("Estimated Height = " + (L1real(3)-h));
disp("Error: " + (sqrt( ((L1real(3)-h) - t(3))^2 )));

disp(" ");

% Using proportions in the image to find the coordinate of 
% the principal point in the real system
position = ((C-L1px)*cm_by_px)+L1real;
disp("Calculated Position = (" + position(1) + ";" + position(2) + ")");
error = abs(t(1:2)-position(1:2));
disp("Error = (" + error(1) + ";" + error(2) + ")");

function d = PDistance(P1, P2)
    d = sqrt( (P1(1)-P2(1))^2 + (P1(2)-P2(2))^2 + (P1(3)-P2(3))^2 );
end
