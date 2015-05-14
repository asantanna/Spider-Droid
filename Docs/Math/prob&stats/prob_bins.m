function prob_bins

    %{
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    % sequence length
    N=4;
    col(1:4)=['r', 'm', 'g', 'b'];

    % number of bins
    numBins = N+1;
    numProbs = 100;
    
    binSize = 1/numBins;
    probInc = 1/numProbs;

    prob_input = 0:probInc:(numProbs-1)*probInc;
    prob_output = zeros(size(prob_input));

    figure(1);
    hold off;

    for Nt=0:N
        
        use_color = col(mod(Nt,4) + 1);
        binMax = 0;
        binBestP = 0;
        
        for i = 1:numel(prob_input)
            
            p = prob_input(i);
            prob_seq = prob_equiv_seq(p, N, Nt);
            prob_output(i) = prob_seq;
            
            bin_index = floor(p/binSize);
            if (bin_index == Nt)
                if (prob_seq > binMax)
                    binMax = prob_seq;
                    binBestP = p;
                end
            end
        end
        

        plot(prob_input, prob_output, use_color);
        hold on;

        % p_hat is the label
        p_hat = Nt/N;
        line([p_hat p_hat], [0 1], 'Color', use_color, 'LineWidth', 3);

        % bin_bound is the bin boundary
        bin_bound = Nt/numBins;
        line([bin_bound bin_bound], [0 1], 'LineStyle', ':');

        % sum of all probs for this label plaedd at bin center
        bin_center = (2*Nt + 1) / (N+1) / 2;
        bin_probs_sum = sum(prob_output) / numProbs;
        stem(bin_center, bin_probs_sum, use_color);

        title(sprintf(...
            ['Solid lines are position of labels\n' ...
            'Colored curves are probabilities of generating each label\n' ...
            'Colored ctems are prob of label being generated and are placed in centers of label bins\n' ...
            'Dotted lines are label bin boundaries']));
    end

    % plot probs of each label
    hold on;

    %}
    %{
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    N=10000;
    nSeq = 10000;
    randProb = rand(nSeq, 1);

    % compute the bin centers
    bin_bounds = [calc_bin_lower_bound(0:N, N) 1];
    bin_center = zeros(N+1);
    for Nt=0:N
        bin_center(Nt+1)=(bin_bounds(Nt+1) + bin_bounds(Nt+2)) / 2;
    end

    % sample N items, numSamp times and count T events
    Nt = zeros(nSeq,1);
    for i=1:nSeq
        p = randProb(i);
        rawSamp = txSamplePDF([1-p p], N) - 1;
        Nt(i) = sum(rawSamp);
    end

    % compute error from guesses of bin center
    prob_guesses = bin_center(Nt+1);
    error = prob_guesses - randProb;
    avg_error = mean(error);
    avg_abs_error = mean(abs(error));
    
    % compute error from guesses of simple ratio
    prob_guesses_simple_ratio = Nt / N;
    error_simple_ratio = prob_guesses_simple_ratio - randProb;
    avg_error_simple_ratio = mean(error_simple_ratio);
    avg_abs_error_simple_ratio = mean(abs(error_simple_ratio));

    figure(2);
    hold on;

    b = -1:0.01:1;
    h_error = hist(error , b);
    h_error_simple_ratio = hist(error_simple_ratio , b);
    plot(b, [h_error' h_error_simple_ratio']);
    legend(...
        sprintf('bin center (err=%.2f, abs\\_err=%.2f)', avg_error, avg_abs_error),...
        sprintf('simple ratio (err=%.2f, abs\\_err=%.2f)', avg_error_simple_ratio, avg_abs_error_simple_ratio));

    ax = axis;
    axis([-1 1 0 ax(4)]);

    title(sprintf(...
        ['ERROR Histogram of individual sequence guesses for N=%g, nSeq=%g)\n' ...
        'When a sample arrives, we guess its prob is the bin center\n' ...
        '(if bin technique narrower, SUCCESS)'], ...
        N, nSeq));

    %}
    %{
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % DISABLED BECAUSE BORING
    
    figure(3);
    hold on;

    plot(b,h_error-h_error_simple_ratio);

    ax=axis;
    line([-1 1],[0 0]);

    % let paint
    pause(0.1);
    
    %}
    %{
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    figure(4);
    
    N = 1;
    p_vec = 0:0.05:1; p_vec(end) = 0.9999999;
    pc = zeros(size(p_vec));
    
    for i = 1:numel(p_vec)
        p = p_vec(i);
        cb = floor(p*(N+1));
        pc(i) = prob_target_bin(cb, p, N);
    end
    
    plot(p_vec, pc);
    ax = axis;
    axis([ax(1) ax(2) 0 ax(4)]);
    set(gca,'YGrid', 'on');
    xlabel('true value of p');
    ylabel('prob of being in correct bin');
    
    %}
    %{%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    figure(5);
    
    N = 1;
    dp = 0.025;
    p_vec = 0:dp:1; p_vec(end) = 0.9999999;
    pc = zeros(size(p_vec));
    
    for i = 1:numel(p_vec)
        p = p_vec(i);
        p = max(p, 1-p);
        acc = 0;
        % first bin >= 0.5
        fb = floor((N+1)/2);
        weight=1.0;
        if (mod(N+1,2)==1)
            % num bins is odd, only count half of first bin
            weight=0.5;
        end
        for tb=fb:N
            acc = acc + weight*prob_target_bin(tb, p, N);
            % after first bin, all others have full weight
            weight = 1.0;
        end
        % acc contains prob we will end up on a bin in the correct
        % half of the probs.  prob of guessing correctly is when
        % p(behaving)*acc + p(not_behaving)*(1-acc)
        pc(i)=p*acc+(1-p)*(1-acc);
    end
    
    plot(p_vec, pc, 'r', 'LineWidth', 2);
    ax = axis;
    axis([ax(1) ax(2) 0 ax(4)]);
    grid on;
    xlabel('true value of p');
    ylabel('prob of guessing correctly');
    line([0 0.5 1.0], [1.0 0.5 1.0], 'LineStyle', ':', 'Color', 'b', 'LineWidth', 2);
    title(sprintf('N=%g\n(If we had perfect knowledge of p, this graph would be a linear V).', N));
    
  %}
    %{
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    figure(6);
    hold off;
    
    %N_vec = [2 5 10];
    N_vec = 5;
    dp = 0.025;
    p_vec = 0:dp:1; p_vec(end) = 0.9999999;
    cols = [ 'b', 'g', 'r', 'm', 'k'];
    
    for iN=1:numel(N_vec)
        N = N_vec(iN);
        col = cols(mod(iN-1, numel(cols)) + 1);
        pb_p = zeros(size(p_vec));
        pb_tb = zeros(N+1,1);        
        for tb=0:N        
            for i = 1:numel(p_vec)
                p = p_vec(i);
                pb_p(i) = prob_target_bin(tb, p, N);
            end
            
            % prob gen this bin
            pb_tb(tb+1) = sum(pb_p);
            
            plot(p_vec, pb_p, col);
            hold on;
        end
        % normalize bins
        pb_tb = pb_tb ./ sum(pb_tb);
        stem(1/(2*(N+1)):1/(N+1):1, pb_tb, col);
    end
    
    ax = axis;
    axis([ax(1) ax(2) 0 ax(4)]);
    % grid on;
    xlabel('true value of p');
    ylabel('probs of each bin');
    title(sprintf(',%d', N_vec));
  %}  
end

function p = prob_equiv_seq(p, N, Nt)
    p = (p^Nt)*((1-p)^(N-Nt)) * nchoosek(N,Nt);
end

function bounds = calc_bin_lower_bound(index, N)
    bounds = index ./ (N+1);
end

function p_tb = prob_target_bin(tb,p,N)
    % prob ta bin
    p_tb = nchoosek(N, tb) * (p^tb) * ((1-p)^(N-tb));
    
end   
    
    
    