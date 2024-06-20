# read out.txt
# plot the data
# save the plot as a .png file
import matplotlib.pyplot as plt
import numpy as np

f = open("out.txt", "r")
data = f.readlines()
cleaned_data = []
for line in data:
    cleaned_data.append(line.strip())
f.close()
# print(cleaned_data)

latencies = [[] for i in range(18)]
print(len(cleaned_data))
for i in range(0,len(cleaned_data),6):
    loc = int(cleaned_data[i])-1
    for j in range(1,6):
        latencies[loc].append(float(cleaned_data[i+j]))
print(len(latencies))
latencies = np.array(latencies)
latent_T = latencies.T
print(latent_T.shape)

# Plot all the rows
plt.plot(latent_T[0], label='TAS')
plt.plot(latent_T[1], label='TTAS')
plt.plot(latent_T[2], label='Anderson')
plt.plot(latent_T[3], label='CLH')
plt.plot(latent_T[4], label='MCS')
plt.grid(color='gray', linestyle='--', linewidth=1)

plt.xlabel('Number Of Threads')
plt.ylabel('Latency against Benchmarked Code (seconds)')
# adjust ticks of x axis
plt.xticks(np.arange(1, 18, 1))

# show legends
plt.legend()
plt.savefig('latency.png')
