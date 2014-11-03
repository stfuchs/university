from numpy import *
from matplotlib.pyplot import *

def gram_schmidt(A):
    m,n = A.shape
    Q = mat(vstack([identity(n),zeros([m-n,n])]))
    for k in range(n):
        u = A[:,k]
        for i in range(k):
            u = u - float(Q[:,i].T*u) * Q[:,i]
        Q[:,k] = u/linalg.norm(u)
    return Q, Q.T*A

def householder(A):
    m,n = A.shape
    Q = mat(identity(m))
    R = A
    for k in range(n):
        ak = R[k:,k]
        bk = mat(hstack([1.,zeros(m-k-1)])).T
        v = ak - linalg.norm(ak)*bk
        v = v/linalg.norm(v)
        Qkm = identity(m-k) - 2.*v*v.T
        Qk = mat(vstack([hstack([identity(k), zeros([k,m-k])]),
                         hstack([zeros([m-k,k]), Qkm])]))
        Q = Q*Qk.T
        R = Qk * R
    return Q,R

def solve(A,b):
    m,n = A.shape
    x = zeros(n)
    for i in range(n):
        ni = n-1-i
        nom = b[ni]
        for j in range(ni,n):
            nom = nom - A[ni,j]*x[j]
        x[ni] = nom/A[ni,ni]
    return mat(x).T
    

A = mat([[1.,2.,3.],[4.,5.,6.],[7.,8.,0],[1.,0,0]])
Q1,R1 = gram_schmidt(A)
Q2,R2 = householder(A)

print "Gram-Schmidt QR:\n",Q1
print R1, "\n"
print "Householder QR:\n",Q2
print R2

A2 = mat([[1,1],[1,0],[1,0]])
p = mat([0,0,1]).T
Q,R = gram_schmidt(A2)
x = solve(R,Q.T*p)

