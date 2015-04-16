//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __integration_abstract_task_H_
#define __integration_abstract_task_H_


#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/utilities/random_string.h"


#define __CPP_TRANSPORT_NODE_FAST_FORWARD                  "fast-forward"
#define __CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS           "ff-efolds"
#define __CPP_TRANSPORT_NODE_MESH_REFINEMENTS              "mesh-refinements"

#define __CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE           "time-config-storage-policy"
#define __CPP_TRANSPORT_NODE_TIME_RANGE                    "time-config-range"

#define __CPP_TRANSPORT_NODE_KSTAR                         "kstar"
#define __CPP_TRANSPORT_NODE_PACKAGE_NAME                  "package"


namespace transport
	{

    template <typename number> class integration_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj);

    //! An 'integration_task' is a specialization of 'task'. It contains the basic information
    //! needed to carry out an integration. The more specialized two- and three-pf integration
    //! tasks are derived from it, as is a concrete 'background' task object which is used
    //! internally for integrating the background only.

    //! An 'integration_task' contains information on the initial conditions, horizon-crossing
    //! time and sampling times.
    template <typename number>
    class integration_task: public derivable_task<number>
	    {

      public:

        //! defines an object which computes kstar for a given integration task
        typedef std::function<double(integration_task<number>*)> kconfig_kstar;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named integration task with supplied initial conditions and time-configuration storage policy
        integration_task(const std::string& nm, const initial_conditions<number>& i, const range<double>& t, time_config_storage_policy p);

        //! Construct an anonymized integration task with supplied initial conditions.
        //! Anonymized tasks are used for things like constructing initial conditions,
        //! integrating the background only, finding H at horizon-crossing etc.
        //! Science output is expected to be generated using named tasks.
        integration_task(const initial_conditions<number>& i, const range<double>& t)
	        : integration_task(random_string(), i, t, default_time_config_storage_policy())
	        {
	        }

        //! deserialization constructor
        integration_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i);

        //! override copy constructor to perform a deep copy of associated range<> object
        integration_task(const integration_task<number>& obj);

        //! Destroy an integration task
        virtual ~integration_task();


        // INTERFACE - TASK COMPONENTS

      public:

        //! Get 'initial conditions' object associated with this task
        const initial_conditions<number>& get_ics() const { return(this->ics); }

        //! Get 'parameters' object associated with this task
        const parameters<number>& get_params() const { return(this->ics.get_params()); }

        //! Get model associated with this task
        model<number>* get_model() const { return(this->ics.get_model()); }


        // INTERFACE - implements a 'derivable task' interface

      public:

        //! Get vector of time configurations to store
        virtual const std::vector<time_config>& get_time_config_list() const override { return(this->time_config_list); }


        // INTERFACE - INTEGRATION DETAILS - TIMES AND INITIAL CONDITIONS

      public:

        //! Get initial times
        double get_Ninit() const { return(this->times->get_min()); }

        //! Get horizon-crossing time
        double get_Nstar() const { return(this->ics.get_Nstar()); }

        //! Get std::vector of integration step times
        const std::vector<double> get_integration_step_times() const;

        //! Get std::vector of integration step times, and simultaneously populate a storage list
        const std::vector<double> get_integration_step_times(std::vector<time_storage_record>& slist, unsigned int refine = 0) const;

        //! Get std::vector of integration step times, truncated for fast-forwarding if enabled
        std::vector<double> get_integration_step_times(const twopf_kconfig& kconfig, std::vector<time_storage_record>& slist, unsigned int refine = 0) const;

        //! Get std::vector of integration step times, truncated for fast-forwarding if enabled
        std::vector<double> get_integration_step_times(const threepf_kconfig& kconfig, std::vector<time_storage_record>& slist, unsigned int regine = 0) const;

        //! Get std::vector of integration step times, truncated at Nstart if fast-forwarding enabled
        std::vector<double> get_integration_step_times(double Nstart, std::vector<time_storage_record>& slist, unsigned int refine = 0) const;

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const twopf_kconfig& kconfig) const;

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const threepf_kconfig& kconfig) const;

        //! Get std::vector of initial conditions, offset by Nstar using fast forwarding if enables
        std::vector<number> get_ics_vector(double Nstart=0.0) const;

      protected:

        //! insert refinements in the list of integration step times
        void refine_integration_step_times(std::vector<double>& time_values, std::vector<time_storage_record>& slist,
                                           unsigned int refine, double step_min, double step_max) const;


        // FAST-FORWARD INTEGRATION MANAGEMENT

      public:

        //! Get fast-forward integration setting
        bool get_fast_forward() const { return(this->fast_forward); }

        //! Set fast-forward integration setting
        void set_fast_forward(bool g) { this->fast_forward = g; this->apply_time_storage_policy(); }

        //! Get number of fast-forward e-folds
        double get_fast_forward_efolds() const { return(this->ff_efolds); }

        //! Set number of fast-forward e-folds
        void set_fast_forward_efolds(double N) { this->ff_efolds = (N >= 0.0 ? N : this->ff_efolds); this->apply_time_storage_policy(); }

        //! Get largest wavenumber included in the integration
        virtual double get_kmax() const { return(1.0); }

        //! Get smallest wavenumber included in the integration
        virtual double get_kmin() const { return(1.0); }

      protected:

        //! Populate list of time configurations to be sorted, given that the earliest allowed time is tmin.
        //! If we are using fast-forward integration, tmin will be Nstar + ln(kmax) - ff_efolds,
        //! where kmax is the largest k number we are evolving for.
        //! Since this is the start time for that mode, we can't guarantee that results will be available
        //! be at earlier times
        void apply_time_storage_policy();


        // MESH REFINEMENT

      public:

        //! Get number of allowed mesh refinements
        unsigned int get_max_refinements() const { return(this->max_refinements); }

        //! Set number of allowed mesh refinements
        void set_max_refinemenets(unsigned int max) { this->max_refinements = (max > 0 ? max : this->max_refinements); }


        // SERIALIZE - implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;



        // WRITE TO STREAM

      public:

        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const integration_task<number>& obj);


        // INTERNAL DATA

      protected:

        //! Initial conditions for this task (including parameter choices)
        initial_conditions<number>       ics;

        //! Range of times at which to sample for this task;
        //! really kept only for serialization purposes
        range<double>*                   times;


        // TIME STORAGE POLICY

        //! Time configuration storage policy for this task
        const time_config_storage_policy time_storage_policy;

        //! Unfiltered list of time-vales
        std::vector<double>              raw_time_list;

        //! Filtered list of time-configurations (corresponding to values stored in the database)
        std::vector<time_config>         time_config_list;


        // FAST FORWARD INTEGRATION

        //! Whether to use fast-forward integration
        bool                             fast_forward;

        //! Number of e-folds to use in fast-forward integration
        double                           ff_efolds;


        // MESH REFINEMENT

        //! How many mesh refinements to allow per triangle
        unsigned int                     max_refinements;

	    };


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, const initial_conditions<number>& i,
                                               const range<double>& t, time_config_storage_policy p)
	    : derivable_task<number>(nm),
	      ics(i),
	      times(t.clone()),
	      time_storage_policy(p),
	      fast_forward(true), ff_efolds(__CPP_TRANSPORT_DEFAULT_FAST_FORWARD_EFOLDS),
	      max_refinements(__CPP_TRANSPORT_DEFAULT_MESH_REFINEMENTS)
	    {
        // validate relation between Nstar and the sampling time
        assert(times->get_steps() > 0);
        assert(i.get_Nstar() > times->get_min());
        assert(i.get_Nstar() < times->get_max());

        if(times->get_steps() == 0)
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NO_TIMES;
            throw std::logic_error(msg.str());
	        }

        if(times->get_min() >= ics.get_Nstar())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_EARLY << " (Ninit = " << times->get_min() << ", Nstar = " << ics.get_Nstar() << ")";
            throw std::logic_error(msg.str());
	        }

        if(times->get_max() <= ics.get_Nstar())
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_NSTAR_TOO_LATE << " (Nend = " << times->get_max() << ", Nstar = " << ics.get_Nstar() << ")";
            throw std::logic_error(msg.str());
	        }
	    }


    template <typename number>
    integration_task<number>::integration_task(const std::string& nm, Json::Value& reader, const initial_conditions<number>& i)
	    : derivable_task<number>(nm, reader),
	      time_storage_policy(default_time_config_storage_policy()),
	      ics(i),
	      times(range_helper::deserialize<double>(reader[__CPP_TRANSPORT_NODE_TIME_RANGE]))
	    {
        // deserialize fast-forward integration setting
        fast_forward = reader[__CPP_TRANSPORT_NODE_FAST_FORWARD].asBool();

        // deserialize number of fast-forward efolds
        ff_efolds = reader[__CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS].asDouble();

        // deserialize max number of mesh refinements
        max_refinements = reader[__CPP_TRANSPORT_NODE_MESH_REFINEMENTS].asUInt();

        // raw grid of sampling times can be reconstructed from the range object
        raw_time_list = times->get_grid();

        // filtered times have to be reconstructed from the database
        Json::Value& time_storage = reader[__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE];
        assert(time_storage.isArray());

        time_config_list.clear();
        time_config_list.reserve(time_storage.size());

        for(Json::Value::iterator t = time_storage.begin(); t != time_storage.end(); t++)
	        {
            unsigned int sn = t->asUInt();
            if(sn >= raw_time_list.size()) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_TIME_CONFIG_SN_TOO_BIG);

            time_config tc;
            tc.t = raw_time_list[sn];
            tc.serial = sn;
            time_config_list.push_back(tc);
	        }

        // the integrator relies on the list of time configurations being in order
        struct TimeConfigSorter
	        {
          public:
            bool operator() (const time_config& a, const time_config& b) { return(a.serial < b.serial); }
	        };

        std::sort(time_config_list.begin(), time_config_list.end(), TimeConfigSorter());
	    }


    template <typename number>
    integration_task<number>::integration_task(const integration_task<number>& obj)
	    : derivable_task<number>(obj),
	      ics(obj.ics),
	      times(obj.times->clone()),
	      time_storage_policy(obj.time_storage_policy),
	      raw_time_list(obj.raw_time_list),
	      time_config_list(obj.time_config_list),
	      fast_forward(obj.fast_forward),
	      ff_efolds(obj.ff_efolds),
	      max_refinements(obj.max_refinements)
	    {
	    }


    template <typename number>
    integration_task<number>::~integration_task()
	    {
        assert(this->times != nullptr);

        delete this->times;
	    }


    template <typename number>
    void integration_task<number>::serialize(Json::Value& writer) const
	    {
        // store name of package
        writer[__CPP_TRANSPORT_NODE_PACKAGE_NAME] = this->ics.get_name();

        // store fast-forward integration setting
        writer[__CPP_TRANSPORT_NODE_FAST_FORWARD] = this->fast_forward;

        // store number of fast-forward efolds
        writer[__CPP_TRANSPORT_NODE_FAST_FORWARD_EFOLDS] = this->ff_efolds;

        // store max number of mesh refinements
        writer[__CPP_TRANSPORT_NODE_MESH_REFINEMENTS] = this->max_refinements;

        // serialize range of sampling times
        Json::Value time_range(Json::objectValue);
        this->times->serialize(time_range);
        writer[__CPP_TRANSPORT_NODE_TIME_RANGE] = time_range;

        // serialize filtered range of times, those which will be stored in the database
        Json::Value time_storage(Json::arrayValue);
        for(std::vector<time_config>::const_iterator t = this->time_config_list.begin(); t != this->time_config_list.end(); t++)
	        {
            Json::Value storage_element = t->serial;
            time_storage.append(storage_element);
	        }
        writer[__CPP_TRANSPORT_NODE_TIME_CONFIG_STORAGE] = time_storage;

        // note that we DO NOT serialize the initial conditions;
        // these are handled separately by the repository layer

        // call next serializers in the queue
        this->derivable_task<number>::serialize(writer);
	    }


    template <typename number>
    void integration_task<number>::apply_time_storage_policy()
	    {
        // get raw grid of sampling times
        raw_time_list.clear();
        raw_time_list = times->get_grid();

        // compute earliest time at which all modes will have available data, if using fast-forward integration
        double tmin = this->ics.get_Nstar() + log(this->get_kmax()) - this->ff_efolds;
        if(tmin <= 0.0) tmin = 0.0;   // TODO: consider whether this is correct

        double tstart = this->ics.get_Nstar() + log(this->get_kmin()) - this->ff_efolds;
        if(tstart < 0.0 && fabs(this->ff_efolds-5.0) > 0.1)
	        {
            std::cerr << "Nstart = " << tstart << ", N* = " << this->ics.get_Nstar() << ", log(kmin) = " << log(this->get_kmin()) << ", kmin = " << this->get_kmin() << ", ff_efolds = " << this->ff_efolds << std::endl;
            assert(false);
	        }

        // filter sampling times according to our storage policy
        time_config_list.clear();
        for(unsigned int i = 0; i < raw_time_list.size(); i++)
	        {
            time_config tc;
            tc.t = raw_time_list[i];
            tc.serial = i;

            time_config_storage_policy_data data(tc.t, tc.serial);
            if((!this->fast_forward || (this->fast_forward && tc.t >= tmin)) && time_storage_policy(data))
	            {
                time_config_list.push_back(tc);
	            }
	        }
	    }


    template <typename number>
    const std::vector<double> integration_task<number>::get_integration_step_times() const
	    {
        return this->raw_time_list;
	    }


    template <typename number>
    const std::vector<double> integration_task<number>::get_integration_step_times(std::vector<time_storage_record>& slist, unsigned int refine) const
	    {
        return(this->get_integration_step_times(0.0, slist, refine));
	    }


    template <typename number>
    std::vector<double> integration_task<number>::get_integration_step_times(const twopf_kconfig& kconfig, std::vector<time_storage_record>& slist, unsigned int refine) const
	    {
        double Nstart = this->ics.get_Nstar() + log(kconfig.k_conventional) - this->ff_efolds;
        if(Nstart < 0.0 && this->ff_efolds) std::cerr << "Task '" << this->name << "', Nstart = " << Nstart << ", N* = " << this->ics.get_Nstar() << ", log(k) = " << log(kconfig.k_conventional) << ", kmin = " << kconfig.k_conventional << ", ff_efolds = " << this->ff_efolds << std::endl;
        assert(!this->ff_efolds || Nstart >= 0.0);

        return this->get_integration_step_times(Nstart, slist, refine);
	    }


    template <typename number>
    std::vector<double> integration_task<number>::get_integration_step_times(const threepf_kconfig& kconfig, std::vector<time_storage_record>& slist, unsigned int refine) const
	    {
        double kmin = std::min(std::min(kconfig.k1_conventional, kconfig.k2_conventional), kconfig.k3_conventional);

        double Nstart = this->ics.get_Nstar() + log(kmin) - this->ff_efolds;
        if(Nstart < 0.0 && this->ff_efolds) std::cerr << "Task '" << this->name << "', Nstart = " << Nstart << ", N* = " << this->ics.get_Nstar() << ", log(kmin) = " << log(kmin) << ", kmin = " << kmin << ", ff_efolds = " << this->ff_efolds << std::endl;
        assert(!this->ff_efolds || Nstart >= 0.0);

        return this->get_integration_step_times(Nstart, slist, refine);
	    }


    template <typename number>
    std::vector<double> integration_task<number>::get_integration_step_times(double Nstart, std::vector<time_storage_record>& slist, unsigned int refine) const
	    {
        if(refine > this->max_refinements) throw runtime_exception(runtime_exception::REFINEMENT_FAILURE, __CPP_TRANSPORT_REFINEMENT_TOO_DEEP);

        if(!this->fast_forward) Nstart = 0.0;

        unsigned int reserve_size = (this->raw_time_list.size()+1) * static_cast<unsigned int>(pow(4.0, refine));

        slist.clear();
        slist.reserve(reserve_size);

        std::vector<double> time_values;
        time_values.reserve(reserve_size);
        if(Nstart < this->time_config_list.front().t)
	        {
            time_values.push_back(Nstart);
            slist.push_back(time_storage_record(false, 0, 0));
	        }

        std::vector<time_config>::const_iterator t = this->time_config_list.begin();

        for(unsigned int i = 0; i < this->raw_time_list.size(); i++)
	        {
            // ensure next time to be stored is not earlier than the time we are currently looking at
            assert(t == this->time_config_list.end() || t->t >= this->raw_time_list[i]);

            if(this->raw_time_list[i] >= Nstart)
	            {
                if(t != this->time_config_list.end() && t->serial == i)
	                {
                    time_values.push_back(this->raw_time_list[i]);
                    slist.push_back(time_storage_record(true, t->serial, t->t));
                    t++;
	                }
                else
	                {
                    time_values.push_back(this->raw_time_list[i]);
                    slist.push_back(time_storage_record(false, 0, 0));
	                }

                if(refine > 0 && i < this->raw_time_list.size()-1)
	                {
                    this->refine_integration_step_times(time_values, slist, refine, this->raw_time_list[i], this->raw_time_list[i+1]);
	                }
	            }
	        }

        assert(time_values.size() == slist.size());
        return(time_values);
	    }


    template <typename number>
    void integration_task<number>::refine_integration_step_times(std::vector<double>& time_values, std::vector<time_storage_record>& slist,
                                                                 unsigned int refine, double step_min, double step_max) const
	    {
        unsigned int steps = static_cast<unsigned int>(pow(4.0, refine));

        stepping_range<double> mesh(step_min, step_max, steps, linear_stepping);
        const std::vector<double>& mesh_grid = mesh.get_grid();

        for(unsigned int i = 1; i < mesh_grid.size(); i++)
	        {
            time_values.push_back(mesh_grid[i]);
            slist.push_back(time_storage_record(false, 0, 0));
	        }
	    }


    template <typename number>
    std::vector<number> integration_task<number>::get_ics_vector(const twopf_kconfig& kconfig) const
	    {
        double Nstart = this->ics.get_Nstar() + log(kconfig.k_conventional) - this->ff_efolds;

        return this->get_ics_vector(Nstart);
	    }


    template <typename number>
    std::vector<number> integration_task<number>::get_ics_vector(const threepf_kconfig& kconfig) const
	    {
        double kmin = std::min(std::min(kconfig.k1_conventional, kconfig.k2_conventional), kconfig.k3_conventional);

        double Nstart = this->ics.get_Nstar() + log(kmin) - this->ff_efolds;

        return this->get_ics_vector(Nstart);
	    }


    template <typename number>
    std::vector<number> integration_task<number>::get_ics_vector(double Nstart) const
	    {
        if(this->fast_forward && Nstart > 0.0) return this->ics.get_offset_vector(Nstart);
        else                                   return this->ics.get_vector();
	    }


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const integration_task<number>& obj)
	    {
        out << __CPP_TRANSPORT_FAST_FORWARD     << ": " << (obj.fast_forward ? __CPP_TRANSPORT_YES : __CPP_TRANSPORT_NO) << ", ";
        out << __CPP_TRANSPORT_MESH_REFINEMENTS << ": " << obj.max_refinements << std::endl;
        out << obj.ics << std::endl;
//        out << __CPP_TRANSPORT_TASK_TIMES << obj.times;
        return(out);
	    }

	}   // namespace transport


#endif //__integration_abstract_task_H_
