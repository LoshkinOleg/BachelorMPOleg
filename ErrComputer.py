import sys
import numpy as np
import pandas as pd
import ast
import ExpCommon

CONFUSION_THRESHOLD = 0.25

def RadToDeg(x):
    returnVal = []
    for i in x:
        returnVal.append(round(i * (180.0 / 3.14159265359), 0))
    return returnVal

def SameSign(x, y)->bool:
    if x == 0.0 or y == 0.0:
        return True
    if x > 0.0 and y < 0.0:
        return False
    if x < 0.0 and y > 0.0:
        return False
    return True

if len(sys.argv) != 3:
    print("Please pass input csv as first argument and desired output csv as second argument.")
    quit()

df = pd.read_csv(sys.argv[1])
df = df.iloc[:, 1:5]

cartAp = []
cartPp = []
sphAp = []
sphPp = []
errEuclidean = []
errAzimuthal = []
errSagittal = []
errDepth = []
frontBackConfusion = []
leftRightConfusion = []
upDownConfusion = []

for _, value in np.ndenumerate(df['Actual'].to_numpy()):
    cartAp.append(ast.literal_eval(value))
for _, value in np.ndenumerate(df['Percieved'].to_numpy()):
    cartPp.append(ast.literal_eval(value))

if len(cartAp) != len(cartPp):
    print("Length of array of actual positions and array of percieved positions are mismatching!")
    quit()

for i in range(len(cartAp)):
    sphAp.append(ExpCommon.CartesianToSpherical(cartAp[i][0], cartAp[i][1], cartAp[i][2]))
    sphPp.append(ExpCommon.CartesianToSpherical(cartPp[i][0], cartPp[i][1], cartPp[i][2]))

for i in range(len(cartAp)):
    deltaEuclid = [cartPp[i][0] - cartAp[i][0], cartPp[i][1] - cartAp[i][1], cartPp[i][2] - cartAp[i][2]]
    errEuclidean.append(round(ExpCommon.CartesianMagnitude(deltaEuclid[0], deltaEuclid[1], deltaEuclid[2]), 2))
    # TODO: clamp to +-PI instead of the full 2PI
    errAzimuthal.append(round(abs(sphPp[i][0] - sphAp[i][0]), 2))
    errSagittal.append(round(abs(sphPp[i][1] - sphAp[i][1]), 2))
    errDepth.append(round(abs(sphPp[i][2] - sphAp[i][2]), 2))

for i in range(len(cartAp)):
    frontBack = False
    leftRight = False
    upDown = False
    
    deltaX = abs(cartPp[i][0] - cartAp[i][0])
    deltaY = abs(cartPp[i][1] - cartAp[i][1])
    deltaZ = abs(cartPp[i][2] - cartAp[i][2])
    
    if not (SameSign(cartPp[i][0], cartAp[i][0])):
        frontBack = deltaY < CONFUSION_THRESHOLD and deltaZ < CONFUSION_THRESHOLD
    if not (SameSign(cartPp[i][1], cartAp[i][1])):
        leftRight = deltaX < CONFUSION_THRESHOLD and deltaZ < CONFUSION_THRESHOLD
    if not (SameSign(cartPp[i][2] - ExpCommon.HEAD_ALTITUDE, cartAp[i][2] - ExpCommon.HEAD_ALTITUDE)):
        upDown = deltaX < CONFUSION_THRESHOLD and deltaY < CONFUSION_THRESHOLD
    
    frontBackConfusion.append(frontBack)
    leftRightConfusion.append(leftRight)
    upDownConfusion.append(upDown)


err = pd.DataFrame({"Euclidean error in meters" : errEuclidean, "Depth error in meters" : errDepth, "Azimuthal error in radians" : errAzimuthal, "Sagittal error in radians" : errSagittal, "Azimutal error in degrees: " : RadToDeg(errAzimuthal), "Sagittal error in degrees: " : RadToDeg(errSagittal), "Front-back confusion: " : frontBackConfusion, "Left-right confusion: " : leftRightConfusion, "Up-down confusion: " : upDownConfusion})

err.to_csv(sys.argv[2])
