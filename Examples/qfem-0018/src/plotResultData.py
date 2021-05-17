import numpy as np
import matplotlib.pyplot as plt

strain = np.genfromtxt("stress.1.coords")
stress = np.genfromtxt("calDataField.csv", delimiter=",")

outputs = np.genfromtxt("resultData.csv", delimiter=",", skip_header=1)
best_fit = outputs[-1, 8:]
best_fit_params = outputs[-1, :8]

res = np.genfromtxt("results.out")

print(np.shape(best_fit), np.shape(stress))

plt.figure(figsize=(12, 8), dpi=80, facecolor='w', edgecolor='k')
plt.plot(strain, linewidth=2, color='blue', label='Strain history')
# plt.plot(best_fit, color='black', label='Result')
plt.xlim([0, 342])
plt.xlabel('Time', FontSize=18)
plt.ylabel('Strain', FontSize=18)
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)
plt.legend(fontsize=12)
plt.tight_layout()
plt.savefig('StrainHistory.png')
plt.show()

plt.figure(figsize=(12, 8), dpi=80, facecolor='w', edgecolor='k')
plt.plot(stress, linewidth=2, color='blue', label='Data')
plt.plot(res, linewidth=2, color='red', label='Best fit')
# plt.plot(best_fit, color='black', label='Result')
plt.xlim([0, 342])
plt.xlabel('Time', FontSize=18)
plt.ylabel('Stress (MPa)', FontSize=18)
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)
plt.legend(fontsize=12)
plt.tight_layout()
plt.savefig('StressResults.png')
plt.show()

plt.figure(figsize=(12, 8), dpi=80, facecolor='w', edgecolor='k')
plt.plot(strain, stress, linewidth=2, color='blue', label='Data')
plt.plot(strain, res, linewidth=2, color='red', label='Best fit')
# plt.plot(strain, best_fit, color='black', label='Result')
plt.xlabel('Strain', FontSize=18)
plt.ylabel('Stress (MPa)', FontSize=18)
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)
plt.legend(fontsize=12)
plt.tight_layout()
plt.savefig('DeterministicCalibrationResults.png')
plt.show()
