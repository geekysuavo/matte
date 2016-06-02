
function y = gtest
  global x;
  y = x .*= -0.1
end

x = 55

k = 1;
while (k < 100)
  gtest;
  k++;
end

