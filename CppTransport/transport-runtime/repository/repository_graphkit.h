//
// Created by David Seery on 22/04/2016.
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

#ifndef CPPTRANSPORT_REPOSITORY_GRAPHKIT_H
#define CPPTRANSPORT_REPOSITORY_GRAPHKIT_H


#include "transport-runtime/repository/repository.h"
#include "transport-runtime/repository/repository_cache.h"

#include "transport-runtime/manager/message_handlers.h"

#include "transport-runtime/utilities/match.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"

// use adjacency lists to represent graphs
#include "boost/graph/adjacency_list.hpp"

// use Floyd-Warshall algorithm from Boost::Graph to compute transitive closures
#include "boost/graph/floyd_warshall_shortest.hpp"

// need topological sort in order to get an order on object dependencies
#include "boost/graph/topological_sort.hpp"

// need graphviz for visualization
#include "boost/graph/graphviz.hpp"


namespace transport
  {

    namespace graphkit_graph
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


    // pull in graphkit_graph for this scope
    using namespace graphkit_graph;


    enum class repository_vertex
      {
        package, task, content_group, product
      };

    
    //! represent a map from vertex names to vertex values
    class repository_vertex_map
      {
        
        // ASSOCIATED TYPES
      
      protected:
        
        //! typedef representing map from names to vertices
        typedef std::map< std::string, std::pair< const unsigned int, const repository_vertex > > name_to_vertex_map;
        
        //! typedef representing inverse map from vertices to names
        typedef std::map< unsigned int, std::pair< const std::string, const repository_vertex > > vertex_to_name_map;

        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor is default
        repository_vertex_map()
          : M(std::make_shared<name_to_vertex_map>()),
            invM(std::make_shared<vertex_to_name_map>())
          {
          }
        
        //! destructor is default
        ~repository_vertex_map() = default;
        
        
        // INTERFACE
      
      public:
        
        //! reset all data
        void reset() { this->M->clear(); this->invM->clear(); }
        
        //! add a vertex to the map if it is not already present
        void insert(const std::string& vertex_name, repository_vertex type);
        
        //! overload [] to allow indexing of names to vertices
        unsigned int operator[](std::string vertex_name) const;
        
        //! overload [] to allow reverse indexing of vertices to names
        const std::string& operator[](unsigned int vertex) const;

        //! get vertex type from number
        repository_vertex get_type(unsigned int vertex) const;
        
        //! get number of vertices
        size_t size() const { return M->size(); }
        
        
        // INTERNAL DATA
      
      private:
        
        //! map database; ownership is shared among all copies of this vertex map
        std::shared_ptr<name_to_vertex_map> M;
        
        //! inverse map; ownership is likewise shared among all copies of this vertex map
        std::shared_ptr<vertex_to_name_map> invM;

      };
    
    
    void repository_vertex_map::insert(const std::string& vertex_name, repository_vertex type)
      {
        // search for a vertex with this name
        name_to_vertex_map::const_iterator t = this->M->find(vertex_name);

        if(t == this->M->end())
          {
            unsigned int vertex_number = static_cast<unsigned int>(this->M->size());
            this->M->insert( std::make_pair(vertex_name, std::make_pair(vertex_number, type)) );
            this->invM->insert( std::make_pair(vertex_number, std::make_pair(vertex_name, type)) );
          }
      }
    
    
    unsigned int repository_vertex_map::operator[](std::string vertex_name) const
      {
        name_to_vertex_map::const_iterator t = this->M->find(vertex_name);
        if(t == this->M->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex_name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
        return t->second.first;
      }
    
    
    const std::string& repository_vertex_map::operator[](unsigned int vertex) const
      {
        vertex_to_name_map::const_iterator t = this->invM->find(vertex);
        if(t == this->invM->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
        return t->second.first;
      }


    repository_vertex repository_vertex_map::get_type(unsigned int vertex) const
      {
        vertex_to_name_map::const_iterator t = this->invM->find(vertex);
        if(t == this->invM->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }
        return t->second.second;
      }

    
    //! represent a dependency graph, bundling a boost graph representation
    //! together with a vertex map
    class repository_dependency_graph
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor accepts a graph and a vertex map
        repository_dependency_graph(graph_type& g, repository_vertex_map m)
          : G(std::make_unique<graph_type>(g)),
            M(std::make_unique<repository_vertex_map>(m))
          {
          }
        
        //! destructor is default
        ~repository_dependency_graph() = default;


        // INDIRECTION

      public:

        //! applying * returns underlying vertex map
        repository_vertex_map& operator*() { return *this->M; }

        //! applying * returns underlying vertex map (const version)
        const repository_vertex_map& operator*() const { return *this->M; }

        //! get graph
        graph_type& get_graph() { return *this->G; }
        
        
        // SERVICES
      
      public:

        //! build topological ordering on graph
        std::unique_ptr< std::list<std::string> > compute_topological_order() const;

        //! write graphviz representation of the plot
        void write_graphviz(boost::filesystem::path& file);
        
        
        // INTERNAL DATA
      
      private:
        
        //! underlying graph
        std::unique_ptr<graph_type> G;
        
        //! distance matrix
        std::unique_ptr<repository_vertex_map> M;
        
      };


    namespace repository_dependency_graph_impl
      {

        class label_writer
          {

          public:

            label_writer(repository_vertex_map& m)
              : map(m)
              {
              }


            // INTERFACE

          public:

            //! write label
            void operator()(std::ostream& out, const unsigned int& v) const
              {
                out << "[label=" << boost::escape_dot_string(this->map[v]) << ",shape=box,style=filled,fillcolor=\"";

                switch(this->map.get_type(v))
                  {
                    case repository_vertex::package:
                      {
                        out << "#BFEBC6";
                        break;
                      }

                    case repository_vertex::task:
                      {
                        out << "#EBE8D3";
                        break;
                      }

                    case repository_vertex::content_group:
                      {
                        out << "#F3C5D7";
                        break;
                      }

                    case repository_vertex::product:
                      {
                        out << "#B6BDEB";
                        break;
                      }
                  }

                out << "\",fontname=\"Helvetica\",fontsize=12]";
              }


            // INTERNAL DATA

          private:

            const repository_vertex_map& map;

          };

      }   // namespace repository_dependency_graph_impl


    using namespace repository_dependency_graph_impl;


    void
    repository_dependency_graph::write_graphviz(boost::filesystem::path& file)
      {
        std::ofstream out(file.string(), std::ios::out | std::ios::trunc);
        
        if(out)
          {
            boost::write_graphviz(out, *this->G, label_writer(*this->M));
          }

        out.close();
      }


    std::unique_ptr< std::list<std::string> > repository_dependency_graph::compute_topological_order() const
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


    //! represent the distance matrix between any two products
    class repository_distance_matrix
      {

        // ASSOCIATED TYPES

      public:

        //! distance matrix type
        typedef std::vector< std::vector<unsigned int> > matrix_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor sets up an empty distance matrix but leaves M uninitialized;
        //! its values are filled in by using a suitable algorithm
        //! and this object as the matrix into which it writes its results
        repository_distance_matrix(graph_type& g, repository_vertex_map& m)
          : G(g, m),
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

        //! get dimension of distance matrix
        size_t size() const { if(this->D) { return this->D->size(); } else { return 0; } }

        //! overload [] to provide subscripting of distance matrix
        matrix_type::value_type& operator[](size_t n);

        //! get underlying graph
        const repository_dependency_graph& get_graph() const { return(this->G); }


        // SERVICES

      public:

        //! determine a list of objects (vertices) which depend on a given named object (vertex)
        std::unique_ptr< std::list<std::string> > find_dependent_objects(const std::string& name) const;

        //! determine a list of objects (vertices) on which a given named object (vertex) depends
        std::unique_ptr< std::list<std::string> > find_dependencies(const std::string& name) const;


        // INTERNAL DATA

      private:

        //! underlying graph
        repository_dependency_graph G;

        //! matrix
        std::unique_ptr<matrix_type> D;

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

        if(!this->D) return objects;

        const repository_vertex_map& map = *(this->G);
        unsigned int vertex = map[name];

        for(unsigned int i = 0; i < this->D->size(); ++i)
          {
            // get distance *from* this object (vertex) *to* some other object (i)
            // following directed arrows in the graph
            // (arrows are not bidirectional; this is an inclusion relationship)
            unsigned int dist = (*this->D)[vertex][i];
            if(i != vertex && dist < std::numeric_limits<unsigned int>::max())
              {
                objects->push_back(map[i]);
              }
          }

        return objects;
      }


    std::unique_ptr< std::list<std::string> > repository_distance_matrix::find_dependencies(const std::string& name) const
      {
        std::unique_ptr< std::list<std::string> > objects = std::make_unique< std::list<std::string> >();

        if(!this->D) return objects;

        const repository_vertex_map& map = *(this->G);
        unsigned int vertex = map[name];

        for(unsigned int i = 0; i < this->D->size(); ++i)
          {
            // get distance *from* some other group (i) *to* this object (vertex)
            // following directed arrows in the graph
            unsigned int dist = (*this->D)[i][vertex];
            if(i != vertex && dist < std::numeric_limits<unsigned int>::max())
              {
                objects->push_back(map[i]);
              }
          }

        return objects;
      }

    
    //! repository_graphkit provides graph algorithms on repository records
    template <typename number>
    class repository_graphkit
      {
        
        // CONSTRUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor
        repository_graphkit(repository<number>& r, error_handler eh, warning_handler wh, message_handler mh)
          : repo(r),
            cache(r),
            err(eh),
            warn(wh),
            msg(mh)
          {
          }
        
        //! destructor is default
        ~repository_graphkit() = default;


        // BASIC FUNCTIONS

      public:

        //! reset repository cache
        void reset() { this->cache.reset(); }


        // DISTANCE MATRICES -- INTERFACE
  
      public:
    
        //! construct distance matrix for the task graph
        std::unique_ptr<repository_distance_matrix> task_distance_matrix();
    
        //! construct distance matrix for the content group graph
        std::unique_ptr<repository_distance_matrix> content_group_distance_matrix();

        
        // DEPENDENCY GRAPHS -- INTERFACE
        
      public:

        //! construct dependency graph for a list of content groups
        std::unique_ptr<repository_dependency_graph> content_group_dependency(const std::list<std::string>& groups);

        //! convenience function to construct dependency graph for a piece of content
        //! which depends on a list of content groups
        std::unique_ptr<repository_dependency_graph> derived_content_dependency(const std::string& name, const std::list<std::string>& groups);

        
        // DEPENDENCY GRAPHS -- INTERNAL API
        
      protected:
    
        //! trace the dependencies of an individual content group into a dependency graph
        void follow_content_dependency(graph_type& G, repository_vertex_map& vmap, const std::string& name);
    
        //! trace the dependencies of a task into a dependency graph
        void follow_task_dependency(graph_type& G, repository_vertex_map& vmap, const std::string& name);
    
    
    
        // INTERNAL DATA
  
      private:
    
        //! reference to repository object
        repository<number>& repo;
    
        //! repository cache, used to avoid costly multiple enumerations
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
    std::unique_ptr<repository_distance_matrix> repository_graphkit<number>::task_distance_matrix()
      {
        typename task_db<number>::type& db = this->cache.get_task_db();
        
        // build graph representing tasks and their connexions
        repository_vertex_map vmap;
        graph_type G;
        
        // build directed graph representing the dependency chain among tasks
        for(const typename task_db<number>::type::value_type& item : db)
          {
            const task_record<number>& rec = *item.second;
            vmap.insert(rec.get_name(), repository_vertex::task);
            
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
                    vmap.insert(ptk.get_name(), repository_vertex::task);

                    boost::add_edge(vmap[ptk.get_name()], vmap[rec.get_name()], 1, G);
                    
                    break;
                  }
                
                case task_type::output:
                  {
                    const output_task_record<number>& orec = dynamic_cast< const output_task_record<number>& >(rec);
                    
                    // output tasks depend on derived products, each of which may depend on other tasks
                    const output_task<number>& tk = *orec.get_task();

                    const typename std::vector< output_task_element<number> > elements = tk.get_elements();
                    
                    for(const output_task_element<number>& elt : elements)
                      {
                        derived_data::derived_product<number>& product = elt.get_product();

                        // get list of tasks this product depends on
                        typename std::list< derivable_task<number>* > task_list;
                        product.get_task_list(task_list);
                        
                        for(derivable_task<number>* depend_tk : task_list)
                          {
                            vmap.insert(depend_tk->get_name(), repository_vertex::task);
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
        
        return(std::move(dmat));
      }
    
    
    template <typename number>
    std::unique_ptr<repository_distance_matrix> repository_graphkit<number>::content_group_distance_matrix()
      {
        // cache content databases
        integration_content_db& integration_content = this->cache.get_integration_content_db();
        postintegration_content_db& postintegration_content = this->cache.get_postintegration_content_db();
        output_content_db& output_content = this->cache.get_output_content_db();

        // build graph representing content groups and their connexions
        repository_vertex_map vmap;
        graph_type G;
        
        // build directed graph representing the dependency chain among content groups
        for(const integration_content_db::value_type& item : integration_content)
          {
            const content_group_record<integration_payload>& rec = *item.second;
            vmap.insert(rec.get_name(), repository_vertex::content_group);
          }
        
        for(const postintegration_content_db::value_type& item : postintegration_content)
          {
            const content_group_record<postintegration_payload>& rec = *item.second;
            vmap.insert(rec.get_name(), repository_vertex::content_group);
            
            // postintegration content will depend on the parent group, but possibly also a seed group
            const postintegration_payload& payload = rec.get_payload();
            vmap.insert(payload.get_parent_group(), repository_vertex::content_group);
            boost::add_edge(vmap[payload.get_parent_group()], vmap[rec.get_name()], 1, G);
            
            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group(), repository_vertex::content_group);
                boost::add_edge(vmap[payload.get_seed_group()], vmap[rec.get_name()], 1, G);
              }
          }
        
        for(const output_content_db::value_type& item : output_content)
          {
            const content_group_record<output_payload>& rec = *item.second;
            vmap.insert(rec.get_name(), repository_vertex::content_group);
            
            // postintegration content dependency is summarized in the payload
            const output_payload& payload = rec.get_payload();
            const std::list<std::string>& groups = payload.get_content_groups_summary();
            for(const std::string& group : groups)
              {
                vmap.insert(group, repository_vertex::content_group);
                boost::add_edge(vmap[group], vmap[rec.get_name()], 1, G);
              }
          }
        
        // initialize distance matrix with graph and vertex list
        std::unique_ptr<repository_distance_matrix> dmat = std::make_unique<repository_distance_matrix>(G, vmap);
        
        // run Floyd-Warshall algorithm
        boost::floyd_warshall_all_pairs_shortest_paths(G, *dmat);
        
        return(std::move(dmat));
      }
    
    
    template <typename number>
    std::unique_ptr<repository_dependency_graph> repository_graphkit<number>::content_group_dependency(const std::list<std::string>& groups)
      {
        // build graph representing content groups and their dependencies
        repository_vertex_map vmap;
        graph_type G;
        
        for(const std::string& group : groups)
          {
            this->follow_content_dependency(G, vmap, group);
          }
        
        std::unique_ptr<repository_dependency_graph> graph = std::make_unique<repository_dependency_graph>(G, vmap);
        return(std::move(graph));
      }
    
    
    template <typename number>
    void repository_graphkit<number>::follow_content_dependency(graph_type& G, repository_vertex_map& vmap, const std::string& name)
      {
        // find content group
        integration_content_db& integration_content = this->cache.get_integration_content_db();
        postintegration_content_db& postintegration_content = this->cache.get_postintegration_content_db();
        
        integration_content_db::const_iterator t = integration_content.find(name);
        if(t != integration_content.end())
          {
            const content_group_record<integration_payload>& rec = *t->second;
            vmap.insert(name, repository_vertex::content_group);
            
            // integration content groups depend on a possible seed group and the parent integration task
            const integration_payload& payload = rec.get_payload();
            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group(), repository_vertex::content_group);
                boost::add_edge(vmap[payload.get_seed_group()], vmap[name], 1, G);
                this->follow_content_dependency(G, vmap, payload.get_seed_group());
              }
            
            vmap.insert(rec.get_task_name(), repository_vertex::task);
            boost::add_edge(vmap[rec.get_task_name()], vmap[name], 1, G);
            this->follow_task_dependency(G, vmap, rec.get_task_name());
            
            return;
          }
        
        postintegration_content_db::const_iterator u = postintegration_content.find(name);
        if(u != postintegration_content.end())
          {
            const content_group_record<postintegration_payload>& rec = *u->second;
            vmap.insert(name, repository_vertex::content_group);
            
            // postintegration content groups depend on a possible seed group, the parent task, and a parent content group
            // we don't link directly to the parent task; that will be linked via its content group
            const postintegration_payload& payload = rec.get_payload();
            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group(), repository_vertex::content_group);
                boost::add_edge(vmap[payload.get_seed_group()], vmap[name], 1, G);
                this->follow_content_dependency(G, vmap, payload.get_seed_group());
              }
            
            vmap.insert(payload.get_parent_group(), repository_vertex::content_group);
            boost::add_edge(vmap[payload.get_parent_group()], vmap[name], 1, G);
            this->follow_content_dependency(G, vmap, payload.get_parent_group());
            
            vmap.insert(rec.get_task_name(), repository_vertex::task);
            boost::add_edge(vmap[rec.get_task_name()], vmap[name], 1, G);
            this->follow_task_dependency(G, vmap, rec.get_task_name());
            
            return;
          }
      }
    
    
    template <typename number>
    void repository_graphkit<number>::follow_task_dependency(graph_type& G, repository_vertex_map& vmap, const std::string& name)
      {
        // find task
        typename task_db<number>::type& db = this->cache.get_task_db();
        typename task_db<number>::type::const_iterator t = db.find(name);
        
        if(t != db.end())
          {
            const task_record<number>& rec = *t->second;
            switch(rec.get_type())
              {
                case task_type::integration:
                  {
                    const integration_task_record<number>& irec = dynamic_cast< const integration_task_record<number>& >(rec);
                    
                    // integration task depends only on initial conditions group
                    const std::string& ics_group = irec.get_task()->get_ics().get_name();
                    vmap.insert(ics_group, repository_vertex::package);
                    boost::add_edge(vmap[ics_group], vmap[name], 1 , G);
                    break;
                  }
                
                case task_type::postintegration:
                  {
                    const postintegration_task_record<number>& prec = dynamic_cast< const postintegration_task_record<number>& >(rec);
                    
                    // postintegration task depend on parent integration task
                    const std::string& parent = prec.get_task()->get_parent_task()->get_name();
                    vmap.insert(parent, repository_vertex::task);
                    boost::add_edge(vmap[parent], vmap[name], 1, G);
                    break;
                  }
                
                case task_type::output:
                  {
                    // shouldn't occur?
                    assert(false);
                  }
              }
          }
      }


    template <typename number>
    std::unique_ptr<repository_dependency_graph> repository_graphkit<number>::derived_content_dependency(const std::string& name, const std::list<std::string>& groups)
      {
        std::unique_ptr<repository_dependency_graph> graph = this->content_group_dependency(groups);

        graph_type& G = graph->get_graph();
        repository_vertex_map& vmap = *(*graph);

        vmap.insert(name, repository_vertex::product);
        for(const std::string& group : groups)
          {
            boost::add_edge(vmap[group], vmap[name], 1, G);
          }

        return std::move(graph);
      }
    
    
  }   // namespace transport



#endif //CPPTRANSPORT_REPOSITORY_GRAPHKIT_H
