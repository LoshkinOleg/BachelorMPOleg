from numpy.random import RandomState
import math
import sys
import random
import matplotlib.pyplot as plt
from mpl_toolkits import mplot3d
import numpy as np

# argv 1 is seed, argv 2 is head height, argv 3 is nr of pos to generate

# Init random engine.
rng = RandomState(int(sys.argv[1]))
print("Seed is: {0}".format(sys.argv[1]))

def generate_random_cartesian(headHeight):

    MIN_SOUND_AZIMUTH = math.radians(-180.0)
    MAX_SOUND_AZIMUTH = math.radians(180.0)
    MIN_SOUND_DISTANCE = 0.33
    MAX_SOUND_DISTANCE = 3.0
    MIN_SOUND_ELEVATION = math.radians(50.0)
    MAX_SOUND_ELEVATION = math.radians(120.0)

    # Random spherical coords.
    a = rng.uniform(MIN_SOUND_AZIMUTH, MAX_SOUND_AZIMUTH)
    e = rng.uniform(MIN_SOUND_ELEVATION, MAX_SOUND_ELEVATION)
    r = rng.uniform(MIN_SOUND_DISTANCE, MAX_SOUND_DISTANCE)

    # Convert to cartesian.
    x = r*math.sin(e)*math.cos(a)
    y = r*math.sin(e)*math.sin(a)
    z = r*math.cos(e) + headHeight
    
    return x,y,z

# fig = plt.figure()
# ax = fig.add_subplot(projection='3d')
# ax.set_xlabel('x (m)')
# ax.set_ylabel('y (m)')
# ax.set_zlabel('z (m)')
for x in range(int(sys.argv[3])):
    x,y,z = generate_random_cartesian(float(sys.argv[2]))
    x = round(x, 2)
    y = round(y, 2)
    z = round(z, 2)
    print("Play sound at: ({0:.2f};{1:.2f};{2:.2f})".format(x, y, z))
    # ax.scatter(x,y,z,marker='o')
# plt.show()