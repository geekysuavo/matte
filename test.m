
function y = gtest
  global x;
  y = x .*= -0.1
end

x = 1
y = 2
z = 3

value = 2

switch value
  case x
    disp('value equals x');
  case y
    disp('value equals y');
  case z
    disp('value equals z');
  otherwise
    disp('value is different');
end

