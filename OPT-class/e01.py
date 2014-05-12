from numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


class Fsquare:
    def __init__(self, C):
        self.Csq = C.T.dot(C)

    def value(self,x):
        return x.dot(self.Csq).dot(x)

    def gradient(self, x):
        return 2.*self.Csq.dot(x)

class Fhole:
    def __init__(self, C):
        self.Csq = C.T.dot(C)

    def value(self,x):
        return 1.-exp(-x.dot(self.Csq).dot(x))

    def gradient(self, x):
        return 2.*self.Csq.dot(x)*exp(-x.dot(self.Csq).dot(x))


class GradientDescent:
    def __init__(self, function):
        self.f = function

    def solveFixedSteps(self, start, alpha = 0.01, sigma = 0.0001):
        dx = sigma + 1.
        x = start
        i = 0
        while(dx > sigma and i < 10000):
            xold = x
            x = x - alpha * self.f.gradient(x)
            dx = linalg.norm(xold - x)
            i = i+1
        print "reached " + str(x) + " after " + str(i) + " iterations."
        return x


n=2
c=10.
C = zeros([n,n])
for i in range(n):
    C[i,i] = c**( (i)/(n-1.) )

f1 = Fsquare(C)
f2 = Fhole(C)

vx = linspace(-1.,1.,41)
vy = linspace(-1.,1.,41)
X0, X1 = meshgrid(vx,vy)
X = array([X0,X1])
y1 = zeros([len(vx),len(vy)])
y2 = zeros([len(vx),len(vy)])


for i in range(len(vx)):
    for j in range(len(vy)):
        x = X[:,i,j]
        y1[i,j] = f1.value(x)
        y2[i,j] = f2.value(x)


sf1 = GradientDescent(f1)
sf2 = GradientDescent(f2)
o1 = sf1.solveFixedSteps(array([1.,1.]), .5)
o2 = sf2.solveFixedSteps(array([1.,1.]), .5)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_wireframe(X0,X1,y1,color='r')
ax.plot_wireframe(X0,X1,y2,color='b')
ax.plot([o1[0]],[o1[1]],[f1.value(o1)],'or')
ax.plot([o2[0]],[o2[1]],[f2.value(o2)],'ob')
plt.show()
