// backend = cpp, minver = 201901
//
// Created by amaraio on 05/11/2019.
//

#ifndef $GUARD
#define $GUARD

#include "transport-runtime/transport.h"
#include "transport-runtime/models/nontrivial_metric_model.h"
#include "transport-runtime/models/odeint_defaults.h"

#include "$MODEL_core.h"

namespace transport {
    template<typename number>
    $MODEL<number>::$MODEL(local_environment &e, argument_cache &a)
      : $COSMO_LAGRANGIANTYPE<number>("$UNIQUE_ID", $NUMERIC_VERSION, e, a) {
        __A_k1k2k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __A_k1k3k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k1k2k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k1k3k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k1k2k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k1k3k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        __A_k2k1k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __A_k2k3k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k2k1k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k2k3k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k2k1k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k2k3k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        __A_k3k1k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __A_k3k2k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k3k1k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k3k2k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k3k1k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k3k2k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        $IF{!fast&&canonical}
        // Canonical version
        __dV = new number[$NUMBER_FIELDS];
        __ddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
        __dddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        $ENDIF

        $IF{!fast&&noncanonical}
        // Non-canonical version
        __dV = new number[$NUMBER_FIELDS];
        __ddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
        __dddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __G = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
        __Ginv = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
        __A2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
        __A3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        $ENDIF

        __raw_params = new number[$NUMBER_PARAMS];

        // Sets the model's cosmology class to the values specified in the model file. This can be overridden
        // at complation time in the model's .cpp file if needed
        Cosmo.set_H0($COSMO_H0);
        Cosmo.set_Omega_Bh2($COSMO_OMEGA_B);
        Cosmo.set_Omega_CDMh2($COSMO_OMEGA_CDM);
        Cosmo.set_tau($COSMO_TAU);

        // Put in parameter values
        Params.set_MPlanck(1.0);

        Params.set_params( { $FOR{ £PARAM, "£PARAM£COMMA ", ParameterValues, False, True } } );

        InitConds.set_N_initial(0.05);

        InitConds.set_N_subhorizon_efolds(5.0);

        InitConds.set_name("$MODEL default initial conditions from model file");

        InitConds.set_vector( { $FOR{ £FIELD, "£FIELD£COMMA ", FieldValues, False, True } } );

        // TODO: add constructor objects of the parameters and initial conditions blocks here
    }

} //namespace transport

#endif //$GUARD
