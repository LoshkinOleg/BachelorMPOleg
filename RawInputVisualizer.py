import sys
import ast
import matplotlib.pyplot as plt
import numpy as np
import os.path
import pandas as pd

import ExpCommon

pp = []
ap = []
sampleIdx = []
plotAll = False

# Ensure there's a filepath passed to the script.
if len(sys.argv) < 2:
    print("Please pass the relative path to .csv file as first argument to the command line.")
    quit()
elif len(sys.argv) == 2:
    plotAll = True
else:
    for i in range(2, len(sys.argv)):
        sampleIdx.append(int(sys.argv[i]))

df = pd.read_csv(sys.argv[1])
df = df.iloc[:, 0:4]

for i, value in np.ndenumerate(df['Actual'].to_numpy()):
    if plotAll or i[0] in sampleIdx:
        ap.append(ast.literal_eval(value))
        
for i, value in np.ndenumerate(df['Percieved'].to_numpy()):
    if plotAll or i[0] in sampleIdx:
        pp.append(ast.literal_eval(value))

if len(ap) != len(pp):
    print("Length of array of actual positions and array of percieved positions are mismatching!")
    quit()

# Configuring plot graph
fig = plt.figure()
ax = fig.add_subplot(projection='3d', proj_type='ortho', autoscale_on=False) # orthographic projection for ease of mesuring if needed
ax.set_xlabel('x [m]')
ax.set_ylabel('y [m]')
ax.set_zlabel('z [m]')
ax.set_xlim(-2.0, 2.0) # fixed plot range for consistency's sake
ax.set_ylim(-2.0, 2.0)
ax.set_zlim(0.0, 2.0)
ax.set_xticks(np.arange(-2.0, 2.0, 0.5)) # Don't want the axis ticks to change depending on zoom level.
ax.set_yticks(np.arange(-2.0, 2.0, 0.5))
ax.set_zticks(np.arange(0.0, 2.0, 0.5))
ax.scatter(0.0, 0.0, ExpCommon.HEAD_ALTITUDE, color=ExpCommon.CONDUCTOR_COLOR, label='Head position', marker=ExpCommon.HEAD_POS_MARKER) # mark head position
ax.plot([0.0, ExpCommon.FRONT_ARROW_LENGTH], [0.0, 0.0], [ExpCommon.HEAD_ALTITUDE, ExpCommon.HEAD_ALTITUDE], color=ExpCommon.CONDUCTOR_COLOR) # mark head's forward direction in the +x direction

for i in range(len(ap)):
    ax.scatter(pp[i][0], pp[i][1], pp[i][2],    marker=ExpCommon.SOUND_MARKER,   color=ExpCommon.PARTICIPANT_COLOR,  label=('Percieved position' if i == 0 else None))
    ax.scatter(ap[i][0], ap[i][1], ap[i][2],    marker=ExpCommon.SOUND_MARKER,   color=ExpCommon.CONDUCTOR_COLOR,    label=('Actual position' if i == 0 else None))
    ax.plot([pp[i][0], ap[i][0]], [pp[i][1], ap[i][1]], [pp[i][2], ap[i][2]],                                                         color=ExpCommon.PARTICIPANT_COLOR,  label=('errEuclidean' if i == 0 else None))

ax.legend(loc='upper left', bbox_to_anchor=(-0.4, 1.15))
plt.show()
