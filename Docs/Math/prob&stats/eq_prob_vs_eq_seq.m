%
% RESEARCH
%
% Compare sequences when probabilities are evenly distributed
% against probabilities when sequences are evenly distributed
%

function eq_prob_vs_eq_seq

%
% First do evenly distributed sequences
%

N=30;
numSamples = 31000 * 2;

% select type of random number generator (just to experiment)
%rname = 'mt19937ar';
%rname = 'mcg16807';
%rname = 'mlfg6331_64';
rname = 'mrg32k3a';
%rname = 'mt19937ar';
%rname = 'shr3cong';
%rname = 'swb2712';
RandStream.setDefaultStream(RandStream(rname, 'seed', sum(100*clock)));

% generate numbers N numbers in [0,2^(N+1))
samples = rand(numSamples, 1);
samples = floor(samples .* (2^(N+1)));

% compute sample prob of each sequence of ones and zeros
probs = computeSampleProbs(samples, N);

figure(1);

subplot(2,3,1);
plotSeqCoverage(samples, N);
title(sprintf('Sequence coverage, numSamples=%g\n(should be uniform)', numSamples));

subplot(2,3,2);
plotSeqDistrib(samples, N+1);
title(sprintf('Sequence distribution\n(histogram)'));

subplot(2,3,3);
centers = 0 : 1/N : 1;
hold off;
h = hist(probs, centers);
plot(centers, h/numSamples);
hold on;
plot(centers, approx_nchoosek(N, centers*N) / (2^N), 'r.');
title(sprintf('If sequences are uniformly distributed\nthis is the prob distrib (red=theory)'));

pause(0.1);

%
% Now do evenly distributed probabilities
%

N=30;
numProbs = (N+1);

if true
    % this one only does p at certain points - probing only at possible
    % label positions - not sure what it means
    
    numSamples = numProbs*1000;

    if (rem(numSamples, numProbs) ~= 0)
        error('numSamples must be a multiple of (N+1)');
    end
    
    sampSize = numSamples / numProbs * N;
    rawSamples = cell(numProbs, 1);
    samples = cell(numProbs,1);
    
    for i= 0:numProbs-1
        p = 1/(numProbs-1) * i;
        % fprintf('doing p=%g\n', p);
        rawSamples{i+1} = txSamplePDF([p 1-p], sampSize);
        samples{i+1} = cvtOutcomesToSeq(rawSamples{i+1}, N);
    end

    samples = cell2mat(samples);
else
    % this one picks random uniform p - picks any p
    
    numSamples = 100000;
    samples = zeros(numSamples,1);
    
    for i=1:numSamples
        p = rand(1,1);
        rawSample = txSamplePDF([p 1-p], N);
        samples(i) = cvtOutcomesToSeq(rawSample, N);        
    end
end
       
samples = double(samples(:));

subplot(2,3,4);
plot(0:1/(numProbs-1):1, ones(numProbs,1)*1/(numProbs-1), 'b.');
axis([0 1 0 1/(numProbs-1)*1.1]);
title(sprintf('Generating prob distribution\n(set to equal)'));

subplot(2,3,5);
plotSeqCoverage(samples, N);
title(sprintf('Sequence coverage, numSamples=%g\n(should be uneven)', numSamples));

subplot(2,3,6);

hold off;
probs = computeSampleProbs(samples, N);
centers = 0 : 1/N : 1;
h = hist(probs, centers);
plot(centers, h/numSamples,'.--');

hold on;
numComb = zeros(N+1,1);
for i=0:N
    numComb(i+1) = nchoosek(N,i);
end
expected = zeros(N+1, 1);
for i=0:N
    for p = 0:1/N:1
        expected(i+1) = ...
            expected(i+1) ...                         % accum         
            + (p^i) * ((1-p)^(N-i)) ...               % prob of seq
            * numComb(i+1) ...                        % num of equiv seq
            * numSamples/numProbs ...                 % samples each
            * numProbs/(N+1);                         % bucket scaling (approximation)
    end
end

% sanity check
totCheck = sum(expected);
if (abs(numSamples - totCheck) > 1)
    error('something wrong - things don''t add up: should get %g, instead got %g\n', numSamples, totCheck);
end
    
plot(0:1/N:1, expected/numSamples, 'r--');
plot(0:1/N:1, expected/numSamples, 'r.');
title(sprintf('Sequence sample probability\n(histogram, red = theory)'));

end  % function

function numOnes = countOnes(s,N)
    numOnes = 0;
    for b=1:N
        if (bitget(s,1) == 1)
            numOnes = numOnes + 1;
        end
        s = bitshift(s,-1);
    end
end

function seq = cvtOutcomesToSeq(outcomes, N)

    numSamples = numel(outcomes) / N;
    seq = uint32(zeros(numSamples,1));
    
    idx = 0;
    for i=1:numSamples
        v = uint32(0);
        for b=1:N
            idx = idx + 1;
            v = v * 2;
            if (outcomes(idx) == 1)
                % heads
                v = v + 1;
            end
        end
        seq(i) = v;
    end
end

function x = approx_nchoosek(n, k)
    % from Wikipedia
    x = exp( gammaln(n+1) - gammaln(k+1) - gammaln(n-k+1) );
end

function plotSeqCoverage(seq, N)
    if (true)
        % limit sequence size for prettiness
        seq = seq(1:min(10000, numel(seq)));
    end
    n = ceil(sqrt(2^N));
    y = floor(seq ./ n);
    x = rem(seq, n);
    plot(x, y, '.');
    axis([0 n 0 n]);
end

function plotSeqDistrib(seq, numBuckets)
    [h, centers] = hist(seq, numBuckets);
    plot(centers, h ,'o');
    ax = axis;
    axis([0 max(centers) 0 ax(4)]);
end

function probs = computeSampleProbs(samples, numBits)

    samples = uint32(samples);
    probs = zeros(numel(samples),1);

    for i=1:numel(samples)
      s = samples(i);
      numOnes = countOnes(s, numBits);
      probs(i) = numOnes / numBits;
    end
    
end
        