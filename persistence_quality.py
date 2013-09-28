from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt

x = []
y = []
z = []
quality = []

fp = open("res_horseshoe_u.txt", "r")
line = fp.readline()
while line:
    tokens = line.split(" ")
    if len(tokens) == 5:
        x.append(float(tokens[0]))
        y.append(float(tokens[1]))
        z.append(float(tokens[2]))
        quality.append(float(tokens[3]))
    elif len(tokens) == 6:
        x.append(float(tokens[2]))
        y.append(float(tokens[3]))
        z.append(0)
        quality.append(float(tokens[4]))
    line = fp.readline()
fp.close()

colors = [(i,1-i,0) for i in quality]

fig = plt.figure()
ax = fig.gca(projection='3d')
ax.scatter(x, y, z, c=colors)
plt.show()

