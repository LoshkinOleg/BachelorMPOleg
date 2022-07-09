import sys
import numpy as np
import pandas as pd
import ast
import ExpCommon

if len(sys.argv) != 3:
    print("Please pass input csv as first argument and desired output csv as second argument.")
    quit()

df = pd.read_csv(sys.argv[1])
df = df.iloc[:, 0:4]

cartAp = []
cartPp = []
sphAp = []
sphPp = []
errEuclidean = []
errAzimuthal = []
errSagittal = []
errDepth = []

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
    errAzimuthal.append(round(abs(sphPp[i][0] - sphAp[i][0]), 2))
    errSagittal.append(round(abs(sphPp[i][1] - sphAp[i][1]), 2))
    errDepth.append(round(abs(sphPp[i][2] - sphAp[i][2]), 2))

err = pd.DataFrame({"Euclidean" : errEuclidean, "Azimuthal" : errAzimuthal, "Sagittal" : errSagittal, "Depth" : errDepth})

err.to_csv(sys.argv[2])
