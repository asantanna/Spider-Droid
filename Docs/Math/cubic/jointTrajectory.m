
%
% Ideal trajectory for a joint
%

function jointTrajectory

    % prototypical position curve from (t=0, p=0) to (t=1, p=1)
    % constraints:
    %   1) accel phase is circular arc lasting (1-FRAC_ACCEL)
    %   2) decel phase is circular arc lasting (1-FRAC_DECEL)
    %   3) cruise phase is line segment lasting 1 - (FRAC_ACCEL + FRAC_DECEL)
    %   4) line segment must be tangent to both arcs

    % Simultaneous equations:
    %
    %  (A)          R1          =  || C1 - P1 ||
    %  (B)          R2          =  || C2 - P2 ||
    %  (C)  <(C1-P1) , (P2-P1)> = 0
    %  (D)  <(C2-P2) , (P2-P1)> = 0
    
    global FRAC_ACCEL FRAC_DECEL
    global sol_P1_y sol_P2_y sol_R1 sol_R2

    syms R1 R2
    syms P1_y  P2_y
    
    FRAC_ACCEL = 0.2;
    FRAC_DECEL = 0.4;

    C1 = [0, R1];
    C2 = [1, (1-R2) ];
    P1 = [FRAC_ACCEL, P1_y ];
    P2 = [(1-FRAC_DECEL), P2_y ];

    eqA = R1 - sym_norm(C1-P1);
    eqB = R2 - sym_norm(C2-P2);
    eqC = sym_inner((C1-P1), (P2-P1));
    eqD = sym_inner((C2-P2), (P2-P1));

    % solve equations
    sol = solve(eqA,eqB, eqC, eqD, R1, R2, P1_y, P2_y);

    % show all solutions
    
    figure(1);
    clf();
    subplot(2,2,1);
        
    numSol = numel(sol.P1_y);
    title(sprintf('Valid Solutions (%d)', numSol));
    
    correctN = -1;
    
    fprintf('*\n* Joint Trajectory Solutions\n*\n');
    
    for n = 1:numSol
        
        sol_P1_y = real(eval(sol.P1_y(n)));
        sol_P2_y = real(eval(sol.P2_y(n)));
        sol_R1 = eval(sol.R1(n));
        sol_R2 = eval(sol.R2(n));
        fprintf('sol(%d): P1.y=%g, p2.y=%g, R1=%g, R2=%g\n', n, sol_P1_y, sol_P2_y, sol_R1, sol_R2);
        
        if ((sol_R1 < 0) || (sol_R2 < 0))
            % sometimes Matlab gives "spurious solutions" - ignore
            fprintf('*** Ignoring Matlab "spurious solution"\n');
            continue;
        end
        
        if (sol_P1_y < sol_P2_y)
            correctN = n;
            fprintf('Correct solution is %d\n', correctN);
        end
        
        disp_solution(n, sol_P1_y, sol_P2_y, sol_R1, sol_R2);
        
        axis equal;
        axis([-0.1, 1.1, -0.1, 1.1]);
        
        if (0) % n < numSol)
            pause(1);
        end
        
    end;

    % pick correct one and display

    sol_P1_y = real(eval(sol.P1_y(correctN)));
    sol_P2_y = real(eval(sol.P2_y(correctN)));
    sol_R1 = eval(sol.R1(correctN));
    sol_R2 = eval(sol.R2(correctN));
    
    subplot(2,2,3);
    
    disp_trajectory(0, 1, 1, [-0.1, 1.1]);
   
    axis equal
    axis([-0.1, 1.1, -0.1, 1.1]);
    title('Selected solution for prototype');

    % do some scaled ones

    subplot(2,2,[2 4]);     % select regions 3 *and* 4
    hold on;
    
    N = 10;
    startP_arr = rand(N,1);
    deltaP_arr = (rand(N,1) - 0.5) / 0.5;
    deltaT_arr = 0.4 + (rand(N,1) * 0.6);
    
    maxT = -1;
    
    for n = 1:N
        
        startP = startP_arr(n);
        deltaP = deltaP_arr(n);
        deltaT = deltaT_arr(n);
        
        endP = startP + deltaP;
        
        if (endP < 0)
            endP = 0; 
        elseif (endP > 1)
            endP = 1;
        end;
        
        deltaP = endP-startP;
        
        disp_trajectory(startP, deltaP, deltaT, [-0.1, 1.1]);
        
        hold on;
            
    end
    
    ax = axis();
    axis([-0.1, 1.1, -0.1, 1.1]);
    
    title(sprintf( ...
        'Sample of Trajectories\np1Y=%.3f, p2Y=%.3f, r1=%.3f, r2=%.3f', ...
        sol_P1_y, sol_P2_y, sol_R1, sol_R2));
        
