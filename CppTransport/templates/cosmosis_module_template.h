// backend = cpp, minver = 201901
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
#include <chrono>
#include <ctime>
#include "boost/filesystem.hpp"
#include <boost/range/adaptors.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/assert.hpp>

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

    // New bool to enable debugging
    bool Debug = false;

    // Bool option to optionally report m^2/H^2 for the mass-matrix eigenvalues instead of m/H
    bool MassEigenSquare = false;

    // bools for enabling the three-point function runs or not.
    bool ThreepfEqui = false;
    bool ThreepfSqueeze = false;
    bool ThreepfFold = false;

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
        time_var_pow_spec = 0, runtime_exception = 0;
}

// Here we need to have StoragePolicy & TrianglePolicy classes which enable the 'folded' triangle configuration,
// as normally it would be rejected.
struct StoragePolicy
{
public:
    transport::storage_outcome operator()(const transport::threepf_kconfig& data)
    {
      return transport::storage_outcome::accept;
    }
};

struct TrianglePolicy
{
public:
    bool operator()(double alpha, double beta)
    {
      // require only triangle condition, but *not* ordering condition

      // beta should lie between 0 and 1
      if(beta < 0.0) return false;
      if(beta > 1.0) return false;

      // alpha should lie between 1-beta and beta-1
      if(beta - 1.0 - alpha > 1E-6) return false;
      if(alpha - (1.0 - beta) > 1E-6) return false;

      // demand that squeezing should not be too small
      if(std::abs(1.0 - beta) < 1E-6) return false;
      if(std::abs(1.0 + alpha + beta) < 1E-6) return false;
      if(std::abs(1.0 - alpha + beta) < 1E-6) return false;

      return true;
    }
};

// exception for catching when <60 e-folds inflation given as we need at least 60 e-folds for sampling
struct le60inflation : public std::exception {
    const char * what () const throw () {
        return "<60 e-folds!";
    }
};

