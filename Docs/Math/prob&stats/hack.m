function hack

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

N=20;

prob_equiv_seq = zeros(N+1, 1);

for Nt=0:N
    prob_single_seq = beta(Nt+1, N-Nt+1);
    prob_equiv_seq(Nt+1) = prob_single_seq * nchoosek(N,Nt);
end


figure (1);
stem(0:1/N:1, prob_equiv_seq);
ax=axis;
axis([0 1 ax(3) ax(4)*1.1]);
title(sprintf(...
    ['This graph show the probability that each label will occur\n' ...
    'when the true probability is uniform ... huh ... uniform too! ...']));


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% THIS IS THE HACK VERSION !!! EDIT THE ONE IN prob_bins.m if for real

% sequence length
N=5;
col(1:4)=['r', 'm', 'g', 'b'];

% number of bins
numBins = N+1;
totProbs = 100;
probInc = 1/totProbs;

prob_input = 0:probInc:1;
prob_output = zeros(size(prob_input));
bin_probs_sum = zeros(N+1,1);

figure(2);
hold off;

for Nt=0:N
    use_color = col(mod(Nt,4) + 1);
    for i = 1:numel(prob_input)
        prob_output(i) = prob_p_hat_seq(prob_input(i), N, Nt);
    end
    
    % sum of all probs for this label
    bin_probs_sum(Nt+1) = sum(prob_output) / totProbs;
    
    plot(prob_input, prob_output, use_color);
    hold on;
    
    % p_hat is the label
    p_hat = Nt/N;
    line([p_hat p_hat], [0 1], 'Color', use_color, 'LineWidth', 3);
    
    % bin_bound is the bin boundary
    bin_bound = Nt/numBins;
    line([bin_bound bin_bound], [0 1], 'LineStyle', ':');
    
    title(sprintf(...
        ['Colored curves are probabilities of generating a label\n' ...
        'Solid lines are position of labels\n' ...
        'Dotted lines are label "bin" boundaries']));
end

% plot probs of each label
hold on;
stem(0:1/N:1, bin_probs_sum);


end

function p = prob_p_hat_seq(p, N, Nt)
    p = (p^Nt)*((1-p)^(N-Nt)) * nchoosek(N,Nt);
end