% Test fading sparse binary memory traces.
%

% number of vars (if > 3, can't plot)
NV=3;

% num time steps
NT = 4;

% fading rate per time step
FADE_RATE = 0.5;

% array of possible inputs in NS timesteps
% represented as index of var that is one (others are zero)
v = ones(NT, NV^NT);

for c=2: size(v,2)
    % copy prev possibility to current
    for r = 1 : NT
        v(r, c) = v(r,c-1);
    end
    % increment in base NV (one-based)
    carry = 1;
    for r = 1 : NT
        v(r, c) = v(r,c) + carry;
        if v(r,c) > NV 
            v(r,c) = 1;
            carry = 1;
        else
            carry = 0;
        end
    end
end

% plot fading of each possible history

figure(1);
hold off;
plot3([0 .01], [0, .01], [0,.01]);
hold on;

pts_init = zeros(NV,1);
for h = 1:size(v,2)
    mem_trace = zeros(NV, NT);
    for t = 1 : NT
        mem = pts_init;
        mem(v(t,h)) = 1;       % single one in vector (the event to remember)
        if (t == 1)
            pts_accum = mem;
        else
            pts_accum = mem + FADE_RATE*pts_accum;
        end
        mem_trace(:,t) = pts_accum;
    end
    % x at start location
    % o at others
    plot3(mem_trace(1,:), mem_trace(2,:), mem_trace(3,:));   
    plot3(mem_trace(1,1), mem_trace(2,1), mem_trace(3,1), 'Marker', 'x', 'LineStyle', 'none');   
    plot3(mem_trace(1,2:end), mem_trace(2,2:end), mem_trace(3,2:end), 'Marker', 'o', 'LineStyle', 'none');   
end


