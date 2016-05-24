
function p = approx (n)
  p = 4 * sum(1.0 ./ (1 : 4 : n) - 1.0 ./ (3 : 4 : n));
end

i = 42
f = 55.0
c = 1i
s = 'hello world'

pi = [approx(20), ...
      approx(200), ...
      approx(2000), ...
      approx(20000), ...
      approx(200000), ...
      approx(2000000)]

