# Imports
import sys
import ast
import matplotlib.pyplot as plt
import numpy as np
import os.path

# Global constants
HEAD_ALTITUDE = 1.2 # Participant's head elevation from ground (+z axis).
FRONT_ARROW_LENGTH = 0.3 # Length of line used to indicate forward direction of participant's head.
CONDUCTOR_POS_LABEL = "Conductor controller" # Substring to look for to identify conductor's controller position log entry.
PARTICIPANT_POS_LABEL = "Participant controller" # Substring to look for to identify participant's controller position log entry.
CONDUCTOR_COLOR = 'b' # blue
PARTICIPANT_COLOR = 'r' # red
SOUND_MARKER = 'x' # cross
HEAD_POS_MARKER = 'o' # filled circle

# Global variables
cPos = [] # Positions the conductor has logged.
pPos = [] # Positions the participant has logged.
lines = [] # String data from the log file.

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

# Traverse list of lines and extract conductor and participant positions.
for line in lines:
    matchIdx = line.find(CONDUCTOR_POS_LABEL)
    if (matchIdx != -1):
        matchIdx = line.find("(") # Find start of position entry.
        posStr = line[matchIdx:len(line) - 1]
        pos = ast.literal_eval(posStr) # Interprets string as a tuple.
        cPos.append(pos)
    
    matchIdx = line.find(PARTICIPANT_POS_LABEL)
    if (matchIdx != -1):
        matchIdx = line.find("(") # Find start of position entry.
        posStr = line[matchIdx:len(line) - 1]
        pos = ast.literal_eval(posStr) # Interprets string as a tuple.
        pPos.append(pos)

# Display error if there's a mismatch between the number of positions logged by the conductor and participant. Needs one-to-one match to draw euclidean error.
if (len(cPos) != len(pPos)):
    print("Mismatch between number of conductor and participant log entries: %d, %d" %(len(cPos),len(pPos)))
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
ax.scatter(0.0, 0.0, HEAD_ALTITUDE, color=CONDUCTOR_COLOR, label='Head position', marker=HEAD_POS_MARKER) # mark head position
ax.plot([0.0, FRONT_ARROW_LENGTH], [0.0, 0.0], [HEAD_ALTITUDE, HEAD_ALTITUDE], color=CONDUCTOR_COLOR) # mark head's forward direction in the +x direction

# Draw the 3d plot with all positions and their euclidean errors.
for i in range(len(cPos)):
    cp = cPos[i]
    pp = pPos[i]
    
    # Mark actual and percieved sound positions. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.scatter(cp[0],cp[1],cp[2],   marker=SOUND_MARKER,                color=CONDUCTOR_COLOR,   label=('Actual position' if i == 0 else None))
    ax.scatter(pp[0],pp[1],pp[2],   marker=SOUND_MARKER,                color=PARTICIPANT_COLOR, label=('Percieved position' if i == 0 else None))
    
    # Link the two positions with a red line that represents the euclidean error. Add a label if this is the first iteration of the loop to indicate the meaning of the symbol.
    ax.plot([cp[0], pp[0]],     [cp[1], pp[1]],     [cp[2], pp[2]],     color=PARTICIPANT_COLOR, label=('errEuclidean' if i == 0 else None))

# Display the resulting graph.
ax.legend(loc='upper left', bbox_to_anchor=(-0.4, 1.15)) # Add legend at the top left corner of the screen.
plt.show()