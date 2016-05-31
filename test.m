
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

k = 'foo';
x = 55
k1 = 1;
k2 = 5;
for k = k1 : k2, gtest; end

x = 0;
gtest;

x = 0 : 0.1 : 1
for value = x
  disp(value);
end
disp(sprintf('value is now %lf', value));

