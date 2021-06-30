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


    enum class repository_vertex_type
      {
        package, task, content_group, product
      };


    //! represent a map from vertex names to vertex values
    //! The Boost Graph Library interface is broken in the sense that there's no easy map from
    //! vertex indices to vertex_descriptor objects.
    //! To identify a vertex we have to cache the vertex descriptor and keep it available
    //! locally for comparison
    class repository_vertex_map
      {

        // ASSOCIATED TYPES

      protected:

        class vertex_record
          {
          public:
            vertex_record(unsigned int i, graph_type::vertex_descriptor d, std::string n, repository_vertex_type t)
              : index(i),
                descriptor(d),
                name(std::move(n)),
                type(t)
              {
              }

            ~vertex_record() = default;


            // VERTEX IDENTIFIERS

            //! unique numerical identifier for vertex
            unsigned int index;

            //! associated vertex descriptor
            graph_type::vertex_descriptor descriptor;

            //! vertex name (usually the task name or content group name or similar)
            std::string name;


            // METADATA

            //! vertex type: what sort of repository record does this vertex label?
            repository_vertex_type type;

          };

        //! typedef representing map from names to vertices
        using name_to_vertex_map = std::map< std::string, std::shared_ptr<vertex_record> >;

        //! typedef representing map from vertex descriptors to names
        using vertex_to_name_map = std::map< graph_type::vertex_descriptor, std::shared_ptr<vertex_record> >;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        repository_vertex_map()
          : name_map{std::make_shared<name_to_vertex_map>()},
            vertex_map{std::make_shared<vertex_to_name_map>()}
          {
          }

        //! destructor is default
        ~repository_vertex_map() = default;


        // INTERFACE

      public:

        //! reset all data
        void reset() { this->name_map->clear(); this->vertex_map->clear(); }

        //! add a named vertex to the map *if it is not already present*
        //! if the named vertex is already in the graph, no action is taken
        //! (the type property is not validated against the vertex already in the graph)
        void insert(const std::string& name, graph_type& graph, repository_vertex_type type);

        //! overload [] to allow indexing of names to vertices
        const graph_type::vertex_descriptor& operator[](const std::string& name) const;

        //! overload [] to allow reverse indexing of vertex descriptors to names
        const std::string& operator[](const graph_type::vertex_descriptor& vertex) const;

        //! get numeric vertex index from vertex descriptor
        unsigned int get_index(const graph_type::vertex_descriptor& vertex) const;

        //! get vertex type from vertex descriptor
        repository_vertex_type get_type(const graph_type::vertex_descriptor& vertex) const;

        //! get number of vertices
        size_t size() const { return name_map->size(); }


        // WRITE TO STREAM

      public:

        //! write to stream
        void write(std::ostream& out);


        // INTERNAL DATA

      private:

        //! map database; ownership is shared among all copies of this vertex map
        std::shared_ptr<name_to_vertex_map> name_map;

        //! inverse map; ownership is likewise shared among all copies of this vertex map
        std::shared_ptr<vertex_to_name_map> vertex_map;

      };


    void repository_vertex_map::write(std::ostream& out)
      {
        if(name_map)
          {
            for(const auto& v : *name_map)
              {
                const auto& record_ptr = v.second;
                const auto& record = *record_ptr;

                out << record.name << ": " << record.index << '\n';
              }
          }
      }


    void repository_vertex_map::insert(const std::string& name, graph_type& graph, repository_vertex_type type)
      {
        // search for a vertex with this name
        auto t = this->name_map->find(name);

        // if no such vertex already exists, insert it
        if(t == this->name_map->end())
          {
            auto descriptor = boost::add_vertex(graph);
            auto number = static_cast<unsigned int>(this->name_map->size());

            auto record = std::make_shared<vertex_record>(number, descriptor, name, type);

            this->name_map->insert(std::make_pair(name, record));
            this->vertex_map->insert(std::make_pair(descriptor, record));
          }
      }


    const graph_type::vertex_descriptor& repository_vertex_map::operator[](const std::string& vertex_name) const
      {
        auto t = this->name_map->find(vertex_name);
        if(t == this->name_map->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex_name << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        const vertex_record& record = *t->second;
        return record.descriptor;
      }


    const std::string& repository_vertex_map::operator[](const graph_type::vertex_descriptor& vertex) const
      {
        auto t = this->vertex_map->find(vertex);
        if(t == this->vertex_map->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        const vertex_record& record = *t->second;
        return record.name;
      }


    unsigned int repository_vertex_map::get_index(const graph_type::vertex_descriptor& vertex) const
      {
        auto t = this->vertex_map->find(vertex);
        if(t == this->vertex_map->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        const vertex_record& record = *t->second;
        return record.index;
      }


    repository_vertex_type repository_vertex_map::get_type(const graph_type::vertex_descriptor& vertex) const
      {
        auto t = this->vertex_map->find(vertex);
        if(t == this->vertex_map->end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_REPO_TOOLKIT_VERTEX_NOT_FOUND << " '" << vertex << "'";
            throw runtime_exception(exception_type::REPOSITORY_ERROR, msg.str());
          }

        const vertex_record& record = *t->second;
        return record.type;
      }


    //! represent a dependency graph, bundling a boost graph representation
    //! together with a vertex map
    class repository_dependency_graph
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor accepts a graph and a vertex map
        repository_dependency_graph(const graph_type& g, const repository_vertex_map& m)
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

        //! get graph (const version)
        graph_type& get_graph() { return *this->G; }

        //! get graph (const version)
        const graph_type& get_graph() const { return *this->G; }


        // SERVICES

      public:

        //! build topological ordering on graph
        std::unique_ptr<ordered_record_name_set> compute_topological_order() const;

        //! write graphviz representation of the plot
        void write_graphviz(boost::filesystem::path& file) const;


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

            explicit label_writer(repository_vertex_map& m)
              : map(m)
              {
              }


            // INTERFACE

          public:

            //! write label
            void operator()(std::ostream& out, const graph_type::vertex_descriptor& v) const
              {
                out << "[label=" << boost::escape_dot_string(this->map[v]) << R"(,shape=box,style="filled,rounded",fillcolor=")";

                switch(this->map.get_type(v))
                  {
                    case repository_vertex_type::package:
                      {
                        out << "#BFEBC6";
                        break;
                      }

                    case repository_vertex_type::task:
                      {
                        out << "#EBE8D3";
                        break;
                      }

                    case repository_vertex_type::content_group:
                      {
                        out << "#F3C5D7";
                        break;
                      }

                    case repository_vertex_type::product:
                      {
                        out << "#B6BDEB";
                        break;
                      }
                  }

                out << R"(",fontname="Helvetica",fontsize=12])";
              }


            // INTERNAL DATA

          private:

            const repository_vertex_map& map;

          };

      }   // namespace repository_dependency_graph_impl


    using namespace repository_dependency_graph_impl;


    void repository_dependency_graph::write_graphviz(boost::filesystem::path& file) const
      {
        std::ofstream out(file.string(), std::ios::out | std::ios::trunc);

        if(out)
          {
            boost::write_graphviz(out, *this->G, label_writer(*this->M));
          }

        out.close();
      }


    std::unique_ptr<ordered_record_name_set> repository_dependency_graph::compute_topological_order() const
      {
        auto items = std::make_unique<ordered_record_name_set>();

        if(!this->G || !this->M) return items;

        // build ordered list of vertices
        vertex_list list;
        boost::topological_sort(*this->G, std::back_inserter(list));

        // convert list of vertices (remember it is supplied in reverse order) to an ordered list of object names
        for(auto t = list.crbegin(); t != list.crend(); ++t)
          {
            items->push_back((*this->M)[*t]);
          }

        return items;
      }


    //! represents a 1-dimensional vector whose elements can be accessed using graph vertices
    template <typename Value, bool const_only=false>
    class vertex_indexed_vector
      {

        // ASSOCIATED TYPES

      public:

        //! value_type
        using value_type = Value;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vertex_indexed_vector(std::shared_ptr<repository_vertex_map> m, std::vector<Value>& r)
          : map(std::move(m)),
            vec(r)
          {
          }

        //! destructor is default
        ~vertex_indexed_vector() = default;


        // INTERFACE

      public:

        //! overload of [] allows access to vector contents using vertex descriptor;
        //! enabled only if const_only is false
        template <bool c=const_only, typename std::enable_if<!c>::type* = nullptr>
        value_type& operator[](const graph_type::vertex_descriptor& v)
          {
            return this->vec[this->map->get_index(v)];
          }

        //! overload of [] allows access to vector contents using vertex descriptor (const version);
        //! this version is always available
        const value_type& operator[](const graph_type::vertex_descriptor& v) const
          {
            return this->vec[this->map->get_index(v)];
          }


        // INTERNAL DATA

      protected:

        //! copy of repository vertex map
        std::shared_ptr<repository_vertex_map> map;

        //! reference to vector (usually a row or column of a parent matrix)
        std::vector<Value>& vec;

      };


    template <typename Value>
    class vertex_indexed_matrix
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        vertex_indexed_matrix(const repository_vertex_map& m)
          : map{std::make_shared<repository_vertex_map>(m)},
            matrix{std::make_unique< std::vector< std::vector<Value> > >()}
          {
            // resize matrix to be d * d square, where d is the number of vertices in repository_vertex_map
            matrix->resize(m.size());

            for(std::vector<Value>& row : *matrix)
              {
                row.resize(m.size());
              }
          }

        //! destructor is default
        ~vertex_indexed_matrix() = default;


        // ELEMENT ACCESS

      public:

        //! set value_type type to be proxy object
        using value_type = vertex_indexed_vector< Value, false >;
        using const_value_type = vertex_indexed_vector< Value, true >;


        //! overload first subscript to return a vector corresponding to a row of the original matrix;
        value_type operator[](const graph_type::vertex_descriptor& v)
          {
            return value_type(this->map, (*this->matrix)[this->map->get_index(v)]);
          }

        //! overload first subscript to return a vector corresponding to a row of the original matrix (const version)
        const_value_type operator[](const graph_type::vertex_descriptor& v) const
          {
            return const_value_type(this->map, (*this->matrix)[this->map->get_index(v)]);
          }

        //! size should return number of vertices, ie. size of each side of the matrix
        size_t size() const { return this->map->size(); }


        // INTERNAL DATA

      protected:

        //! shared pointer to repository vertex map (shared with any dynamically generated vector wrappers)
        std::shared_ptr<repository_vertex_map> map;

        //! matrix
        std::unique_ptr< std::vector< std::vector<Value> > > matrix;

      };


    //! represent the distance matrix between any two products
    class repository_distance_matrix
      {

        // ASSOCIATED TYPES

      public:

        //! distance matrix type
        typedef vertex_indexed_matrix<unsigned int> matrix_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor sets up an empty distance matrix but leaves M uninitialized;
        //! its values are filled in later using a suitable algorithm
        //! and this object as the matrix into which it writes its results
        repository_distance_matrix(graph_type& g, repository_vertex_map& m)
          : G(g, m),
            D(m)    // automatically assigns matrix dimensions to be correct size
          {
          }

        //! destructor is default
        ~repository_distance_matrix() = default;


        // INTERFACE

      public:

        //! get dimension of distance matrix
        size_t size() const { return this->D.size(); }

        //! overload [] to provide subscripting of distance matrix
        matrix_type::value_type operator[](const graph_type::vertex_descriptor& vertex);

        //! overload [] to provide subscripting of distance matrix (const version)
        matrix_type::const_value_type operator[](const graph_type::vertex_descriptor& vertex) const;

        //! get underlying graph
        const repository_dependency_graph& get_graph() const { return(this->G); }


        // SERVICES

      public:

        //! determine a list of objects (vertices) which depend on a given named object (vertex)
        std::unique_ptr<record_name_set> find_dependent_objects(const std::string& name) const;

        //! determine a list of objects (vertices) on which a given named object (vertex) depends
        std::unique_ptr<record_name_set> find_dependencies(const std::string& name) const;


        // INTERNAL DATA

      private:

        //! underlying graph
        const repository_dependency_graph G;

        //! matrix
        matrix_type D;

      };


    repository_distance_matrix::matrix_type::value_type
    repository_distance_matrix::operator[](const graph_type::vertex_descriptor& vertex)
      {
        return this->D[vertex];
      }


    repository_distance_matrix::matrix_type::const_value_type
    repository_distance_matrix::operator[](const graph_type::vertex_descriptor& vertex) const
      {
        return this->D[vertex];
      }


    std::unique_ptr<record_name_set>
    repository_distance_matrix::find_dependent_objects(const std::string& name) const
      {
        auto objects = std::make_unique<record_name_set>();

        const auto& map = *(this->G);
        const auto& graph = this->G.get_graph();
        const auto vertex = map[name];

        // run through available vertices that have this vertex as a destination
        const auto vertex_list = boost::vertices(graph);
        const auto no_link_value = std::numeric_limits<unsigned int>::max();
        for(auto v = vertex_list.first; v != vertex_list.second; ++v)
          {
            // get distance *from* this object (vertex) *to* some other object (i), following directed arrows in the graph
            // (arrows are not bidirectional; this is an inclusion relationship)
            const auto vtx = *v;
            unsigned int dist = this->D[vertex][vtx];

            // include if vertex is not ourselves and there is a link
            if(vtx != vertex && dist < no_link_value)
              {
                objects->insert(map[vtx]);
              }
          }

        return objects;
      }


    std::unique_ptr<record_name_set> repository_distance_matrix::find_dependencies(const std::string& name) const
      {
        auto objects = std::make_unique<record_name_set>();

        const auto& map = *(this->G);
        const auto& graph = this->G.get_graph();
        const auto vertex = map[name];

        // run through available vertices that have this vertex as a source
        const auto vertex_list = boost::vertices(graph);
        const auto no_link_value = std::numeric_limits<unsigned int>::max();
        for(auto v = vertex_list.first; v != vertex_list.second; ++v)
          {
            // get distance *from* some other group (i) *to* this object (vertex), following directed arrows in the graph
            const auto vtx = *v;
            unsigned int dist = this->D[vtx][vertex];

            // include if vertex is not ourselves and there is a link
            if(vtx != vertex && dist < no_link_value)
              {
                objects->insert(map[vtx]);
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
            err(eh),
            warn(wh),
            msg(mh)
          {
          }

        //! destructor is default
        ~repository_graphkit() = default;


        // DISTANCE MATRICES -- INTERFACE

      public:

        //! construct distance matrix for the task graph
        std::unique_ptr<repository_distance_matrix> task_distance_matrix();

        //! construct distance matrix for the content group graph
        std::unique_ptr<repository_distance_matrix> content_group_distance_matrix();


        // DEPENDENCY GRAPHS -- INTERFACE

      public:

        //! construct dependency graph for a list of content groups
        std::unique_ptr<repository_dependency_graph> content_group_dependency(const content_group_name_set& groups);

        //! convenience function to construct dependency graph for a piece of content
        //! which depends on a list of content groups
        std::unique_ptr<repository_dependency_graph> derived_content_dependency(const std::string& name, const content_group_name_set& groups);


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


        // MESSAGE HANDLERS

        //! error handler
        error_handler err;

        //! warning handler
        warning_handler warn;

        //! message handler
        message_handler msg;

      };


    template <typename number>
    std::unique_ptr<repository_distance_matrix>
    repository_graphkit<number>::task_distance_matrix()
      {
        auto db = this->repo.enumerate_tasks();

        // build graph representing tasks and their connexions
        repository_vertex_map vmap;
        graph_type G;

        // build directed graph representing the dependency chain among tasks
        for(auto& item : db)
          {
            const auto& rec = *item.second;
            vmap.insert(rec.get_name(), G, repository_vertex_type::task);

            switch(rec.get_type())
              {
                case task_type::integration:
                  {
                    const auto& irec = dynamic_cast< const integration_task_record<number>& >(rec);
                    break;
                  }

                case task_type::postintegration:
                  {
                    const auto& prec = dynamic_cast< const postintegration_task_record<number>& >(rec);

                    // postintegration tasks depend on their parent
                    const auto& tk = *prec.get_task();
                    const auto& ptk = *tk.get_parent_task();
                    vmap.insert(ptk.get_name(), G, repository_vertex_type::task);

                    boost::add_edge(vmap[ptk.get_name()], vmap[rec.get_name()], 1, G);

                    break;
                  }

                case task_type::output:
                  {
                    const auto& orec = dynamic_cast< const output_task_record<number>& >(rec);

                    // output tasks depend on derived products, each of which may depend on other tasks
                    const auto& tk = *orec.get_task();
                    const auto elements = tk.get_elements();

                    for(const auto& elt : elements)
                      {
                        auto& product = elt.get_product();

                        // get list of tasks this product depends on
                        auto task_list = product.get_task_dependencies();
                        for(const auto& e : task_list) // TODO: change to std::views::values in C++20 and remove use of .second
                          {
                            const auto& t = e.second.get_task();
                            vmap.insert(t.get_name(), G, repository_vertex_type::task);
                            boost::add_edge(vmap[t.get_name()], vmap[rec.get_name()], 1, G);
                          }
                      }

                    break;
                  }
              }
          }

        // initialize distance matrix with graph and vertex list
        auto dmat = std::make_unique<repository_distance_matrix>(G, vmap);

        // run Floyd-Warshall algorithm to find shortest path between each vertex pair in the graph
        boost::floyd_warshall_all_pairs_shortest_paths(G, *dmat);

        return dmat;
      }


    template <typename number>
    std::unique_ptr<repository_distance_matrix>
    repository_graphkit<number>::content_group_distance_matrix()
      {
        // cache content databases
        auto integration_content = this->repo.enumerate_integration_task_content();
        auto postintegration_content = this->repo.enumerate_postintegration_task_content();
        auto output_content = this->repo.enumerate_output_task_content();

        // build graph representing content groups and their connexions
        repository_vertex_map vmap;
        graph_type G;

        // build directed graph representing the dependency chain among content groups
        for(const auto& item : integration_content)
          {
            const auto& rec = *item.second;
            vmap.insert(rec.get_name(), G, repository_vertex_type::content_group);
          }

        for(const auto& item : postintegration_content)
          {
            const auto& rec = *item.second;
            vmap.insert(rec.get_name(), G, repository_vertex_type::content_group);

            // postintegration content will depend on the parent group, but possibly also a seed group
            const auto& payload = rec.get_payload();
            vmap.insert(payload.get_parent_group(), G, repository_vertex_type::content_group);
            boost::add_edge(vmap[payload.get_parent_group()], vmap[rec.get_name()], 1, G);

            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group(), G, repository_vertex_type::content_group);
                boost::add_edge(vmap[payload.get_seed_group()], vmap[rec.get_name()], 1, G);
              }
          }

        for(const auto& item : output_content)
          {
            const auto& rec = *item.second;
            vmap.insert(rec.get_name(), G, repository_vertex_type::content_group);

            // postintegration content dependency is summarized in the payload
            const auto& payload = rec.get_payload();
            const auto& groups = payload.get_content_groups_summary();
            for(const auto& group : groups)
              {
                vmap.insert(group, G, repository_vertex_type::content_group);
                boost::add_edge(vmap[group], vmap[rec.get_name()], 1, G);
              }
          }

        // initialize distance matrix with graph and vertex list
        auto dmat = std::make_unique<repository_distance_matrix>(G, vmap);

        // run Floyd-Warshall algorithm to find shortest path between each vertex pair in the graph
        boost::floyd_warshall_all_pairs_shortest_paths(G, *dmat);

        return dmat;
      }


    template <typename number>
    std::unique_ptr<repository_dependency_graph>
    repository_graphkit<number>::content_group_dependency(const content_group_name_set& groups)
      {
        // build graph representing content groups and their dependencies
        repository_vertex_map vmap;
        graph_type G;

        for(const auto& group : groups)
          {
            this->follow_content_dependency(G, vmap, group);
          }

        auto graph = std::make_unique<repository_dependency_graph>(G, vmap);
        return graph;
      }


    template <typename number>
    void
    repository_graphkit<number>::follow_content_dependency(graph_type& G, repository_vertex_map& vmap, const std::string& name)
      {
        // find content group
        auto integration_content = this->repo.enumerate_integration_task_content();
        auto postintegration_content = this->repo.enumerate_postintegration_task_content();

        auto t = integration_content.find(name);
        if(t != integration_content.end())
          {
            const auto& rec = *t->second;
            vmap.insert(name, G, repository_vertex_type::content_group);

            // integration content groups depend on a possible seed group and the parent integration task
            const auto& payload = rec.get_payload();
            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group(), G, repository_vertex_type::content_group);
                boost::add_edge(vmap[payload.get_seed_group()], vmap[name], 1, G);
                this->follow_content_dependency(G, vmap, payload.get_seed_group());
              }

            vmap.insert(rec.get_task_name(), G, repository_vertex_type::task);
            boost::add_edge(vmap[rec.get_task_name()], vmap[name], 1, G);
            this->follow_task_dependency(G, vmap, rec.get_task_name());

            return;
          }

        auto u = postintegration_content.find(name);
        if(u != postintegration_content.end())
          {
            const auto& rec = *u->second;
            vmap.insert(name, G, repository_vertex_type::content_group);

            // postintegration content groups depend on a possible seed group, the parent task, and a parent content group
            // we don't link directly to the parent task; that will be linked via its content group
            const auto& payload = rec.get_payload();
            if(payload.is_seeded())
              {
                vmap.insert(payload.get_seed_group(), G, repository_vertex_type::content_group);
                boost::add_edge(vmap[payload.get_seed_group()], vmap[name], 1, G);
                this->follow_content_dependency(G, vmap, payload.get_seed_group());
              }

            vmap.insert(payload.get_parent_group(), G, repository_vertex_type::content_group);
            boost::add_edge(vmap[payload.get_parent_group()], vmap[name], 1, G);
            this->follow_content_dependency(G, vmap, payload.get_parent_group());

            vmap.insert(rec.get_task_name(), G, repository_vertex_type::task);
            boost::add_edge(vmap[rec.get_task_name()], vmap[name], 1, G);
            this->follow_task_dependency(G, vmap, rec.get_task_name());

            return;
          }
      }


    template <typename number>
    void
    repository_graphkit<number>::follow_task_dependency(graph_type& G, repository_vertex_map& vmap, const std::string& name)
      {
        // find task
        auto db = this->repo.enumerate_tasks();
        auto t = db.find(name);

        if(t != db.end())
          {
            const auto& rec = *(t->second);
            switch(rec.get_type())
              {
                case task_type::integration:
                  {
                    const auto& irec = dynamic_cast< const integration_task_record<number>& >(rec);

                    // integration task depends only on initial conditions group
                    const auto& ics_group = irec.get_task()->get_ics().get_name();
                    vmap.insert(ics_group, G, repository_vertex_type::package);
                    boost::add_edge(vmap[ics_group], vmap[name], 1 , G);
                    break;
                  }

                case task_type::postintegration:
                  {
                    const auto& prec = dynamic_cast< const postintegration_task_record<number>& >(rec);

                    // postintegration task depend on parent integration task
                    const auto& parent = prec.get_task()->get_parent_task()->get_name();
                    vmap.insert(parent, G, repository_vertex_type::task);
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
    std::unique_ptr<repository_dependency_graph>
    repository_graphkit<number>::derived_content_dependency(const std::string& name, const content_group_name_set& groups)
      {
        auto graph = this->content_group_dependency(groups);

        auto& G = graph->get_graph();
        auto& vmap = *(*graph);

        vmap.insert(name, G, repository_vertex_type::product);
        for(const auto& group : groups)
          {
            boost::add_edge(vmap[group], vmap[name], 1, G);
          }

        return graph;
      }


  }   // namespace transport



#endif //CPPTRANSPORT_REPOSITORY_GRAPHKIT_H
