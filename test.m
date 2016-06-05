
function y = gtest
  global x;
  y = x .*= -0.1
end

try
  test = [1; 2; 3] == 3;
catch ex
  disp(['caught exception!'; 'here it be...'; '']);
  disp(ex);
end

disp('this will execute, despite the error:');
x = 1 + 1

