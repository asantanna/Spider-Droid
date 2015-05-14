function dp_dN_uncertainty

    % quick test - work in progress

    N = 1:30;
    
    % dp_dN_true(N, Nt) = (N-Nt) / (N*(N+1))
    % dp_dN_false(N, Nt) = -(Nt) / (N*(N+1))
    %
    % dp_dN_span(N) = dp_dN_true(N, Nt) - dp_dN_false(N, Nt)
    %               = N-Nt + Nt / (N*(N+1))
    %               = N / (N*(N+1))

    dp_dN_span = N ./ (N .* (N+1));
    
    plot(N, dp_dN_span, 'o-');
    xlabel('Sample Size');
    ylabel('Span of   \Delta p / \Delta N');
    ax = axis;
    axis( [1 max(N) ax(3) ax(4)] );
    
    title(sprintf([ ...
        'At N, then next sample changes P up or down (either may be = 0 but not both)\n' ...
        'This plot is of the span (distance) between the two possible targets for each N']));
    
end