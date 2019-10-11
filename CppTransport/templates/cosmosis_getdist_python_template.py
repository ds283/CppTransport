// backend = cpp, minver = 201901
# CosmoSIS -> GetDist & Seaborn parser file automatically generated from '$SOURCE'
# processed on $DATE

#! File for turning the CosmoSIS output into something recognised by the GetDist analysis framework and then
#! using Seaborn to plot cosmological parameters


# Importing required modules. If it's not found, we raise an ImportError and print to terminal what's missing
# and then exit gently
from __future__ import print_function # make sure we're using Python3 print functions, not statements
import sys
import os

try:
    import pandas as pd
except ImportError as err:
    print('Pandas not found and is a mandatory requirement. Please install it (e.g. via pip or conda) before continuing')
    sys.exit()

try:
    import numpy as np
except ImportError as err:
    print('NumPy not found and is a mandatory requirement. Please install it (e.g. via pip or conda) before continuing')
    sys.exit()

try:
    from getdist import loadMCSamples
    import getdist.plots as gplot
except ImportError as err:
    print('GetDist not found and is a mandatory requirement. Please install it (e.g. via pip or conda) before continuing')
    sys.exit()

try:
    import seaborn as sns
    import matplotlib.pyplot as plt
    SeabornAvail = True
except ImportError as err:
    print('Seaborn not found. This is not a required package, however no seaborn distribution plots will made')
    SeabornAvail = False


string_out_folder = "./GetDistFiles/"
string_out = "$MODEL_MCMCGetDistData"

# We need to find the location of the start of the actual data in the input file, which changes depending on the sampler etc.
InputFile = open('mcmc_output.txt', 'r')
for i, line in enumerate(InputFile):
    if "END_OF_PRIORS_INI" in line:
        header_end = i + 1
        break
InputFile.close()

# Start reading in the data and adjusting column names
Data = pd.read_csv('mcmc_output.txt', sep = "\t", skiprows = lambda x: x in range(1, header_end), header = 0) #Import the data into a pandas DataFrame
Data = Data[Data['post'] != "-inf"] #We only want accepted samples


Data.columns = Data.columns.str.strip().str.lower().str.replace('cosmological_parameters--', '').str.replace('inflation_parameters--', '').str.replace('twopf_observables--', '').str.replace('thrpf_observables--', '').str.replace('planck--', '').str.replace('#', '')
Data.columns = Data.columns.str.strip().str.upper()

# If there are any NaN's in the data, then replace them with zeros so it's obvious that there's no data present in that column.
Data.fillna(0.0, inplace = True)

# We want the scalar & tensor indices to be given as ln(10^10 * A)
Data["A_S"] = np.log(1E10 * Data["A_S"])
Data["A_T"] = np.log(1E10 * Data["A_T"])

# Renaming the mass-matrix eigenvalues into a nicer form
$FOR{ £FIELDNUM, "Data.rename(columns = {£QUOTENORMMASSMATRIXEIGENVALUE£FIELDNUM_1£QUOTE:£QUOTEEVAL£FIELDNUM_1£QUOTE }, inplace = True)", FieldNum, True, False }
$FOR{ £FIELDNUM, "Data.rename(columns = {£QUOTENORMMASSMATRIXEIGENVALUE£FIELDNUM_2£QUOTE:£QUOTEEVAL£FIELDNUM_2£QUOTE }, inplace = True)", FieldNum, True, False }
$FOR{ £FIELDNUM, "Data.rename(columns = {£QUOTENORMMASSMATRIXEIGENVALUE£FIELDNUM_3£QUOTE:£QUOTEEVAL£FIELDNUM_3£QUOTE }, inplace = True)", FieldNum, True, False }
$FOR{ £FIELDNUM, "Data.rename(columns = {£QUOTENORMMASSMATRIXEIGENVALUE£FIELDNUM_4£QUOTE:£QUOTEEVAL£FIELDNUM_4£QUOTE }, inplace = True)", FieldNum, True, False }

# GetDist requires a 'weights' column as the first entry, which for us is all ones
# as well as having the posterior likelihood as the second entry.
cols = Data.columns.tolist()
cols = cols[-1:] + cols[:-1]
Data = Data[cols]

weights = pd.DataFrame({'weights': np.ones(len(Data.index))})
frames = [weights, Data]
results = pd.concat(frames, axis = 1)

# GetDist requires a positive posterior given as -log(likelihood) (+ve)
results["POST"] = -1.0*results["POST"]


