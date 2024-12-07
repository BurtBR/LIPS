clc; clear; close all;

main();

function main()
    % Intrinsic parameters
    params = load('params.mat').params.Intrinsics;
    
    % Laser1 (X,Y,Z) = (  70,  70, 267 ) cm -> ( 728, 233) px
    % Laser2 (X,Y,Z) = (  70, 120, 267 ) cm -> ( 734, 423 ) px
    % Laser3 (X,Y,Z) = ( 120,  70, 267 ) cm -> ( 917, 231) px

    % Corrected Distortion
    % Laser1 (X,Y,Z) = (  70,  70, 267 ) cm -> ( 728, 231) px
    % Laser2 (X,Y,Z) = (  70, 120, 267 ) cm -> ( 733, 423 ) px
    % Laser3 (X,Y,Z) = ( 120,  70, 267 ) cm -> ( 923, 226) px
    
    L1 = [ 70 728;
           70 231;
          267   1];
    
    L2 = [ 70 733;
          120 423;
          267   1];
    
    L3 = [120 923;
           70 226;
          267   1];
    
    % CameraPosition
    t = [70;120;4.5];
    
    K = params.K; % Intrinsic matrix [fx, fy in mm and cx, cy in pixel]

    P = [];
    p_im = [];
    R = [];
    R0 = [];
    
    % Alpha (0-360), Beta (0-360), Gamma (0-360)
    ShowAllAnglePoints(0:20:360, 0:20:360, 0:20:360);

    ShowAllSimultaneousSolutions(0:5:360, 0:5:360, 0:5:360, 20);
    
    function distance = DistanceP(P1, P2)
        distance = sqrt((P1(1)-P2(1))^2 + (P1(2)-P2(2))^2 + (P1(3)-P2(3))^2);
    end

    function value = CalculateT()
        value = (inv(K)*CalculateS()*p_im - R0*P);
    end

    function value = CalculateS()
        % Estimating s
        value = K*[R t]*P;
        value = value(3)/p_im(3);
    end

    function SetLaser(L)
        P = [L(1,1);L(2,1);L(3,1);1];
        p_im = [L(1,2);L(2,2);L(3,2)];
    end

    function SetRMatrix(alpha, beta, gamma)
        % Rotation Matrix (split into pitch, yaw and roll)
        % https://en.wikipedia.org/wiki/Rotation_matrix
        R = ...%
            ...% Pitch
            [cos(alpha) -sin(alpha) 0; % Pitch
            sin(alpha)  cos(alpha) 0;
            0      0  1] *...
            ... % Yaw
            [ cos(beta) 0 sin(beta);
            0  1     0;
            -sin(beta) 0 cos(beta)] *...
            ... % Roll
            [1     0       0;          %Roll
            0 cos(gamma) -sin(gamma);
            0 sin(gamma)  cos(gamma)]; 
    end

    function ShowAllAnglePoints(alpha, beta, gamma)
        obj1 = [];
        obj2 = [];
        obj3 = [];
        for a = alpha*pi/180
            for b = beta*pi/180
                for g = gamma*pi/180
                    
                    SetRMatrix(a, b, g);
        
                    % R0 Matrix
                    R0 = [R zeros(3,1)];  
    
                    % Laser 1
                    SetLaser(L1);
                    obj1 = [obj1 CalculateT()]; % Append points
    
                    % Laser 2
                    SetLaser(L2);
                    obj2 = [obj2 CalculateT()]; % Append points
    
                    % Laser 3
                    SetLaser(L3);
                    obj3 = [obj3 CalculateT()]; % Append points
    
                end
            end
        end % end for

        figure();
        scatter3(obj1(1,:), obj1(2,:), obj1(3,:), 10, 'o', 'red');grid on;hold on;
        scatter3(obj2(1,:), obj2(2,:), obj2(3,:), 10, 'o', 'green');
        scatter3(obj3(1,:), obj3(2,:), obj3(3,:), 10, 'o', 'blue');
        scatter3(t(1),t(2),t(3), 100,'filled', "magenta");
        handler = text(t(1),t(2),t(3), "  <---- Expected position");
        set(handler, 'Rotation', 90);
        zlim([t(3)-1 t(3)+1]);
    end

    function ShowAllSimultaneousSolutions(alpha, beta, gamma, maxerror)
        bestresult = [];
        allresults = [];
        for a = alpha*pi/180
            for b = beta*pi/180
                for g = gamma*pi/180
                    
                    SetRMatrix(a, b, g);
        
                    % R0 Matrix
                    R0 = [R zeros(3,1)];  
    
                    % Laser 1
                    SetLaser(L1);
                    t1 = CalculateT();
    
                    % Laser 2
                    SetLaser(L2);
                    t2 = CalculateT();
    
                    % Laser 3
                    SetLaser(L3);
                    t3 = CalculateT();

                    error = (DistanceP(t1, t) + DistanceP(t2, t) + DistanceP(t3, t))/3;
                    if(error < maxerror)
                        allresults = [allresults [a;b;g]*180/pi];
                        if isempty(bestresult)
                            disp("BEST RESULT Error = " + error + " Alpha = " + a*180/pi + " Beta = " + b*180/pi + " Gamma = " + g*180/pi);
                            bestresult = [[a;b;g]*180/pi;error];
                        elseif(bestresult(4) > error)
                                bestresult = [[a;b;g]*180/pi;error];
                                disp("BEST RESULT Error = " + error + " Alpha = " + a*180/pi + " Beta = " + b*180/pi + " Gamma = " + g*180/pi);
                        end
                    end
    
                end
            end
        end % end for

        if(~isempty(allresults))
            figure();
            scatter3(allresults(1,:), allresults(2,:), allresults(3,:), 'o', 'blue');
            grid on;hold on;
            scatter3(bestresult(1), bestresult(2), bestresult(3), 200,'filled', 'red');
            handler = text(bestresult(1), bestresult(2), bestresult(3), "  <---- Best Result");
            set(handler, 'Rotation', 90);
            xlabel("Alpha");
            xlim([alpha(1) alpha(end)]);
            ylabel("Beta");
            ylim([beta(1) beta(end)]);
            zlabel("Gamma");
            zlim([gamma(1) gamma(end)]);
            view(162, 13);
            set(gca,'XTick',alpha(1):(alpha(end)-alpha(1))/6:alpha(end));
            set(gca,'YTick',beta(1):(beta(end)-beta(1))/6:beta(end));
            set(gca,'ZTick',gamma(1):(gamma(end)-gamma(1))/6:gamma(end));
            title("Error: " + bestresult(4) + "cm Alpha: " + bestresult(1) + "° Beta: " + bestresult(2) + "° Gamma: " + bestresult(3) + "°");
        end
        
    end

end

