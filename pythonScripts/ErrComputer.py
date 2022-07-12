import sys
import numpy as np
import pandas as pd
import ast

import ExpCommon as ec

cartAp = []
cartPp = []
sphAp = []
sphPp = []
errEuclidean = {ec.CONTROL_SCENARIO_NAME : [], ec.THREEDTI_NO_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_NO_REVERB_SCENARIO_NAME : [], ec.THREEDTI_WITH_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_WITH_REVERB_SCENARIO_NAME : []}
errAzimuthal = {ec.CONTROL_SCENARIO_NAME : [], ec.THREEDTI_NO_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_NO_REVERB_SCENARIO_NAME : [], ec.THREEDTI_WITH_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_WITH_REVERB_SCENARIO_NAME : []}
errSagittal = { ec.CONTROL_SCENARIO_NAME : [], ec.THREEDTI_NO_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_NO_REVERB_SCENARIO_NAME : [], ec.THREEDTI_WITH_REVERB_SCENARIO_NAME : [],\
                ec.FMOD_WITH_REVERB_SCENARIO_NAME : []}
errDepth = {    ec.CONTROL_SCENARIO_NAME : [], ec.THREEDTI_NO_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_NO_REVERB_SCENARIO_NAME : [], ec.THREEDTI_WITH_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_WITH_REVERB_SCENARIO_NAME : []}
sampleIds = {   ec.CONTROL_SCENARIO_NAME : [], ec.THREEDTI_NO_REVERB_SCENARIO_NAME : [], \
                ec.FMOD_NO_REVERB_SCENARIO_NAME : [], ec.THREEDTI_WITH_REVERB_SCENARIO_NAME : [],\
                ec.FMOD_WITH_REVERB_SCENARIO_NAME : []}

ec.Assert(len(sys.argv) == 2, "Please pass input csv as first argument.")
ec.Assert(ec.FileExists(sys.argv[1]), "Input file specified does not exist!")

df = pd.read_csv(sys.argv[1])
df = df.iloc[:, ec.RAW_INPUT_COL_BEGIN : ec.RAW_INPUT_COL_END] # Reads all contents of columns 1 through 5 into df.
control = df.loc[df['Scenario'] == ec.CONTROL_SCENARIO_NAME]
threeDTI_noReverb = df.loc[df['Scenario'] == ec.THREEDTI_NO_REVERB_SCENARIO_NAME]
fmod_noReverb = df.loc[df['Scenario'] == ec.FMOD_NO_REVERB_SCENARIO_NAME]
threeDTI_withReverb = df.loc[df['Scenario'] == ec.THREEDTI_WITH_REVERB_SCENARIO_NAME]
fmod_withReverb = df.loc[df['Scenario'] == ec.FMOD_WITH_REVERB_SCENARIO_NAME]

for _, value in np.ndenumerate(df['Actual'].to_numpy()):
    cartAp.append(ast.literal_eval(value))
for _, value in np.ndenumerate(df['Percieved'].to_numpy()):
    cartPp.append(ast.literal_eval(value))

ec.Assert(len(cartAp) == len(cartPp), "Length of array of actual positions and array of percieved positions are mismatching!")

for i in range(len(cartAp)):
    sphAp.append(ec.CartesianToSpherical(cartAp[i][0], cartAp[i][1], cartAp[i][2]))
    sphPp.append(ec.CartesianToSpherical(cartPp[i][0], cartPp[i][1], cartPp[i][2]))

for i in range(len(cartAp)):
    deltaEuclid = [cartPp[i][0] - cartAp[i][0], cartPp[i][1] - cartAp[i][1], cartPp[i][2] - cartAp[i][2]]
    scenario = df.loc[df['SampleId'] == i]
    errEuclidean[scenario["Scenario"].iloc[0]].append(round(ec.CartesianMagnitude(deltaEuclid[0], deltaEuclid[1], deltaEuclid[2]), 2))
    errAzimuthal[scenario["Scenario"].iloc[0]].append(ec.LimitToPlusMinusPi(round(abs(sphPp[i][0] - sphAp[i][0]), 2)))
    errSagittal[scenario["Scenario"].iloc[0]].append(ec.LimitToPlusMinusPi(round(abs(sphPp[i][1] - sphAp[i][1]), 2)))
    errDepth[scenario["Scenario"].iloc[0]].append(round(abs(sphPp[i][2] - sphAp[i][2]), 2))
    sampleIds[scenario["Scenario"].iloc[0]].append(scenario["SampleId"].iloc[0])

