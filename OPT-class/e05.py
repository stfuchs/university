from numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

class Function:
    def __init__(self,n):
        self.n = n

    ''' limits = [[xmin,xmay,xnum],[ymin,ymax,ynum]] '''
    def grid_values(self,x1,x2):
        y = zeros([len(x2),len(x1)])
        for j in range(len(x2)):
            for i in range(len(x1)):
                y[j,i] = self.value(mat([ [x1[i]] , [x2[j] ]]))

        return y

#-----------------------------------------------------------------------
# Functions
#-----------------------------------------------------------------------


''' f(x) = (1,..,1)*x'''
class Objective(Function):
    def __init__(self,n):
        Function.__init__(self,n)
        self.a = mat(ones(n))

    def value(self,x):    return float(self.a * x)
    def gradient(self,x): return self.a.T

''' g(x) = x.T*x - 1 '''
class Constraint1(Function):
    def __init__(self,n):
        Function.__init__(self,n)

    def value(self,x):    return float(x.T * x - 1.)
    def gradient(self,x): return 2.*x

''' g(x) = -(1,...,0)*x'''
class Constraint2(Function):
    def __init__(self,n):
        Function.__init__(self,n)
        self.a = mat(zeros(n))
        self.a[0,0] = -1.

    def value(self,x):    return float(self.a * x)
    def gradient(self,x): return self.a.T



''' h(x) = f(x) + mu * sum(g_i(x)^2) '''
class SquaredPenalty(Function):
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

    def solve(self, start, sigma=0.0001, psi_ap=1.2, psi_am=.5, psi_ls=.01):
        x = [start]
        i = 0
        alpha = 1.
        while True:
            val = self.f.value(x[i])
            grad = self.f.gradient(x[i])
            d = - grad / linalg.norm(grad)
            ad = alpha * d
            while (self.f.value(x[i] + ad) > val + psi_ls*grad.T*ad):
                alpha = psi_am * alpha
                ad = alpha * d

            x.append(x[i] + ad)
            alpha = psi_ap * alpha
            i = i+1

            if (linalg.norm(x[i-1] - x[i]) < sigma or i > 10000): break

        return x

class SquaredPenaltyMethod:
    def __init__(self, inner_minimizer):
        self.argmin = inner_minimizer
    def setFunction(self, function):
        self.f = function
    def setConstraints(self, constraints):
        self.g = constraints

    def solve(self, start, sigma, eps):
        self.h = SquaredPenalty(self.f, self.g)
        self.argmin.setFunction(self.h)

        dx = sigma + 1.
        x = [start]
        xx = []
        i = 0
        mu = 1.
        gmax = eps + 1.
        while(dx > sigma and gmax > eps):
            self.h.setMu(mu)
            xx.extend(self.argmin.solve(x[i], 10.*sigma))
            x.append(xx[-1])
            i = i+1
            dx = linalg.norm(x[i-1] - x[i])
            mu = 2.*mu
            gmax = 0
            print x[i].T, gmax
            for g in self.g:
                gmax = max(g.value(x[i]), gmax)

        return x, xx

n = 2
f1 = Objective(n)
g1 = Constraint1(n)
g2 = Constraint2(n)

x_init = mat([[-.5] for i in range(n)])

sp = SquaredPenaltyMethod(GradientDescent())
sp.setFunction(f1)
sp.setConstraints([g1,g2])
path, path_dense = sp.solve(x_init, 0.001, 0.05)
path = hstack(array(path))
path_dense = hstack(array(path_dense))

if n==2:
    xx1 = (-2.,2.,50)
    xx2 = (-2.,2.,50)
    x1 = linspace(xx1[0],xx1[1],xx1[2])
    x2 = linspace(xx2[0],xx2[1],xx2[2])
    X1,X2 = meshgrid(x1,x2)

    g1y = g1.grid_values(x1,x2)
    g2y = g2.grid_values(x1,x2)

    fig = plt.figure(figsize=(830/80.,800/80.), dpi=80)
    ax = fig.add_subplot(111)

    # area fill for constrains
    ax.contourf(X1,X2,g1y,[0,100.], alpha = 0.2,colors='b')
    ax.contourf(X1,X2,g2y,[0,100.], alpha = 0.2,colors='r')

    # contour plot for penalized function
    h = SquaredPenalty(f1,[g1,g2])
    h.setMu(1.)
    hy = h.grid_values(x1,x2)
    m = hy.min()
    ma = abs(m)
    cs = ax.contour(X1,X2,hy,[m+.5*ma,m+2.*ma,m+5.*ma,m+10.*ma,m+50.*ma,m+100.*ma])
    #cs = ax.contour(X1,X2,hy,20)

    # plot descent path
    ax.plot(path_dense[0,:],path_dense[1,:],'bx-')
    ax.plot(path[0,:],path[1,:],'rx')

    ax.axis('equal')
    ax.set_xlim(xx1[0]+0.01,xx1[1]-0.01)
    ax.set_ylim(xx2[0]+0.01,xx2[1]-0.01)
    ax.grid()
    ax.clabel(cs,inline=1, fontsize=10)
    plt.show()
