import math
import ast
import os.path

# Global constants
HEAD_ALTITUDE = 1.2 # Participant's head elevation from ground (+z axis).
FRONT_ARROW_LENGTH = 0.3 # Length of line used to indicate forward direction of participant's head.
APPLICATION_TDTI_COLOR = [0.0, 0.5, 0.0]
APPLICATION_FMOD_COLOR = [0.0, 0.0, 1.0]
PARTICIPANT_TDTI_COLOR = [0.6, 1.0, 0.1]
PARTICIPANT_FMOD_COLOR = [0.6, 0.1, 1.0]
CONDUCTOR_COLOR = 'b' # blue
PARTICIPANT_COLOR = 'r' # red
HEAD_COLOR = 'b' # blue
SOUND_MARKER = 'x' # cross
HEAD_POS_MARKER = 'o' # filled circle
APPLICATION_POS_LABEL = "New position of sound is:" # Substring to look for to identify the log entry indicating the position of the spatialized sound.
PARTICIPANT_POS_LABEL = "Participant controller" # Substring to look for to identify participant's controller position log entry.
METHOD_LABEL = "Selected" # Substring to look for to identify a log entry indicating the spatialization method used.
THREEDTI_LABEL = '3dti'
FMOD_LABEL = 'fmod'
CONDUCTOR_POS_LABEL = "Conductor controller" # Substring to look for to identify conductor's controller position log entry.
PI = 3.14159265359
RAW_INPUT_COL_BEGIN = 0
RAW_INPUT_COL_END = 5
CONTROL_SCENARIO_NAME = "control"
THREEDTI_NO_REVERB_SCENARIO_NAME = "3dti w/o reverb"
FMOD_NO_REVERB_SCENARIO_NAME = "fmod w/o reverb"
THREEDTI_WITH_REVERB_SCENARIO_NAME = "3dti w/ reverb"
FMOD_WITH_REVERB_SCENARIO_NAME = "fmod w/ reverb"

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

def ReadPositions(lines):
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
        print("ReadPositions() exception: mismatch between number of logged participant positions and number of spatialized sounds: %d, %d" %(len(partici)))
        quit()
    
    return threeDTIPos, participantPosThreeDTI, fmodPos, participantPosFmod

def CartesianToSpherical(x, y, z):
    if abs(y) < 0.01:
        y = 0.01
    if abs(z) < 0.01:
        z = 0.01
    azimuth = math.atan(x/y)
    elevation = math.atan(math.sqrt(x*x + y*y)/z)
    radius = math.sqrt(x*x + y*y + z*z)
    return azimuth, elevation, radius

def CartesianMagnitude(x, y, z):
    return math.sqrt(x*x + y*y + z*z)

def RadToDeg(x):
    returnVal = []
    for i in x:
        returnVal.append(round(i * (180.0 / PI), 0))
    return returnVal

def SameSign(x, y)->bool:
    if x == 0.0 or y == 0.0:
        return True
    if x > 0.0 and y < 0.0:
        return False
    if x < 0.0 and y > 0.0:
        return False
    return True

def LimitToPlusMinusPi(x):
    if x > PI:
        return -2.0 * PI + x
    else:
        return x

def FileExists(relativePath)->bool:
    return os.path.isfile(relativePath)

def Assert(condition, message):
    if not condition:
        print(message)
        quit()