# Selecting only the output columns that we want to write
OutputColumns = ['weights', 'POST', $FOR{ £PRIOR, "£QUOTE£PRIOR£QUOTE.upper()£COMMA ", PriorNames, False, True}, 'K_PIV', 'N_PIV', 'A_S', 'A_T', 'N_S', 'N_T', 'R', 'NEFOLD', 'B_EQUI', 'FNL_EQUI', 'B_SQU', 'FNL_SQU', 'B_FOLD', 'FNL_FOLD']
results = results[OutputColumns]
results.to_csv(str(string_out_folder) + str(string_out) + '.txt', header = None, index = None, sep = " ")
# Write the output to a text file using pandas

#----------
# We now are going to read in the created csv above into GetDist so we can create a covarience triangle plot
# and print to the terminal a 68% confidence interval for the parameters
#----------

DropAmount = 0.25 #! Percentage that we want to drop from the MCMC chain (between 0 and 1). Can be changed at will

analysis_settings = {'ignore_rows': DropAmount}
samples = loadMCSamples(str(string_out_folder) + str(string_out), settings = analysis_settings) # Load samples into GetDist

# Print the LaTeX table of parameter limits at 68% and 95% levels
print('\n')
print(r'Parameters at 68% limit table')
print(samples.getTable(limit=1).tableTex())
samples.getTable(limit=1).write('AttractorModel68Interval.tex')
print('\n\n')

print(r'Parameters at 95% limit table')
print(samples.getTable(limit=2).tableTex())
samples.getTable(limit=2).write('AttractorModel95Interval.tex')
print('\n\n')

# Generate a triangle plot for the model and save it to current folder.
g=gplot.getSubplotPlotter(chain_dir=str(string_out_folder))
roots = [str(string_out)]

params = []

$FOR{ £PRIOR, "params.append(£QUOTE£PRIOR£QUOTE)", PriorNames, True, False}

params.extend(['A_S', 'A_T', 'N_S', 'N_T', 'R', 'NEFOLD'])

# Now need to see if we have any fNL values, and if so then we want to add them to the triangle plot too
if Data['FNL_EQUI'].all():
    params.append('FNL_EQUI')

if Data['FNL_SQU'].all():
    params.append('FNL_SQU')

if Data['FNL_FOLD'].all():
    params.append('FNL_FOLD')

param_3d = None
g.triangle_plot(roots, params, plot_3d_with_param = param_3d, filled = False, shaded = True, line_args = {'lw':2, 'color':'darkblue'} )
g.export('GetDist_$MODEL_TrainlgePlot.pdf')

#----------
# Now going to be using seaborn to produce a few histograms of important variables that might be useful
# We only want to do this if seaborn is avalible, otherwise we exit after all GetDist stuff is done
#----------

if not SeabornAvail:
    sys.exit()

sns.set(font_scale=1.25)

TotalRows = Data.shape[0] + 1

Data.drop(range(int(DropAmount * TotalRows)), inplace = True)

print('Total data points after cuts: ' + str(Data.shape[0]))


# Spectral index histogram plot
fig, ax = plt.subplots(figsize=(13, 7))
ax = sns.distplot(Data['N_S'], kde = False, color="#00AFAA", label="Data") #EB6BB0 #00AFAA
plt.ylabel('Frequency')
plt.xlabel(r'$$n_s$$')
#ax.axvspan(0.9607, 0.9691, alpha=0.45, color='#0049FF', label=r"Planck $$\bar{n}_s \pm 1\sigma$$")
#ax.axvspan(0.9565, 0.9733, alpha=0.25, color='#5A89FF', label=r"Planck $$\bar{n}_s \pm 2\sigma$$")
#ax.axvspan(0.9523, 0.9775, alpha=0.175, color='#A0BBFF', label=r"Planck $$\bar{n}_s \pm 3\sigma$$")
plt.title(r'Distribution of $$n_s$$ values')
plt.legend()
#plt.xlim(left=0.95, right=1.02)
plt.tight_layout()
plt.savefig('NS.pdf')
plt.close()

# Tensor spectral index histogram plot
fig, ax = plt.subplots(figsize=(12, 7))
ax = sns.distplot(Data['N_T'], kde = False, color="#00AFAA", label="Data") #EB6BB0 #00AFAA
plt.ylabel('Frequency')
plt.xlabel(r'$$n_t$$')
plt.title(r'Distribution of $$n_t$$ values')
plt.legend()
plt.tight_layout()
plt.savefig('NT.pdf')
plt.close()

# Scalar-to-tensor ratio plot
fig, ax = plt.subplots(figsize=(12, 7))
ax = sns.distplot(Data['R'], kde = False, color="#00AFAA", label="Data") #EB6BB0 #00AFAA
plt.ylabel('Frequency')
plt.xlabel(r'$$r$$')
plt.title(r'Distribution of $$r$$ values')
plt.legend()
plt.tight_layout()
plt.savefig('R.pdf')
plt.close()

