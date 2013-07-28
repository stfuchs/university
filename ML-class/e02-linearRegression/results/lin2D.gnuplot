x0=-0.678061
x1=0.245194
x2=1.37055

f(x,y) = x0 + x1*x + x2*y
splot './data/dataLinReg2D.txt' with points, f(x,y)