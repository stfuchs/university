if 1==1:
    colors = [['red', 'green', 'green', 'red' , 'red'],
              ['red', 'red', 'green', 'red', 'red'],
              ['red', 'red', 'green', 'green', 'red'],
              ['red', 'red', 'red', 'red', 'red']]
    measurements = ['green', 'green', 'green' ,'green', 'green']
    motions = [[0,0],[0,1],[1,0],[1,0],[0,1]]
else:
    
    colors = [['green', 'green' , 'green'],
              ['green', 'red', 'red'],
              ['green', 'green', 'green']]         
    measurements = ['red', 'red']
    motions = [[0,0],[0,1]]

colors = [['green','green'],
          ['red','green']]

measurements = ['red', 'red']
motions = [[0,0],[1,0]]

sensor_right = .8
p_move = 1.0

def show(p):
    for i in range(len(p)):
        print p[i]

#DO NOT USE IMPORT
#ENTER CODE BELOW HERE
#ANY CODE ABOVE WILL CAUSE
#HOMEWORK TO BE GRADED
#INCORRECT

nx = len(colors[0])
ny = len(colors)
p = [[1./(nx*ny) for x in range(nx)] for y in range(ny)]

def sense(p, Z):
    q=[]
    s=0
    for y in range(ny):
        qy=[]
        for x in range(nx):
            hit = (Z == colors[y][x])
            qy.append(p[y][x] * (hit * sensor_right + (1-hit) * (1-sensor_right)))
        q.append(qy)
        s = s + sum(qy)
    
    s = 1./s
    for y in range(ny):
        for x in range(nx):
            q[y][x] = q[y][x] * s

    return q

def move(p, U):
    if U[0] == U[1] == 0: return p
    q = []
    for y in range(ny):
        qy=[]
        for x in range(nx):
            s = (p[y][(x-U[1])%nx] * U[1] + p[(y-U[0])%ny][x] * U[0]) * p_move
            s = s + p[y][x] * (1-p_move)
            qy.append(s)
        q.append(qy)
    return q


#for k in range(len(motions)):
#    p = move(p,motions[k])
#    p = sense(p,measurements[k])

p = move(p,motions[0])
show(p)
p = sense(p,measurements[0])
show(p)
p = move(p,motions[1])
#Your probability array must be printed 
#with the following code.

show(p)


