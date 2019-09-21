import numpy as np
import matplotlib.pyplot as plt

# Collect the data from the file, ignore empty lines
data = open('stats.out', 'r')

name = []
CPUtime = []

while True:
    line =data.readline()
    if "Exchange3D" in line:
        line =data.readline()
        line =data.readline()
    if "Exchange_" in line:
        #print line
        line = line.strip('\n')
        name.append(line.replace('GhostExchange_', '').replace('artExchange_','').replace('ArrayAssign','Array').replace('VectorTypes','MPI types'))
    if 'Median' in line:
        #print line
        line = line.strip('\n')
        dummy,value = line.split()
        #print value
        CPUtime.append(float(value))
    if not line: break

fig, ax = plt.subplots()

print name
print CPUtime

bar_width = 0.35

x1 = np.arange(len(name))

plt.bar(x1, CPUtime, width=bar_width, color='#96CDCD', edgecolor='white')

plt.xticks(x1, name)

ax.set_xlabel('2D Neighbor Communication Method',fontsize=18)
ax.set_ylabel('2D Ghost Exchange Runtime',fontsize=18)

fig.tight_layout()
plt.savefig("plottimebytype.pdf")
#plt.savefig("plottimebytype.jpg")
plt.savefig("plottimebytype.svg")
plt.savefig("plottimebytype.png")
plt.show()
