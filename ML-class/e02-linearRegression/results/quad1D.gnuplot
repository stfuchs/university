x0=0.279255
x1=0.461272
x2=0.237743

f(x) = x0 + x1*x + x2*x**2
plot './data/dataQuadReg1D.txt' with points, f(x)
