from numpy import *
from matplotlib.pyplot import *

def iterative_alignment(D,T=5):
    n,d = D.shape
    v = mat(random.rand(d)).T
    v = v/linalg.norm(v)
    A = D.T*D/n
    for t in range(T):
        v = A*v
        v = v/linalg.norm(v)
        print v.T
    return v
    

D = mat(loadtxt('points.mat'))
v = iterative_alignment(D)
print "\nIterative Alignment:\n",v.T

A = mat(D).T*mat(D)
U,s,V = linalg.svd(A)
print "\nEigenvectors:\n",U
