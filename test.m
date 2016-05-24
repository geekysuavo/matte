
function p = approx (n)
  p = 4 * sum(1.0 ./ (1 : 4 : n) - 1.0 ./ (3 : 4 : n));
end

function global_test
  global f c;
  f *= -0.1
  c += 2
end

i = 42
f = 5.5
c = 1i
s = 'hello world'

global_test;
global_test;
global_test;

pi = [approx(20), ...
      approx(200), ...
      approx(2000), ...
      approx(20000), ...
      approx(200000), ...
      approx(2000000)]

