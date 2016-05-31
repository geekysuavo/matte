
function y = gtest
  global x;
  y = x .*= -0.1

  if (x > 0)
    disp('x is positive');
  elseif (x < 0)
    disp('x is negative');
  else
    disp('x is zero');
  end
end

x = 55

k = 1;
while (k < 5)
  gtest;
  k += 1; % inc/dec not yet implemented
end