control_Output = pd.DataFrame({ "SampleId" : sampleIds[ec.CONTROL_SCENARIO_NAME], \
                                "Euclidean Error in meters" : errEuclidean[ec.CONTROL_SCENARIO_NAME], \
                                "Azimuthal Error in degrees" : ec.RadToDeg(errAzimuthal[ec.CONTROL_SCENARIO_NAME]), "Sagittal Error in degrees" : ec.RadToDeg(errSagittal[ec.CONTROL_SCENARIO_NAME]), \
                                "Depth Error in meters" : errDepth[ec.CONTROL_SCENARIO_NAME]})

threeDTI_noReverb_Output = pd.DataFrame({  "SampleId" : sampleIds[ec.THREEDTI_NO_REVERB_SCENARIO_NAME], \
                                "Euclidean Error in meters" : errEuclidean[ec.THREEDTI_NO_REVERB_SCENARIO_NAME], \
                                "Azimuthal Error in degrees" : ec.RadToDeg(errAzimuthal[ec.THREEDTI_NO_REVERB_SCENARIO_NAME]), "Sagittal Error in degrees" : ec.RadToDeg(errSagittal[ec.THREEDTI_NO_REVERB_SCENARIO_NAME]), \
                                "Depth Error in meters" : errDepth[ec.THREEDTI_NO_REVERB_SCENARIO_NAME]})

fmod_noReverb_Output = pd.DataFrame({  "SampleId" : sampleIds[ec.FMOD_NO_REVERB_SCENARIO_NAME], \
                                "Euclidean Error in meters" : errEuclidean[ec.FMOD_NO_REVERB_SCENARIO_NAME], \
                                "Azimuthal Error in degrees" : ec.RadToDeg(errAzimuthal[ec.FMOD_NO_REVERB_SCENARIO_NAME]), "Sagittal Error in degrees" : ec.RadToDeg(errSagittal[ec.FMOD_NO_REVERB_SCENARIO_NAME]), \
                                "Depth Error in meters" : errDepth[ec.FMOD_NO_REVERB_SCENARIO_NAME]})

threeDTI_withReverb_Output = pd.DataFrame({  "SampleId" : sampleIds[ec.THREEDTI_WITH_REVERB_SCENARIO_NAME], \
                                "Euclidean Error in meters" : errEuclidean[ec.THREEDTI_WITH_REVERB_SCENARIO_NAME], \
                                "Azimuthal Error in degrees" : ec.RadToDeg(errAzimuthal[ec.THREEDTI_WITH_REVERB_SCENARIO_NAME]), "Sagittal Error in degrees" : ec.RadToDeg(errSagittal[ec.THREEDTI_WITH_REVERB_SCENARIO_NAME]), \
                                "Depth Error in meters" : errDepth[ec.THREEDTI_WITH_REVERB_SCENARIO_NAME]})

fmod_withReverb_Output = pd.DataFrame({  "SampleId" : sampleIds[ec.FMOD_WITH_REVERB_SCENARIO_NAME], \
                                "Euclidean Error in meters" : errEuclidean[ec.FMOD_WITH_REVERB_SCENARIO_NAME], \
                                "Azimuthal Error in degrees" : ec.RadToDeg(errAzimuthal[ec.FMOD_WITH_REVERB_SCENARIO_NAME]), "Sagittal Error in degrees" : ec.RadToDeg(errSagittal[ec.FMOD_WITH_REVERB_SCENARIO_NAME]), \
                                "Depth Error in meters" : errDepth[ec.FMOD_WITH_REVERB_SCENARIO_NAME]})

control_Output.to_csv("csvTables/Control_Errors.csv")
threeDTI_noReverb_Output.to_csv("csvTables/ThreeDTI_noReverb_Errors.csv")
fmod_noReverb_Output.to_csv("csvTables/FMod_noReverb_Errors.csv")
threeDTI_withReverb_Output.to_csv("csvTables/ThreeDTI_withReverb_Errors.csv")
fmod_withReverb_Output.to_csv("csvTables/FMod_withReverb_Errors.csv")