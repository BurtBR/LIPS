clc; clear; close all;

%% Intrinsic parameters
params = load('params.mat').params.Intrinsics;

% Laser1 (X,Y,Z) = (  70,  70, 267 ) cm -> ( 728, 233) px
% Laser2 (X,Y,Z) = (  70, 120, 267 ) cm -> ( 734, 423 ) px
% Laser3 (X,Y,Z) = ( 120,  70, 267 ) cm -> ( 917, 231) px

L1 = [ 70 728;
       70 233;
      267   1];

L2 = [ 70 734;
      120 423;
      267   1];

L3 = [120 917;
       70 231;
      267   1];

% Select LASER
Laser = L3;

% CameraPosition
t = [70;120;4.5];

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

for alpha = 0:pi/10:2*pi
    for beta = 0:pi/10:2*pi
        for gamma = 0:pi/10:2*pi
            a = alpha;
            b = beta;
            g = gamma;
            %% Rotation Matrix (split into pitch, yaw and roll)
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

            %% R0 Matrix
            R0 = [R zeros(3,1)];

            %% Estimating s
            X = K*[R t]*P;
            s_avg = X(3)/p_im(3);

            if alpha==0 && beta==0 && gamma==0 % Initial value
                obj = (inv(K)*s_avg*p_im - R0*P);
            else
                obj = [obj (inv(K)*s_avg*p_im - R0*P)]; % Append points
            end
        end
    end
end

figure();
plot3(obj(1,:), obj(2,:), obj(3,:), 'LineWidth', 5);grid on;hold on;view([64 0]);
plot3([-500 500], [0 0], [0 0], "black");
plot3([0 0], [-500 500], [0 0], "black");
plot3([0 0], [0 0], [-500 500], "black");
scatter3(t(1),t(2),t(3),'filled', "green");
text(t(1)+10,t(2),t(3), "Expected position");

