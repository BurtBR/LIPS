clear all; close all; clc;

% -----------------------------------
% Variables
% -----------------------------------
% Input Image (With distortion)
imgfilename = "DistortedFrame.jpg";
% -----------------------------------

% Read Image and convert to grayscale
imagem = im2gray(imread(imgfilename));
% Initialize the result with zeros
fixedimage = uint8(zeros(size(imagem)));
% Load calibrated camera parameters
params = load('params.mat').params.Intrinsics;

% Show original image
figure('Name','Imagem Original','NumberTitle','off'); imshow(imagem);

% Loops through the image coordinates without distortion, performing mapping
for y = 1:size(fixedimage,1)
    for x = 1:size(fixedimage,2)
        % Calculate Map
        coord = calculatepositions(x, y, params);
        % Checks if it is within the limits of the distorted image
        if((coord(1) > 0 && coord(1) <= size(imagem,2)) && (coord(2) > 0 && coord(2) <= size(imagem,1)))
            % Map
            fixedimage(y,x) = imagem(coord(2),coord(1));
        end
    end
end

% Show results
figure('Name','Imagem Corrigida','NumberTitle','off'); imshow(fixedimage);

% Mapping function
function out = calculatepositions(x, y, intrinsics)

    % Parameters
    fx = intrinsics.K(1,1);
    fy = intrinsics.K(2,2);
    cx = intrinsics.K(1,3);
    cy = intrinsics.K(2,3);
    k1 = intrinsics.RadialDistortion(1);
    k2 = intrinsics.RadialDistortion(2);
    x = double(x);
    y = double(y);

    % r squared
    r2 = ((x-cx)/fx)^2 + ((y-cy)/fy)^2;

    % equation
    out = round((1 + k1*r2 + k2*r2*r2)*[(x-cx) (y-cy)]+ [cx cy]);
end