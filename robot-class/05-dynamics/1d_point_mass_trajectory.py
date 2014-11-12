from numpy import *
from matplotlib.pyplot import *

class PointMass1D:
    def __init__(self, mass):
        self.m_inv = 1./mass

    def __call__(self, x, u):
        return array([ x[1], u*self.m_inv ]).T

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
        if(((i-1)*tau*100)%100==False):
            X[i,1] += .1 * random.randn()
        u = pid(X[i,:], Xs[i,:])
        X[i+1,:] = X[i,:] + tau * f(X[i,:], u)


m = 3.456
tau = .01
T = 5.
x0 = array([0,0])

t = arange(0,T,tau)
X1 = zeros([t.size,2])
X2 = zeros([t.size,2])
X3 = zeros([t.size,2])
X1[0,:] = x0
X2[0,:] = x0
X3[0,:] = x0
Xs = vstack([cos(t), zeros(t.size)]).T

f = PointMass1D(m)

# oscillate
lam = .03
eps = .01

Kp = m / lam**2
Kd = 2.* m * eps / lam**2
print "--- oscillate ---\nKp: ", Kp, "\nKd: ", Kd
pid = PID(Kp,0,Kd)
simulate(f, pid, tau, Xs, X1)

# overdamped
lam = .03
eps = .07

Kp = m / lam**2
Kd = 2.* m * eps / lam**2
print "\n--- over-damped ---\nKp: ", Kp, "\nKd: ", Kd
pid = PID(Kp,0,Kd)
simulate(f, pid, tau, Xs, X2)

# critical
lam = .03
eps = .03

Kp = m / lam**2
Kd = 2.* m * eps / lam**2
print "\n--- critical ---\nKp: ", Kp, "\nKd: ", Kd
pid = PID(Kp,0,Kd)
simulate(f, pid, tau, Xs, X3)


fig = figure()
ax = fig.add_subplot(111)

ax.plot(t, X1[:,0], label='oscillate')
ax.plot(t, X2[:,0], label='over-damped')
ax.plot(t, X3[:,0], label='critical')
#ax.plot(t, X3[:,1], label='crit vel')
ax.plot(t, Xs[:,0], label='qs')
ax.legend()
ax.grid()
show()
