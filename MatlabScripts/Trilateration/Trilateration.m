clear; close all; clc;

main(1) % Ideal
%main(2) % Anchor on top of position
%main(3) % No anchor forms a triangle with position
%main(4) % No anchor forms triangle with position, no measurement error
%main(5) % Y-aligned, collinear
%main(6) % X-aligned, collinear
%main(7) % Y-aligned, collinear, no measurement error
%main(8) % Equal, collinear anchors
%main(9) % P misaligned collinear anchors

function main(situation)

    switch situation
        case 1
            L1 = [ 20;
                  100];
    
            L2 = [120;
                   80];
            
            L3 = [50;
                  50];
            
            P = [ 70;
                 120];

            sigma = 5;
            plottitle = "Case 1: Ideal";
            disp("------ Case 1 ------");

        case 2
            L1 = [ 70;
                   70];
            
            L2 = [ 70;
                  120];
            
            L3 = [120;
                   70];
            
            P = [ 70;
                 120];

            sigma = 5;
            plottitle = "Case 2: Anchor on top of position";
            disp("------ Case 2 ------");

        case 3
            L1 = [ 70;
                   70];
            
            L2 = [120;
                  120];
            
            L3 = [ 20;
                  120];
            
            P = [ 70;
                 120];

            sigma = 5;
            plottitle = "Case 3: No anchor forms a triangle with position";
            disp("------ Case 3 ------");

        case 4
            L1 = [ 70;
                   70];
            
            L2 = [120;
                  120];
            
            L3 = [ 20;
                  120];
            
            P = [ 70;
                 120];

            sigma = 0;
            plottitle = "Case 4: No anchor forms triangle with position, no measurement error";
            disp("------ Case 4 ------");

        case 5
            L1 = [ 70;
                   90];
            
            L2 = [ 70;
                  100];
            
            L3 = [ 70;
                  140];
            
            P = [ 70;
                 120];

            sigma = 5;
            plottitle = "Case 5: Y-aligned, collinear";
            disp("------ Case 5 ------");

        case 6
            L1 = [ 20;
                  120];
            
            L2 = [ 50;
                  120];
            
            L3 = [ 120;
                  120];
            
            P = [ 70;
                 120];

            sigma = 5;
            plottitle = "Case 6: X-aligned, collinear";
            disp("------ Case 6 ------");

        case 7
            L1 = [ 70;
                   90];
            
            L2 = [ 70;
                  100];
            
            L3 = [ 70;
                  140];
            
            P = [ 70;
                 120];

            sigma = 0;
            plottitle = "Case 7: Y-aligned, collinear, no measurement error";
            disp("------ Case 7 ------");

        case 8
            L1 = [ 100;
                   50];
            
            L2 = [100;
                   50];
            
            L3 = [ 50;
                  140];
            
            P = [ 70;
                 120];

            sigma = 5;
            plottitle = "Case 8: Equal, collinear anchors";
            disp("------ Case 8 ------");

        case 9
            L1 = [ 50;
                   50];
            
            L2 = [80;
                  80];
            
            L3 = [150;
                  150];
            
            P = [ 70;
                 120];

            sigma = 0;
            plottitle = "Case 9: P misaligned collinear anchors";
            disp("------ Case 9 ------");

        otherwise
    end

    PrintAnchors();
    scatter(P(1), P(2), "filled", "black");
    text(P(1)-2, P(2)+5, "P");

    dL1 = abs(PDistance(L1, P) + (rand(1)*sigma - sigma/2));
    dL2 = abs(PDistance(L2, P) + (rand(1)*sigma - sigma/2));
    dL3 = abs(PDistance(L3, P) + (rand(1)*sigma - sigma/2));
    
    PrintAnchors();
    viscircles(transpose(L1), dL1, 'Color', 'r', 'LineStyle', ':');
    viscircles(transpose(L2), dL2, 'Color','g', 'LineStyle', ':');
    viscircles(transpose(L3), dL3, 'Color','b', 'LineStyle', ':');

    x = 1; % x index in vector
    y = 2; % y index in vector

    K1 = ( dL2^2 - dL1^2 - L2(x)^2 + L1(x)^2 - L2(y)^2 + L1(y)^2 )/2;
    K2 = ( dL3^2 - dL1^2 - L3(x)^2 + L1(x)^2 - L3(y)^2 + L1(y)^2 )/2;

    Px = (PDy(L1,L2)*K2 - K1*PDy(L1,L3))/( PDy(L1,L2)*PDx(L1,L3) - PDx(L1,L2)*PDy(L1,L3) );

    Py = (K1 - Px*PDx(L1,L2))/PDy(L1,L2);

    scatter(Px, Py, "filled", "magenta");
    text(Px-2, Py+5,'$$\hat{P}$$','Interpreter','Latex');

    disp("Real P: " + P);
    disp("Calculated P: " + [Px;Py]);
    disp("Sigma Measurement = " + sigma);
    disp("Error: " + abs(P-[Px;Py]));
    disp("--------------------");

    function d = PDistance(P1, P2)
        d = sqrt( (P1(1)-P2(1))^2 + (P1(2)-P2(2))^2);
    end
    
    function d = PDx(P1, P2)
        d = (P1(1) - P2(1));
    end
    
    function d = PDy(P1, P2)
        d = (P1(2) - P2(2));
    end

    function PrintAnchors()
        figure();
        scatter(L1(1), L1(2), "filled", "red");hold on;
        text(L1(1)-2, L1(2)+5, "L1");
        scatter(L2(1), L2(2), "filled", "green");
        text(L2(1)-2, L2(2)+5, "L2");
        scatter(L3(1), L3(2), "filled", "blue");
        text(L3(1)-2, L3(2)+5, "L3");
        xlim([0 200]);
        ylim([0 200]);
        title(plottitle);
    end

end