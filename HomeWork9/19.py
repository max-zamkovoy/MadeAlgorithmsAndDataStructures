n = int(input())
points = []
for i in range(n):
	points.append(list(map(float,input().split(' '))))
n = int(input())
res = [0 for i in range(n)]
for i in range(n):
	cords = list(map(float,input().split(' ')))
	for p in points:
		if cords[0] <= p[0] and p[0] < cords[2] and cords[1] <= p[1] and p[1] < cords[3]:
			res[i] += 1
for i in range(n):
	print(res[i])