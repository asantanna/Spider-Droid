%
% Convert RGB to perceptual, equalize and write out
%

EQ_LEVELS = 0;              % zero means no EQ
NOISE = 0.2;

% say something
fprintf('Convert movie to perceptual, optionally add noise and optionally equalize ...\n');

% orig movie name

% dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\San Diego Fashion\';
% fname = 'Fashion-snippet-tiny';

dir = 'D:\ALS-Projects\Work\Teraconnix\Dev\Third Party Data\Video Data\';
fname = 'Tiger Snippet';

fext = '.avi';

fpath = [dir fname fext];

% read file
rdr = mmreader(fpath);
data = read(rdr);

if (ndims(data) ~= 4)
    error('Movie isn''t RGB ... sheesh');
end

% convert to perceptual
percep_data = double(txRGBtoPerceptual(data));

% add a little NOISE to see effect
if (NOISE ~= 0)
    percep_range = max(percep_data(:)) - min(percep_data(:));
    noise_val = NOISE * percep_range;
    fprintf('Adding some noise, max = %g%% (%g)\n', NOISE*100, noise_val);
    percep_data = percep_data + (rand(size(percep_data))-0.5)*noise_val;
else
    fprintf('No noise added.\n');
end

% equalize
if (EQ_LEVELS ~= 0)
    fprintf('Equalizing to %g levels ...\n', EQ_LEVELS);
    eqData = txEqualizeData(uint8(percep_data), EQ_LEVELS);
else
    fprintf('Not equalizing ...\n');
    eqData = percep_data;
end

% write out new movie
MOV = txCreateMOV(eqData, rdr.FrameRate, @gray);
fname_out = [fname '+percep'];
if (EQ_LEVELS ~= 0)
    fname_out = [fname_out '+eq' sprintf('%d', EQ_LEVELS)];
end
if (NOISE ~= 0)
    fname_out = [fname_out '+n' sprintf('%d%%', floor(NOISE*100))];
end
fname_out = [fname_out fext];
fpath_out = [dir fname_out];
movie2avi(MOV.mov, fpath_out, 'fps', MOV.rate, 'colormap', gray(256), 'compression', 'none', 'quality', 100);
fprintf('Movie ''%s'' created.\n', fname_out);

