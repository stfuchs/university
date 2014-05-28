from numpy import *
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

class Function:
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


''' f(x) = a*x + b '''
class LinearFunction(Function):
    def __init__(self,a,b):
        self.a = mat(a).T
        self.b = b

    def value(self,x):
        return float(x.T*self.a + self.b)
    def gradient(self,x):
        return self.a

''' f(x) = a*x.T*x + b*x +c '''
class QuadraticFunction(Function):
    def __init__(self,A,b,c):
        self.A = mat(A)
        self.b = mat(b).T
        self.c = c

    def value(self,x):
        return float( x.T*self.A*x + x.T*self.b + self.c )
    def gradient(self,x):
        return 2.*self.A*x + self.b


''' h(x) = f(x) + mu * sum(g_i(x)^2) '''
class SquaredPenalty(Function):
    def __init__(self, function, constraints):
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

class LogBarrier(Function):
    def __init__(self, function, constraints):
        self.f = function
        self.g = constraints
        self.mu = 1.

    def setMu(self, mu): self.mu = mu

    def value(self, x):
        res = self.f.value(x)
        for g in self.g:
            res = res - self.mu * log(-g.value(x))
        return res

    def gradient(self, x):
        res = self.f.gradient(x)
        for g in self.g:
            res = res - self.mu * g.gradient(x) / g.value(x)
            #print g.gradient(x).T, g.value(x)
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
            while (self.f.value(x[i] + ad) > val + psi_ls*grad.T*ad
                   or math.isnan(self.f.value(x[i] + ad)) ):
                alpha = psi_am * alpha
                ad = alpha * d

            x.append(x[i] + ad)
            alpha = psi_ap * alpha
            i = i+1

            if (linalg.norm(x[i-1] - x[i]) < sigma or i > 50): break

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

        x = [start]
        xx = []
        i = 0
        mu = 1.
        print x[i].T
        while True:
            self.h.setMu(mu)
            xx.extend(self.argmin.solve(x[i], 10.*sigma))
            x.append(xx[-1])
            i = i+1
            dx = linalg.norm(x[i-1] - x[i])
            mu = 2.*mu
            gmax = 0
            report = str(x[i].T)+": "
            for g in self.g:
                vg = g.value(x[i])
                gmax = max(vg, gmax)
                report = report + str( mu*(vg>0)*vg ) + " "
            print report
            if dx < sigma or gmax < eps: break

        return x, xx

class LogBarrierMethod:
    def __init__(self, inner_minimizer):
        self.argmin = inner_minimizer
    def setFunction(self, function):
        self.f = function
    def setConstraints(self, constraints):
        self.g = constraints

    def solve(self, start, sigma):
        self.h = LogBarrier(self.f, self.g)
        self.argmin.setFunction(self.h)

        x = [start]
        xx = []
        i = 0
        mu = 1.
        print x[i].T
        while True:
            self.h.setMu(mu)
            xx.extend(self.argmin.solve(x[i], 10.*sigma))
            x.append(xx[-1])
            i = i+1
            dx = linalg.norm(xx[-1] - xx[-2])
            mu = .5*mu
            report = str(x[i].T)+": "
            for g in self.g:
                report = report + str( 2.*mu / g.value(x[i]) ) + " "
            print report
            if (dx < sigma): break

        return x, xx


n = 2
eps = 0.001
x_init = mat(ones(n+1)).T
x_init[-1] = float(n)

p = LinearFunction(hstack([zeros(n),1]),0) # phase 1
g1 = QuadraticFunction(diag(hstack([ones(n),0])),hstack([zeros(n),-1]),-1)
g2 = LinearFunction(hstack([-1,zeros(n-1),-1]),0)
g3 = LinearFunction(hstack([zeros(n),-1]),-eps)

sp = LogBarrierMethod(GradientDescent())
sp.setFunction(p)
sp.setConstraints([g1,g2,g3])
print "\nPhase 1:\n"
path, path_dense = sp.solve(x_init, 0.01)
path = hstack(array(path))
path_dense = hstack(array(path_dense))


f = LinearFunction(ones(n),0) # objective
g1 = QuadraticFunction(diag(ones(n)),zeros(n),-1)
g2 = LinearFunction(hstack([-1,zeros(n-1)]),0)

lg = LogBarrierMethod(GradientDescent())
lg.setFunction(f)
lg.setConstraints([g1,g2])
print "\nLog Barrier:\n"
path2, path2_dense = lg.solve(mat(path[0:-1,-1]).T, 0.0001)
path2 = hstack(array(path2))
path2_dense = hstack(array(path2_dense))




if n==2:
    xx1 = (-1.5,1.5,100)
    xx2 = (-1.5,1.5,100)
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
    '''
    h = SquaredPenalty(f1,[g1,g2])
    h.setMu(1.)
    hy = h.grid_values(x1,x2)
    m = hy.min()
    ma = abs(m)
    cs = ax.contour(X1,X2,hy,[m+.5*ma,m+2.*ma,m+5.*ma,m+10.*ma,m+50.*ma,m+100.*ma])
    cs = ax.contour(X1,X2,hy,20)
    '''
    # contour plot for log barrier function
    h = LogBarrier(f,[g1,g2])
    #h.setMu(2.**(-0))
    y = h.grid_values(x1,x2)
    cs = ax.contour(X1,X2,y)

    # plot descent path
    ax.plot(path_dense[0,:],path_dense[1,:],'bx-')
    ax.plot(path[0,:],path[1,:],'rx')
    ax.plot(path2_dense[0,:],path2_dense[1,:],'gx-')
    ax.plot(path2[0,:],path2[1,:],'rx')

    ax.axis('equal')
    ax.set_xlim(xx1[0]+0.01,xx1[1]-0.01)
    ax.set_ylim(xx2[0]+0.01,xx2[1]-0.01)
    ax.grid()
    ax.clabel(cs,inline=1, fontsize=10)
    plt.show()
