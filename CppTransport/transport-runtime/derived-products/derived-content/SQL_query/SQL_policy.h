//
// Created by David Seery on 28/04/15.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef __SQL_policy_H_
#define __SQL_policy_H_


namespace transport
	{

		namespace derived_data
			{

				class SQL_policy
					{

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! constructor
						SQL_policy(std::string time, std::string time_s,
						           std::string twopf, std::string twopf_s,
						           std::string threepf, std::string threepf_s,
						           std::string k1_s, std::string k2_s, std::string k3_s);

						//! destructor is default
						~SQL_policy() = default;


						// INTERFACE

				  public:

						//! get name of time sample table
						const std::string& time_sample_table() const { return(this->time_table); }

						//! get name of twopf sample table
						const std::string& twopf_sample_table() const { return(this->twopf_table); }

						//! get name of threepf sample table
						const std::string& threepf_sample_table() const { return(this->threepf_table); }

						//! get name of time serial number column
						const std::string& time_serial_column() const { return(this->time_serial); }

						//! get name of twopf serial number column
						const std::string& twopf_serial_column() const { return(this->twopf_serial); }

						//! get name of threepf serial number column
						const std::string& threepf_serial_column() const { return(this->threepf_serial); }

						//! get name of k1 serial number column in threepf config
						const std::string& threepf_k1_serial_column() const { return(this->threepf_k1_serial); }

				    //! get name of k2 serial number column in threepf config
				    const std::string& threepf_k2_serial_column() const { return(this->threepf_k2_serial); }

				    //! get name of k2 serial number column in threepf config
				    const std::string& threepf_k3_serial_column() const { return(this->threepf_k3_serial); }


						// INTERNAL DATA

				  protected:

						//! name of time sample table
						std::string time_table;

						//! name of serial-number column in time table
						std::string time_serial;

						//! name of twopf sample table
						std::string twopf_table;

						//! name of serial-number column in twopf table
						std::string twopf_serial;

						//! name of threepf sample table
						std::string threepf_table;

						//! name of serial-number column in threepf table
						std::string threepf_serial;

						//! name of k1 serial number column in threepf
						std::string threepf_k1_serial;

				    //! name of k2 serial number column in threepf
				    std::string threepf_k2_serial;

				    //! name of k3 serial number column in threepf
				    std::string threepf_k3_serial;

					};


				SQL_policy::SQL_policy(std::string time, std::string time_s,
				                       std::string twopf, std::string twopf_s,
				                       std::string threepf, std::string threepf_s,
				                       std::string k1_s, std::string k2_s, std::string k3_s)
					: time_table(time),
					  time_serial(time_s),
		        twopf_table(twopf),
		        twopf_serial(twopf_s),
		        threepf_table(threepf),
						threepf_serial(threepf_s),
						threepf_k1_serial(k1_s),
						threepf_k2_serial(k2_s),
						threepf_k3_serial(k3_s)
					{
					}

			}   // namespace derived_data


	}   // namespace transport


#endif //__SQL_policy_H_
