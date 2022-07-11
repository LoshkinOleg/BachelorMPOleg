# Imports
import sys
import matplotlib.pyplot as plt
import numpy as np
import os.path

import ExpCommon

# Average error global vars
avgEuclideanTdti = 0
avgAzimuthalTdti = 0
avgSagittalTdti = 0
avgDepthTdti = 0
avgEuclideanFmod = 0
avgAzimuthalFmod = 0
avgSagittalFmod = 0
avgDepthFmod = 0

# Ensure there's a filepath passed to the script.
if len(sys.argv) < 2:
    print("Please pass the relative path to .log file as first argument to the command line.")
    quit()

# Try to open and read log file, if it doesn't exist or is empty, display message to user.
if os.path.isfile(sys.argv[1]):
    logFile = open(sys.argv[1], 'r')
    lines = logFile.readlines()
    logFile.close()
    if len(lines) < 2:
        print("Couldn't retireve any text data from file!")
        quit()
else:
    print("Invalid path passed as argument!")
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
ax.scatter(0.0, 0.0, ExpCommon.HEAD_ALTITUDE, color=ExpCommon.HEAD_COLOR, label='Head position', marker=ExpCommon.HEAD_POS_MARKER) # mark head position
ax.plot([0.0, ExpCommon.FRONT_ARROW_LENGTH], [0.0, 0.0], [ExpCommon.HEAD_ALTITUDE, ExpCommon.HEAD_ALTITUDE], color=ExpCommon.HEAD_COLOR) # mark head's forward direction in the +x direction

# Parse log data.
tdtiPos, pTdtiPos, fmodPos, pFmodPos = ExpCommon.ReadPositions(lines)

# Draw the 3d plot with all positions and their euclidean errors for 3dti method.
for i in range(len(tdtiPos)):
    ap = tdtiPos[i]
    pp = pTdtiPos[i]
    
    # Mark actual and percieved sound positions. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.scatter(ap[0],ap[1],ap[2],   marker=ExpCommon.SOUND_MARKER,                color=ExpCommon.APPLICATION_TDTI_COLOR,   label=('Actual 3dti position' if i == 0 else None))
    ax.scatter(pp[0],pp[1],pp[2],   marker=ExpCommon.SOUND_MARKER,                color=ExpCommon.PARTICIPANT_TDTI_COLOR,   label=('Percieved 3dti position' if i == 0 else None))
    
    # Link the two positions with a red line that represents the euclidean error. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.plot([ap[0], pp[0]],     [ap[1], pp[1]],     [ap[2], pp[2]],     color=ExpCommon.PARTICIPANT_TDTI_COLOR,   label=('errEuclidean 3dti' if i == 0 else None))
    
    # Add to error counters.
    deltaCartesian = [pp[0] - ap[0], pp[1] - ap[1], pp[2] - ap[2]]
    a0, e0, r0 = ExpCommon.CartesianToSpherical(ap[0], ap[1], ap[2])
    a1, e1, r1 = ExpCommon.CartesianToSpherical(pp[0], pp[1], pp[2])
    print("3DTI: ")
    print("errEuclidean: %.2f" %abs(ExpCommon.CartesianMagnitude(deltaCartesian[0], deltaCartesian[1], deltaCartesian[2])))
    avgEuclideanTdti = avgEuclideanTdti + abs(ExpCommon.CartesianMagnitude(deltaCartesian[0], deltaCartesian[1], deltaCartesian[2]))
    print("errAzimuthal: %.2f" %abs(a1 - a0))
    avgAzimuthalTdti = avgAzimuthalTdti + abs(a1 - a0)
    print("errSagittal: %.2f" %abs(e1 - e0))
    avgSagittalTdti = avgSagittalTdti + abs(e1 - e0)
    print("errDepth: %.2f" %abs(r1 - r0))
    avgDepthTdti = avgDepthTdti + abs(r1 - r0)
    print("========")