# Scalar amplitude plot
fig, ax = plt.subplots(figsize=(12, 7))
ax = sns.distplot(Data['A_S'], kde = False, color="#00AFAA", label="Data") #EB6BB0 #00AFAA
plt.ylabel('Frequency')
plt.xlabel(r'$$\ln (10^{10} \, A_s)$$')
plt.title(r'Distribution of $$A_s$$ values')
plt.legend()
plt.tight_layout()
plt.savefig('AS.pdf')
plt.close()

# Tensor amplitude plot
fig, ax = plt.subplots(figsize=(12, 7))
ax = sns.distplot(Data['A_T'], kde = False, color="#00AFAA", label="Data") #EB6BB0 #00AFAA
plt.ylabel('Frequency')
plt.xlabel(r'$$\ln (10^{10} \, A_t)$$')
plt.title(r'Distribution of $$A_t$$ values')
plt.legend()
plt.tight_layout()
plt.savefig('AT.pdf')
plt.close()

# fNL plot
fig, ax = plt.subplots(figsize=(12, 7))
if Data['FNL_EQUI'].all():
    ax = sns.distplot(Data['FNL_EQUI'], kde = False, label=r"$$f_{NL}^{eq}$$") #EB6BB0 #00AFAA

if Data['FNL_SQU'].all():
    ax = sns.distplot(Data['FNL_SQU'], kde = False, label=r"$$f_{NL}^{sq}$$") #EB6BB0 #00AFAA

if Data['FNL_FOLD'].all():
    ax = sns.distplot(Data['FNL_FOLD'], kde = False, label=r"$$f_{NL}^{fold}$$") #EB6BB0 #00AFAA

plt.ylabel('Frequency')
plt.xlabel(r'$$f_{NL}$$')
plt.title(r'Distribution of $$f_{NL}$$ values')
plt.legend()
plt.tight_layout()
plt.savefig('FNL.pdf')
plt.close()

# Mass Matrix Plot 1
fig, ax = plt.subplots(figsize=(12, 7))
$FOR{ £FIELDNUM, "ax = sns.distplot(Data[£QUOTEEVAL£FIELDNUM_1£QUOTE], kde=False, label=£QUOTEeigenvalue £QUOTE + str(£FIELDNUM +1))", FieldNum, True, False }
plt.title('Distribution of mass-matrix eigenvalues at horizon crossing')
plt.ylabel('Relative Frequency')
plt.xlabel(r'$$m_i/H$$')
plt.legend()
plt.tight_layout()
plt.savefig('Mass1.pdf')
plt.close()

# Mass Matrix Plot 2
fig, ax = plt.subplots(figsize=(12, 7))
$FOR{ £FIELDNUM, "ax = sns.distplot(Data[£QUOTEEVAL£FIELDNUM_2£QUOTE], kde=False, label=£QUOTEeigenvalue £QUOTE + str(£FIELDNUM +1))", FieldNum, True, False }
plt.title('Distribution of mass-matrix eigenvalues 2.5 e-fold before end of inflation')
plt.ylabel('Relative Frequency')
plt.xlabel(r'$$m_i/H$$')
plt.legend()
plt.tight_layout()
plt.savefig('Mass2.pdf')
plt.close()

# Mass Matrix Plot 3
fig, ax = plt.subplots(figsize=(12, 7))
$FOR{ £FIELDNUM, "ax = sns.distplot(Data[£QUOTEEVAL£FIELDNUM_3£QUOTE], kde=False, label=£QUOTEeigenvalue £QUOTE + str(£FIELDNUM +1))", FieldNum, True, False }
plt.title('Distribution of mass-matrix eigenvalues 1 e-fold before end of inflation')
plt.ylabel('Relative Frequency')
plt.xlabel(r'$$m_i/H$$')
plt.legend()
plt.tight_layout()
plt.savefig('Mass3.pdf')
plt.close()

# Mass Matrix Plot 4
fig, ax = plt.subplots(figsize=(12, 7))
$FOR{ £FIELDNUM, "ax = sns.distplot(Data[£QUOTEEVAL£FIELDNUM_4£QUOTE], kde=False, label=£QUOTEeigenvalue £QUOTE + str(£FIELDNUM +1))", FieldNum, True, False }
plt.title('Distribution of mass-matrix eigenvalues at the end of inflation')
plt.ylabel('Relative Frequency')
plt.xlabel(r'$$m_i/H$$')
plt.legend()
plt.tight_layout()
plt.savefig('Mass4.pdf')
plt.close()
