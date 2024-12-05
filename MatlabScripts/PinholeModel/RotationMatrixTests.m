clc; clear; close all;

main();

function main()
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
    
    % CameraPosition
    t = [70;120;4.5];
    
    K = params.K; % Intrinsic matrix [fx, fy in mm and cx, cy in pixel]

    obj1 = [];
    obj2 = [];
    obj3 = [];
    P = [];
    p_im = [];
    
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

                %% Laser 1
                SetLaser(L1);
                obj1 = [obj1 CalculateT()]; % Append points

                %% Laser 2
                SetLaser(L2);
                obj2 = [obj2 CalculateT()]; % Append points

                %% Laser 3
                SetLaser(L3);
                obj3 = [obj3 CalculateT()]; % Append points

            end
        end
    end
    
    figure();
    scatter3(obj1(1,:), obj1(2,:), obj1(3,:), 'filled', 'o', 'red');grid on;hold on;
    scatter3(obj2(1,:), obj2(2,:), obj2(3,:), 'filled', 'o', 'green');
    scatter3(obj3(1,:), obj3(2,:), obj3(3,:), 'filled', 'o', 'blue');
    scatter3(t(1),t(2),t(3),'filled', "magenta");
    text(t(1)+10,t(2),t(3), "Expected position");

    function value = CalculateT()
        value = (inv(K)*CalculateS()*p_im - R0*P);
    end

    function value = CalculateS()
        %% Estimating s
        value = K*[R t]*P;
        value = value(3)/p_im(3);
    end

    function SetLaser(L)
        P = [L(1,1);L(2,1);L(3,1);1];
        p_im = [L(1,2);L(2,2);L(3,2)];
    end
end

