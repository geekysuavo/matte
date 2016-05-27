
function p = approx (n)
  p = 4 * sum(1.0 ./ (1 : 4 : n) - 1.0 ./ (3 : 4 : n));
end

function y = gtest
  global x;
  y = x *= -0.1
end

x = 55

gtest; gtest; gtest;

pi = [approx(20), ...
      approx(200), ...
      approx(2000), ...
      approx(20000), ...
      approx(200000), ...
      approx(2000000)]

% hmm... looks like vectorization of ranges has a bug.
v = [0 : 2 : 10]

ccat = [1i; 2; approx(100)];