// exception for dealing with long integrations times which we have to kill and move on
struct LongIntTime : public std::exception {
    const char * what () const throw () {
      return "Integration taking too long, killing sample and moving on...";
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
static std::unique_ptr< transport::$MODEL_mpi<DataType, std::vector<DataType>> > model;
static std::unique_ptr< transport::twopf_task<DataType> > tk2;
static std::unique_ptr< transport::twopf_task<DataType> > tk2_piv;
static std::unique_ptr< transport::threepf_alphabeta_task<DataType> > tk3e;
static std::unique_ptr< transport::threepf_alphabeta_task<DataType> > tk3s;
static std::unique_ptr< transport::threepf_alphabeta_task<DataType> > tk3f;

extern "C" {

void * setup(cosmosis::DataBlock * options)
{
    // Read options from the CosmoSIS configuration ini file, passed via the "options" argument
    options->get_val(inflation::sectionName, "M_P", inflation::M_P);
    options->get_val(inflation::sectionName, "k_samples", inflation::num_k_samples);
    options->get_val(inflation::sectionName, "k_pivot", inflation::k_pivot_choice);
    options->get_val(inflation::sectionName, "ThreepfEqui", inflation::ThreepfEqui);
    options->get_val(inflation::sectionName, "ThreepfSqueeze", inflation::ThreepfSqueeze);
    options->get_val(inflation::sectionName, "ThreepfFold", inflation::ThreepfFold);
    options->get_val(inflation::sectionName, "Debug", inflation::Debug);
    options->get_val(inflation::sectionName, "MassEigenSquare", inflation::MassEigenSquare);

    // Record any configuration information required
    model = std::make_unique< transport::$MODEL_mpi<DataType, std::vector<DataType>> > (env, arg);

    // Pass back any object you like
    return options;
}

DATABLOCK_STATUS execute(cosmosis::DataBlock * block, void * config)
{
    // Initialise DATABLOCK_STATUS to 0 - this is returned at end of function
    DATABLOCK_STATUS status = (DATABLOCK_STATUS)0;
    // Add failure DATABLOCK_STATUS variable - returned if an integration fails.
    const DATABLOCK_STATUS failure = (DATABLOCK_STATUS)1;

    // Record the start time of the module to see how long each run takes.
    auto CppTStartTime = std::chrono::system_clock::now();

    //! Read in inflation parameters (Lagrangian and field initial values)
    $FOR{ £PARAM, "block->get_val(inflation::paramsSection£COMMA £QUOTE£PARAM£QUOTE£COMMA inflation::£PARAM);", Params, True, False }
    $FOR{ £FIELD, "block->get_val(inflation::paramsSection£COMMA £QUOTE£FIELD£QUOTE£COMMA inflation::£FIELD);", FieldsInit, True, False }

    // Set-up initial time for integration (N_init) and N_pre which is used to set the amount of sub-horizon evolution
    // to integrate before the chosen mode crosses the horizon.
    constexpr DataType N_init        = 0.0;
    constexpr DataType N_pre_bkg     = 15.0;
    constexpr DataType Nendhigh      = 10000;
    DataType nEND                    = 0;

    // Create the parameters and initial_conditions objects that CppTransport needs using
    // the two functions defined above.
    transport::parameters<DataType> params{inflation::M_P,
                                    inflation::parameter_creator( $FOR{ £PARAM, "inflation::£PARAM£COMMA ", Params, False, True } ), model.get()};
    transport::initial_conditions<DataType>   ics_bkg{"$MODEL", params,
                                              inflation::init_cond_creator( $FOR{ £FIELD, "inflation::£FIELD£COMMA ", FieldsInit, False, True } ), N_init, N_pre_bkg};

    if(inflation::Debug){std::cout << "Parameters read in" << std::endl;}

    // Use a silly end value to find nEND and set the time range used by CppT to finish at nEND.
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

    DataType B_fold_piv;
    DataType fNL_fold_piv;

    //* Customised vector to store the normalised mass-matrix eigenvalues for the model
    //std::vector<DataType> NormMassEigenValues;
    $FOR{ £FIELDNUM, "DataType NormMassMatrixEigenValue£FIELDNUM = 0;", NumFields , True, False}

    const int NumFields = model->get_N_fields();
    std::vector<double> FieldVals;
    std::vector<double> TempVec(NumFields);
    std::vector<std::vector<double>> EigenValues(4, TempVec);

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
        nEND = model->compute_end_of_inflation(&bkg, Nendhigh);
        if(inflation::Debug){std::cout << "Inflation lasts for: " << nEND << " e-folds." << std::endl;}

        if (nEND < 60.0)
        {
            throw le60inflation(); // If less than 60 e-folds throw an error
        }

        DataType N_pre;
        if (nEND < 75)
        {
          N_pre = 15.0;
        } else{
          N_pre = nEND - 60.0;
        }
        // Sets N_pre to an appropriate value for nEND.

        // Now we re-construct the initial conditions block with the actual N_pre value
        transport::initial_conditions<DataType>   ics{"$MODEL", params,
                                                  inflation::init_cond_creator( $FOR{ £FIELD, "inflation::£FIELD£COMMA ", FieldsInit, False, True } ), N_init, N_pre};

        //! construct a test twopf task to use with the compute_H function later
        transport::basic_range<DataType> times{N_init, nEND, 1000, transport::spacing::linear};
        transport::basic_range<DataType> k_test{exp(0.0), exp(0.0), 0, transport::spacing::log_bottom};
        transport::twopf_task<DataType> tk2_test{"$MODEL.twopf_test", ics, times, k_test};
        tk2_test.set_collect_initial_conditions(true).set_adaptive_ics_efolds(5.0);

        //! Matching equation for physical wave-numbers
        // Compute the log(H) values across the inflation time range.
        std::vector<DataType> N_H;
        std::vector<DataType> log_H;
        model->compute_H(&tk2_test, N_H, log_H);

        // Set-up the different parameters needed for the matching equation
        DataType Hend = 0.5 * log_H.back(); // value of H at the end of inflation
        constexpr DataType norm_const   = std::log(243.5363 * pow(3.0, 0.25)); // dimnless matching eq has const = (3^(1/4)*M_P)/1E16 GeV
        constexpr DataType k_pivot      = std::log(0.05); // pivot scale defined as 0.05 Mpc^-1 in the matching eq (DO NOT CHANGE!)
        constexpr DataType e_fold_const = 55.75; // constant defined in the matching eq.
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

        //! Construct the wave-numbers using a linearity relation.
        // Build CppT normalised wave-numbers by using the linear relation k_phys = gamma * k_cppt and k_cppt[Npre] == 1
        DataType gamma = spline_match_eq(N_pre);

        for (int i = 0; i < k_conventional.size(); ++i)
        {
            k_conventional[i] = Phys_waveno_sample[i] / exp(gamma);
        }

        // Put these into a transport::aggregate range one-by-one
        transport::aggregate_range<DataType> ks;
        for (int i = 0; i < k_conventional.size(); ++i)
        {
            transport::basic_range<DataType> k_temp{k_conventional[i], k_conventional[i], 0, transport::spacing::linear};
            ks += k_temp;
        }

        // Construct a CppT normalised wave-number for the chosen pivot scale using the linearity constant
        k_pivot_cppt = inflation::k_pivot_choice / std::exp(gamma);

        // Use the CppT normalised kpivot value to build a wave-number range for kpivot with some other values to use
        // for finding the spectral indices.
        DataType dk = 1E-3 * k_pivot_cppt;
        transport::basic_range<DataType> k_pivot_range{k_pivot_cppt-(7.0*dk), k_pivot_cppt+(7.0*dk), 14, transport::spacing::log_bottom};

        // Use the CppT normalised kpivot value to build a range with kt = 3*kpivot only
        transport::basic_range<DataType> kt_pivot_range{3.0*k_pivot_cppt, 3.0*k_pivot_cppt, 0, transport::spacing::linear};

        //! Construct the integration tasks for the different configurations
        // Some alphas and betas needed specifically for equilateral, squeezed & folded configs.
        transport::basic_range<DataType> alpha_equi{0.0, 0.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> beta_equi{1.0/3.0, 1.0/3.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> alpha_sqz{0.0, 0.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> beta_sqz{0.98, 0.98, 0, transport::spacing::linear};
        transport::basic_range<DataType> alpha_fold{0.0, 0.0, 0, transport::spacing::linear};
        transport::basic_range<DataType> beta_fold{0.005, 0.005, 0, transport::spacing::linear};

        // Set-up a time sample for integrations at the end of inflation so we can extract A_s, A_t etc. while giving
        // a wide enough interval to check the values are stable.
        transport::basic_range<DataType> times_sample{nEND-11.0, nEND, 12, transport::spacing::linear};

        // Now construct an aggregate time-range of four time points that represent horizon-crossing and some
        // at the end of inflation
        transport::basic_range<DataType> Times55{nEND-55, nEND-55, 0, transport::spacing::linear};
        transport::basic_range<DataType> Times25{nEND-2.5, nEND-2.5, 0, transport::spacing::linear};
        transport::basic_range<DataType> Times1{nEND-1, nEND-1, 0, transport::spacing::linear};
        transport::basic_range<DataType> Times0{nEND, nEND, 0, transport::spacing::linear};
        auto LastTimes = Times55 + Times25 + Times1 + Times0;
        
        // construct a twopf task based on the k values generated above
        if(inflation::Debug){std::cout << "Processing the two-point functions" << std::endl;}
        tk2 = std::make_unique< transport::twopf_task<DataType> > ("$MODEL.twopf", ics, times_sample, ks);
        tk2->set_adaptive_ics_efolds(4.5);
        // construct a twopf task for the pivot scale
        tk2_piv = std::make_unique< transport::twopf_task<DataType> > ("$MODEL.twopf-pivot", ics, times_sample, k_pivot_range);
        tk2_piv->set_adaptive_ics_efolds(4.5);

        // construct an equilateral threepf task based on the kt pivot scale made above only if we want to
        if (inflation::ThreepfEqui){
          tk3e = std::make_unique<  transport::threepf_alphabeta_task<DataType> > ("$MODEL.threepf-equilateral", ics,
                                    times_sample, kt_pivot_range, alpha_equi, beta_equi);
          tk3e->set_adaptive_ics_efolds(4.5);
        }

        // construct a squeezed threepf task based on the kt pivot scale made above, again, only if we want to
        if (inflation::ThreepfSqueeze){
          tk3s = std::make_unique<  transport::threepf_alphabeta_task<DataType> > ("$MODEL.threepf-squeezed", ics,
                                    times_sample, kt_pivot_range, alpha_sqz, beta_sqz);
          tk3s->set_adaptive_ics_efolds(4.5);
        }

        // construct a folded threepf task based on the kt pivot scale made above, again, only if we want to
        if (inflation::ThreepfFold){
          tk3f = std::make_unique<  transport::threepf_alphabeta_task<DataType> > ("$MODEL.threepf-folded", ics,
                  times_sample, kt_pivot_range, alpha_fold, beta_fold, false, StoragePolicy(), TrianglePolicy() );
          tk3f->set_adaptive_ics_efolds(4.5);
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


        // Use the function defined above to find dA/dk and compute n_s and n_t from those
        ns_pivot = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_s_spec, true);
        nt_pivot = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_t_spec, false);

        ns_pivot_linear = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_s_spec, true, 1);
        nt_pivot_linear = spec_index_deriv(inflation::k_pivot_choice, k_pivots, A_s_spec, false, 1);

        //! Big twopf task for CLASS or CAMB
        // Add a 2pf batcher here to collect the data - this needs a vector to collect the zeta-twopf samples.
        std::vector<DataType> samples;
        std::vector<DataType> tens_samples_twpf;
        twopf_sampling_batcher<DataType> batcher(samples, tens_samples_twpf, lp, w, model.get(), tk2.get(), g, no_log);

        // Integrate all of the twopf samples provided above in the tk2 task
        if(inflation::Debug){std::cout << "Constructing a big two-point function run for CLASS" << std::endl;}
        auto db = tk2->get_twopf_database();
        for (auto t = db.record_cbegin(); t != db.record_cend(); ++t)
        {
            model->twopf_kmode(*t, tk2.get(), batcher, 1);
        }

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
          if(inflation::Debug){std::cout << "Integrating the equilateral three-point funciton" << std::endl;}
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
          if(inflation::Debug){std::cout << "Integrating the squeezed three-point function" << std::endl;}
          auto sq_db = tk3s->get_threepf_database();
          for (auto t = sq_db.record_cbegin(); t!= sq_db.record_cend(); ++t)
          {
            model->threepf_kmode(*t, tk3s.get(), sq_thpf_batcher, 1);
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

        //* Integrate the task for the folded 3-point function above, if we want to
        if (inflation::ThreepfFold){
          // Add a 3pf batcher here to collect the data - this needs 3 vectors for the z2pf, z3pf and redbsp data samples
          // as well as the same boost::filesystem::path and unsigned int variables used in the 2pf batcher.
          std::vector<DataType> fold_twopf_samples;
          std::vector<DataType> fold_tens_samples;
          std::vector<DataType> fold_threepf_samples;
          std::vector<DataType> fold_redbsp_samples;
          threepf_sampling_batcher<DataType>  fold_thpf_batcher(fold_twopf_samples, fold_tens_samples, fold_threepf_samples,
                                                                fold_redbsp_samples, lp, w, model.get(), tk3f.get());

          // Integrate all of the threepf samples provided in the tk3f task
          if(inflation::Debug){std::cout << "Integrating the folded three-point function" << std::endl;}
          auto fold_db = tk3f->get_threepf_database();
          for (auto t = fold_db.record_cbegin(); t!= fold_db.record_cend(); ++t)
          {
            model->threepf_kmode(*t, tk3f.get(), fold_thpf_batcher, 1);
          }

          // Perform a dispersion check - throw time_varying_spectrum if spectra aren't stable
          dispersion<DataType> fold_B_disp_check(kt_pivot_range, times_sample, fold_threepf_samples);
          dispersion<DataType> fold_fNL_disp_check(kt_pivot_range, times_sample, fold_redbsp_samples);
          if ( (fold_B_disp_check.dispersion_check() == true) or (fold_fNL_disp_check.dispersion_check() == true) ) {
            throw time_varying_spectrum();
          }

          //Find the bispectrum amplitude and f_NL amplitude at the end of inflation for the pivot scale
          B_fold_piv = fold_threepf_samples.back();
          fNL_fold_piv = fold_redbsp_samples.back();
        }

      //* Alex's code to compute the normalised mass-matrix eigenvalues for the model:
      if(inflation::Debug){std::cout << "Staring mass-matrix processing" << std::endl;}

      //? Need to evolve the background to extract the field values at the end of inflation. This is the last 12 e-folds
      transport::background_task<DataType> bckg{ics, LastTimes};
      transport::backg_history<DataType> history;
      model->backend_process_backg(&bckg, history, true);

      for ( int i = 0; i != history.size(); ++i)
      {
        transport::flattened_tensor<DataType> TempMasses(NumFields*NumFields);
        transport::flattened_tensor<DataType> TempNormEigenValues(NumFields);

        std::copy(history[i].begin(), history[i].end(), std::back_inserter(FieldVals));

        model->sorted_mass_spectrum(&bckg, FieldVals, nEND-i, false, TempMasses, TempNormEigenValues);

        int k = 0;
        for (auto j = TempNormEigenValues.begin(); j != TempNormEigenValues.end(); ++j, ++k)
        {
          DataType EigenValue = 0;
          
          // Are we going to be reporting m^2/H^2, or just m/H. If we are reporting m/H, we need to square-root the
          // values returned by sorted_mass_spectrum and be careful about keeping the data signed.
          if(inflation::MassEigenSquare){
            // Simply return the squared value
            EigenValue = *j;
          }
          else {
            // Square root the eigenvalue keeping the sign
            auto absEigenValue = std::abs(*j);
            auto EigenValueSign = *j / absEigenValue;
            EigenValue =  EigenValueSign * std::sqrt(absEigenValue);
          }

          EigenValues[i][k] = EigenValue;
        }

        FieldVals.clear();
        TempMasses.clear();
        TempNormEigenValues.clear();
      }

    tk2.reset();
    tk2_piv.reset();
    tk3e.reset();
    tk3s.reset();
    tk3f.reset();

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
    } catch (transport::runtime_exception& xe) {
      std::cout << "!!! RUNTIME EXCEPTION: SEE TERMINAL OUTPUT FOR REASON !!!";
      inflation::runtime_exception = 1;
    } catch (LongIntTime& xe) {
      std::cout << "Integrations took over 1 min, killing :( ";
      inflation::runtime_exception = 1;
    } // end of try-catch block

    // FAILED SAMPLE INFO
    // Sum all the failed sample ints and add that to status - if any = 1 then break out of pipeline for this sample.
    int err_sum = inflation::no_end_inflate + inflation::neg_Hsq + inflation::integrate_nan + inflation::zero_massless +
                  inflation::neg_epsilon + inflation::large_epsilon + inflation::neg_V + inflation::failed_horizonExit +
                  inflation::ics_before_start + inflation::inflate60 + inflation::time_var_pow_spec + inflation::runtime_exception;
    if (err_sum >= 1)
    {
      std::cout << "An exception has been thrown, sample rejected!" << std::endl;
    }

    //! Create a temporary path & file for passing wave-number information to the datablock for class
    boost::filesystem::path temp_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.dat");
    std::ofstream outf(temp_path.string(), std::ios_base::out | std::ios_base::trunc);
    if ( err_sum == 0) {
      for (int i = 0; i < Phys_waveno_sample.size(); ++i) {
        std::setprecision(9);
        outf << Phys_waveno_sample[i] << "\t";
        outf << A_s[i] << "\t";
        outf << A_t[i] << "\n";
      }
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

    if (inflation::ThreepfFold){
      status = block->put_val( inflation::thrpf_name, "B_fold", B_fold_piv );
      status = block->put_val( inflation::thrpf_name, "fNL_fold", fNL_fold_piv );
    }

    // CMB TASK for Boltzmann solver
    // Use put_val to write the temporary file with k, P_s(k) and P_t(k) information for CLASS
    status = block->put_val( inflation::spec_file, "spec_table", temp_path.string() );

    // Return the mass-matrix eigenvalues.
    $FOR{ £FIELDNUM, "status = block->put_val( inflation::twopf_name£COMMA £QUOTENormMassMatrixEigenValue£FIELDNUM_1£QUOTE£COMMA EigenValues[0][£FIELDNUM] );", FieldNum , True, False }
    $FOR{ £FIELDNUM, "status = block->put_val( inflation::twopf_name£COMMA £QUOTENormMassMatrixEigenValue£FIELDNUM_2£QUOTE£COMMA EigenValues[1][£FIELDNUM] );", FieldNum , True, False }
    $FOR{ £FIELDNUM, "status = block->put_val( inflation::twopf_name£COMMA £QUOTENormMassMatrixEigenValue£FIELDNUM_3£QUOTE£COMMA EigenValues[2][£FIELDNUM] );", FieldNum , True, False }
    $FOR{ £FIELDNUM, "status = block->put_val( inflation::twopf_name£COMMA £QUOTENormMassMatrixEigenValue£FIELDNUM_4£QUOTE£COMMA EigenValues[3][£FIELDNUM] );", FieldNum , True, False }

    // Return the error-status and the end of inflation
    status = block->put_val( inflation::twopf_name, "ErrStatus", err_sum );
    status = block->put_val( inflation::twopf_name, "Nefold", nEND );

    // Return how long each run took.
    auto CppTFinishTime = std::chrono::system_clock::now();
    double CppTElapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(CppTFinishTime - CppTStartTime).count();
    status = block->put_val( inflation::twopf_name, "CppTTime", CppTElapsedSeconds );

    // return status variable declared at the start of the function
    if(inflation::Debug){std::cout << "CppTSample done, returning status." << std::endl;}
    return status;
}


int cleanup(void * config)
{
    // Config is whatever you returned from setup above
    // Free it 
    model.release();
    return 0;
}

} // end of extern C

#endif  // $GUARD
