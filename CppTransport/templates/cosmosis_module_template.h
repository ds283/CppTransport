// backend = cpp, minver = 201801
//
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
//
// This template file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// As a special exception, you may create a larger work that contains
// part or all of this template file and distribute that work
// under terms of your choice.  Alternatively, if you modify or redistribute
// this template file itself, you may (at your option) remove this
// special exception, which will cause the template and the resulting
// CppTransport output files to be licensed under the GNU General Public
// License without this special exception.
//
// @license: GPL-2
// @contributor: Sean Butchers <smlb20@sussex.ac.uk>
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $TOOL $VERSION
//
// '$SAMPLE' generated from '$SOURCE'
// processed on $DATE

#ifndef $GUARD   // avoid multiple inclusion
#define $GUARD

// INCLUDES
// Cosmosis includes
#include <cosmosis/datablock/datablock.hh>
#include <cosmosis/datablock/section_names.h>

// CppTransport includes
#include "$MODEL_mpi.h"
#include "transport-runtime/utilities/spline1d.h"
#include "transport-runtime/tasks/integration_detail/abstract.h"
#include "transport-runtime/models/advisory_classes.h"
#include "transport-runtime/tasks/integration_detail/twopf_task.h"
#include "transport-runtime/enumerations.h"
#include "transport-runtime/tasks/integration_detail/twopf_db_task.h"

// Other includes
#include <memory>
#include "math.h"
#include <exception>
#include "boost/filesystem.hpp"
#include <boost/range/adaptors.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/assert.hpp>

// Includes for mass-matrix statistics
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
using namespace boost::accumulators;

// Include batcher file for integration
#include "transport-runtime/tasks/sampling_integration_batcher.h"

// Easy method of changing variable types - double, long double etc.
using DataType = double;

namespace inflation {
    // Some names for different sections in the cosmosis datablock
    const char *sectionName = "cppt_sample";
    const char *paramsSection = "inflation_parameters";
    const char *twopf_name = "twopf_observables";
    const char *thrpf_name = "thrpf_observables";
    const char *spec_file = "wavenumber_spectrum";
    const char *fail_names = "failed_samples";

    // These are the Lagrangian parameters for our model (including field initial conditions).
    DataType M_P, $PARAM_LIST, $FIELD_LIST_INIT;

    // no. of k samples for CLASS read in from ini file
    int num_k_samples;

    // User-chosen k_pivot scale [Mpc^-1]
    DataType k_pivot_choice;

    //* New bool to enable debugging
    bool Debug = false;

    bool ThreepfEqui = false;
    bool ThreepfSqueeze = false;

    // function to create a transport::parameters<DataType> object called params
    std::vector<DataType> parameter_creator( $FOR{ £PARAM, "DataType £PARAM£COMMA ", Params, False, True} ) {
        std::vector<DataType> output{ $PARAM_LIST };
        return output; 
    }

    // function to create a std::vector<DataType> vector containing initial conditions
    std::vector<DataType> init_cond_creator( $FOR{ £FIELD, "DataType £FIELD£COMMA ", FieldsInit, False, True} ) {
        std::vector<DataType> output{ $FIELD_LIST_INIT };
        return output;
    }

    // ints for capturing failed samples
    int no_end_inflate = 0, neg_Hsq = 0, integrate_nan = 0, zero_massless = 0, neg_epsilon = 0,
        large_epsilon = 0, neg_V = 0, failed_horizonExit = 0, ics_before_start = 0, inflate60 = 0,
        time_var_pow_spec = 0;
}

// exception for catching when <60 e-folds inflation given as we need at least 60 e-folds for sampling
struct le60inflation : public std::exception {
    const char * what () const throw () {
        return "<60 e-folds!";
    }
};

// exception for catching a power spectrum with too much time-dependence on super-horizon scales
struct time_varying_spectrum : public std::exception {
    const char * what () const throw () {
        return "time varying spectrum";
    }
};

// definition of tolerance for the bisection of physical k values
struct ToleranceCondition {
    bool operator () (DataType min, DataType max) {
        return abs(min - max) <= 1E-12;
    }
};

// Set-up a bisection function using a spline to extract a value of N_exit from some desired value of
// phys_k as no. of e-folds before end of inflation
DataType compute_Nexit_for_physical_k (DataType Phys_k, transport::spline1d<DataType>& matching_eq,
                                    ToleranceCondition tol, DataType Nend)
{
    matching_eq.set_offset(std::log(Phys_k));
    std::string task_name = "find N_exit of physical wave-number";
    std::string bracket_error = "extreme values of N didn't bracket the exit value";
    DataType Nexit;
    Nexit = transport::task_impl::find_zero_of_spline(task_name, bracket_error, matching_eq, tol);
    matching_eq.set_offset(0.0);
    Nexit = Nend - Nexit;
    return Nexit;
}

// Set-up a function to create a log-spaced std::vector similar to numpy.logspace
class Logspace {
private:
    DataType curValue, base;

public:
    Logspace(DataType first, DataType base) : curValue(first), base(base) {}

    DataType operator()() {
        DataType retval = curValue;
        curValue *= base;
        return retval;
    }
};
std::vector<DataType> pyLogspace (DataType start, DataType stop, int num, DataType base = 10) {
    DataType logStart = pow(base, start);
    DataType logBase = pow(base, (stop-start)/num);

    std::vector<DataType> log_vector;
    log_vector.reserve(num+1);
    std::generate_n(std::back_inserter(log_vector), num+1, Logspace(logStart, logBase));
    return log_vector;
}

// Set-up a function that applies ln to every element of the vector (useful for dlnA/dlnk derivatives)
std::vector<DataType> vector_logger( std::vector<DataType>& no_log_vec)
{
    size_t vecSize = no_log_vec.size();
    std::vector<DataType> logvec(vecSize);
    for (size_t i = 0; i < vecSize; i++)
    {
        logvec[i] = std::log(no_log_vec[i]);
    }

    return logvec;
}

