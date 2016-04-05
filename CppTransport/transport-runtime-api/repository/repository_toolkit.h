//
// Created by David Seery on 03/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_TOOLKIT_H
#define CPPTRANSPORT_REPOSITORY_TOOLKIT_H


#include "transport-runtime-api/repository/repository.h"
#include "transport-runtime-api/repository/repository_cache.h"

#include "transport-runtime-api/manager/message_handlers.h"

#include "transport-runtime-api/utilities/match.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

// use adjacency lists to represent graphs
#include "boost/graph/adjacency_list.hpp"

// use Floyd-Warshall algorithm from Boost::Graph to compute transitive closures
#include "boost/graph/floyd_warshall_shortest.hpp"

// need topological sort in order to get an order on object dependencies
#include "boost/graph/topological_sort.hpp"


namespace transport
  {

    namespace repository_toolkit_impl
      {

        //! template method to extract a content group
        template <typename number, typename Payload>
        std::unique_ptr< output_group_record<Payload> > get_rw_content_group(repository<number>& repo, const std::string& name, transaction_manager& mgr);


        //! specialize for integration payloads
        template <>
        std::unique_ptr< output_group_record< integration_payload> > get_rw_content_group(repository<double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_integration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< integration_payload> > get_rw_content_group(repository<float>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_integration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< integration_payload> > get_rw_content_group(repository<long double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_integration_content(name, mgr);
          }


        //! specialize for postintegration payloads
        template <>
        std::unique_ptr< output_group_record< postintegration_payload> > get_rw_content_group(repository<double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_postintegration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< postintegration_payload> > get_rw_content_group(repository<float>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_postintegration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< postintegration_payload> > get_rw_content_group(repository<long double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_postintegration_content(name, mgr);
          }


        //! specialize for output payloads
        template <>
        std::unique_ptr< output_group_record< output_payload> > get_rw_content_group(repository<double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_output_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< output_payload> > get_rw_content_group(repository<float>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_output_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< output_payload> > get_rw_content_group(repository<long double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_output_content(name, mgr);
          }


        //! template method to delete a content group
        template <typename number, typename Payload>
        void erase_repository_record(repository<number>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr);


        // specialize for integration payloads
        template <>
        void erase_repository_record<double, integration_payload>(repository<double>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_integration_content(name, task_name, mgr);
          }

        template <>
        void erase_repository_record<float, integration_payload>(repository<float>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_integration_content(name, task_name, mgr);
          }

        template <>
        void erase_repository_record<long double, integration_payload>(repository<long double>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_integration_content(name, task_name, mgr);
          }


        // specialize for postintegration payloads
        template <>
        void erase_repository_record<double, postintegration_payload>(repository<double>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_postintegration_content(name, task_name, mgr);
          }

        template <>
        void erase_repository_record<float, postintegration_payload>(repository<float>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_postintegration_content(name, task_name, mgr);
          }

        template <>
        void erase_repository_record<long double, postintegration_payload>(repository<long double>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_postintegration_content(name, task_name, mgr);
          }


        // specialize for output payloads
        template <>
        void erase_repository_record<double, output_payload>(repository<double>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_output_content(name, task_name, mgr);
          }

        template <>
        void erase_repository_record<float, output_payload>(repository<float>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_output_content(name, task_name, mgr);
          }

        template <>
        void erase_repository_record<long double, output_payload>(repository<long double>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
          {
            repo.delete_output_content(name, task_name, mgr);
          }

      }   // namespace repository_toolkit_impl


    // pull in repository_toolkit_impl for this scope
    using namespace repository_toolkit_impl;


    namespace repository_toolkit_graph
      {

        //! weight for graph edges
        typedef unsigned int edge_weight;

        //! convert to a edge property
        //! since the Boost::Graph Floyd-Warshall algorithm expects the graph to have weighted edges (in its default incarnation),
        //! we adopt the simple strategy of adding edge weights and fixing them all to be unity.
        //! An alternative would be to pass a fictitious weight map to Floyd-Warshall which returns unity for all edges.
        typedef boost::property<boost::edge_weight_t, edge_weight> WeightProperty;

        //! typedef for graph; edges stored as a set to avoid duplicates;
        //! points stored as vectors; edges are directed
        typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS, boost::no_property, WeightProperty> graph_type;

        typedef boost::graph_traits<graph_type>::vertex_descriptor vertex_type;

        typedef std::vector<vertex_type> vertex_list;

        typedef boost::property_map<graph_type, boost::vertex_index_t>::type index_map_type;

      }

    // pull in repository_toolkit for this scope
    using namespace repository_toolkit_graph;


    //! represent a map from vertex names to vertex values
    class repository_vertex_map
      {

        // ASSOCIATED TYPES

      protected:

        //! typedef representing map from names to vertices
        typedef std::map< std::string, unsigned int > map_type;

        //! typedef representing inverse map from vertices to names
        typedef std::map< unsigned int, const std::string > inverse_map_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        repository_vertex_map()
          : M(std::make_shared<map_type>()),
            invM(std::make_shared<inverse_map_type>())
          {
          }

        //! destructor is default
        ~repository_vertex_map() = default;


        // INTERFACE

      public:

        //! reset all data
        void reset() { this->M->clear(); this->invM->clear(); }

        //! add a vertex to the map if it is not already present
        void insert(const std::string& vertex_name);

        //! overload [] to allow indexing of names to vertices
        unsigned int operator[](std::string vertex_name) const;

        //! overload [] to allow reverse indexing of vertices to names
        const std::string& operator[](unsigned int vertex) const;

        //! get number of vertices
        size_t size() const { return M->size(); }


        // INTERNAL DATA

      private:

        //! map database; ownership is shared among all copies of this vertex map
        std::shared_ptr<map_type> M;

        //! inverse map; ownership is likewise shared among all copies of this vertex map
        std::shared_ptr<inverse_map_type> invM;

      };


    void repository_vertex_map::insert(const std::string& vertex_name)
      {
        map_type::const_iterator t = this->M->find(vertex_name);
        if(t == this->M->end())
          {
            unsigned int vertex_number = static_cast<unsigned int>(this->M->size());
            this->M->insert( std::make_pair(vertex_name, vertex_number) );
            this->invM->insert( std::make_pair(vertex_number, vertex_name));
          }
      }


    unsigned int repository_vertex_map::operator[](std::string vertex_name) const
      {
        map_type::const_iterator t = this->M->find(vertex_name);
        if(t == this->M->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex_name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
        return t->second;
      }


    const std::string& repository_vertex_map::operator[](unsigned int vertex) const
      {
        inverse_map_type::const_iterator t = this->invM->find(vertex);
        if(t == this->invM->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
        return t->second;
      }


    //! represent the distance matrix between any two products
    class repository_distance_matrix
      {

        // ASSOCIATED TYPES

      public:

        //! distance matrix type
        typedef std::vector< std::vector<unsigned int> > matrix_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor sets up an empty distance matrix but leaves M uninitialized
        repository_distance_matrix(graph_type& g, repository_vertex_map m)
          : G(std::make_unique<graph_type>(g)),
            M(std::make_unique<repository_vertex_map>(m)),
            D(std::make_unique<matrix_type>(m.size()))
          {
            // reassign distance matrix to be of correct size
            unsigned int vertices = m.size();
            for(unsigned int i = 0; i < vertices; ++i)
              {
                (*this->D)[i].resize(vertices);
              }
          }

        //! destructor is default
        ~repository_distance_matrix() = default;


        // INTERFACE

      public:

        //! reset all data
        void reset() { this->D->clear(); this->M.reset(); }

        //! get dimension of distance matrix
        size_t size() const { if(this->D) { return this->D->size(); } else { return 0; } }

        //! overload [] to provide subscripting of distance matrix
        matrix_type::value_type& operator[](size_t n);


        // SERVICES

      public:

        //! determine a list of objects (vertices) which depend on a given named object (vertex)
        std::unique_ptr< std::list<std::string> > find_dependent_objects(const std::string& name) const;

        //! determine a list of objects (vertices) on which a given named object (vertex) depends
        std::unique_ptr< std::list<std::string> > find_dependencies(const std::string& name) const;

        //! build topological ordering on graph
        std::unique_ptr< std::list<std::string> > compute_topological_order() const;


        // INTERNAL DATA

      private:

        //! underlying graph
        std::unique_ptr<graph_type> G;

        //! distance matrix
        std::unique_ptr<matrix_type> D;

        //! mapping from record names to vertex values
        std::unique_ptr<repository_vertex_map> M;

      };


    repository_distance_matrix::matrix_type::value_type& repository_distance_matrix::operator[](size_t n)
      {
        if(!this->D) throw runtime_exception(exception_type::REPOSITORY_ERROR, CPPTRANSPORT_REPO_TOOLKIT_D_NOT_INITIALIZED);

        if(n >= this->D->size())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_D_OUT_OF_RANGE << " " << n;
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        return (*this->D)[n];
      }


    std::unique_ptr< std::list<std::string> > repository_distance_matrix::find_dependent_objects(const std::string& name) const
      {
        std::unique_ptr< std::list<std::string> > objects = std::make_unique< std::list<std::string> >();

        if(!this->D || !this->M) return objects;

        unsigned int vertex = (*this->M)[name];
        for(unsigned int i = 0; i < this->D->size(); ++i)
          {
            // get distance *from* this object (vertex) *to* some other object (i)
            // following directed arrows in the graph
            // (arrows are not bidirectional; this is an inclusion relationship)
            unsigned int dist = (*this->D)[vertex][i];
            if(i != vertex && dist < std::numeric_limits<unsigned int>::max())
              {
                objects->push_back((*this->M)[i]);
              }
          }

        return objects;
      }


    std::unique_ptr< std::list<std::string> > repository_distance_matrix::find_dependencies(const std::string& name) const
      {
        std::unique_ptr< std::list<std::string> > objects = std::make_unique< std::list<std::string> >();

        if(!this->D || !this->M) return objects;

        unsigned int vertex = (*this->M)[name];
        for(unsigned int i = 0; i < this->D->size(); ++i)
          {
            // get distance *from* some other group (i) *to* this object (vertex)
            // following directed arrows in the graph
            unsigned int dist = (*this->D)[i][vertex];
            if(i != vertex && dist < std::numeric_limits<unsigned int>::max())
              {
                objects->push_back((*this->M)[i]);
              }
          }

        return objects;
      }


    std::unique_ptr< std::list<std::string> > repository_distance_matrix::compute_topological_order() const
      {
        std::unique_ptr< std::list<std::string> > objects = std::make_unique< std::list<std::string> >();

        if(!this->G || !this->M) return objects;

        // build ordered list of vertices
        vertex_list list;
        boost::topological_sort(*this->G, std::back_inserter(list));

        index_map_type index = boost::get(boost::vertex_index, *this->G);

        // convert list of vertices (remember it is supplied in reverse order) to an ordered list of object names
        for(vertex_list::const_reverse_iterator t = list.crbegin(); t != list.crend(); ++t)
          {
            objects->push_back((*this->M)[index(*t)]);
          }

        return objects;
      }


    //! repository_toolkit provides tools to manipulate repository records,
    //! or otherwise perform useful services which are not part of the core
    //! repository API
    template <typename number>
    class repository_toolkit
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        repository_toolkit(repository<number>& r, error_handler eh, warning_handler wh, message_handler mh)
          : repo(r),
            cache(r),
            err(eh),
            warn(wh),
            msg(mh)
          {
          }

        //! destructor
        ~repository_toolkit() = default;


        // REPOSITORY RECORDS -- INTERFACE

      public:

        //! update tags and notes
        void update_tags_notes(const std::vector<std::string>& objects,
                               const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                               const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove);

        //! remove content groups
        void delete_content(const std::vector<std::string>& objects);


        // REPOSITORY RECORDS -- INTERNAL API

      protected:

        //! iterate over a content database, updating tags and notes
        template <typename ContentDatabase>
        void update_tags_notes(ContentDatabase& db, const std::vector<std::string>& objects,
                               const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                               const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove);

        //! iterate over a content database, deleting content groups
        template <typename ContentDatabase>
        void delete_content(ContentDatabase& db, std::map< std::string, bool >& items,
                            std::unique_ptr<repository_distance_matrix>& dmat, integration_content_db& integration_content,
                            postintegration_content_db& postintegration_content, output_content_db& output_content);


        // REPOSITORY OBJECT MODEL -- INTERFACE

      public:

        //! construct distance matrix for the task graph
        std::unique_ptr<repository_distance_matrix> task_distance_matrix();

        //! construct distance matrix for the content group graph
        std::unique_ptr<repository_distance_matrix> content_group_distance_matrix();


        // REPOSITORY OBJECT MODEL -- INTERNAL API

      protected:

        //! construct distance matrix for the content group graph
        //! use specified databases to build the dependency graph
        std::unique_ptr<repository_distance_matrix> content_group_distance_matrix(integration_content_db& integration_db,
                                                                                  postintegration_content_db& postintegration_db,
                                                                                  output_content_db& output_db);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

        //! repository cache, used to avoid constly multiple enumerations
        repository_cache<number> cache;


        // MESSAGE HANDLERS

        //! error handler
        error_handler err;

        //! warning handler
        warning_handler warn;

        //! message handler
        message_handler msg;

      };


    template <typename number>
    void repository_toolkit<number>::update_tags_notes(const std::vector<std::string>& objects,
                                                       const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                                                       const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove)
      {
        // only output group records carry tags and notes
        integration_content_db& integration_content = this->cache.get_integration_content_db();
        postintegration_content_db& postintegration_content = this->cache.get_postintegration_content_db();
        output_content_db& output_content = this->cache.get_output_content_db();

        this->update_tags_notes(integration_content, objects, tags_add, tags_remove, notes_add, notes_remove);
        this->update_tags_notes(postintegration_content, objects, tags_add, tags_remove, notes_add, notes_remove);
        this->update_tags_notes(output_content, objects, tags_add, tags_remove, notes_add, notes_remove);
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::update_tags_notes(ContentDatabase& db, const std::vector<std::string>& objects,
                                                       const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                                                       const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(const typename ContentDatabase::value_type& item : db)
          {
            // ContentDatabase has mapped_type equal to std::unique_ptr< output_group_record<PayloadType> >
            // to get the output_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(const std::string& match_expr : objects)
              {
                if(check_match(record.get_name(), match_expr), true)    // true = insist on exact match
                  {
                    // re-query the database to get a read/write version of this record
                    typename ContentDatabase::mapped_type rw_record = get_rw_content_group<number, typename ContentDatabase::mapped_type::element_type::payload_type>(this->repo, record.get_name(), mgr);

                    for(const std::string& tag : tags_remove)
                      {
                        rw_record->remove_tag(tag);
                      }

                    for(const std::string& tag : tags_add)
                      {
                        rw_record->add_tag(tag);
                      }

                    for(const std::string& note : notes_remove)
                      {
                        rw_record->remove_note(boost::lexical_cast<unsigned int>(note));
                      }

                    for(const std::string& note : notes_add)
                      {
                        rw_record->add_note(note);
                      }

                    // recommit record
                    // in the current implementation there is no need to re-read the value in the enumerated database
                    // (the record it contains is now out of date) because each record in the enumeration is inspected only once

                    // However, if multiple passes were ever implemented, the record in the enumeration would have to
                    // be modified
                    rw_record->commit();
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


    template <typename number>
    std::unique_ptr<repository_distance_matrix> repository_toolkit<number>::task_distance_matrix()
      {
        typename task_db<number>::type& db = this->cache.get_task_db();

        // build graph representing tasks and their connexions
        repository_vertex_map vmap;
        graph_type G;

        // build directed graph representing the dependency chain among tasks
        for(const typename task_db<number>::type::value_type& item : db)
          {
            const task_record<number>& rec = *item.second;
            vmap.insert(rec.get_name());

            switch(rec.get_type())
              {
                case task_type::integration:
                  {
                    const integration_task_record<number>& irec = dynamic_cast< const integration_task_record<number>& >(rec);
                    break;
                  }

                case task_type::postintegration:
                  {
                    const postintegration_task_record<number>& prec = dynamic_cast< const postintegration_task_record<number>& >(rec);

                    // postintegration tasks depend on their parent
                    const postintegration_task<number>& tk = *prec.get_task();
                    const derivable_task<number>& ptk = *tk.get_parent_task();
                    vmap.insert(ptk.get_name());

//                    std::cout << "Postintegration task '" << rec.get_name() << "' depends on derivable task '" << ptk.get_name() << "'" << '\n';
                    boost::add_edge(vmap[ptk.get_name()], vmap[rec.get_name()], 1, G);

                    break;
                  }

                case task_type::output:
                  {
                    const output_task_record<number>& orec = dynamic_cast< const output_task_record<number>& >(rec);

                    // output tasks depend on derived products, each of which may depend on other tasks
                    const output_task<number>& tk = *orec.get_task();
//                    std::cout << "Output task '" << rec.get_name() << "' depends on derived products:" << '\n';

                    const typename std::vector< output_task_element<number> > elements = tk.get_elements();

                    for(const output_task_element<number>& elt : elements)
                      {
                        derived_data::derived_product<number>& product = elt.get_product();
//                        std::cout << "  -- Derived product '" << product.get_name() << "' depends on tasks:" << '\n';

                        // get list of tasks this product depends on
                        typename std::list< derivable_task<number>* > task_list;
                        product.get_task_list(task_list);

                        for(derivable_task<number>* depend_tk : task_list)
                          {
//                            std::cout << "     ++ '" << depend_tk->get_name() << "'" << '\n';
                            vmap.insert(depend_tk->get_name());
                            boost::add_edge(vmap[depend_tk->get_name()], vmap[rec.get_name()], 1, G);
                          }
                      }

                    break;
                  }
              }
          }

        // initialize distance matrix with graph and vertex list
        std::unique_ptr<repository_distance_matrix> dmat = std::make_unique<repository_distance_matrix>(G, vmap);

        // run Floyd-Warshall algorithm
        boost::floyd_warshall_all_pairs_shortest_paths(G, *dmat);

        return(dmat);
      }


    template <typename number>
    std::unique_ptr<repository_distance_matrix> repository_toolkit<number>::content_group_distance_matrix()
      {
        // cache content databases
        integration_content_db& integration_content = this->cache.get_integration_content_db();
        postintegration_content_db& postintegration_content = this->cache.get_postintegration_content_db();
        output_content_db& output_content = this->cache.get_output_content_db();

        return this->content_group_distance_matrix(integration_content, postintegration_content, output_content);
      }



    template <typename number>
    std::unique_ptr<repository_distance_matrix> repository_toolkit<number>::content_group_distance_matrix(integration_content_db& integration_content,
                                                                                                          postintegration_content_db& postintegration_content,
                                                                                                          output_content_db& output_content)
      {
        // build graph representing output groups and their connexions
        repository_vertex_map vmap;
        graph_type G;

        // build directed graph representing the dependency chain among output groups
        for(const integration_content_db::value_type& item : integration_content)
          {
            const output_group_record<integration_payload>& rec = *item.second;
            vmap.insert(rec.get_name());
          }

        for(const postintegration_content_db::value_type& item : postintegration_content)
          {
            const output_group_record<postintegration_payload>& rec = *item.second;
            vmap.insert(rec.get_name());

            // postintegration content will depend on the parent group, but possibly also a seed group
            const postintegration_payload& payload = rec.get_payload();
            vmap.insert(payload.get_parent_group());
            boost::add_edge(vmap[payload.get_parent_group()], vmap[rec.get_name()], 1, G);

            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group());
                boost::add_edge(vmap[payload.get_seed_group()], vmap[rec.get_name()], 1, G);
              }
          }

        for(const output_content_db::value_type& item : output_content)
          {
            const output_group_record<output_payload>& rec = *item.second;
            vmap.insert(rec.get_name());

            // postintegration content dependency is summarized in the payload
            const output_payload& payload = rec.get_payload();
            const std::list<std::string>& groups = payload.get_content_groups_summary();
            for(const std::string& group : groups)
              {
                vmap.insert(group);
                boost::add_edge(vmap[group], vmap[rec.get_name()], 1, G);
              }
          }

        // initialize distance matrix with graph and vertex list
        std::unique_ptr<repository_distance_matrix> dmat = std::make_unique<repository_distance_matrix>(G, vmap);

        // run Floyd-Warshall algorithm
        boost::floyd_warshall_all_pairs_shortest_paths(G, *dmat);

        return(dmat);
      }


    template <typename number>
    void repository_toolkit<number>::delete_content(const std::vector<std::string>& objects)
      {
        // make local copy of objects to process; we will tag items in this list as 'processed' as we go
        std::map<std::string, bool> items;
        for(const std::string& item : objects)
          {
            items[item] = false;
          }

        // cache distance matrix for the content group graph
        std::unique_ptr<repository_distance_matrix> dmat = this->content_group_distance_matrix();

        // cache content databases
        integration_content_db& integration_content = this->cache.get_integration_content_db();
        postintegration_content_db& postintegration_content = this->cache.get_postintegration_content_db();
        output_content_db& output_content = this->cache.get_output_content_db();

        this->delete_content(output_content, items, dmat, integration_content, postintegration_content, output_content);
        this->delete_content(postintegration_content, items, dmat, integration_content, postintegration_content, output_content);
        this->delete_content(integration_content, items, dmat, integration_content, postintegration_content, output_content);

        for(const std::pair< const std::string, bool >& item : items)
          {
            if(!item.second)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT << " '" << item.first << "'";
                this->warn(msg.str());
              }
          }
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::delete_content(ContentDatabase& db, std::map< std::string, bool >& items,
                                                    std::unique_ptr<repository_distance_matrix>& dmat,
                                                    integration_content_db& integration_content,
                                                    postintegration_content_db& postintegration_content,
                                                    output_content_db& output_content)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(typename ContentDatabase::const_iterator t = db.begin(); t != db.end(); /* intentionally empty increment field */)
          {
            const typename ContentDatabase::value_type& item = *t;
            // ContentDatabase has mapped_type equal to std::unique_ptr< output_group_record<PayloadType> >
            // to get the output_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(std::pair< const std::string, bool >& item : items)
              {
                if(check_match(record.get_name(), item.first, true))    // true = insist on exact match
                  {
                    item.second = true;   // mark as a match for this item
                    std::unique_ptr< std::list<std::string> > dependent_groups = dmat->find_dependent_objects(
                      record.get_name());

                    if(dependent_groups)
                      {
                        if(dependent_groups->size() == 0)
                          {
                            erase_repository_record<number, typename ContentDatabase::mapped_type::element_type::payload_type>(this->repo, record.get_name(), record.get_task_name(),mgr);

                            // erase record and reset iterator to point to following element
                            t = db.erase(t);

                            // delete this record from the repository enumeration, and then recompute the distance matrix
                            std::unique_ptr<repository_distance_matrix> new_dmat =
                              this->content_group_distance_matrix(integration_content, postintegration_content, output_content);
                            dmat.swap(new_dmat);
                          }
                        else
                          {
                            std::ostringstream msg;
                            msg << CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_A << " '" << record.get_name() << "', "
                                << CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_B << " ";

                            unsigned int count = 0;
                            for(const std::string& group : *dependent_groups)
                              {
                                if(count > 0) msg << ", ";
                                msg << group;
                                ++count;
                              }

                            this->err(msg.str());
                            ++t;
                          }
                      }
                  }
                else // no match, just increment iterator and move on
                  {
                    ++t;
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


}   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_TOOLKIT_H

