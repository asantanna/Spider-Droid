%
% Calc best linear interp from a present patch to a future pixel
%

clear all;

REMOVE_GLOBAL_MEAN = 1;
REMOVE_PATCH_MEAN = 0;      % DEBUG ONLY
NORMALIZE_PATCH = 0;        % DEBUG ONLY

fprintf('********************* start noise assess *************************\n');

% read grayscale movie

dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\San Diego Fashion\';
fname = 'Fashion-snippet-tiny';

% dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\';
% fname = 'Tiger Snippet';
% fname = 'Tiger Snippet+percep+n10%';
% fname = 'Tiger Snippet+percep+n20%';

fext = '.avi';
fpath = [dir fname fext];

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

% convert to double
data = double(data);

% scale globally
orig_globalMin = min(data(:));
orig_globalMax = max(data(:));
data = data - orig_globalMin;
data = data ./ (orig_globalMax - orig_globalMin);

% remove global mean if requested
orig_globalMean = 0;
if (REMOVE_GLOBAL_MEAN == 1)
    orig_globalMean = mean(data(:));
    data = data - orig_globalMean;
end

% core the movie several times

numCores = 500;
fprintf('Coring %d times ...\n', numCores);

nR = size(data,1);
nC = size(data,2);

radP = 6;
nP = 2*radP + 1;
centerOff = (radP * nP) + radP + 1;

maxR = nR - nP + 1;
maxC = nC - nP + 1;

acc = [];

for i = 1:numCores
    
    coreR = 1 + floor(rand * maxR);
    coreC = 1 + floor(rand * maxC);    

    core = data(coreR:coreR+nP-1, coreC:coreC+nP-1, :);

    % convert to img vectors
    imgV = reshape(core, nP*nP, []);

    if (REMOVE_PATCH_MEAN == 1)
        % remove mean
        imgV = imgV - repmat(mean(imgV),size(imgV,1),1);
    end
    
    if (NORMALIZE_PATCH == 1)
        % HACK - add micro mean because some frames may be all zeros
        imgV = imgV + 0.1;

        % normalize columns
        imgV = normc(imgV);
    end
    
    % tack on
    acc = [acc imgV];
    
end
    
imgV = acc;  

% build matrix A with frames 1:end-1 as ROWS
A = imgV(:,1:end-1)';

% build matrix A2 with frames 2:end as ROWS but with no center pixel
% and extend rows of A with it
A2 = imgV(:,2:end)';
A2(:, centerOff) = 0;
A = [A A2];

% build vector b with patch centers of frames 2:end
b = imgV((radP * nP) + radP + 1, 2:end)';

% we seek the linear interpolator x that best predicts the next frame's
% center pixel using the current frame's patch ...
% In linear algebra terms:
%   We want the best (least squares) solution for x in Ax = b
%   The solution is given by x = pinv(A) * b

fprintf('Calculating pseudo-inverse ... \n');
inv_A = pinv(A);

fprintf('Solving ... \n');
x = inv_A * b;

% write out interpolator
fprintf('Saving interpolator ...\n');
ip = reshape(x, nP, []);
ip_fpath = [dir 'interp-' fname];
save(ip_fpath, 'ip', 'orig_globalMean', 'orig_globalMin', 'orig_globalMax');

% show contributions of frame-1 and frame(no center)
figure(1);
subplot(131);
amax = max(abs(ip(:)));
imagesc(ip,[-amax amax]);
colormap (txPosNegCmap);
axis image
title(sprintf('Recep at t-1 and t\npatch radius=%g\nnum patches=%g', radP, numCores * size(data,3)));

subplot(132);
v = ip(radP+1,:);
stem(v);

subplot(133);
ip_comp = reshape(ip,nP, nP, []);
ip_merged = sum(ip_comp,3);
v = ip_merged(radP+1,:);
stem(v);

% show "receptive fields"
figure(2);
surf(ip);

fprintf('Done.\n');



