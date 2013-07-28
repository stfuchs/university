# ----------
# User Instructions:
# 
# Create a function compute_value() which returns
# a grid of values. Value is defined as the minimum
# number of moves required to get from a cell to the
# goal. 
#
# If it is impossible to reach the goal from a cell
# you should assign that cell a value of 99.

# ----------

grid = [[0, 0, 1, 0, 0, 0],
        [0, 0, 1, 0, 0, 0],
        [0, 0, 1, 0, 0, 0],
        [0, 0, 1, 0, 1, 0],
        [0, 0, 1, 1, 1, 0],
        [0, 0, 0, 0, 1, 0]]

init = [0, 0]
goal = [len(grid)-1, len(grid[0])-1]

delta = [[-1, 0 ], # go up
         [ 0, -1], # go left
         [ 1, 0 ], # go down
         [ 0, 1 ]] # go right

delta_name = ['^', '<', 'v', '>']

cost_step = 1 # the cost associated with moving from a cell to an adjacent one.

# ----------------------------------------
# insert code below
# ----------------------------------------
X = len(grid)
Y = len(grid[0])

def print_mat(matrix):
    for i in range(len(matrix)):
        print matrix[i]

def is_valid_position(x, y, closed):
    if x < 0 or x >= X or y < 0 or y >= Y:
        return False
    if grid[x][y] == 1 or closed[x][y] == 1:
        return False
    return True

def pick_cell(opened):
    g_min = 100000
    g_min_idx = 0
    for i in range(len(opened)):
        if opened[i][0] < g_min:
            g_min = opened[i][0]
            g_min_idx = i

    return opened.pop(g_min_idx)

def pick_best_command(x, y, values):
    v_min = 99
    v_idx = 0
    for i in range(len(delta)):
        x_n = x + delta[i][0]
        y_n = y + delta[i][1]
        if x_n >= 0 and y_n >= 0 and x_n < X and y_n < Y and grid[x_n][y_n] != 1:
            if values[x_n][y_n] < v_min:
                v_idx = i
                v_min = values[x_n][y_n]
    
    return v_idx

def compute_value():
    opened = [[0] + goal]
    closed = [[0 for row in range(Y)] for col in range(X)]
    closed[goal[0]][goal[1]] = 1
    value = [[99 for row in range(Y)] for col in range(X)]
    value[goal[0]][goal[1]] = 0
    policy = [[' ' for row in range(Y)] for col in range(X)]
    policy[goal[0]][goal[1]] = '*'

    while opened:
        v,x,y = pick_cell(opened)     
        for i in range(len(delta)):
            x_n = x + delta[i][0]
            y_n = y + delta[i][1]
            if is_valid_position(x_n, y_n, closed):
                opened.append([v + cost_step, x_n, y_n])
                closed[x_n][y_n] = 1
                value[x_n][y_n] = v + cost_step

    print_mat(value)
    
    for x in range(X):
        for y in range(Y):
            if (value[x][y] == 99 or value[x][y] == 0):
                continue
            policy[x][y] = delta_name[pick_best_command(x,y,value)]

 
    return policy #make sure your function returns a grid of values as demonstrated in the previous video.

print_mat(grid)
print_mat(compute_value())
