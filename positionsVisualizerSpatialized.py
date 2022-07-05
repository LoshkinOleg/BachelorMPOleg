# Imports
import sys
import ast
import matplotlib.pyplot as plt
import numpy as np
import os.path

# Global constants
HEAD_ALTITUDE = 1.2 # Participant's head elevation from ground (+z axis).
FRONT_ARROW_LENGTH = 0.3 # Length of line used to indicate forward direction of participant's head.
APPLICATION_TDTI_COLOR = [0.0, 0.5, 0.0]
APPLICATION_FMOD_COLOR = [0.0, 0.0, 1.0]
PARTICIPANT_TDTI_COLOR = [0.6, 1.0, 0.1]
PARTICIPANT_FMOD_COLOR = [0.6, 0.1, 1.0]
HEAD_COLOR = 'b' # blue
SOUND_MARKER = 'x' # cross
HEAD_POS_MARKER = 'o' # filled circle
APPLICATION_POS_LABEL = "New position of sound is:" # Substring to look for to identify the log entry indicating the position of the spatialized sound.
PARTICIPANT_POS_LABEL = "Participant controller" # Substring to look for to identify participant's controller position log entry.
METHOD_LABEL = "Selected" # Substring to look for to identify a log entry indicating the spatialization method used.
THREEDTI_LABEL = '3dti'
FMOD_LABEL = 'fmod'

def LineContains(line, substring)->bool:
    return (line.find(substring) != -1)

def ExtractSpatMethod(string)->str:
    start = string.find(METHOD_LABEL) + 9
    if start < 9:
        print("ExtractSpatMethod() has been passed a string that does not contain the spatialization method label!")
        quit()
    end = start + 4
    returnVal = string[start : end]
    if returnVal != THREEDTI_LABEL and returnVal != FMOD_LABEL:
        print("ExtractSpatMethod() has retrieved an invalid spatialization method label: %s" %returnVal)
        quit()
    return returnVal

def CartesianFromLine(string):
    start = string.find('(')
    end = string.find(')')
    if start == -1 or end == -1:
        print("CartesianFromLine() cannot parse string.")
        quit()
    return ast.literal_eval(string[start + 1:end]) # Interprets string as a tuple.

def ReadPositions():
    method = ''
    threeDTIPos = []
    fmodPos = []
    participantPosThreeDTI = []
    participantPosFmod = []
    
    for line in lines:
        if LineContains(line, METHOD_LABEL):
            if LineContains(line, THREEDTI_LABEL):
                method = ExtractSpatMethod(line)
            elif LineContains(line, FMOD_LABEL):
                method = ExtractSpatMethod(line)
            else:
                print("Unknown spatialization method encountered!")
                quit()
        elif LineContains(line, APPLICATION_POS_LABEL):
            if method == THREEDTI_LABEL:
                threeDTIPos.append(CartesianFromLine(line))
            elif method == FMOD_LABEL:
                fmodPos.append(CartesianFromLine(line))
            else:
                print("ReadPositions() is trying to parse a spatialized sound position with an invalid spatialization method.")
                quit()
        elif LineContains(line, PARTICIPANT_POS_LABEL):
            if method == THREEDTI_LABEL:
                participantPosThreeDTI.append(CartesianFromLine(line))
            elif method == FMOD_LABEL:
                participantPosFmod.append(CartesianFromLine(line))
            else:
                print("ReadPositions() is trying to parse a participant position with an invalid spatialization method.")
                quit()
    
    if len(threeDTIPos) != len(participantPosThreeDTI) or len(fmodPos) != len(participantPosFmod):
        print("ReadPositions() exception: mismatch between number of logged participant positions and number of spatialized sounds.")
        quit()
    
    return threeDTIPos, participantPosThreeDTI, fmodPos, participantPosFmod

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
ax.scatter(0.0, 0.0, HEAD_ALTITUDE, color=HEAD_COLOR, label='Head position', marker=HEAD_POS_MARKER) # mark head position
ax.plot([0.0, FRONT_ARROW_LENGTH], [0.0, 0.0], [HEAD_ALTITUDE, HEAD_ALTITUDE], color=HEAD_COLOR) # mark head's forward direction in the +x direction

# Parse log data.
tdtiPos, pTdtiPos, fmodPos, pFmodPos = ReadPositions()

# Draw the 3d plot with all positions and their euclidean errors for 3dti method.
for i in range(len(tdtiPos)):
    ap = tdtiPos[i]
    pp = pTdtiPos[i]
    
    # Mark actual and percieved sound positions. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.scatter(ap[0],ap[1],ap[2],   marker=SOUND_MARKER,                color=APPLICATION_TDTI_COLOR,   label=('Actual 3dti position' if i == 0 else None))
    ax.scatter(pp[0],pp[1],pp[2],   marker=SOUND_MARKER,                color=PARTICIPANT_TDTI_COLOR,   label=('Percieved 3dti position' if i == 0 else None))
    
    # Link the two positions with a red line that represents the euclidean error. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.plot([ap[0], pp[0]],     [ap[1], pp[1]],     [ap[2], pp[2]],     color=PARTICIPANT_TDTI_COLOR,   label=('errEuclidean 3dti' if i == 0 else None))

# Draw the 3d plot with all positions and their euclidean errors for fmod method.
for i in range(len(fmodPos)):
    ap = fmodPos[i]
    pp = pFmodPos[i]
    
    # Mark actual and percieved sound positions. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.scatter(ap[0],ap[1],ap[2],   marker=SOUND_MARKER,                color=APPLICATION_FMOD_COLOR,   label=('Actual fmod position' if i == 0 else None))
    ax.scatter(pp[0],pp[1],pp[2],   marker=SOUND_MARKER,                color=PARTICIPANT_FMOD_COLOR,   label=('Percieved fmod position' if i == 0 else None))
    
    # Link the two positions with a red line that represents the euclidean error. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.plot([ap[0], pp[0]],     [ap[1], pp[1]],     [ap[2], pp[2]],     color=PARTICIPANT_FMOD_COLOR,   label=('errEuclidean fmod' if i == 0 else None))

# Display the resulting graph.
ax.legend(loc='upper left', bbox_to_anchor=(-0.4, 1.15)) # Add legend at the top left corner of the screen.
plt.show()