end

function n = sym_norm(v)
    n =  sqrt( (v(1)*v(1)) + v(2)*v(2));
end

function d = sym_inner(a, b)
    d = sum(a .* b);
end

function disp_solution(solNum, p1y, p2y, r1, r2)

    global FRAC_ACCEL FRAC_DECEL

    p1x = FRAC_ACCEL;
    p2x = 1-FRAC_DECEL;

    colorArr = [ 'b', 'r', 'g', 'k' ];
    color = colorArr(mod(solNum-1,4) + 1);

    % draw circle 1
    rectangle('Position',[-r1, 0, 2*r1, 2*r1],...
          'Curvature',[1, 1],...
          'LineStyle','--',...
          'EdgeColor', color);
      
    % draw circle 2
    rectangle('Position',[1-r2, 1-2*r2, 2*r2, 2*r2],...
          'Curvature',[1, 1],...
          'LineStyle','--',...
          'EdgeColor', color);
     
    % connecting line
    line([p1x ; p2x], [p1y ; p2y], 'LineStyle', '--', 'Color', color); 

end

function disp_trajectory(startP, deltaP, deltaT, dispRange)

    tArr = dispRange(1) : (dispRange(2) - dispRange(1))/100 : dispRange(2) * 1.1;
    p = zeros(numel(tArr),1);
    
    for n = 1:numel(tArr)
        t = tArr(n);
        p(n) = protoJointTrajectory(t, deltaP, deltaT) + startP;
    end

    plot(tArr, p, 'LineWidth', 1, 'LineStyle', '--');

    % save hold state and turn on
    saveHold = isHold();
    hold on;
     
    % put red circle at beginning and end
    plot([0, deltaT], [startP, startP + deltaP], 'ro', 'LineWidth', 1);
    
    % restore hold state
    if (saveHold == 0)
        hold off;
    else
        hold on;
    end;

end

function p = protoJointTrajectory(t, deltaP, deltaT)

    global FRAC_ACCEL FRAC_DECEL
    global sol_P1_y sol_P2_y sol_R1 sol_R2
    
    p1x = FRAC_ACCEL;
    p1y = sol_P1_y;
    p2x = 1-FRAC_DECEL;
    p2y = sol_P2_y;
    r1 = sol_R1;
    r2 = sol_R2;

    % adjust t so complete at t = deltaT
    t = t / deltaT;
    
    % find out which phase we're in
    if (t < 0)
        % before curve
        p = 0;
    elseif (t < p1x)
        % accel phase
        y = getCircleY(t, r1);
        p = r1 - y;
    elseif (t < p2x)
        % linear phase
        slope = (p2y - p1y) / (p2x - p1x);
        p = p1y + slope * (t-p1x);
    elseif (t < 1)
        % decel phase
        y = getCircleY(1-t, r2);
        p = y + (1-r2);
    else
        % past end
        p = 1;
    end

    % adjust p so total move p = deltaP
    p = p * deltaP;

end

function y = getCircleY(x, r)
    y = sqrt(r*r - x*x);
end