# Compute averages
avgEuclideanTdti = avgEuclideanTdti / len(tdtiPos)
avgAzimuthalTdti = avgAzimuthalTdti / len(tdtiPos)
avgSagittalTdti = avgSagittalTdti / len(tdtiPos)
avgDepthTdti = avgDepthTdti / len(tdtiPos)
print("avg errEuclidean: %.2f, avg errAzimuthal: %.2f, avg errSagittal: %.2f, avg errDepth: %.2f" %(avgEuclideanTdti, avgAzimuthalTdti, avgSagittalTdti, avgDepthTdti))

# Draw the 3d plot with all positions and their euclidean errors for fmod method.
for i in range(len(fmodPos)):
    ap = fmodPos[i]
    pp = pFmodPos[i]
    
    # Mark actual and percieved sound positions. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.scatter(ap[0],ap[1],ap[2],   marker=ExpCommon.SOUND_MARKER,                color=ExpCommon.APPLICATION_FMOD_COLOR,   label=('Actual fmod position' if i == 0 else None))
    ax.scatter(pp[0],pp[1],pp[2],   marker=ExpCommon.SOUND_MARKER,                color=ExpCommon.PARTICIPANT_FMOD_COLOR,   label=('Percieved fmod position' if i == 0 else None))
    
    # Link the two positions with a red line that represents the euclidean error. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.plot([ap[0], pp[0]],     [ap[1], pp[1]],     [ap[2], pp[2]],     color=ExpCommon.PARTICIPANT_FMOD_COLOR,   label=('errEuclidean fmod' if i == 0 else None))
    
    # Add to error counters.
    deltaCartesian = [pp[0] - ap[0], pp[1] - ap[1], pp[2] - ap[2]]
    a0, e0, r0 = ExpCommon.CartesianToSpherical(ap[0], ap[1], ap[2])
    a1, e1, r1 = ExpCommon.CartesianToSpherical(pp[0], pp[1], pp[2])
    print("3DTI: ")
    print("errEuclidean: %.2f" %abs(ExpCommon.CartesianMagnitude(deltaCartesian[0], deltaCartesian[1], deltaCartesian[2])))
    avgEuclideanFmod = avgEuclideanFmod + abs(ExpCommon.CartesianMagnitude(deltaCartesian[0], deltaCartesian[1], deltaCartesian[2]))
    print("errAzimuthal: %.2f" %abs(a1 - a0))
    avgAzimuthalFmod = avgAzimuthalFmod + abs(a1 - a0)
    print("errSagittal: %.2f" %abs(e1 - e0))
    avgSagittalFmod = avgSagittalFmod + abs(e1 - e0)
    print("errDepth: %.2f" %abs(r1 - r0))
    avgDepthFmod = avgDepthFmod + abs(r1 - r0)
    print("========")

# Compute averages
avgEuclideanFmod = avgEuclideanFmod / len(fmodPos)
avgAzimuthalFmod = avgAzimuthalFmod / len(fmodPos)
avgSagittalFmod = avgSagittalFmod / len(fmodPos)
avgDepthFmod = avgDepthFmod / len(fmodPos)
print("avg errEuclidean: %.2f, avg errAzimuthal: %.2f, avg errSagittal: %.2f, avg errDepth: %.2f" %(avgEuclideanFmod, avgAzimuthalFmod, avgSagittalFmod, avgDepthFmod))

# Display the resulting graph.
ax.legend(loc='upper left', bbox_to_anchor=(-0.4, 1.15)) # Add legend at the top left corner of the screen.
plt.figtext(0.0, 0.01, "3dti: avg errEuclidean: %.2f, avg errAzimuthal: %.2f, avg errSagittal: %.2f, avg errDepth: %.2f" %(avgEuclideanTdti,avgAzimuthalTdti,avgSagittalTdti,avgDepthTdti))
plt.figtext(0.0, 0.05, "fmod: avg errEuclidean: %.2f, avg errAzimuthal: %.2f, avg errSagittal: %.2f, avg errDepth: %.2f" %(avgEuclideanFmod,avgAzimuthalFmod,avgSagittalFmod,avgDepthFmod))
plt.show()