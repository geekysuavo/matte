
function p = approx (n)
  p = 4 * sum(1.0 ./ (1 : 4 : n) - 1.0 ./ (3 : 4 : n));
end

function dx_rel = relerr (x_true, x_calc)
  dx_rel = (x_true - x_calc) ./ x_true;
end

% FIXME: globals are not yet properly handled by the zone allocators.
%{
function y = gtest
  global x;
  y = x *= -0.1
end
%}

x = 55

%{
gtest; gtest; gtest;
%}

pi = [approx(20), ...
      approx(200), ...
      approx(2000), ...
      approx(20000), ...
      approx(200000), ...
      approx(2000000)]

v = [1 : 2 : 10, -1, relerr(3.14159265359, approx(1000000)), -1, 2 : 2 : 10]

str = sprintf(['hello world!'; 'the answer is %d'; 'complex %g'], 42, 1j)

