
function p = approx (n)
  p = 4 * sum(1.0 ./ (1 : 4 : n) - 1.0 ./ (3 : 4 : n));
end

pi = approx(100)
pi = approx(1000)
pi = approx(10000)
pi = approx(100000)
pi = approx(1000000)

