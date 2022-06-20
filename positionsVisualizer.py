import sys
import ast
import matplotlib.pyplot as plt

def lerp(a: float, b: float, t: float) -> float:
    """Linear interpolate on the scale given by a to b, using t as the point on that scale.
    Examples
    --------
        50 == lerp(0, 100, 0.5)
        4.2 == lerp(1, 5, 0.8)
    """
    return (1 - t) * a + t * b

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
c = [0.0, 0.0, 0.0]
idx = 0
for pos in positions:
    idx += 1
    print(pos[0],pos[1],pos[2])
    ax.scatter(pos[0],pos[1],pos[2],marker='o', color=(c[0], c[1], c[2]))
    c[0] = lerp(0, 1.0, idx / len(positions))
plt.show()