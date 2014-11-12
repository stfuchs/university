from numpy import *
from matplotlib.pyplot import *

class PointMass1D:
    def __init__(self, mass):
        self.m = mass
        self.m_inv = 1./mass

    def __call__(self, x, u):
        return array([ x[1], u*self.m_inv]).T

class PID:
    def __init__(self, kp, ki, kd):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.err_i = 0

    def __call__(self, xt, xs):
        dx = xs-xt
        self.err_i = self.err_i + dx[0]
        return self.kp*dx[0] + self.kd*dx[1] + self.ki*self.err_i

def simulate(f, pid, tau, Xs, X):
    xs = array([0,0])
    for i in range(X[:,0].size-1):
        if(((i)*tau*100)%100==False):
            X[i,1] += 10. * random.randn()
        u = pid(X[i,:], Xs[i,:])
        X[i+1,:] = X[i,:] + tau * f(X[i,:], u)


m = 3.456
tau = .01
T = 10.
x0 = array([0,0])


t = arange(0,T,tau)
X = zeros([3,t.size,2])
X[:,0,:] = x0
f = PointMass1D(m)
Xs = vstack([cos(t), -sin(t)]).T

lam = array([.03,.1,.1])
xi = array([.5,1.5,1.])

for i in range(3):
    Kp = m / (lam[i]**2.)
    Kd = 2.* m * xi[i] / lam[i]
    pid = PID(Kp,0.,Kd)
    simulate(f, pid, tau, Xs, X[i,:,:])

fig = figure()
ax = fig.add_subplot(111)

ax.plot(t, X[0,:,0], label='oscillate')
#ax.plot(t, X[1,:,0], label='over-damped')
#ax.plot(t, X[2,:,0], label='critical')
ax.plot(t, Xs[:,0], label='qs')

ax.legend(loc='best')
ax.grid()
show()
