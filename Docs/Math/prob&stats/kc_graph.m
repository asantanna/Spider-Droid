function kc_graph

    maxN = 10;
    maxK = maxN / (maxN + 1);
    
    vecIntN = 0:maxN;        
    numSteps = 50;
    vecN = 0:maxN/numSteps:maxN;
    
    if (false)
        % calc evenly spaced K
        vecK = 0:1/20:maxK;    
        vecN_of_K = -vecK ./ (vecK - 1);
    else
        % calc K at integer N
        vecN_of_K = vecIntN;
        vecK = vecN_of_K ./ (vecN_of_K+1);
    end
    
    % use evenly spaced N
    
    figure(1);
    subplot(121); hold off;
    subplot(122); hold off;
    
    for p=1/2:1/20:1
        % p* as function of N
        subplot(121);
        path = ((2*p*vecN) + 1) ./ (2*(vecN+1));
        plot(vecN, path);
        hold on;
        line([0 maxN], [p p], 'Color', 'r', 'LineStyle', ':');
        
        % p* as function of K
        subplot(122);
        path = ((2*p*vecN_of_K) + 1) ./ (2*(vecN_of_K+1));
        plot(vecK, path);
        hold on;
        line([0 maxK], [p p], 'Color', 'r', 'LineStyle', ':');
    end;
    
    subplot(121);
    
    line([vecIntN ; vecIntN], [zeros(size(vecIntN)) ; ones(size(vecIntN)) ], 'LineStyle', '--');    
    
    axis([0 maxN 1/2 1]);
    xlabel('N');
    ylabel('P*');

    title(sprintf([...
        'Horiz lines are different values of p\n' ...
        'Vert lines are integer values of N\n' ...
        'Curves are values of p* as N increases' ...
        ]));
    
    subplot(122);
    
    line([vecK ; vecK], [zeros(size(vecK)) ; ones(size(vecK)) ]);    
    
    axis([0 1 0 1]);
    xlabel('K');
    ylabel('P*');
    axis square;

    title(sprintf([...
        'Horiz lines are different values of p\n' ...
        'Vert lines are K values at integer values of N\n' ...
        'Curves are values of p* as K increases' ...
        ]));
        
end

function c = conf(p)
    c = 2 * abs(p-0.5);
end