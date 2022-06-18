from numpy.random import RandomState
import math
import sys

# Oleg@self: fix distribution, spawning incoherent positions.

MIN_SOUND_AZIMUTH = math.radians(-180.0)
MAX_SOUND_AZIMUTH = math.radians(180.0)
MIN_SOUND_DISTANCE = 0.15
MAX_SOUND_DISTANCE = 2.5
MIN_SOUND_ELEVATION = math.radians(-15.0)
MAX_SOUND_ELEVATION = math.radians(30.0)

# Init random engine.
rng = RandomState()

# Random spherical coords.
a = rng.normal((MAX_SOUND_AZIMUTH + MIN_SOUND_AZIMUTH) / 2, (MAX_SOUND_AZIMUTH - MIN_SOUND_AZIMUTH) / 2)
e = rng.normal((MAX_SOUND_ELEVATION + MIN_SOUND_ELEVATION) / 2, (MAX_SOUND_ELEVATION - MIN_SOUND_ELEVATION) / 2)
r = rng.normal((MAX_SOUND_DISTANCE + MIN_SOUND_ELEVATION) / 2, (MAX_SOUND_DISTANCE - MIN_SOUND_ELEVATION) / 2)

# Convert to cartesian.
x = r*math.sin(e)*math.cos(a)
y = r*math.sin(e)*math.sin(a)
z = r*math.cos(e)

print("Play sound at: ({0:.2f};{1:.2f};{2:.2f})".format(x, y, z))