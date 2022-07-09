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
