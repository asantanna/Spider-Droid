%{

Demonstrate that a decorrelated image can be recovered

%}

clear all

fprintf('********************* start recorrelation *************************\n');

dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\';
base_fname = 'Tiger Snippet';
% base_fname = 'Tiger Snippet+percep+n10%';
% base_fname = 'Tiger Snippet+percep+n20%';

fname = [base_fname '+decorr'];
fext = '.avi';
fpath = [dir fname fext];

ip_fname = ['interp-' base_fname];
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

% compute some stuff

nP = size(ip,1);
radP = (nP - 1) / 2;

nR = size(data,1);
nC = size(data,2);

maxR = nR - nP + 1;
maxC = nC - nP + 1;
maxF = size(data,3);

% convert interpolator to decorrelator
decorr = -ip;
decorr(radP+1, nP+radP+1, end) = decorr(radP+1, nP+radP+1, end) + 1;

% convert decorr to vector
decorrV = decorr(:);

%%
% Recorrelate (reverse the decorrelation process)

fprintf('Recorrelating ...\n');

DISP_EVERY_SECS = 4;
lastTime = 0;

recorrData = zeros(maxR, maxC, maxF-1);

for f = 1:maxF-1
    
    if (cputime - lastTime > DISP_EVERY_SECS)
        % time to print a msg
        fprintf('doing frame %g of %g ...\n', f, maxF-1);
        lastTime = cputime;
    end
    
    error('TODO - this is kind of hard because since we need a matrix to be assembled then inverted');
end

%% stop for now
error('stop here');

%% create output movie
 
% restore range to same as original movie
movData = recorrData;
minVal = min(movData(:));
maxVal = max(movData(:));
movData = movData - minVal;
movData = movData .* ((global_maxVal - global_minVal) / (maxVal-minVal));
movData = movData + orig_globalMin;

MOV = txCreateMOV(movData, rdr.FrameRate, @gray);
fname_out = [base_fname '+restored' fext];
fpath_out = [dir fname_out];
movie2avi(MOV.mov, fpath_out, 'fps', MOV.rate, 'colormap', gray(256), 'compression', 'none', 'quality', 100);
fprintf('Movie ''%s'' created.\n', fname_out);

