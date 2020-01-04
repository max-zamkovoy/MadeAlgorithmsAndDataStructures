from math import sqrt
def orientation(a,b,c):
    #Returns 0 if collinear, 1 if clockwise, 2 if counterclockwise
    # u = [b[0] - a[0], b[1] - a[1]]
    # v = [c[0] - b[0], c[1] - b[1]]
    # val = u[0] * v[1] - u[1] * v[0]
    val = (b[1] - a[1]) * (c[0] - b[0]) - (b[0] - a[0]) * (c[1] - b[1])
    if val == 0:
            return 0
    if val > 0:
        return 1
    else:
        return 2

def distance(a,b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

def quicksort(A, lo = None, hi = None):
    if lo == None:
        lo = 0
        hi = len(A) - 1
    if lo < hi:
        p = partition(A, lo, hi)
        quicksort(A, lo, p - 1)
        quicksort(A, p + 1, hi)

def next_to_top(my_stack):
    try:
        return my_stack[-2]
    except:
        return [0,0]

def partition(A, lo, hi):
    global point_0
    pivot = A[hi]
    i = lo
    for j in range(lo,hi):
        o = orientation(point_0,A[j],pivot)
        if o == 2 or o == 0 and distance(point_0,pivot) >= distance(point_0,A[j]):
            A[i], A[j] = A[j], A[i]
            i += 1
    A[i], A[hi] = A[hi], A[i]
    return i


n = int(input())
points = []
point_0 = [100000,100000]
min_ind = -1
for i in range(n):
    points.append(list(map(float,input().split(' '))))
    if points[i][1] < point_0[1] or (points[i][1] == point_0[1] and points[i][0] < point_0[0]):
        point_0 = points[i]
        min_ind = i
points[0],points[min_ind] = points[min_ind],points[0]
quicksort(points)
len_p = len(points)
m = 1
pointer = 1
for i in range(1,len_p):
    if pointer >= len_p - 1:
        break
    # or_ = orientation(point_0,points[pointer],points[pointer + 1])
    if orientation(point_0,points[pointer],points[(pointer + 1)]) == 0:
        pointer += 1
    else:
        points[m] = (points[pointer])
        m += 1
        pointer += 1
if points[m - 1] != points[pointer]:
    points[m] = points[pointer]
    m += 1
if m < 3:
    if m == 2:
        print(sqrt(distance(points[0],points[1])) * 2)
    else:
        print(0)
else:
    points = points[:m]
    stack = []
    stack.append(points[0])
    stack.append(points[1])
    stack.append(points[2])
    for i in range(3, m):
        while len(stack) > 1 and orientation(stack[-2], stack[-1], points[i]) != 2:
            stack.pop()
        stack.append(points[i])
    summ = 0
    left = 0
    for i in range(1,len(stack)):
        summ += sqrt(distance(stack[i - 1],stack[i]))
    summ += sqrt(distance(stack[0],stack[-1]))
    print(summ)