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

positionsScientist = []
positionsParticipant = []

for line in lines:
    matchIdx = line.find("Scientist controller")
    if (matchIdx != -1):
        matchIdx = line.find("(")
        posStr = line[matchIdx:len(line) - 1]
        pos = ast.literal_eval(posStr)
        positionsScientist.append(pos)
    
    matchIdx = line.find("Paticipant controller")
    if (matchIdx != -1):
        matchIdx = line.find("(")
        posStr = line[matchIdx:len(line) - 1]
        pos = ast.literal_eval(posStr)
        positionsParticipant.append(pos)

if (positionsScientist.size() != positionsParticipant.size()):
    print("Mismatch between number of scientist and participant data.")
    quit()

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.set_xlabel('x (m)')
ax.set_ylabel('y (m)')
ax.set_zlabel('z (m)')
c = [0.0, 0.0, 0.0]
idx = 0
for i in range(positions.size()):
    scientistPos = positionsScientist[i]
    participantPos = positionsParticipant[i]
    ax.scatter(scientistPos[0],scientistPos[1],scientistPos[2],marker='o', color=(0.0, 1.0, 0.0))
    ax.scatter(participantPos[0],participantPos[1],participantPos[2],marker='o', color=(0.0, 0.0, 1.0))
plt.show()