//
// Created by David Seery on 22/05/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
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
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_LINECACHE_H
#define CPPTRANSPORT_LINECACHE_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdexcept>

#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

#include "boost/timer/timer.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


//#define CPPTRANSPORT_LINECACHE_DEBUG


namespace transport
	{

		namespace linecache
			{

				// template function, which must be specialized later, used to obtain the size of an element
        // held by a container (measured in bytes)
				template <typename Container> unsigned int sizeof_container_element();
				
        // template function, which must be specialized later, used to obtain the number of elements in a container
        template <typename Container> unsigned int elementsof_container(const Container& c);

				// forward declare constituent classes
				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				class table;

				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				class serial_group;

		    //! 'cache' implements an in-memory cache for the database backends.
		    //! The cache tries to manage itself to fit within a certain capacity
		    //! (although the calculations used to achieve this are approximate)

		    template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
		    class cache
			    {

		      public:

			      typedef std::list< table<DataContainer, DataTag, QueryObject, HashSize> > data_table_list;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Create a cache object
		        cache(size_t cap)
		          : capacity(cap), data_size(0), hit_counter(0), unload_counter(0)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
			        , copied(0)
#endif
			        {
				        eviction_timer.stop();
			        }

				    //! Copy a cache object. After copying all of our contents we have to reset the
				    //! point to ourselves which they contain
				    cache(const cache<DataContainer, DataTag, QueryObject, HashSize>& obj)
					    : capacity(obj.capacity),
					      data_size(obj.data_size),
					      hit_counter(obj.hit_counter),
					      tables(obj.tables)

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
					    , copied(obj.copied+1)
#endif

						    {

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
				        std::cerr << "WARNING -- COPYING CACHE OBJECT, COUNT = " << copied << '\n';
#endif

						    for(typename data_table_list::iterator t = this->tables.begin(); t != this->tables.end(); ++t)
							    {
						        (*t).reset_parent_cache(this);
							    }
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

				    //! Advise a new hit
				    void hit() { this->hit_counter++; }

				    //! Read total capacity of the cache
				    size_t get_capacity() const { return(this->capacity); }

				    //! Read total size of the cache
				    size_t get_size() const { return(this->data_size); }

				    //! Read total number of cache hits
				    unsigned int get_hits() const { return(this->hit_counter); }

				    //! Return total number of cache unloads
				    unsigned int get_unloads() const { return(this->unload_counter); };

				    //! Return time spent evicting cache lines
				    boost::timer::nanosecond_type get_eviction_timer() const { return(this->eviction_timer.elapsed().wall); }


		        // TABLE MANAGEMENT

		      public:

		        //! Get a cache table handle for a named file. A new table is not necessarily created;
		        //! if an existing cache table exists for this file (from some earlier sequence
		        //! of read operations) then it is re-used.
		        table<DataContainer, DataTag, QueryObject, HashSize>& get_table_handle(const std::string& fnam);


		        // INTERNAL DATA

		      protected:

		        //! Capacity of the cache. The cache evicts cachelines when memory use exceeds the
		        //! stated capacity
		        size_t capacity;

		        //! Current memory usage
		        size_t data_size;

				    //! Hit counter -- how many times do we hit a cache line, saving us from going out to the database?
				    unsigned int hit_counter;

				    //! Unload counter - how many times do we have to unload a cache line? Too many and we need a larger cache
				    unsigned int unload_counter;

				    //! Eviction timer - how long do we spend evicting cache lines?
				    boost::timer::cpu_timer eviction_timer;

		        //! List of tables belonging to this cache.
				    //! We use a list because iterators pointing to list elements
				    //! are not invalidated by insertion or removal operations.
		        data_table_list tables;

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
		        //! Copy counter
		        unsigned int copied;
#endif

			    };


		    //! 'table' represents a cache for a single file.
		    //! A 'cache' instance will keep data in memory even after the
		    //! database has closed on the assumption that it will not be modified.
		    //! This helps minimize the amount of reloading which has to occur.
		    template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
		    class table
			    {

		      public:

			      typedef std::list< serial_group<DataContainer, DataTag, QueryObject, HashSize> > serial_group_list;

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Create a table object
		        table(const std::string& fnam, cache<DataContainer, DataTag, QueryObject, HashSize>* p);

				    //! Override default copy constructor
				    table(const table<DataContainer, DataTag, QueryObject, HashSize>& obj)
				      : filename(obj.filename), parent_cache(obj.parent_cache), groups(obj.groups)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
					    , copied(obj.copied+1)
#endif
					    {
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
						    if(copied >= 2) std::cerr << "WARNING -- COPYING TABLE OBJECT, COUNT = " << copied << '\n';
#endif
					    }

				    ~table() = default;


		        // ADMIN

		        //! Gather all data_items belonging to this table
		        void gather_data_items(std::list< typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::cache_line::iterator >& item_list);

				    //! Check for equality with a given filename
				    bool operator==(const std::string& fnam) const { return(this->filename == fnam); }

				    //! Reset parent cache
				    void reset_parent_cache(linecache::cache<DataContainer, DataTag, QueryObject, HashSize>* p)
					    {
						    assert(p != nullptr);
						    this->parent_cache = p;
						    for(typename serial_group_list::iterator t = this->groups.begin(); t != this->groups.end(); ++t)
							    {
						        (*t).reset_parent_cache(p);
							    }
					    }


		        // SERIAL NUMBER GROUPS

		      public:

		        //! Get a serial group handle. A new handle is not necessarily created;
		        //! if an existing handle exists for the same group of serial numbers
		        //! then it is re-used
		        serial_group<DataContainer, DataTag, QueryObject, HashSize>& get_serial_handle(const QueryObject& q);


		        // INTERNAL DATA

		      protected:

				    //! Filename corresponding to this table
				    const std::string filename;

				    //! Parent 'cache' object
				    cache<DataContainer, DataTag, QueryObject, HashSize>* parent_cache;

		        //! List of serial-group handles.
				    //! Held in a std::list so that iterators pointing to serial groups
				    //! remain valid after insertion or removal of elements
		        serial_group_list groups;

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
				    //! Copy counter
				    unsigned int copied;
#endif

			    };


				//! 'serial_group' represents the data entries which
				//! have been cached for a particular group of serial numbers
				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				class serial_group
					{

				  public:

						class data_item
							{

								// CONSTRUCTOR, DESTRUCTOR

						  public:

								//! construct a new data item
								data_item(const DataContainer& d, DataTag& t, std::list<data_item>* p
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
									, const std::string& tn
#endif
								)
						      : data(d), tag(t.clone()), parent_list(p), last_access(boost::posix_time::microsec_clock::universal_time()), locked(true)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
									, table_name(tn), copied(0)
#endif
							    {
									}

								//! perform deep copy
								data_item(const data_item& obj)
									: data(obj.data), tag(obj.tag->clone()), parent_list(obj.parent_list), last_access(obj.last_access), locked(obj.locked)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
									, copied(obj.copied+1), table_name(obj.table_name)
#endif
									{
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
										if(copied >= 2) std::cerr << "WARNING - COPYING DATA_ITEM OBJECT, COUNT = " << copied << '\n';
#endif
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

								//! Reset owning list
								void reset_owner_list(std::list<data_item>* owner) { this->parent_list = owner; }


								// ACCESS

								//! Return this item's tag
								DataTag* get_tag() const { return(this->tag); }

								//! Return this item's data
								const DataContainer& get_data() { this->last_access = boost::posix_time::microsec_clock::universal_time(); return(this->data); }

								//! Return size of data held in this item, in bytes
								unsigned int get_size() const { return(::transport::linecache::sizeof_container_element<DataContainer>() * ::transport::linecache::elementsof_container(this->data)); }

								//! Return this item's parent list
								std::list<data_item>* get_parent_list() const { return(this->parent_list); }

								//! Return this item's access time
								const boost::posix_time::ptime& get_last_access_time() const { return(this->last_access); }

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
								//! Return this item's owning table
								const std::string& get_table_name() const { return(this->table_name); }
#endif

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
								std::list<data_item>* parent_list;

								//! Last access time for this item. Used when deciding which cache entries to drop
								boost::posix_time::ptime last_access;

								//! Data
								const DataContainer data;

								//! 'Locked' flag marks this data item as not-evictable. Prevents eviction before the client has even seen the data!
								bool locked;

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
								//! Copy count
								unsigned int copied;

				        //! parent table name
				        const std::string table_name;
#endif

							};

						typedef std::list< data_item > cache_line;
				    typedef std::array< cache_line, HashSize > line_array;

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! Create a serial_group object
						serial_group(const QueryObject& q, typename linecache::cache<DataContainer, DataTag, QueryObject, HashSize>* p
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
							, const std::string& tn
#endif
						);

						//! Copy a serial_group object. We need to perform a deep copy, in the sense that
						//! individual data_items contain references back to their owning std::list<> object.
						//! Those references will be meaningless after the copy, and need to be reset.
						//! We also have to copy the query object, but its lifetime is managed with std::shared_ptr
						//! so this is easier.
						serial_group(const serial_group<DataContainer, DataTag, QueryObject, HashSize>& obj)
				      : parent_cache(obj.parent_cache),
				        query(obj.query),
				        cache(obj.cache)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
							, copied(obj.copied+1), table_name(obj.table_name)
#endif
							{
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
								if(copied >= 2) std::cerr << "WARNING - COPYING SERIAL_GROUP OBJECT, COUNT = " << copied << '\n';
#endif
						    for(unsigned int i = 0; i < HashSize; ++i)
							    {
								    for(typename cache_line::iterator t = cache[i].begin(); t != cache[i].end(); ++t)
									    {
								        (*t).reset_owner_list(&(cache[i]));
									    }
							    }
							}


						// ADMIN

						//! Reset parent cache
						void reset_parent_cache(linecache::cache<DataContainer, DataTag, QueryObject, HashSize>* c) { assert(c != nullptr); this->parent_cache = c; }

						//! Gather all data_items belonging to this group
						void gather_data_items(typename std::list< typename cache_line::iterator >& item_list);

						//! Check for equality of serial groups
						bool match(const QueryObject& q) { return(*(this->query) == q); }

						// CACHE LOOKUP

						//! Lookup data in the cache.
            //! Returns a reference, but client code *must* take a copy, *not* just link to the reference - the referenced data
            //! is not guaranteed to persist after a subsequent call to lookup_tag(), because it might be
            //! unloaded from the cache before that point.
						const DataContainer& lookup_tag(DataTag& tag);


						// INTERNAL DATA

				  protected:

						//! Parent 'cache' object
						linecache::cache<DataContainer, DataTag, QueryObject, HashSize>* parent_cache;

						//! QueryObject identifying this group; we take it to be a pointer so it can
						//! be polymorphic if required and use std::shared_ptr to manage its lifetime
						std::shared_ptr<QueryObject> query;

						//! Hash table of cached data lines.
						//! Each element in the has table is a std::list of data items.
						//! We use std::list so that iterators pointing to the cache elements
						//! are not invalidated by insertion or removal.
						//! This is very important in order that cache evictions can be
						//! carried out efficiently.
						line_array cache;

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
						//! Copy count
						unsigned int copied;

				    //! Name of parent table
				    const std::string table_name;
#endif

					};


		    // CACHE METHODS


		    template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
		    table<DataContainer, DataTag, QueryObject, HashSize>& cache<DataContainer, DataTag, QueryObject, HashSize>::get_table_handle(const std::string& name)
			    {
		        typename data_table_list::iterator t;

		        if((t = std::find(this->tables.begin(), this->tables.end(), name)) == this->tables.end())   // table doesn't already exist
			        {
		            this->tables.push_front( table<DataContainer, DataTag, QueryObject, HashSize>(name, this) );
		            t = this->tables.begin();
			        }

		        return(*t);
			    }


				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				void cache<DataContainer, DataTag, QueryObject, HashSize>::advise_size_increase(unsigned int bytes)
					{
						this->data_size += bytes;

						if(this->data_size > this->capacity)   // run evictions if we are now too large
							{
								this->eviction_timer.start();

								// build a list of data items on which to run clean-up
						    typename std::list< typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::cache_line::iterator > clean_up_list;

								for(typename data_table_list::iterator t = this->tables.begin(); t != this->tables.end(); ++t)
									{
								    (*t).gather_data_items(clean_up_list);
									}

								// set up a lambda to sort the list is data items into ascending least-recently used
						    struct DataItemSorter
							    {
						        bool operator()(const typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::cache_line::iterator& a,
						                        const typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::cache_line::iterator& b)
							        {
								        // older items go nearer the front of the queue
						            return (*a).get_last_access_time() > (*b).get_last_access_time();
							        }
							    };

								// sort queue
								clean_up_list.sort(DataItemSorter());

								// work through the queue, evicting items until we have saved sufficient space.
                // Because there is an overhead in building the clean-up list (and sorting it),
                // we evict more items than are necessary in order that repeated requests
                // for cache line of a similar size don't all result in clean_up operations
						    auto t = clean_up_list.begin();
                auto target_size = static_cast<size_t>(std::max(static_cast<int>(this->capacity) - 10*static_cast<int>(this->data_size), 0));
								while(this->data_size > target_size && t != clean_up_list.end())
									{
								    if(!(*(*t)).get_locked())   // can't evict this item if it is locked
									    {
								        std::list< typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::data_item >* list = (*(*t)).get_parent_list();

								        // reduce size of
								        this->data_size -= (*(*t)).get_size();

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
												std::ostringstream msg;
												msg << "@@ Cache table '" << (*(*t)).get_table_name() << "': unloaded cache line '" << (*(*t)).get_tag()->name() << "' of size " << format_memory((*(*t)).get_size()) << ", last accessed " << boost::posix_time::to_simple_string((*(*t)).get_last_access_time())
														<< ". Cache size now " << format_memory(this->data_size) << " (capacity " << format_memory(this->capacity) << ")";
												(*(*t)).get_tag()->log(msg.str());
#endif

										    // remove data item from the list which owns it
										    // this will destroy the data item, and the cache line it contains
										    list->erase(*t);

										    this->unload_counter++;
									    }
                    t++;
									}

								this->eviction_timer.stop();
							}
					}


				// TABLE METHODS


		    template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
		    table<DataContainer, DataTag, QueryObject, HashSize>::table(const std::string& fnam, cache<DataContainer, DataTag, QueryObject, HashSize>* p)
			    : filename(fnam), parent_cache(p)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
			    , copied(0)
#endif
			    {
				    assert(p != nullptr);
			    }


		    template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
		    serial_group<DataContainer, DataTag, QueryObject, HashSize>& table<DataContainer, DataTag, QueryObject, HashSize>::get_serial_handle(const QueryObject& q)
			    {
		        typename std::list< serial_group<DataContainer, DataTag, QueryObject, HashSize > >::iterator t;
				    for(t = this->groups.begin(); t != this->groups.end(); ++t)
					    {
						    if((*t).match(q)) break;
					    }

				    if(t == this->groups.end())
					    {
						    this->groups.push_front( serial_group<DataContainer, DataTag, QueryObject, HashSize>(q, this->parent_cache
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
							    , this->filename
#endif
						    ) );
						    t = this->groups.begin();
					    }

				    return(*t);
			    }


				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				void table<DataContainer, DataTag, QueryObject, HashSize>::gather_data_items(typename std::list< typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::cache_line::iterator >& item_list)
					{
						// work through all serial groups owned by this table, pushing their data_items into item_list
						for(typename serial_group_list::iterator t = this->groups.begin(); t != this->groups.end(); ++t)
							{
						    (*t).gather_data_items(item_list);
							}
					}


				// SERIAL GROUP METHODS


        template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
        serial_group<DataContainer, DataTag, QueryObject, HashSize>::serial_group(const QueryObject& q,
                                                                                  linecache::cache<DataContainer, DataTag, QueryObject, HashSize>* p
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
	        , const std::string& tn
#endif
        )
	        : query(q.clone()),
	          parent_cache(p)
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
	        , copied(0), table_name(tn)
#endif
	        {
            assert(p != nullptr);
	        }


				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				const DataContainer& serial_group<DataContainer, DataTag, QueryObject, HashSize>::lookup_tag(DataTag& tag)
					{
						unsigned int hash = tag.hash();

						assert(hash < HashSize);

				    typename std::list< serial_group<DataContainer, DataTag, QueryObject, HashSize>::data_item >::iterator t;
						if((t = std::find(this->cache[hash].begin(), this->cache[hash].end(), tag)) == this->cache[hash].end())     // data item doesn't already exist
							{
								DataContainer data;
						    tag.pull(*this->query, data);

								this->cache[hash].push_front( data_item(data, tag, &(this->cache[hash])
#ifdef CPPTRANSPORT_LINECACHE_DEBUG
									, this->table_name
#endif
								) );
								t = this->cache[hash].begin();

								// update size data - note that advising the cache of a size increase could lead to evictions,
								// but this cannot evict the data item we have just created because it is locked by
								// default -- until we explicitly unlock it below
								this->parent_cache->advise_size_increase((*t).get_size());

#ifdef CPPTRANSPORT_LINECACHE_DEBUG
								std::ostringstream msg;
								msg << "@@ Cache table '" << this->table_name << "': loaded cache line '" << tag.name() << "' of size " << format_memory((*t).get_size()) << ". Cache size now " << format_memory(this->parent_cache->get_size()) << " (capacity " << format_memory(this->parent_cache->get_capacity()) << ")";
								tag.log(msg.str());
#endif
							}
						else
							{
						    (*t).update_access_time();
								this->parent_cache->hit();
							}

						// all evictions have now taken place, so it is safe to unlock this data item
				    (*t).unlock();

						return((*t).get_data());
					}

				template <typename DataContainer, typename DataTag, typename QueryObject, unsigned int HashSize>
				void serial_group<DataContainer, DataTag, QueryObject, HashSize>::gather_data_items(typename std::list< typename std::list< typename serial_group<DataContainer, DataTag, QueryObject, HashSize>::data_item >::iterator >& item_list)
					{
						for(unsigned int i = 0; i < HashSize; ++i)
							{
								for(typename std::list<data_item>::iterator t = this->cache[i].begin(); t != this->cache[i].end(); ++t)
									{
										item_list.push_front(t);
									}
							}
					}


			}   // namespace linecache


	}   // namespace transport


#endif //CPPTRANSPORT_LINECACHE_H
