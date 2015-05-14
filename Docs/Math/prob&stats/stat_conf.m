function stat_conf

    % EXPERIMENTAL - not sure if this is good for anything yet

    maxN = 30;
    corr_label = zeros(maxN, maxN+1);
    
    for N=1:maxN
        for Nt=0:N
            % TODO: bUseApprox isn't working - always use false - fix
            corr_label(N,Nt+1) = computeProbOfCorrectLabel(N, Nt, false);
            % TODO: have to multiply by something to get confidence
        end
    end

    figure(1);
    
    subplot(1,2,1);
    hold off;
    contour(corr_label .^ 0.2, 20);
    % surf(corr_label .^ 0.2);
    hold on;
    line([1 N/2]+1,[2 N]);
    line([1 N/4]+1,[4 N]);
    line([3 3*N/4]+1,[4 N]);
    
    axis square;
    title('Probability of being correct label');
    xlabel('Nt');
    ylabel('N');
    
end

function p_lc_given_seq = computeProbOfCorrectLabel(N, Nt, bUseApprox)
    
%{
    Sequences are categorized and placed in buckets.  These buckets are
    labeled by the sample probability p_hat = Nt/N which is also the
    *best* guess of the true probability given our observed data.
    
    We wish to compute the probability that p_hat is the correct label
    among all the other labels we could have chosen.  Note that for a
    sequence of length N there are only (N+1) possible labels, one for
    each possible number of TRUE events in a sequence of length N.
    
    IOW, we wish to compute p(label_correct | seq) and by Bayes' formula:

    p(label_correct|seq) = P(seq|label_correct) * p(label_correct) / p(seq)
%}

%{
    Computing P(seq|label_correct):

    This value is computed by first noting that there are 2^N possible
    sequences that can be generated.  Next we note that all permutations
    of the observed sequence are equally likely and would generate the
    same label.  Thus, given that the label is correct, the number of 
    sequences assigned to this label divided by the total number
    sequences that are possible is the value of this term.
%}

    p_seq_given_lc = nchoosek(N,Nt) / (2^N);

%{
    Computing p(label_correct):
    
    Since the TRUE probability is what we are wondering about and we have
    no reason to suspect one value over another, all possible values are
    equally likely. A label is the BEST estimate of the true probability
    which implies it is also the CLOSEST. Since for any sequence length,
    the possible labels are always evenly spaced, it follows that all labels
    are equally likely to be the closest.  Thus the probability of a label
    being the correct one without any additional information is one divided
    by the number of possible labels for a sequence of length N.
%}
    
    p_lc = 1 / (N+1);
    
%{
    Computing p(seq):
    
    HMMMMMM ... don't know about this ....
    
    We have stated above that all LABELS have equal probability when we have no
    further information.  This, however, means that all sequences DO NOT have
    equal probability.  Instead, we must cycle through all possible labels 
    and add up the expected number sequences they contribute to the label
    identifying the observed sequence.  If we divide this number by the
    number of possible sequences, we arrive at our result.
    
    See eq_prob_vs_eq_seq.m for more info.
%}
    
    expected_numSeq = 0;
    numSeqPerm = nchoosek(N,Nt);    
    
    for p = 0:1/N:1
        expected_numSeq = expected_numSeq + ...         % accum numSeq
                (p^Nt) * ((1-p)^(N-Nt));                % prob of seq
                % (same for all so moved outside of loop)
                % * numSeqPerm ...                      % num of equiv seq
    end
    
    expected_numSeq = expected_numSeq * numSeqPerm;
    p_seq = expected_numSeq / (2^N);
   
%{    
    Compute final value:
%}
    
    p_lc_given_seq = p_seq_given_lc * p_lc / p_seq;
    
end
    

function rho = computeCategoryRatio(N, Nt, bUseApprox)

    % we are considering a sample of N binary events of which Nt are
    % "true".

    % compute number of possible sequences in the "sequence space" disregarding Nt.

    nSeq = 2 ^ N;

    % each category is associated with a value of Nt and contains all
    % sequences that have Nt true events.  The number of sequences contained
    % in the category labeled by our parameter Nt is given by number of
    % distinct ways that a sequence with Nt true events can be constructed
    % in a sequence that is N elements long.

    if (bUseApprox == false)
        % use integer nchoosek()
        nCatSeq = nchoosek(N,Nt);
    else
        % use approximation function
        nCatSeq = approx_nchoosek(N,Nt);
    end;

    % we can define a "label ratio" for this tuple (N, Nt) by computing
    % what proportion of the sequence space is contained in the category 
    % identified by the label.

    rho = nCatSeq / nSeq;

end

function x = approx_nchoosek(n, k)
    % from Wikipedia
    x = exp( gammaln(n+1) - gammaln(k+1) - gammaln(n-k+1) );
end

