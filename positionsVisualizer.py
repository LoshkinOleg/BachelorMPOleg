import sys
import ast
import matplotlib.pyplot as plt

logFile = open(sys.argv[1])
lines = logFile.readlines()

positions = []

for line in lines:
    matchIdx = line.find("(")
    if (matchIdx != -1):
        posStr = line[matchIdx:len(line) - 1]
        pos = ast.literal_eval(posStr)
        positions.append(pos)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.set_xlabel('x (m)')
ax.set_ylabel('y (m)')
ax.set_zlabel('z (m)')
for pos in positions:
    print(pos[0],pos[1],pos[2])
    ax.scatter(pos[0],pos[1],pos[2],marker='o')
plt.show()