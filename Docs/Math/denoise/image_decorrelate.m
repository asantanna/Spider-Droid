%{

Apply linear interp from image_interp() to decorrelate

The idea is to subtract the best interp from the actual pixel.
The result must be decorrelated otherwise the interp wouldn't 
have been the best ...

%}

clear all

% optionally crop images
% usually because there might be text on the edges that
% is not represented properly by the sampling that was
% done by the interpolator ... hopefully preventing too
% many outliers that tend to make the movie all gray

CROP_BORDER = 40;

fprintf('********************* start decorrelation *************************\n');

dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\';
fname = 'Tiger Snippet';
% fname = 'Tiger Snippet+percep+n10%';
% fname = 'Tiger Snippet+percep+n20%';

fext = '.avi';
fpath = [dir fname fext];

ip_fname = ['interp-' fname];
ip_fpath = [dir ip_fname];

% read file
fprintf('Reading ''%s''...\n', [fname fext]);
rdr = mmreader(fpath);
data = read(rdr);

% hack
if (size(data,4) ~= 1) 
    % avi files not being converted to grayscale properly
    % so extract green plane
    data = squeeze(data(:,:,2,:));
end

% crop if requested
if (CROP_BORDER ~= 0)
    data = data(CROP_BORDER:end-CROP_BORDER, CROP_BORDER:end-CROP_BORDER, :);
end

% convert to double
data = double(data);

% load interp
fprintf('Loading denoise interp ''%s'' ...\n', ip_fname);
load(ip_fpath);

% compute some stuff

nP = size(ip,1);
radP = (nP - 1) / 2;

nR = size(data,1);
nC = size(data,2);

maxR = nR - nP + 1;
maxC = nC - nP + 1;
maxF = size(data,3);

% remove mean and scale like the interp wants it
data = data - orig_globalMin;
data = data ./ (orig_globalMax - orig_globalMin);
data = data - orig_globalMean;

% convert interpolator to decorrelator
decorr = -ip;
decorr(radP+1, nP+radP+1, end) = decorr(radP+1, nP+radP+1, end) + 1;

% convert decorr to vector
decorrV = decorr(:);

%%
% Apply decorr

fprintf('Decorrelating ...\n');

DISP_EVERY_SECS = 4;
lastTime = 0;

decorrData = zeros(maxR, maxC, maxF-1);

for f = 1:maxF-1
    
    if (cputime - lastTime > DISP_EVERY_SECS)
        % time to print a msg
        fprintf('doing frame %g of %g ...\n', f, maxF-1);
        lastTime = cputime;
    end
    
    for r = 1:maxR
        for c = 1:maxC
            % grab 2 frame patch
            p = data(r:r+nP-1, c:c+nP-1, f:f+1);
            % convert to img vector
            v = p(:);
            % decorrelate pixel
            decorrData(r,c,f) = v' * decorrV;
        end
    end
end


%% show auto correlation
autoCorr = [txAutoCorrMovie(decorrData,10) txAutoCorrMovie(decorrData,10, 'vertical')];
figure(1);
plot(autoCorr);
a = axis;
a(3) = -1;
a(4) = 1;
axis(a);
title(sprintf('Final Horiz and Vert Auto-Corr\n(should be zero)'));

%% stop for now
% error('stop here');

%% create output movie

movData = decorrData;

if (true)
    % linear scale to [0,255]
    if (false)
        % full range
        minVal = min(movData(:));
        maxVal = max(movData(:));
    else
        % hardwired range
        % minVal = -0.2;
        % maxVal =  0.2;
        minVal = -0.025;
        maxVal =  0.025;
    end
    
    movData = movData - minVal;
    movData = movData .* (255/(maxVal-minVal));
    
    movData(movData<0) = 0;
    movData(movData>255) = 255;
end

MOV = txCreateMOV(movData, rdr.FrameRate, @gray);
fname_out = [fname '+decorr' fext];
fpath_out = [dir fname_out];
movie2avi(MOV.mov, fpath_out, 'fps', MOV.rate, 'colormap', gray(256), 'compression', 'none', 'quality', 100);
fprintf('Movie ''%s'' created.\n', fname_out);

%% show smoothed decorrelator
figure(2);
% interpolate in between pixels for prettiness
INTERP_STEP = 0.2;
[xx yy] = meshgrid(1:INTERP_STEP:size(decorr,2),1:INTERP_STEP:size(decorr,1));
surf(interp2(decorr,xx,yy))

