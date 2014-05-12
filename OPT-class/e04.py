from numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

class Function:
    def __init__(self,n):
        self.n = n

    ''' limits = [[xmin,xmay,xnum],[ymin,ymax,ynum]] '''
    def grid_values(self,x1,x2):
        y = zeros([len(x1),len(x2)])
        for i in range(len(x1)):
            for j in range(len(x2)):
                y[i,j] = self.value(mat([ [x1[i]] , [x2[j] ]]))

        return y

#-----------------------------------------------------------------------
# Functions
#-----------------------------------------------------------------------

''' f(x) = 1-exp(-x.T*C*x) '''
class Fhole(Function):
    def __init__(self,n,c):
        Function.__init__(self,n)
        self.C = mat(zeros([n,n]))
        for i in range(n):
            self.C[i,i] = c**(i/(n-1.))

    def value(self,x):    return float(1.-exp(-x.T * self.C * x))
    def gradient(self,x): return 2. * self.C * x * exp(-x.T * self.C * x)

''' g(x) = x.T*x - 1 '''
class Constraint1(Function):
    def __init__(self,n):
        Function.__init__(self,n)

    def value(self,x):    return float(x.T * x - 1.)
    def gradient(self,x): return 2.*x

''' g(x) = (0,...,1)*x + 1/c '''
class Constraint2(Function):
    def __init__(self,n,c):
        Function.__init__(self,n)
        self.c = c
        self.a = mat(zeros(n))
        self.a[0,-1] = 1.

    def value(self,x):    return float(self.a * x + 1./self.c)
    def gradient(self,x): return mat([[1.],[0]])

''' h(x) = f(x) + mu * sum(g_i(x)^2) '''
class SquaredPenalizer(Function):
    def __init__(self, function, constraints):
        Function.__init__(self, function.n)
        self.f = function
        self.g = constraints
        self.mu = 1.

    def setMu(self, mu): self.mu = mu

    def value(self, x):
        res = self.f.value(x)
        for g in self.g:
            vg = g.value(x)
            res = res + (vg>0) * self.mu * vg**2
        return res

    def gradient(self, x):
        res = self.f.gradient(x)
        for g in self.g:
            vg = g.value(x)
            res = res + (vg>0) * self.mu * 2. * vg * g.gradient(x)
        return res

#-----------------------------------------------------------------------
# Optimizer
#-----------------------------------------------------------------------

class GradientDescent:
    def __init__(self):
        None
    def setFunction(self,function):
        self.f = function

    def solve(self, start, alpha = 0.1, sigma = 0.0001):
        dx = sigma + 1.
        x = [start]
        i = 0
        while(dx > sigma and i < 10000):
            x.append(x[i] - alpha * self.f.gradient(x[i]))
            dx = linalg.norm(x[i-1] - x[i])
            i = i+1

        return x

class SquaredPenalty:
    def __init__(self, inner_minimizer):
        self.argmin = inner_minimizer
    def setFunction(self, function):
        self.f = function
    def setConstraints(self, constraints):
        self.g = constraints

    def solve(self, start, sigma):
        self.h = SquaredPenalizer(self.f, self.g)
        self.argmin.setFunction(self.h)

        dx = sigma + 1.
        x = [start]
        xx = []
        i = 0
        mu = 1.
        eps = 1.
        while(dx > sigma and eps > 0.001):
            self.h.setMu(mu)
            xx.extend(self.argmin.solve(x[i], 0.1, 10.*sigma))
            x.append(xx[-1])
            dx = linalg.norm(x[i-1] - x[i])
            mu = 2.*mu
            eps = 0
            for g in self.g:
                eps = max(g.value(x[i]), eps)
            print shape(xx), eps
            i = i+1

        return x, xx


n = 2
c = 4.
x_init = mat([[.5] for i in range(n)])

f1 = Fhole(n,c)
g1 = Constraint1(n)
g2 = Constraint2(n,c)

sp = SquaredPenalty(GradientDescent())
sp.setFunction(f1)
sp.setConstraints([g1,g2])
path, path_dense = sp.solve(x_init, 0.001)
path = hstack(array(path))
path_dense = hstack(array(path_dense))

if n==2:
    xx1 = (-2.,2.,50)
    xx2 = (-2.,2.,50)
    x1 = linspace(xx1[0],xx1[1],xx1[2])
    x2 = linspace(xx2[0],xx2[1],xx2[2])
    X1,X2 = meshgrid(x1,x2)

    y = f1.grid_values(x1,x2)
    y1 = g1.grid_values(x1,x2)
    y2 = g2.grid_values(x1,x2)
    fig = plt.figure(figsize=(830/80.,800/80.), dpi=80)
    #ax = fig.add_subplot(111, projection='3d')
    #ax.plot_wireframe(X1,X2,y)

    ax = fig.add_subplot(111)
    if 1==1:
        ax.contourf(X1,X2,y1,[0,100.], alpha = 0.2,colors='b')
        ax.contourf(X1,X2,y2,[0,100.], alpha = 0.2,colors='r')
        y1[y1<=0] = ma.masked
        y2[y2<=0] = ma.masked
        y = y + 100.*(y1**2+y2**2)
        ymin = y.min()
        cs = ax.contour(X1,X2,y,[2.*ymin,5.*ymin,10.*ymin,20.*ymin,50.*ymin,100.*ymin])
        ax.plot(path_dense[0,:],path_dense[1,:],'bx-')
        ax.plot(path[0,:],path[1,:],'rx')
    else:
        cs = ax.contour(X1,X2,y)
        ax.contourf(X1,X2,y1,[0,100.], alpha = 0.2,colors='b')
        ax.contourf(X1,X2,y2,[0,100.], alpha = 0.2,colors='r')
        ax.plot(path_dense[0,:],path_dense[1,:],'bx-')
        ax.plot(path[0,:],path[1,:],'rx')

    ax.axis('equal')
    ax.set_xlim(xx1[0]+0.01,xx1[1]-0.01)
    ax.set_ylim(xx2[0]+0.01,xx2[1]-0.01)
    ax.grid()
    ax.clabel(cs,inline=1, fontsize=10)
    plt.show()

