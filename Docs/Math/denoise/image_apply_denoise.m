%{

Apply linear interp from image_interp() to denoise

The motivation of this routine is to try to remove "noise"
from an image formed on photoreceptors.

It assumes that photoreceptors have a higher slew rate than
we care about later on.  In other words, the image is sampled
in the time domain faster than we need. This, in turn, means that
we can afford to "blur" the image in the time domain because 
we won't be sampling that fast anyway.

Thus, we can linearly interpolate a pixel from several previous
patches and use the interpolated value instead.  Because this
will involve many pixels, we tend to average out the noise.

Note that this does not work with a simple
%}

fprintf('********************* start apply denoise *************************\n');

clear all

dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\';
% fname = 'Tiger Snippet';
fname = 'Tiger Snippet+percep+n10%';

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

% convert to double
data = double(data);

% load interp
fprintf('Loading denoise interp ''%s'' ...\n', ip_fname);
load(ip_fpath);

% remove mean and scale like the interp wants it
data = data - orig_globalMin;
data = data ./ (orig_globalMax - orig_globalMin);
data = data - orig_globalMean;

% convert interpolator to a vector
ipV = ip(:);

%%
% Apply denoise interp

fprintf('Interpolating ...\n');

DISP_EVERY_SECS = 10;
lastTime = 0;

totPix = numel(data);
nP = size(ip,1);
radP = (nP - 1) / 2;

maxR = size(data,1) - nP + 1;
maxC = size(data,2) - nP + 1;
maxF = size(data,3);

pixIdx = 0;
interpData = zeros(maxR, maxC, maxF-1);

for f = 2:maxF
    
    if (cputime - lastTime > DISP_EVERY_SECS)
        % time to print a msg
        fprintf('doing frame %g of %g ...\n', f-1, maxF-1);
        lastTime = cputime;
    end
    
    for r = 1:maxR
        for c = 1:maxC
            % grab 2 frame patch
            p = data(r:r+nP-1, c:c+nP-1, f-1:f);
            % convert to img vector
            v = p(:);
            % interpolate pixel
            interpData(r,c,f-1) = v' * ipV;
        end
    end
end

%% scale back and restore mean to create movie
interpData = interpData + orig_globalMean;
interpData = interpData .* (orig_globalMax - orig_globalMin);
interpData = interpData + orig_globalMin;

%% create output movie

MOV = txCreateMOV(interpData, rdr.FrameRate, @gray);
fname_out = [fname '+interp' fext];
fpath_out = [dir fname_out];
movie2avi(MOV.mov, fpath_out, 'fps', MOV.rate, 'colormap', gray(256), 'compression', 'none', 'quality', 100);
fprintf('Movie ''%s'' created.\n', fname_out);
