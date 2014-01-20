%
% Solve for cubic interpolator/extrapolator
%

% declare variables
syms a b c d
syms x1 y1 x2 y2 x3 y3 x4 y4

% solve for a using eq 1

eq1 = a*x1^3 + b*x1^2 + c*x1 + d - y1;
a = solve(eq1, a);

fprintf('The value of a is:');
pretty(a)

% solve for b using eq 2

eq2 = a*x2^3 + b*x2^2 + c*x2 + d - y2;
eq2 = subs(eq2);
b = solve(eq2, b);

fprintf('The value of b is:');
pretty(b)

% solve for c using eq 3

eq3 = a*x3^3 + b*x3^2 + c*x3 + d - y3;
eq3 = subs(eq3);
c = solve(eq3, c);

fprintf('The value of c is:');
pretty(simplify(c))

% solve for d using eq 4

eq4 = a*x4^3 + b*x4^2 + c*x4 + d - y4;
eq4 = subs(eq4);
eq4 = subs(eq4);
d = solve(eq4, d);

fprintf('The value of d is:');
pretty(simplify(d))

%
% Try out some values to see if it works
%

figure;

for i=1:10

    x = rand(4,1) * 2 - 1 + [ -4 -2 1 5 ]' ;
    y = (rand(4,1) * 2 - 1) * 10 ;

    guess_x = ((rand(1,1) * 2 ) - 1) * 6;
    
    % shift all x coords by guess_x so that we are always
    % guessing the value at x = 0
    
    new_x = x - guess_x;
    
    d_val = subs(d, [x1 x2 x3 x4 y1 y2 y3 y4], [ new_x(1) new_x(2) new_x(3) new_x(4) y(1) y(2) y(3) y(4)]);
    
    % draw black plus signs at every point given
    hold off;
    plot(x, y, 'k+', 'LineWidth', 2);
    hold on;

    % now determine the values for a, b and c
    c_val = subs(c, [ 'd' x1 x2 x3 x4 y1 y2 y3 y4], [ d_val new_x(1) new_x(2) new_x(3) new_x(4) y(1) y(2) y(3) y(4)]);
    b_val = subs(b, [ 'c' 'd' x1 x2 x3 x4 y1 y2 y3 y4], [ c_val d_val new_x(1) new_x(2) new_x(3) new_x(4) y(1) y(2) y(3) y(4)]);
    a_val = subs(a, [ 'b' 'c' 'd' x1 x2 x3 x4 y1 y2 y3 y4], [ b_val c_val d_val new_x(1) new_x(2) new_x(3) new_x(4) y(1) y(2) y(3) y(4)]);

    % plot cubic function
    
    s = sprintf('(%f)*(x-%f)^3 + (%f)*(x-%f)^2 + (%f)*(x-%f) + (%f)',a_val, guess_x, b_val, guess_x, c_val, guess_x, d_val);
    h = ezplot(s, [-10 10]);
    % set(h, 'Color', 'g');
    
    % show our guess as a red circle
    plot(guess_x, d_val, 'ro', 'LineWidth', 2);
    
    % wait a bit
    pause(2);

end;

% close figure
close

% axis([-10 10 -10 10]);