// Set-up a function that fits an nDegree polynomial to equal-sized vectors oX and oY
std::vector<DataType> polyfit( const std::vector<DataType>& oX,
                             const std::vector<DataType>& oY, int nDegree )
{
    using namespace boost::numeric::ublas;

    if ( oX.size() != oY.size() )
        throw std::invalid_argument( "X and Y vector sizes do not match" );

    // more intuitive this way
    nDegree++;

    size_t nCount =  oX.size();
    matrix<DataType> oXMatrix( nCount, nDegree );
    matrix<DataType> oYMatrix( nCount, 1 );

    // copy y matrix
    for ( size_t i = 0; i < nCount; i++ )
    {
        oYMatrix(i, 0) = oY[i];
    }

    // create the X matrix
    for ( size_t nRow = 0; nRow < nCount; nRow++ )
    {
        DataType nVal = 1.0;
        for ( int nCol = 0; nCol < nDegree; nCol++ )
        {
            oXMatrix(nRow, nCol) = nVal;
            nVal *= oX[nRow];
        }
    }

    // transpose X matrix
    matrix<DataType> oXtMatrix( trans(oXMatrix) );
    // multiply transposed X matrix with X matrix
    matrix<DataType> oXtXMatrix( prec_prod(oXtMatrix, oXMatrix) );
    // multiply transposed X matrix with Y matrix
    matrix<DataType> oXtYMatrix( prec_prod(oXtMatrix, oYMatrix) );

    // lu decomposition
    permutation_matrix<int> pert(oXtXMatrix.size1());
    const std::size_t singular = lu_factorize(oXtXMatrix, pert);
    // must be singular
    BOOST_ASSERT( singular == 0 );

    // backsubstitution
    lu_substitute(oXtXMatrix, pert, oXtYMatrix);

    // copy the result to coeff
    return std::vector<DataType>( oXtYMatrix.data().begin(), oXtYMatrix.data().end() );
}

// Set-up a function that computes the spectral index derivatives using the polyfit and vector_logger functions
// This takes in a k_value for the desired wavenumber to evaluate the derivative, a vector of wavenumbers k, a
// vector of spectrum values A, a boolean set to true for scalar spectral index and an optional argument for nDegree(=2)
DataType spec_index_deriv (DataType k_value, std::vector<DataType>& k, std::vector<DataType>& A,
                         bool scalar, int nDegree = 2)
{
    std::vector<DataType> coeffs = polyfit(vector_logger(k), vector_logger(A), nDegree);
    DataType spectral_index = 0.0;
    for (int i = 1; i < coeffs.size(); i++)
    {
        DataType power = i - 1.0;
        spectral_index += i * pow(std::log(k_value), power) * coeffs[i];
    }
    if (scalar == true)
    {
        spectral_index++;
    }
    return spectral_index;
}

// Set-up a function that returns the k-derivative of A_s or A_t as a DataType.
//DataType spec_derivative(const DataType k, const DataType dk, std::vector<DataType> Amplitude)
//{
//    // compute d/dk[Amplitude] using a three-point central difference of O(dk^6).
//    const DataType dk1 = dk;
//    const DataType dk2 = dk1*2.0;
//    const DataType dk3 = dk1*3.0;
//
//    const DataType Amp1 = (Amplitude[4] - Amplitude[2]) / 2;
//    const DataType Amp2 = (Amplitude[5] - Amplitude[1]) / 4;
//    const DataType Amp3 = (Amplitude[6] - Amplitude[0]) / 6;
//
//    const DataType fifteen_Amp1 = 15 * Amp1;
//    const DataType six_Amp2 = 6 * Amp2;
//    const DataType ten_dk1 = 10 * dk1;
//
//    const DataType dAdk = ((fifteen_Amp1 - six_Amp2) + Amp3) / ten_dk1;
//    const DataType k_div_A = k / Amplitude[3];
//    return (dAdk * k_div_A);
//}

// Set-up a dispersion class that has a function which checks the power spectrum values contained in samples with k and
// t samples stored in k_size and time_size for strongly-varying spectrum values. This is when the std deviation of
// the times samples for a given k_sample is 10% of of the mean value.
template <typename number> class dispersion;

template <typename number>
class dispersion
{
// Constructors etc.
public:
    // Constructor for transport::basic_range k samples
    dispersion(transport::basic_range<number>& k_samples, transport::basic_range<DataType>& time_samples,
            std::vector<number>& spectrum_samples)
            : k_size(k_samples.size()),
              time_size(time_samples.size()),
              samples(spectrum_samples)
    {
    }
    // Constructor for transport::aggregate_range k samples
    dispersion(transport::aggregate_range<number>& k_samples, transport::basic_range<DataType>& time_samples,
            std::vector<number>& spectrum_samples)
            : k_size(k_samples.size()),
              time_size(time_samples.size()),
              samples(spectrum_samples)
    {
    }
    // move constructor
    dispersion(dispersion<number>&&) = default;
    // destructor
    ~dispersion() = default;

// Dispersion calculation
public:
    bool dispersion_check()
    {
        // find the mean of the power spectrum amplitudes
        std::vector<number> mean(k_size), std_dev(k_size);
        for (int i = 0; i < k_size; i++)
        {
            number sum = 0;
            for (int j = 0; j < time_size; j++)
            {
                sum += samples[(time_size*i)+j];
            }
            mean[i] = sum / time_size;
        }

        // find sum_square values for standard deviation
        for (int i = 0; i < k_size; i++)
        {
            number sum_sq = 0;
            for (int j = 0; j < time_size; j++)
            {
                sum_sq += pow(samples[(time_size*i)+j] - mean[i], 2);
            }
            std_dev[i] = sqrt(sum_sq / (time_size - 1)); // divide by time_size-1 for N-1 samples
        }

        // find a measure of the dispersion of power spectrum values -> std-dev/mean
        std::vector<number> dispersion(k_size);
        for (int i = 0; i < mean.size(); ++i)
        {
            dispersion[i] = (1 + 1.0 / (4.0 * time_size)) * std_dev[i]/mean[i]; // unbiased estimator (1 + 1/4n)
        }

        // return true if the dispersion is >1% for any of the k samples
        for (auto i: dispersion)
        {
            if (i > 0.05)
            {
                return true;
            }
        }

        return false;
    }

// Internal data
protected:
    size_t k_size;
    size_t time_size;
    std::vector<number>& samples;
};

// Create instances of the model and separate integration tasks for the two-point function -> a sampling one and a task
// at k_pivot, and three-point function task with k=k_pivot for the equilateral and squeezed configurations
static transport::local_environment env;
static transport::argument_cache arg;
static std::unique_ptr< transport::$MODEL_mpi<DataType, std::vector<DataType>> > model; //! CppT parser version
static std::unique_ptr< transport::twopf_task<DataType> > tk2;
static std::unique_ptr< transport::twopf_task<DataType> > tk2_piv;
static std::unique_ptr< transport::threepf_alphabeta_task<DataType> > tk3e;
static std::unique_ptr< transport::threepf_alphabeta_task<DataType> > tk3s;

