from numpy import *
from matplotlib.pyplot import *
import cma

class Function:
    ''' limits = [[xmin,xmay,xnum],[ymin,ymax,ynum]] '''
    def __call__(self, x):
        return self.value(x)

    def grid_values(self,x1,x2):
        y = zeros([len(x2),len(x1)])
        for j in range(len(x2)):
            for i in range(len(x1)):
                y[j,i] = self.value(mat([ [x1[i]] , [x2[j] ]]))

        return y

class Rosenbrock(Function):
    def __init__(self, a = 1., b = 100.):
        self.a = a
        self.b = b

    def value(self, x):
        f = 0
        for i in range(len(x)-1):
            f = f + (self.a - x[i])**2 + self.b*(x[i+1] - x[i]**2)**2
        return f

class Twiddle():
    def __init__(self): None

    def fmin(self, f, x0, alpha):
        xlog = [x0]
        flog = [f(x0)]
        elog = [0]
        x = x0
        while True:
            xold = array(x)
            for i in range(len(x0)):
                tw = array([x,x,x])
                tw[0,i] = x[i] - alpha[i]
                tw[1,i] = x[i]
                tw[2,i] = x[i] + alpha[i]
                x = tw[argmin([f(tw[0]),f(tw[1]),f(tw[2])])]
                if x[i] == tw[1,i]:
                    alpha[i] = .5*alpha[i]
                else:
                    alpha[i] = 1.5*alpha[i]
            xlog.append(x)
            flog.append(f(x))
            elog.append(elog[-1] + len(x0)*3)
            if (linalg.norm(alpha) <= 10**-7 and
                linalg.norm(xold - x) <= 10**-7):
                print "\nTwiddle solution after",len(flog),"iterations"
                print "and",elog[-1],"function evaluations:"
                print "best x:", xlog[-1]
                print "f-value:", flog[-1]
                break
        return [range(1,len(flog)+1),vstack(xlog),flog,elog]

n = 10

x0 = hstack([0,ones(n-1)])
sigma0 = 0.1
f = Rosenbrock(.01,1.)
res_cma = cma.fmin(f,x0,sigma0)
logger = res_cma[-1]  # the CMADataLogger
logger.load()  # by "default" data are on disk

twiddle = Twiddle()
res_twi = twiddle.fmin(f,x0,ones(n))



if n==2:
    xx1 = (-1.,1.,50)
    xx2 = (-.5,1.5,50)
    x1 = linspace(xx1[0],xx1[1],xx1[2])
    x2 = linspace(xx2[0],xx2[1],xx2[2])
    X1,X2 = meshgrid(x1,x2)

    fy = f.grid_values(x1,x2)

    fig = figure(figsize=(830/80.,800/80.), dpi=80)
    ax = fig.add_subplot(111)

    cs = ax.contour(X1,X2,fy,20,linewidth=.5,colors='k')
    ax.imshow(fy,origin='lower',extent=[xx1[0],xx1[1],xx2[0],xx2[1]],cmap=cm.YlOrRd)
    ax.plot(logger.xmean[:,-2],logger.xmean[:,-1],'bx-',label='CMA-ES')
    ax.plot(res_twi[1][:,0], res_twi[1][:,1], 'gx-',label='Twiddle')
    ax.legend()
    ax.grid()
    #ax.clabel(cs,inline=1, fontsize=10)
'''
fig2 = figure(figsize=(830/80.,800/80.), dpi=80)
ax2 = fig2.add_subplot(111)
ax2.semilogy(logger.f[:,0], logger.f[:,5],label='CMA-ES')  # plot f versus iteration
ax2.semilogy(res_twi[0], res_twi[2],label='Twiddle')
ax2.legend()
ax2.set_ylabel(r'$f(x_{best})$',size=16)
ax2.set_xlabel('iteration')
ax2.grid()
'''
fig3 = figure(figsize=(830/80.,800/80.), dpi=80)
ax3 = fig3.add_subplot(111)
ax3.semilogy(logger.f[:,1], logger.f[:,5],label='CMA-ES')
ax3.semilogy(res_twi[3],res_twi[2],label='Twiddle')
ax3.legend()
ax3.set_ylabel(r'$f(x_{best})$',size=16)
ax3.set_xlabel('number of function evaluations')
ax3.grid()


show()
