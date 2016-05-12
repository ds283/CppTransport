import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import csv

with open('/Users/ds283/Desktop/fNL_local.csv') as f:
    time = []
    fNL_local = []

    reader = csv.DictReader(f)
    for row in reader:
        time.append(float(row['time']))
        fNL_local.append(float(row['fNL']))

    time_data = np.array(time)
    fNL_local_data = np.array(fNL_local)

plt.figure()
plt.plot(time_data, fNL_local_data, label='$f_{\mathrm{NL}}^{\mathrm{local}}$')

plt.xlabel(r'e-folds $N$')
plt.legend(frameon=False)
plt.savefig('/Users/ds283/Desktop/fNL_local.pdf')
