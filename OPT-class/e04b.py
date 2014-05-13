from numpy import *
import matplotlib.pyplot as plt

def f(x):
    return x**2+1.

def g(x):
    return (x-2.)*(x-4.)

x = linspace(-1.,5.)
l = array([0, 0.1, 0.5, 1., 2., 3.])
y = zeros([len(l),len(x)])

fig = plt.figure()
ax = fig.add_subplot(111)

for i in range(len(l)):
    y[i,:] = f(x) + l[i] * g(x)
    ax.plot(x,y[i,:], label=r'$\lambda_'+str(i)+'= '+str(l[i])+'$')

#handles, lables = ax.get_legend_handles_labels()
ax.legend()
ax.grid()
plt.show()


