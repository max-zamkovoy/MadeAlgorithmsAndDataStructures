from math import sqrt
import numpy as np
def orientation(a,b,c):
    #Returns 0 if collinear, 1 if clockwise, 2 if counterclockwise
    # u = [b[0] - a[0], b[1] - a[1]]
    # v = [c[0] - b[0], c[1] - b[1]]
    # val = u[0] * v[1] - u[1] * v[0]
    val = (b[1] - a[1]) * (c[0] - b[0]) - (b[0] - a[0]) * (c[1] - b[1])
    if abs(val) < 1e-10:
            return 0
    if val > 1e-10:
        return 1
    else:
        return 2

def distance(a,b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2

def quicksort(A, lo = None, hi = None):
    if lo == None:
        lo = 0
        hi = len(A) - 1
        # Create an auxiliary stack
    size = hi - lo + 1
    stack = [0] * (size)

    # initialize top of stack
    top = -1

    # push initial values of l and h to stack
    top = top + 1
    stack[top] = lo
    top = top + 1
    stack[top] = hi

    # Keep popping from stack while is not empty
    while top >= 0:

        # Pop h and l
        hi = stack[top]
        top = top - 1
        lo = stack[top]
        top = top - 1

        # Set pivot element at its correct position in
        # sorted array
        p = partition(A, lo, hi)

        # If there are elements on left side of pivot,
        # then push left side to stack
        if p - 1 > lo:
            top = top + 1
            stack[top] = lo
            top = top + 1
            stack[top] = p - 1

        # If there are elements on right side of pivot,
        # then push right side to stack
        if p + 1 < hi:
            top = top + 1
            stack[top] = p + 1
            top = top + 1
            stack[top] = hi

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

def perimeter(stack):
    summ = 0
    for i in range(1, len(stack)):
        summ += sqrt(distance(stack[i - 1], stack[i]))
    summ += sqrt(distance(stack[0], stack[-1]))
    return summ

def find_hull(points):
    len_p = len(points)
    m = 1
    pointer = 1

    #Removing the points on the same line, so that only the farthest of them stays
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
    if not np.array_equal(points[m - 1],points[pointer]):
        points[m] = points[pointer]
        m += 1
    #If there is less than 3 points
    if m < 3:
        if m == 2:
            #If two points - return doubled length of the line
            print(sqrt(distance(points[0],points[1])) * 2)
        else:
            print(0)
    else:
        #Creating the convex hull
        points = points[:m]
        stack = []
        stack.append(points[0])
        stack.append(points[1])
        stack.append(points[2])
        for i in range(3, m):
            while len(stack) > 1 and orientation(stack[-2], stack[-1], points[i]) != 2:
                stack.pop()
            stack.append(points[i])

        print(perimeter(stack))


n = int(input())
points = np.zeros((n,2),dtype='float16')
point_0 = [100000,100000]
min_ind = 0
for i in range(n):
    temp = (list(map(float,input().split(' '))))
    points[i][0], points[i][1] = temp[0],temp[1]
    if points[i][1] < point_0[1] or (points[i][1] == point_0[1] and points[i][0] < point_0[0]):
        point_0 = points[i]
        min_ind = i

points[0],points[min_ind] = points[min_ind],points[0]
quicksort(points)
find_hull(points)
