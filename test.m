
function y = gtest
  global x;
  y = x .*= -0.1
end

% this code will compile.
try
  test = [1; 2; 3] == 3;
catch ex
  try
    test = 1 : 3 == 3;
  catch ex2
    disp(ex2);
  end
  disp(['caught exception!'; 'here it be...'; '']);
  disp(ex);
end

% this code will not.
try
  disp('oops');
  try
  catch f
  end
catch e
end

disp('this will execute, despite the error:');
x = 1 + 1

