import numpy as np
from matplotlib.pyplot import *

X1 = np.loadtxt('res01')
X2 = np.loadtxt('res02')
X3 = np.loadtxt('res03')
X4 = np.loadtxt('res04')

plot(X1[:,0],X1[:,1])
plot(X2[:,0],X2[:,1])
plot(X3[:,0],X3[:,1])
plot(X4[:,0],X4[:,1])

show()