extern "C" {

void * setup(cosmosis::DataBlock * options)
{
    // Read options from the CosmoSIS configuration ini file,
    // passed via the "options" argument
    options->get_val(inflation::sectionName, "M_P", inflation::M_P); // TODO: get rid of this option?
    options->get_val(inflation::sectionName, "k_samples", inflation::num_k_samples);
    options->get_val(inflation::sectionName, "k_pivot", inflation::k_pivot_choice);
    options->get_val(inflation::sectionName, "ThreepfEqui", inflation::ThreepfEqui);
    options->get_val(inflation::sectionName, "ThreepfSqueeze", inflation::ThreepfSqueeze);
    options->get_val(inflation::sectionName, "Debug", inflation::Debug);

    // Record any configuration information required
    model = std::make_unique< transport::$MODEL_mpi<DataType, std::vector<DataType>> > (env, arg); //! CppT parser

    // Pass back any object you like
    return options;
}

DATABLOCK_STATUS execute(cosmosis::DataBlock * block, void * config)
{
    // Initialise DATABLOCK_STATUS to 0 - this is returned at end of function
    DATABLOCK_STATUS status = (DATABLOCK_STATUS)0;
    // Add failure DATABLOCK_STATUS variable - returned if an integration fails.
    const DATABLOCK_STATUS failure = (DATABLOCK_STATUS)1;

    //! Read in inflation parameters (Lagrangian and field initial values)
    $FOR{ £PARAM, "block->get_val(inflation::paramsSection£COMMA £QUOTE£PARAM£QUOTE£COMMA inflation::£PARAM);", Params, True, False }
    $FOR{ £FIELD, "block->get_val(inflation::paramsSection£COMMA £QUOTE£FIELD£QUOTE£COMMA inflation::£FIELD);", FieldsInit, True, False }

    // Set-up initial time for integration (N_init) and N_pre which is used to set the amount of sub-horizon evolution
    // to integrate before the chosen mode crosses the horizon.
    const DataType N_init        = 0.0;
    const DataType N_pre_bkg     = 15.0;

    // Create the parameters and initial_conditions objects that CppTransport needs using
    // the two functions defined above.
    transport::parameters<DataType> params{inflation::M_P,
                                    inflation::parameter_creator( $FOR{ £PARAM, "inflation::£PARAM£COMMA ", Params, False, True } ), model.get()};
    transport::initial_conditions<DataType>   ics_bkg{"$MODEL", params,
                                              inflation::init_cond_creator( $FOR{ £FIELD, "inflation::£FIELD£COMMA ", FieldsInit, False, True } ), N_init, N_pre_bkg};
    
    // Use a silly end value to find nEND and set the time range used by CppT to finish at nEND.
    DataType Nendhigh = 10000;
    transport::basic_range<DataType> dummy_times{N_init, Nendhigh, 2, transport::spacing::linear};
    transport::background_task<DataType> bkg{ics_bkg, dummy_times};

    //! Declare the DataTypes needed for storing the observables - here so it's accessible outside of the try-block
    // Pivot task observables
    // Twopf observables
    DataType k_pivot_cppt;
    DataType N_pivot_exit;
    DataType A_s_pivot;
    DataType A_t_pivot;
    DataType r_pivot;
    DataType ns_pivot;
    DataType nt_pivot;
    DataType ns_pivot_linear;
    DataType nt_pivot_linear;
    std::vector<DataType> r;

    // Threepf observables (at pivot scale)
    DataType B_equi_piv;
    DataType fNL_equi_piv;

    DataType B_squ_piv;
    DataType fNL_squ_piv;

    //* Customised vector to store the normalised mass-matrix eigenvalues for the model
    std::vector<DataType> NormMassEigenValues;

    //! Objects needed for creating & storing the big twopf task for passing to a Boltzmann code.
    // Wavenumber k vectors for passing to CLASS, CAMB or another Boltzmann code
    // Use the pyLogspace function to produce log-spaced values between 10^(-6) & 10^(0) Mpc^(-1) with the number of
    // k samples given in 'num_k_samples' read-in above.
    std::vector<DataType> Phys_waveno_sample = pyLogspace(-6.0, 1.7, inflation::num_k_samples, 10);
    std::vector<DataType> k_conventional(Phys_waveno_sample.size());
    // Vectors for storing A_s and A_t before writing them to a temporary file
    std::vector<DataType> A_s;
    std::vector<DataType> A_t;

    // From here, we need to enclose the rest of the code in a try-catch statement in order to catch
    // when a particular set of initial conditions fails to integrate or if there is a problem with the
    // physics such as when the end of inflation can't be found or when H is complex etc. these cases are
    // given a specific flag for the type of problem encountered and logged in the cosmosis datablock.
    try {
        //! compute nEND-> throw exception struct defined above if we have nEND < 60.0 e-folds
        DataType nEND = model->compute_end_of_inflation(&bkg, Nendhigh);
        if(inflation::Debug){std::cout << "Inflation lasts for: " << nEND << " e-folds." << std::endl;}
        if (nEND < 60.0)
        {
            throw le60inflation();
        }
        const DataType N_pre = nEND - 60; // Now that nEND is guaranteed >= 60, this is safe to do so.

        // Now we re-construct the initial conditions block with the actual n_pre value
        transport::initial_conditions<DataType>   ics{"$MODEL", params,
                                                  inflation::init_cond_creator( $FOR{ £FIELD, "inflation::£FIELD£COMMA ", FieldsInit, False, True } ), N_init, N_pre};

        //! construct a test twopf task to use with the compute_H function later
        transport::basic_range<DataType> times{N_init, nEND, 500, transport::spacing::linear};
        transport::basic_range<DataType> k_test{exp(0.0), exp(0.0), 1, transport::spacing::log_bottom};
        transport::twopf_task<DataType> tk2_test{"$MODEL.twopf_test", ics, times, k_test};
        tk2_test.set_collect_initial_conditions(true).set_adaptive_ics_efolds(5.0);

        //! Matching equation for physical wave-numbers
        // Compute the log(H) values across the inflation time range.
        std::vector<DataType> N_H;
        std::vector<DataType> log_H;
        model->compute_H(&tk2_test, N_H, log_H);

        // Set-up the different parameters needed for the matching equation
        DataType Hend = 0.5 * log_H.back(); // value of H at the end of inflation
        DataType norm_const = std::log(243.5363 * pow(3.0, 0.25)); // dimnless matching eq has const = (3^(1/4)*M_P)/1E16 GeV
        DataType k_pivot = std::log(0.05); // pivot scale defined as 0.05 Mpc^-1 in the matching eq (DO NOT CHANGE!)
        DataType e_fold_const = 55.75; // constant defined in the matching eq.
        DataType constants = e_fold_const + k_pivot + norm_const - Hend; // wrap up constants in a single term

        // Find the matching equation solutions across the inflation time range.
        std::vector<DataType> log_physical_k (N_H.size());
        for (int i = 0; i < N_H.size(); ++i)
        {
            log_physical_k[i] = log_H[i] - (nEND - N_H[i]) + constants;
        }

        // Set-up a tolerance condition for using with the bisection function
        ToleranceCondition tol;
        // Set-up a spline to use with the bisection method defined later.
        transport::spline1d<DataType> spline_match_eq (N_H, log_physical_k);

        // Use the bisection method to find the e-fold exit of k pivot_choice.
        N_pivot_exit = compute_Nexit_for_physical_k(inflation::k_pivot_choice, spline_match_eq, tol, nEND);
        // std::cout << "e-fold exit for k* is: " << N_pivot_exit << std::endl;
        // std::cout << "k* from spline is:" << spline_match_eq(N_pivot_exit) << std::endl;

        // Construct a vector of exit times (= no. of e-folds BEFORE the end of inflation!)
//        std::vector<DataType> Phys_k_exits (Phys_waveno_sample.size());
//        for (int i = 0; i < Phys_k_exits.size(); ++i)
//        {
//            Phys_k_exits[i] = compute_Nexit_for_physical_k(Phys_waveno_sample[i], spline_match_eq, tol);
//            std::cout << Phys_waveno_sample[i] << "Mpc^(-1) exits at: " << Phys_k_exits[i] << " e-folds." << std::endl;
//        }

        //! Construct the wave-numbers using a linearity relation.
        // Build CppT normalised wave-numbers by using the linear relation k_phys = gamma * k_cppt and k_cppt[Npre] == 1
        DataType gamma = spline_match_eq(N_pre);

        for (int i = 0; i < k_conventional.size(); ++i)
        {
            k_conventional[i] = Phys_waveno_sample[i] / exp(gamma);
            //std::cout << Phys_k_exits[i] << "\t" << k_conventional[i] << std::endl;
        }

        // Put these into a transport::aggregate range one-by-one
        transport::aggregate_range<DataType> ks;
        for (int i = 0; i < k_conventional.size(); ++i)
        {
            transport::basic_range<DataType> k_temp{k_conventional[i], k_conventional[i], 1, transport::spacing::linear};
            ks += k_temp;
        }

        // Construct a CppT normalised wave-number for the chosen pivot scale using the linearity constant
        k_pivot_cppt = inflation::k_pivot_choice / std::exp(gamma);

        // Use the CppT normalised kpivot value to build a wave-number range for kpivot with some other values to use
        // for finding the spectral indices.
        DataType dk = 1E-3 * k_pivot_cppt;
        transport::basic_range<DataType> k_pivot_range{k_pivot_cppt-(7.0*dk), k_pivot_cppt+(7.0*dk), 14, transport::spacing::log_bottom};

        // Use the CppT normalised kpivot value to build a range with kt = 3*kpivot only
        transport::basic_range<DataType> kt_pivot_range{3.0*k_pivot_cppt, 3.0*k_pivot_cppt, 1, transport::spacing::linear};

        //! ################################################################################

        //! OLD WAY OF CONSTRUCTING THE WAVENUMBERS - this found wavenumbers exiting at nEND-60 up to nEND-50 by finding
        //! the value of aH at these times and then rescaling with the value at nPre for CppT normalisation
        // Use the compute aH method to be able to find the values through-out the duration of inflation.
        // These will be used to find appropriate k values exiting at specific e-foldings by setting k=aH
        // at the desired value of N.
//        std::vector<DataType> N;
//        std::vector<DataType> log_aH;
//        std::vector<DataType> log_a2H2M;
//        model->compute_aH(&tk2_test, N, log_aH, log_a2H2M);
//        // Interpolate the N and log(aH) values.
//        transport::spline1d<DataType> aH_spline(N, log_aH);
//        // Construct some (comoving) k values exiting at at nEND-60, nEND-59, nEND-58, ..., nEND-50.
//        for (int i = 60; i >= 50; --i)
//        {
//            DataType N_value = nEND - i;
//            DataType k_value = exp( aH_spline(N_value) );
//            k_values.push_back(k_value);
//        }
//        // To get these k numbers to be conventionally normalised, we need the value of aH given at
//        // N_pre as defined above. Divide the k numbers by this value to get conventional normalisation.
//        for (int i = 0; i < k_values.size(); ++i)
//        {
//            k_values[i] = k_values[i] / exp( aH_spline(N_pre) );
//        }
//        // use the vector of k values to build a transport::basic_range object to use for integration
//        transport::aggregate_range<DataType> ks;
//        for (int i = 0; i < k_conventional.size(); ++i)
//        {
//            transport::basic_range<DataType> k_temp{k_conventional[i], k_conventional[i], 1, transport::spacing::log_bottom};
//            ks += k_temp;
//        }
//        // Do the same thing for kt but not as many because 3pf integrations are longer!
//        transport::aggregate_range<DataType> kts;
//        for (int i = 0; i < k_values.size(); ++i)
//        {
//            transport::basic_range<DataType> kt_temp{3.0*k_values[i], 3.0*k_values[i], 1, transport::spacing::log_bottom};
//            kts += kt_temp;
//        }

        //! END OF OLD WAY OF CONSTRUCTING WAVENUMBERS

        //! ################################################################################

        //! Construct the integration tasks for the different configurations
        // Some alphas and betas needed specifically for equilateral and squeezed configs.
        transport::basic_range<DataType> alpha_equi{0.0, 0.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> beta_equi{1.0/3.0, 1.0/3.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> alpha_sqz{0.0, 0.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> beta_sqz{0.98, 0.98, 0, transport::spacing::linear};
        // Set-up a time sample for integrations at the end of inflation so we can extract A_s, A_t etc. while giving
        // a wide enough interval to check the values are stable.
        transport::basic_range<DataType> times_sample{nEND-11.0, nEND, 12, transport::spacing::linear};
        transport::basic_range<DataType> Times{N_pre_bkg, nEND, 5000, transport::spacing::linear};
        
        // construct a twopf task based on the k values generated above
        tk2 = std::make_unique< transport::twopf_task<DataType> > ("$MODEL.twopf", ics, times_sample, ks);
        tk2->set_adaptive_ics_efolds(4.5);
        // construct a twopf task for the pivot scale
        tk2_piv = std::make_unique< transport::twopf_task<DataType> > ("$MODEL.twopf-pivot", ics, times_sample, k_pivot_range);
        tk2_piv->set_adaptive_ics_efolds(4.5);

        // construct an equilateral threepf task based on the kt pivot scale made above if we want to
        if (inflation::ThreepfEqui){
          tk3e = std::make_unique<  transport::threepf_alphabeta_task<DataType> > ("$MODEL.threepf-equilateral", ics,
                                    times_sample, kt_pivot_range, alpha_equi, beta_equi);
          tk3e->set_adaptive_ics_efolds(4.5);
        }

        // construct a squeezed threepf task based on the kt pivot scale made above, again, if we want to
        if (inflation::ThreepfSqueeze){
          tk3s = std::make_unique<  transport::threepf_alphabeta_task<DataType> > ("$MODEL.threepf-squeezed", ics,
                                    times_sample, kt_pivot_range, alpha_sqz, beta_sqz);
          tk3s->set_adaptive_ics_efolds(4.5);
        }


        //! INTEGRATE OUR TASKS CREATED FOR THE TWO-POINT FUNCTION ABOVE
        // All batchers need the filesystem path and an unsigned int for logging
        boost::filesystem::path lp(boost::filesystem::current_path());
        unsigned int w;
        int g = 0;
        bool no_log = true;

        //! Twopf pivot task
        std::vector<DataType> pivot_twopf_samples;
        std::vector<DataType> tens_pivot_samples;
        twopf_sampling_batcher<DataType> pivot_batcher(pivot_twopf_samples, tens_pivot_samples, lp, w, model.get(), tk2_piv.get(), g, no_log);

        // Integrate the pivot task
        auto db_piv = tk2_piv->get_twopf_database();
        for (auto t = db_piv.record_cbegin(); t != db_piv.record_cend(); ++t)
        {
            model->twopf_kmode(*t, tk2_piv.get(), pivot_batcher, 1);
        }

        // Print out of samples for the pivot task
//        for (int i = 0; i < pivot_twopf_samples.size(); ++i)
//        {
//            std::cout << "Sample no: " << i << " :-. Zeta 2pf: " << pivot_twopf_samples[i] << " ; Tensor 2pf: " << tens_pivot_samples[i] << std::endl;
//        }

        std::vector<DataType> k_pivots;
        for (int i = 0; i < k_pivot_range.size(); ++i)
        {
            k_pivots.push_back(k_pivot_range[i] * std::exp(gamma) );
        }

        // Perform a dispersion check on the spectrum values - throw time_varying_spectrum if they're varying.
        dispersion<DataType> twpf_pivot_dispersion(k_pivot_range, times_sample, pivot_twopf_samples);
        if(twpf_pivot_dispersion.dispersion_check() == true)
        {
            throw time_varying_spectrum();
        }

        // Extract the A_s & a_t values: put the 15 A_s & A_t values into vectors for finding n_s and n_t with, then
        // take the values at index 7 (centre) to get the pivot scale.
        std::vector<DataType> A_s_spec(k_pivot_range.size());
        std::vector<DataType> A_t_spec(k_pivot_range.size());
        for (int k = 0; k < k_pivot_range.size(); ++k)
        {
            int index = (times_sample.size() * k) + (times_sample.size() - 1);
            A_s_spec[k] = pivot_twopf_samples[index];
            A_t_spec[k] = tens_pivot_samples[index];
            if (k==7)
            {
                A_s_pivot = A_s_spec[k];
                A_t_pivot = A_t_spec[k];
                r_pivot = ( A_t_pivot / A_s_pivot );
            }
        }

        //! Create a temporary path & file for passing the power spectrum to the datablock for the spectral indices
        boost::filesystem::path temp_spec_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.dat");
        std::ofstream out_f(temp_spec_path.string(), std::ios_base::out | std::ios_base::trunc);
        for (int i = 0; i < k_pivots.size(); ++i) {
            std::setprecision(9);
            out_f << k_pivots[i] << "\t";
            out_f << A_s_spec[i] << "\t";
            out_f << A_t_spec[i] << "\n";
        }
        out_f.close();

        // Use put_val to write the temporary file with k, P_s(k) and P_t(k) information for spectral indices
        status = block->put_val( inflation::spec_file, "spec_index_table", temp_spec_path.string() );

        // for (int i = 0; i < A_s_spec.size(); ++i)
        // {
        //     std::cout << std::setprecision(9) << A_s_spec[i] << std::endl;
        //     std::cout << std::setprecision(9) << A_t_spec[i] << std::endl;
        // }

        // std::cout << "r_pivot is: " << r_pivot << std::endl;
        // std::cout << "A_s (pivot) is: " << A_s_pivot << std::endl;
        // std::cout << "A_t (pivot) is: " << A_t_pivot << std::endl;

        // Use the function defined above to find dA/dk and compute n_s and n_t from those
        ns_pivot = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_s_spec, true);
        nt_pivot = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_t_spec, false);

        ns_pivot_linear = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_s_spec, true, 1);
        nt_pivot_linear = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_s_spec, false, 1);

//        DataType dk_phys = dk*std::exp(gamma);
//        DataType ns_pivot_central = spec_derivative(inflation::k_pivot_choice, dk_phys, A_s_spec) + 1.0;
//        DataType nt_pivot_central = spec_derivative(inflation::k_pivot_choice, dk_phys, A_t_spec);

        // transport::spline1d<DataType> ns_piv_spline(k_pivots, A_s_spec);
        // DataType ns_pivot_spline = ns_piv_spline.eval_diff(inflation::k_pivot_choice) * (inflation::k_pivot_choice / A_s_pivot) + 1.0;

        // transport::spline1d<DataType> nt_piv_spline(k_pivots, A_t_spec);
        // DataType nt_pivot_spline = nt_piv_spline.eval_diff(inflation::k_pivot_choice) * (inflation::k_pivot_choice / A_t_pivot);

        // std::cout << "ns: " << ns_pivot << "\t" << "ns(linear): " << ns_pivot_linear << std::endl;
        // std::cout << "nt: " << nt_pivot << "\t" << "nt(linear): " << nt_pivot_linear << std::endl;

        // //! Create a temporary path & file for passing wave-number information to the datablock for class
        // boost::filesystem::path temp_path_small = boost::filesystem::current_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.dat");
        // std::ofstream outf(temp_path_small.string(), std::ios_base::out | std::ios_base::trunc);
        // for (int i = 0; i < k_pivots.size(); ++i) {
        //     outf << k_pivots[i] << "\t";
        //     outf << A_s_spec[i] << "\t";
        //     outf << ns_pivot << "\t";
        //     outf << ns_pivot_linear << "\n";
        // }
        // outf.close();

        //! Big twopf task for CLASS or CAMB
        // Add a 2pf batcher here to collect the data - this needs a vector to collect the zeta-twopf samples.
        std::vector<DataType> samples;
        std::vector<DataType> tens_samples_twpf;
        twopf_sampling_batcher<DataType> batcher(samples, tens_samples_twpf, lp, w, model.get(), tk2.get(), g, no_log);

        // Integrate all of the twopf samples provided above in the tk2 task
        auto db = tk2->get_twopf_database();
        for (auto t = db.record_cbegin(); t != db.record_cend(); ++t)
        {
            model->twopf_kmode(*t, tk2.get(), batcher, 1);
        }

//        for (int i = 0; i < tens_samples_twpf.size(); ++i)
//        {
//            std::cout << "Sample no: " << i << " :-. Zeta 2pf: " << samples[i] << " ; Tensor 2pf: " << tens_samples_twpf[i] << std::endl;
//        }

        // Perform a dispersion check on the spectrum values - throw time_varying_spectrum if they're varying.
        dispersion<DataType> twopf_task_disp(ks, times_sample, samples);
        if ( twopf_task_disp.dispersion_check() == true)
        {
            std::cout << "time-varying spectrum" << std::endl;
            throw time_varying_spectrum();
        }

        // find A_s & A_t for each k mode exiting at Nend-10, ..., Nend etc. We take the final time value at Nend to be
        // the amplitude for the scalar and tensor modes. The tensor-to-scalar ratio r is the ratio of these values.
        for (int k = 0; k < k_conventional.size(); ++k)
        {
            int index = (times_sample.size() * k) + (times_sample.size() -1);
            A_s.push_back(samples[index]);
            A_t.push_back(tens_samples_twpf[index]);
            r.push_back( tens_samples_twpf[index] / samples[index] );
        }

//        for (int i=0; i < A_s.size(); i++)
//        {
//            std::cout << "A_s: " << A_s[i] << std::endl;
//            std::cout << "A_t: " << A_t[i] << std::endl;
//            std::cout << "r: " << r[i] << std::endl;
//        }

        //! Integrate the tasks created for the equilateral 3-point function above, if we want to
        if (inflation::ThreepfEqui){
          // Add a 3pf batcher here to collect the data - this needs 3 vectors for the z2pf, z3pf and redbsp data samples
          // as well as the same boost::filesystem::path and unsigned int variables used in the 2pf batcher.
          std::vector<DataType> eq_twopf_samples;
          std::vector<DataType> eq_tens_samples;
          std::vector<DataType> eq_threepf_samples;
          std::vector<DataType> eq_redbsp_samples;
          threepf_sampling_batcher<DataType>  eq_thpf_batcher(eq_twopf_samples, eq_tens_samples, eq_threepf_samples,
                                              eq_redbsp_samples, lp, w, model.get(), tk3e.get(), g, no_log);

          // Integrate all of the threepf samples provided in the tk3e task
          auto eq_db = tk3e->get_threepf_database();
          for (auto t = eq_db.record_cbegin(); t!= eq_db.record_cend(); ++t)
          {
            model->threepf_kmode(*t, tk3e.get(), eq_thpf_batcher, 1);
          }

          // Perform a dispersion check - throw time_varying_spectrum if spectra aren't stable
          dispersion<DataType> equi_B_disp_check(kt_pivot_range, times_sample, eq_threepf_samples);
          dispersion<DataType> equi_fNL_disp_check(kt_pivot_range, times_sample, eq_redbsp_samples);
          if ( (equi_B_disp_check.dispersion_check() == true) or (equi_fNL_disp_check.dispersion_check() == true) ) {
            throw time_varying_spectrum();
          }

          // Find the bispectrum amplitude and f_NL amplitude at the end of inflation for the pivot scale
          // do this by taking the value at the end of inflation
          B_equi_piv = eq_threepf_samples.back();
          fNL_equi_piv = eq_redbsp_samples.back();
        }

        //! Integrate the task for the squeezed 3-point function above, if we want to
        if (inflation::ThreepfSqueeze){
          // Add a 3pf batcher here to collect the data - this needs 3 vectors for the z2pf, z3pf and redbsp data samples
          // as well as the same boost::filesystem::path and unsigned int variables used in the 2pf batcher.
          std::vector<DataType> sq_twopf_samples;
          std::vector<DataType> sq_tens_samples;
          std::vector<DataType> sq_threepf_samples;
          std::vector<DataType> sq_redbsp_samples;
          threepf_sampling_batcher<DataType>  sq_thpf_batcher(sq_twopf_samples, sq_tens_samples, sq_threepf_samples,
                                              sq_redbsp_samples, lp, w, model.get(), tk3s.get());

          // Integrate all of the threepf samples provided in the tk3s task
          auto sq_db = tk3s->get_threepf_database();
          for (auto t = sq_db.record_cbegin(); t!= sq_db.record_cend(); ++t)
          {
            model->threepf_kmode(*t, tk3s.get(), sq_thpf_batcher, 1);
          }

          // Print-out of squeezed threepf data
          if (inflation::Debug){
            for (auto i = 0; i < sq_threepf_samples.size(); i++)
            {
              std::cout << "Squeezed threepf sample no: " << i << " - " << sq_threepf_samples[i] << " ; Redbsp: " << sq_redbsp_samples[i] << std::endl;
            }
            std::cout << std::endl;
          }

          // Perform a dispersion check - throw time_varying_spectrum if spectra aren't stable
          dispersion<DataType> sq_B_disp_check(kt_pivot_range, times_sample, sq_threepf_samples);
          dispersion<DataType> sq_fNL_disp_check(kt_pivot_range, times_sample, sq_redbsp_samples);
          if ( (sq_B_disp_check.dispersion_check() == true) or (sq_fNL_disp_check.dispersion_check() == true) ) {
            throw time_varying_spectrum();
          }

          //Find the bispectrum amplitude and f_NL amplitude at the end of inflation for the pivot scale
          B_squ_piv = sq_threepf_samples.back();
          fNL_squ_piv = sq_redbsp_samples.back();
        }

      //* Alex's code to compute the normalised mass-matrix eigenvalues for the model:

      //? Need to evolve the background to extract the field values at the end of inflation
      //! transport::background_task<DataType> bckg{ics, times_sample};
      transport::background_task<DataType> bckg{ics, Times};
      transport::backg_history<DataType> history;
      model->backend_process_backg(&bckg, history, true);

      //? Sets up two flattened tensors to store the raw mass-matrix in, and the eigenvalues in
      const int NumFields = model->get_N_fields();
      transport::flattened_tensor<DataType> Masses(NumFields*NumFields);
      transport::flattened_tensor<DataType> NormEigenValues(NumFields);
      ////transport::flattened_tensor<DataType> EigenValues(2);

      //! First attempt at trying to implement a way to test for the adiabatic limit

      boost::filesystem::path Path = "MassEVals.csv";
      std::ofstream MassOut(Path.string(), std::ios_base::out | std::ios_base::trunc);

      std::vector<double> FieldVals;
      std::vector<double> TempVec(history.size());
      std::vector<std::vector<double>> EigenValues(NumFields, TempVec );

      for ( int i = 0; i != history.size(); ++i)
      {
        std::setprecision(9);
        transport::flattened_tensor<DataType> TempMasses(NumFields*NumFields);
        transport::flattened_tensor<DataType> TempNormEigenValues(NumFields);

        std::copy(history[i].begin(), history[i].end(), std::back_inserter(FieldVals));

        model->sorted_mass_spectrum(&bckg, FieldVals, nEND-i, true, TempMasses, TempNormEigenValues);

        MassOut << i << ",";  // Puts the current time serial in the csv
        int j = 0;
        for (auto k = TempNormEigenValues.begin(); k != TempNormEigenValues.end(); ++j, ++k)
        {
          auto absEigenval = std::abs(*k);
          auto EigenvalSign = *k / absEigenval;
          EigenValues[j][i] =  EigenvalSign * std::sqrt(absEigenval);  //*k;
          if(inflation::Debug){
            std::cout << "i: " << i << " , j: " << j << " , k: " << EigenvalSign * std::sqrt(absEigenval) << std::endl;
          }
          MassOut << EigenvalSign * std::sqrt(absEigenval) << ",";

        }
        if(inflation::Debug){std::cout << std::endl;}

        FieldVals.clear();
        TempMasses.clear();
        TempNormEigenValues.clear();
        MassOut << "\n";
      }
      MassOut.close();




      //* Approach: store the eigenvalues for the last 13 e-folds in a vector and then find the
      //* st.-dev. & mean of this, like the dispersion class above for the power-spectrum test.
      //std::vector<double> FieldVals;
      //std::vector<double> TempVec(history.size());
      //std::vector<std::vector<double>> EigenValues(NumFields, TempVec );

      if(inflation::Debug){std::cout << "Size of history: " << history.size() << std::endl;}
      if(inflation::Debug){std::cout << "Size of TempVec: " << TempVec.size() << std::endl;}

      for ( int i = 0; i != history.size(); ++i)
      {
        transport::flattened_tensor<DataType> TempMasses(NumFields*NumFields);
        transport::flattened_tensor<DataType> TempNormEigenValues(NumFields);

        std::copy(history[i].begin(), history[i].end(), std::back_inserter(FieldVals));

        model->sorted_mass_spectrum(&bckg, FieldVals, nEND-i, true, TempMasses, TempNormEigenValues);

        int j = 0;
        for (auto k = TempNormEigenValues.begin(); k != TempNormEigenValues.end(); ++j, ++k)
        {
          auto absEigenval = std::abs(*k);
          auto EigenvalSign = *k / absEigenval;
          EigenValues[j][i] =  EigenvalSign * std::sqrt(absEigenval);  //*k;
          if(inflation::Debug){
            std::cout << "i: " << i << " , j: " << j << " , k: " << EigenvalSign * std::sqrt(absEigenval) << std::endl;
          }
        }
        if(inflation::Debug){std::cout << std::endl;}

        FieldVals.clear();
        TempMasses.clear();
        TempNormEigenValues.clear();

      }

      for (int j = 0; j != NumFields; ++j)
      {
        std::vector<double> TempVec = EigenValues[j];
        accumulator_set<double, stats<tag::mean, tag::moment<2> > > acc;
        acc = std::for_each(TempVec.begin(), TempVec.end(), acc);

        if(inflation::Debug){std::cout << "Mean: " << mean(acc) << std::endl;}
        double StDev = sqrt(moment<2>(acc) - pow(mean(acc),2));
        if(inflation::Debug){std::cout << "St. dev: " <<  StDev << std::endl;} //sqrt(moment<2>(acc)) << std::endl;
      }

      //! This is for extracting the values at the end of inflation.
      //? Extracts the field values at the end of inflation
      std::vector<double> Fields;
      int Len = history.size();
      if(inflation::Debug){std::cout << "Len: " << Len << std::endl;}
      std::copy(history[Len-1].begin(), history[Len-1].end(), std::back_inserter(Fields));

      //? Calls the built-in sorted_mass_spectrum function to extract the normalised eigenvalues
      model->sorted_mass_spectrum(&bckg, Fields, nEND, true, Masses, NormEigenValues);
      /* //TODO: I don't know what the third argument is (currently set to the end of inflation e-fold number).
      It is required by mass_spectrum and sorted_mass_spectrum functions as it is passed into the function M.
      However it doesn't appear to be used at all and looks redundent, as all of the time evolution
      is governed by the values of the Fields parameter at that specific e-fold, thereby eliminating
      any dependence on the __N parameter.
      TODO: Figure out what it's meant to be doing and see if I'm doing the time dependence correctly! */

      //? Now we have the mass eigenvalues in our CppT array NormEigenValues, and so we need to
      //? copy it over into the Cosmosis NormMassEigenValues vector

      if(inflation::Debug){std::cout << "Printing the normalised mass eigenvalues: " << std::endl;}
      for (auto j = NormEigenValues.begin(); j != NormEigenValues.end(); ++j)
      {
        auto absEigenval = std::abs(*j);
        auto EigenvalSign = *j / absEigenval;
        auto EigenVal = EigenvalSign * std::sqrt(absEigenval);
        NormMassEigenValues.push_back(EigenVal);
        if(inflation::Debug){std::cout << EigenVal << std::endl;}
      }
      if(inflation::Debug){std::cout << std::endl;}

      if(inflation::Debug){
        std::cout << "Printing the mass matrix: " << std::endl;
        for (auto j = Masses.begin(); j != Masses.end(); ++j)
        {
          std::cout << *j << std::endl;
        }
        std::cout << std::endl;
      }



    tk2.reset();
    tk2_piv.reset();
    tk3e.reset();
    tk3s.reset();

    // Begin catches for different exceptions thrown from a failed integration sample.
    } catch (transport::end_of_inflation_not_found& xe) {
        std::cout << "!!! END OF INFLATION NOT FOUND !!!" << std::endl;
        inflation::no_end_inflate = 1;
    } catch (transport::Hsq_is_negative& xe) {
        std::cout << "!!! HSQ IS NEGATIVE !!!" << std::endl;
        inflation::neg_Hsq = 1;
    } catch (transport::integration_produced_nan& xe) {
        std::cout << "!!! INTEGRATION PRODUCED NAN !!!" << std::endl;
        inflation::integrate_nan = 1;
    } catch (transport::no_massless_time& xe) {
        std::cout << "!!! NO MASSLESS TIME FOR THIS K MODE !!!" << std::endl;
        inflation::zero_massless = 1;
    } catch (transport::eps_is_negative& xe) {
        std::cout << "!!! EPSILON PARAMETER IS NEGATIVE !!!" << std::endl;
        inflation::neg_epsilon = 1;
    } catch (transport::eps_too_large& xe) {
        std::cout << "!!! EPSILON > 3 !!!" << std::endl;
        inflation::large_epsilon = 1;
    } catch (transport::V_is_negative& xe) {
        std::cout << "!!! NEGATIVE POTENTIAL !!!" << std::endl;
        inflation::neg_V = 1;
    } catch (transport::failed_to_compute_horizon_exit& xe) {
        std::cout << "!!! FAILED TO COMPUTE HORIZON EXIT FOR ALL K MODES !!!" << std::endl;
        inflation::failed_horizonExit = 1;
    } catch (transport::adaptive_ics_before_Ninit& xe) {
        std::cout << "!!! THE ADAPTIVE INITIAL CONDITIONS REQUIRE INTEGRATION TIME BEFORE N_INITIAL !!!" <<  std::endl;
        inflation::ics_before_start = 1;
    } catch (le60inflation& xe) {
        std::cout << "!!! WE HAVE LESS THAN 60 E-FOLDS OF INFLATION !!!" << std::endl;
        inflation::inflate60 = 1;
    } catch (time_varying_spectrum& xe) {
        std::cout << "!!! THE POWER SPECTRUM VARIES TOO MUCH !!!" << std::endl;
        inflation::time_var_pow_spec = 1;
    } // end of try-catch block

    // FAILED SAMPLE INFO
    // Use put_val to write the information about any caught exceptions to the datablock.
    status = block->put_val( inflation::fail_names, "no_end_inflation",   inflation::no_end_inflate);
    status = block->put_val( inflation::fail_names, "negative_Hsq",       inflation::neg_Hsq);
    status = block->put_val( inflation::fail_names, "integrate_nan",      inflation::integrate_nan);
    status = block->put_val( inflation::fail_names, "zero_massless_time", inflation::zero_massless);
    status = block->put_val( inflation::fail_names, "negative_epsilon",   inflation::neg_epsilon);
    status = block->put_val( inflation::fail_names, "eps_geq_three",      inflation::large_epsilon);
    status = block->put_val( inflation::fail_names, "negative_pot",       inflation::neg_V);
    status = block->put_val( inflation::fail_names, "noFind_hor_exit",    inflation::failed_horizonExit);
    status = block->put_val( inflation::fail_names, "ICs_before_start",   inflation::ics_before_start);
    status = block->put_val( inflation::fail_names, "leq_60_efolds",      inflation::inflate60);
    status = block->put_val( inflation::fail_names, "varying_Spec",       inflation::time_var_pow_spec);

    // Sum all the failed sample ints and add that to status - if any = 1 then break out of pipeline for this sample.
    int err_sum = inflation::no_end_inflate + inflation::neg_Hsq + inflation::integrate_nan + inflation::zero_massless +
                  inflation::neg_epsilon + inflation::large_epsilon + inflation::neg_V + inflation::failed_horizonExit +
                  inflation::ics_before_start + inflation::inflate60 + inflation::time_var_pow_spec;
    if (err_sum >= 1)
    {
        //return failure;
        std::cout << "An exception has been thrown!" << std::endl;
    }

    //! Create a temporary path & file for passing wave-number information to the datablock for class
    boost::filesystem::path temp_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.dat");
    // std::cout << "Temp. path = " << temp_path.string() << std::endl;
    std::ofstream outf(temp_path.string(), std::ios_base::out | std::ios_base::trunc);
    for (int i = 0; i < Phys_waveno_sample.size(); ++i) {
        std::setprecision(9);
        outf << Phys_waveno_sample[i] << "\t";
        outf << A_s[i] << "\t";
        outf << A_t[i] << "\n";
    }
    outf.close();

    //! Return the calculated observables to the datablock
    // PIVOT TASKS
    // Use put_val method to add second-order observables (k, A_s, A_t, n_s, n_t, r) at the pivot scale to the datablock
    status = block->put_val( inflation::twopf_name, "k_piv", k_pivot_cppt );
    status = block->put_val( inflation::twopf_name, "N_piv", N_pivot_exit );
    status = block->put_val( inflation::twopf_name, "A_s", A_s_pivot );
    status = block->put_val( inflation::twopf_name, "A_t", A_t_pivot );
    status = block->put_val( inflation::twopf_name, "n_s", ns_pivot );
    status = block->put_val( inflation::twopf_name, "n_t", nt_pivot );
    status = block->put_val( inflation::twopf_name, "n_s_lin", ns_pivot_linear );
    status = block->put_val( inflation::twopf_name, "n_t_lin", nt_pivot_linear );
    status = block->put_val( inflation::twopf_name, "r", r_pivot );

    // Use put_val to put the three-point observables (B_equi, fNL_equi) onto the datablock
    if (inflation::ThreepfEqui){
      status = block->put_val( inflation::thrpf_name, "B_equi", B_equi_piv );
      status = block->put_val( inflation::thrpf_name, "fNL_equi", fNL_equi_piv );
    }

    if (inflation::ThreepfSqueeze){
      status = block->put_val( inflation::thrpf_name, "B_squ", B_squ_piv );
      status = block->put_val( inflation::thrpf_name, "fNL_squ", fNL_squ_piv );
    }


    // CMB TASK for Boltzmann solver
    // Use put_val to write the temporary file with k, P_s(k) and P_t(k) information for CLASS
    status = block->put_val( inflation::spec_file, "spec_table", temp_path.string() );

    status = block->put_val( inflation::twopf_name, "MassMatrixEigenvalues", NormMassEigenValues );
    std::cout << "NormMassEigenValues: " ;
    for (const int& i : NormMassEigenValues){
      std::cout << i << ",  ";
    }
    std::cout << std::endl;

    // return status variable declared at the start of the function
    return status;
}


int cleanup(void * config)
{
    // Config is whatever you returned from setup above
    // Free it 
    model.release();
}

} // end of extern C

#endif  // $GUARD
