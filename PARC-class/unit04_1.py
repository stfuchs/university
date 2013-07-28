# ----------
# User Instructions:
# 
# Define a function, search() that takes no input
# and returns a list
# in the form of [optimal path length, x, y]. For
# the grid shown below, your function should output
# [11, 4, 5].
#
# If there is no valid path from the start point
# to the goal, your function should return the string
# 'fail'
# ----------

# Grid format:
#   0 = Navigable space
#   1 = Occupied space

grid = [[0, 1, 0, 0, 0, 0],
        [0, 0, 0, 1, 1, 0],
        [0, 1, 1, 0, 0, 0],
        [0, 1, 0, 1, 1, 0],
        [0, 0, 0, 0, 0, 0]]

heuristic = [[0, 0, 20, 30, 40, 40],
            [10, 20, 30, 40, 40, 30],
            [20, 30, 40, 40, 30, 20],
            [30, 40, 40, 30, 20, 10],
            [40, 90, 30, 20, 10, 0]]

init = [0, 0]
goal = [len(grid)-1, len(grid[0])-1] # Make sure that the goal definition stays in the function.

delta = [[-1, 0 ], # go up
        [ 0, -1], # go left
        [ 1, 0 ], # go down
        [ 0, 1 ]] # go right

delta_name = ['^', '<', 'v', '>']

cost = 1

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

def pick_state(opened):
    g_min = 100000
    g_min_idx = 0
    for i in range(len(opened)):
        if (opened[i][0] + opened[i][1]) < g_min:
            g_min = opened[i][0] + opened[i][1]
            g_min_idx = i

    return opened.pop(g_min_idx)

def search():
    opened = [[0] + [ heuristic[init[0]][init[1]] ] + init]
    closed = [[0 for row in range(Y)] for col in range(X)]
    closed[init[0]][init[1]] = 1
    action = [[-1 for row in range(Y)] for col in range(X)]
    expand = [[-1 for row in range(Y)] for col in range(X)]
    path = [[' ' for row in range(Y)] for col in range(X)]
    success = False
    count = 0
    while not success:
        if len(opened) == 0:
            return 'fail'

        g, h, x, y = pick_state(opened)
        if x == goal[0] and y == goal[1]:
            success = True
        
        expand[x][y] = count
        count += 1
        for i in range(len(delta)):
            x_n = x + delta[i][0]
            y_n = y + delta[i][1]
            if is_valid_position(x_n, y_n, closed):
                opened.append([g + cost, heuristic[x_n][y_n], x_n, y_n])
                closed[x_n][y_n] = 1
                action[x_n][y_n] = i

    print_mat (expand)
    x = goal[0]
    y = goal[1]
    path[x][y] = '*'
    while x != init[0] or y != init[1]:
        command = action[x][y]
        x = x - delta[command][0]
        y = y - delta[command][1]
        path[x][y] = delta_name[command]
        
    print_mat (path)
    return path

search()
