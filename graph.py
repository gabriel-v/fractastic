import io
import sys 
import matplotlib.pyplot as plt
import numpy as np

num_t = [1, 2, 4, 8, 12, 16, 20, 24]
S = []
E = []

with open('fractal.data') as file:
    times = [line.strip() for line in file]

for i in range(len(times)):
    times[i] = times[i][:-1]
    mins, seconds = times[i].split('m')
    times[i] = float(mins) * 60 + float(seconds)
    S.append(times[0] / times[i])
    E.append(S[i] / num_t[i])

my_xticks = np.array(num_t[0:len(S)])
ys = np.array(S)
ye = np.array(E)
x = np.arange(len(ys))

plt.subplot(211)
# plt.title(sys.argv[1].split('.')[0].capitalize())
plt.title('Fractal_pthreads')
plt.axis([0, x.max(), 0, ys.max()])
plt.xlabel("Threads")
plt.xticks(x, my_xticks)
plt.ylabel("Speed up")
plt.plot(x, ys, 'r-')

plt.subplot(212)
plt.axis([0, x.max(), 0, 1])
plt.xlabel("Threads")
plt.xticks(x, my_xticks)
plt.ylabel("Efficinecy")
plt.plot(x, ye, 'g-')
plt.subplots_adjust(top=0.92, bottom=0.08, left=0.10, right=0.95, hspace=0.25,
                    wspace=0.35)
plt.show()


    