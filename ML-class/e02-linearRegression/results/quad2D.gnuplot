x0=1.26429
x1=-0.153188
x2=-0.270729
x3=1.0873
x4=-0.75216
x5=1.72878

f(x,y) = x0 + x1*x + x2*y + x3*x**2 + x4*x*y + x5*y**2
splot './data/dataQuadReg2D.txt' with points, './results/quad2DPrediction.txt' with points, f(x,y)