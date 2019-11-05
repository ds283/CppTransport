import os
from cosmosis.datablock import names, option_section
import sys
import traceback
import numpy as np

try:
    import classy
except ImportError as err:
    print('CLASS Python wrapper can\'t be imported, please double check CLASS installation and try again.')
    sys.exit()

# These are pre-defined strings we use as datablock
# section names
cosmo = names.cosmological_parameters
distances = names.distances
cmb_cl = names.cmb_cl
spec_file = "wavenumber_spectrum"
twopf_obs = "twopf_observables"


def setup(options):
    # Read options from the ini file which are fixed across
    # the length of the chain
    config = {'lmax': options.get_int(option_section, 'lmax', default=2000),
              'zmin': options.get_double(option_section, 'zmin', default=0.0),
              'zmax': options.get_double(option_section, 'zmax', default=4.0),
              'nz': options.get_int(option_section, 'nz', default=401),
              'kmax': options.get_double(option_section, 'kmax', default=1.0),
              'debug': options.get_bool(option_section, 'debug', default=False),
              'do_lensing': options.get_bool(option_section, 'do_lensing', default=False),
              'cosmo': classy.Class()  # Create the object that connects to Class
              }

    # Return all this config information
    return config


def get_class_inputs(block, config):
    # Get parameters from block and give them the
    # names and form that class expects
    params = {
        'output': 'tCl pCl mPk',
        'l_max_scalars': config["lmax"],
        'P_k_max_h/Mpc': config["kmax"],
        'z_pk': ', '.join(str(z) for z in np.linspace(config['zmin'], config['zmax'], config['nz'])),
        'lensing': 'no' if not config['do_lensing'] else 'yes',
        'P_k_ini type': 'external_Pk',
        'command': 'cat ' + block[spec_file, 'spec_table'],
        'H0': 100 * block[cosmo, 'h0'],
        'omega_b': block[cosmo, 'ombh2'],
        'omega_cdm': block[cosmo, 'omch2'],
        'tau_reio': block[cosmo, 'tau'],
        'T_cmb': block.get_double(cosmo, 't_cmb', default=2.726),
        'N_eff': block.get_double(cosmo, 'massless_nu', default=3.046),
    }
    return params


def get_class_outputs(block, c, config):
    ##
    # Derived cosmological parameters
    ##

    block[cosmo, 'sigma_8'] = c.sigma8()
    h0 = block[cosmo, 'h0']

    ##
    # Matter power spectrum
    ##

    # Ranges of the redshift and matter power
    dz = 0.01
    kmin = 1e-4
    kmax = config['kmax'] * h0
    nk = 100

    # Define k,z we want to sample
    z = np.arange(0.0, config["zmax"] + dz, dz)
    k = np.logspace(np.log10(kmin), np.log10(kmax), nk)
    nz = len(z)

    # Extract (interpolate) P(k,z) at the requested
    # sample points.
    P = np.zeros((nk, nz))
    for i, ki in enumerate(k):
        for j, zj in enumerate(z):
            P[i, j] = c.pk(ki, zj)

    # Save matter power as a grid
    block.put_grid("matter_power_lin", "k_h", k / h0, "z", z, "p_k", P * h0 ** 3)

    ##
    # Distances and related quantities
    ##

    # save redshifts of samples
    block[distances, 'z'] = z
    block[distances, 'nz'] = nz

    # Save distance samples
    block[distances, 'd_l'] = np.array([c.luminosity_distance(zi) for zi in z])
    d_a = np.array([c.angular_distance(zi) for zi in z])
    block[distances, 'd_a'] = d_a
    block[distances, 'd_m'] = d_a * (1 + z)

    # Save some auxiliary related parameters
    block[distances, 'age'] = c.age()
    block[distances, 'rs_zdrag'] = c.rs_drag()

    ##
    # Now the CMB C_ell
    ##
    c_ell_data = c.raw_cl()
    ell = c_ell_data['ell']
    ell = ell[2:]

    # Save the ell range
    block[cmb_cl, "ell"] = ell

    # t_cmb is in K, convert to mu_K, and add ell(ell+1) factor
    tcmb_muk = block[cosmo, 't_cmb'] * 1e6
    f = ell * (ell + 1.0) / 2 / np.pi * tcmb_muk ** 2

    # Save each of the four spectra
    for s in ['tt', 'ee', 'te', 'bb']:
        block[cmb_cl, s] = c_ell_data[s][2:] * f


def execute(block, config):
    c = config['cosmo']

    err_sum = block[twopf_obs, 'ErrStatus']

    if err_sum != 0:
        os.remove(block[spec_file, 'spec_table'])
        return 1

    try:
        # Set input parameters
        params = get_class_inputs(block, config)
        c.set(params)

        # Run calculations
        c.compute()

        # Extract outputs
        get_class_outputs(block, c, config)

    except classy.CosmoError as error:
        if config['debug']:
            sys.stderr.write("Error in class. You set debug=T so here is more debug info:\n")
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write("Error in class. Set debug=T for info: {}\n".format(error))
        return 1

    finally:
        # Reset for re-use next time
        c.struct_cleanup()
        os.remove(block[spec_file, 'spec_table'])

    return 0


def cleanup(config):
    config['cosmo'].empty()
