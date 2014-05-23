//
// Created by David Seery on 22/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __linecache_H_
#define __linecache_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdexcept>

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/date_time/posix_time/posix_time.hpp"


namespace transport
	{

		namespace linecache
			{

				// template functions, which must be specialized later, used to obtain the size of a
				// container in bytes, and the number of elements in the container
				template <typename Container> unsigned int sizeof_container_element();
				template <typename Container> unsigned int elementsof_container(const Container& c);

				// forward declare constituent classes
				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				class table;

				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				class serial_group;

		    //! 'cache' implements an in-memory cache for the database backends.
		    //! The cache tries to manage itself to fit within a certain capacity
		    //! (although the calculations used to achieve this are approximate)

		    template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
		    class cache
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Create a cache object
		        cache(unsigned int cap)
		          : capacity(cap), data_size(0), hit_counter(0)
			        {
			        }

				    //! Destroy a cache object
				    ~cache() = default;


		        // ADMIN

		      public:

				    //! Advise the cache of an increase in size. Used by table objects belonging to this
				    //! cache when new data elements are added.
				    //! If the new size exceeds the target capacity, then the cache will
				    //! carry out a clean up operation in which least-recently-used cache lines
				    //! are evicted until the cache size is smaller than the target capacity.
		        void advise_size_increase(unsigned int bytes);


		        // TABLE MANAGEMENT

		      public:

		        //! Get a cache table handle for a named file. A new table is not necessarily created;
		        //! if an existing cache table exists for this file (from some earlier sequence
		        //! of read operations) then it is re-used.
		        table<DataContainer, DataTag, SerialTag, HashSize>& get_table_handle(const std::string& fnam);


		        // INTERNAL DATA

		      protected:

		        //! Capacity of the cache. The cache evicts data when memory use exceeds the
		        //! stated capacity
		        unsigned int capacity;

		        //! Current memory usage
		        unsigned int data_size;

				    //! Hit counter
				    unsigned int hit_counter;

		        //! List of tables belonging to this cache.
				    //! We use a list because iterators pointing to list elements
				    //! are not invalidated by insertion or removal operations.
		        std::list< table<DataContainer, DataTag, SerialTag, HashSize> > tables;

			    };


		    //! 'table' represents a cache for a single file.
		    //! A 'cache' instance will keep data in memory even after the
		    //! database has closed on the assumption that it will not be modified.
		    //! This helps minimize the amount of reloading which has to occur.
		    template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
		    class table
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Create a table object
		        table(const std::string& fnam, cache<DataContainer, DataTag, SerialTag, HashSize>* p, unsigned int& hc);

				    ~table() = default;


		        // ADMIN

		        //! Gather all data_items belonging to this table
		        void gather_data_items(typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator >& item_list);

				    //! Check for equality with a given filename
				    bool operator==(const std::string& fnam) const { return(this->filename == fnam); }


		        // SERIAL NUMBER GROUPS

		      public:

		        //! Get a serial group handle. A new handle is not necessarily created;
		        //! if an existing handle exists for the same group of serial numbers
		        //! then it is re-used
		        serial_group<DataContainer, DataTag, SerialTag, HashSize>& get_serial_handle(const std::vector<unsigned int>& serials, const SerialTag& tag);


		        // INTERNAL DATA

		      protected:

				    //! Filename corresponding to this table
				    const std::string& filename;

				    //! Parent 'cache' object
				    cache<DataContainer, DataTag, SerialTag, HashSize>* parent_cache;

		        //! Hit counter -- reference to that owned by the parent cache
		        unsigned int& hit_counter;

		        //! List of serial-group handles.
				    //! Held in a std::list so that iterators pointing to serial groups
				    //! remain valid after insertion or removal of elements
		        std::list< serial_group<DataContainer, DataTag, SerialTag, HashSize> > groups;

			    };


				//! 'serial_group' represents the data entries which
				//! have been cached for a particular group of serial numbers
				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				class serial_group
					{

				  public:

						class data_item
							{

								// CONSTRUCTOR, DESTRUCTOR

						  public:

								//! construct a new data item
								data_item(const DataContainer& d, DataTag& t, std::list<data_item>& p)
						      : data(d), tag(t.clone()), parent_list(p), last_access(boost::posix_time::microsec_clock::universal_time()), locked(true)
									{
									}

								//! perform deep copy
								data_item(const data_item& obj)
									: data(obj.data), tag(obj.tag->clone()), parent_list(obj.parent_list), last_access(obj.last_access), locked(obj.locked)
									{
									}

								~data_item()
									{
										delete this->tag;
									}


								// ADMIN

								//! Update time of last access
								void update_access_time() { this->last_access = boost::posix_time::microsec_clock::universal_time(); }

								//! Compare for equality of tags
								bool operator==(const DataTag& t) const { return(*(this->tag) == t); }


								// ACCESS

								//! Return this item's tag
								DataTag* get_tag() const { return(this->tag); }

								//! Return this item's data
								const DataContainer& get_data() { this->last_access = boost::posix_time::microsec_clock::universal_time(); return(this->data); }

								//! Return size of data held in this item, in bytes
								unsigned int get_size() const { return(::transport::linecache::sizeof_container_element<DataContainer>() * ::transport::linecache::elementsof_container(this->data)); }

								//! Return this item's parent list
								std::list<data_item>& get_parent_list() const { return(this->parent_list); }

								//! Return this item's access time
								const boost::posix_time::ptime& get_last_access_time() const { return(this->last_access); }


								// LOCKING

								//! Unlock this item
								void unlock() { this->locked = false; }

								//! Get lock status
								bool get_locked() const { return(this->locked); }


								// INTERNAL DATA

						  protected:

								//! tag for this item
								DataTag* tag;

								//! parent list, used when evicting this item
								std::list<data_item>& parent_list;

								//! Last access time for this item. Used when deciding which cache entries to drop
								boost::posix_time::ptime last_access;

								//! Data
								const DataContainer data;

								//! 'Locked' flag marks this data item as not-evictable. Prevents eviction before the client has even seen the data!
								bool locked;

							};


						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Create a serial_group object
						serial_group(const std::vector<unsigned int>& sns, const SerialTag& t, typename linecache::cache<DataContainer, DataTag, SerialTag, HashSize>* p, unsigned int& hc);


						// ADMIN

						//! Gather all data_items belonging to this group
						void gather_data_items(typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator >& item_list);

						//! Check for equality of serial groups
						bool match(const std::vector<unsigned int>& sns, const SerialTag& t) { return(this->serial_numbers == sns && this->tag == t); }

						// CACHE LOOKUP

						//! Lookup data in the cache
						const DataContainer& lookup_tag(DataTag& tag);


						// INTERNAL DATA

				  protected:

						//! Parent 'cache' object
						linecache::cache<DataContainer, DataTag, SerialTag, HashSize>* parent_cache;

						//! Hit counter -- reference to counter owned by cache object
						unsigned int& hit_counter;

						//! Serial numbers identifying this group
						const std::vector<unsigned int> serial_numbers;

						//! tag identifying this group
						const SerialTag tag;

						//! Hash table of cached data lines.
						//! Each element in the has table is a std::list of data items.
						//! We use std::list so that iterators pointing to the cache elements
						//! are not invalidated by insertion or removal.
						//! This is very important in order that cache evictions can be
						//! carried out efficiently.
						std::array< std::list<data_item>, HashSize > cache;
					};


		    // CACHE METHODS


		    template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
		    table<DataContainer, DataTag, SerialTag, HashSize>& cache<DataContainer, DataTag, SerialTag, HashSize>::get_table_handle(const std::string& name)
			    {
		        typename std::list< table<DataContainer, DataTag, SerialTag, HashSize> >::iterator t;
		        if((t = std::find(this->tables.begin(), this->tables.end(), name)) == this->tables.end())   // table doesn't already exist
			        {
		            this->tables.push_front( table<DataContainer, DataTag, SerialTag, HashSize>(name, this, this->hit_counter) );
		            t = this->tables.begin();
			        }

		        return(*t);
			    }


				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				void cache<DataContainer, DataTag, SerialTag, HashSize>::advise_size_increase(unsigned int bytes)
					{
						this->data_size += bytes;

						if(this->data_size > this->capacity)   // run evictions if we are now too large
							{
								// build a list of data items on which to run clean-up
						    typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator > clean_up_list;

								for(typename std::list< table<DataContainer, DataTag, SerialTag, HashSize> >::iterator t = this->tables.begin(); t != this->tables.end(); t++)
									{
								    (*t).gather_data_items(clean_up_list);
									}

								// set up a lambda to sort the list is data items into ascending order of size
						    struct DataItemSorter
							    {
						        bool operator()(const typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator& a,
						                        const typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator& b)
							        {
						            return (*a).get_last_access_time() < (*b).get_last_access_time();
							        }
							    };

								// sort queue
								clean_up_list.sort(DataItemSorter());

								// work through the queue, evicting items until we have saved sufficient size
						    typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator >::iterator t = clean_up_list.begin();
								while(this->data_size > this->capacity && t != clean_up_list.end())
									{
								    if(!(*(*t)).get_locked())
									    {
								        std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >& list = (*(*t)).get_parent_list();
										    this->data_size -= (*(*t)).get_size();
										    list.erase(*t);
									    }
									}
							}
					}


				// TABLE METHODS


		    template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
		    table<DataContainer, DataTag, SerialTag, HashSize>::table(const std::string& fnam, cache<DataContainer, DataTag, SerialTag, HashSize>* p, unsigned int& hc)
			    : filename(fnam), parent_cache(p), hit_counter(hc)
			    {
				    assert(p != nullptr);
			    }


		    template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
		    serial_group<DataContainer, DataTag, SerialTag, HashSize>& table<DataContainer, DataTag, SerialTag, HashSize>::get_serial_handle(const std::vector<unsigned int>& serials, const SerialTag& tag)
			    {
		        typename std::list< serial_group<DataContainer, DataTag, SerialTag, HashSize > >::iterator t;
				    for(t = this->groups.begin(); t != this->groups.end(); t++)
					    {
						    if((*t).match(serials, tag)) break;
					    }

				    if(t == this->groups.end())
					    {
						    this->groups.push_front(serial_group<DataContainer, DataTag, SerialTag, HashSize>(serials, tag, this->parent_cache, this->hit_counter));
						    t = this->groups.begin();
					    }

				    return(*t);
			    }


				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				void table<DataContainer, DataTag, SerialTag, HashSize>::gather_data_items(typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator >& item_list)
					{
						// work through all serial groups owned by this table, pushing their data_items into item_list
						for(typename std::list< serial_group<DataContainer, DataTag, SerialTag, HashSize> >::iterator t = this->groups.begin(); t != this->groups.end(); t++)
							{
						    (*t).gather_data_items(item_list);
							}
					}


				// SERIAL GROUP METHODS


		    template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
		    serial_group<DataContainer, DataTag, SerialTag, HashSize>::serial_group(const std::vector<unsigned int>& sns, const SerialTag& t,
		                                                                            linecache::cache<DataContainer, DataTag, SerialTag, HashSize>* p, unsigned int& hc)
			    : serial_numbers(sns), tag(t), parent_cache(p), hit_counter(hc)
			    {
		        assert(p != nullptr);
			    }


				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				const DataContainer& serial_group<DataContainer, DataTag, SerialTag, HashSize>::lookup_tag(DataTag& tag)
					{
						unsigned int hash = tag.hash();

						assert(hash < HashSize);

				    typename std::list< serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator t;
						if((t = std::find(this->cache[hash].begin(), this->cache[hash].end(), tag)) == this->cache[hash].end())     // data item doesn't already exist
							{
								DataContainer data;
						    tag.pull(this->serial_numbers, data);
								assert(::transport::linecache::elementsof_container(data) == this->serial_numbers.size());

								this->cache[hash].push_front(data_item(data, tag, this->cache[hash]));
								t = this->cache[hash].begin();

								// update size data - note that advising the cache of a size increase could lead to evictions,
								// but cannot evict the data item we have just created because it is locked by
								// default -- until we explicitly unlock it below
								this->parent_cache->advise_size_increase((*t).get_size());
							}
						else
							{
						    (*t).update_access_time();
						    this->hit_counter++;
							}

						// unlock this data item
				    (*t).unlock();

						return((*t).get_data());
					}

				template <typename DataContainer, typename DataTag, typename SerialTag, unsigned int HashSize>
				void serial_group<DataContainer, DataTag, SerialTag, HashSize>::gather_data_items(typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, SerialTag, HashSize>::data_item >::iterator >& item_list)
					{
						for(unsigned int i = 0; i < HashSize; i++)
							{
								for(typename std::list<data_item>::iterator t = this->cache[i].begin(); t != this->cache[i].end(); t++)
									{
										item_list.push_front(t);
									}
							}
					}


			}   // namespce linecache


	}   // namespace transport


#endif //__linecache_H_
