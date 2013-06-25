//
// Created by David Seery on 21/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __symbol_table_H_
#define __symbol_table_H_

#include <iostream>

#include "quantity.h"

#include "boost/functional/hash.hpp"


// symbol table class for an template type 'container'
// 'container' must provide a get_name() method, which is used for hashing

template <class container>
class symbol_table
  {
    public:
      symbol_table(unsigned int s);
      ~symbol_table();

      void print (std::ostream& stream);                        // output contents

      bool insert(container* obj);                              // insert *copy* of an object into the table, if it does not already exist
      bool find  (std::string const name, container*& obj);     // find an object by name

    private:
      boost::hash<std::string> hash_object;

      std::deque<container>*	 table;
      unsigned int             size;
  };

//  IMPLEMENTATION

template <class container>
symbol_table<container>::symbol_table(unsigned int s)
  : size(s)
  {
    table = new std::deque<container>[size];
  }


template <class container>
symbol_table<container>::~symbol_table()
  {
    delete[] this->table;
  }

template <class container>
void symbol_table<container>::print(std::ostream& stream)
  {
    stream << "Symbol table size = " << this->size << "\n";

    for(int i = 0; i < this->size; i++)
      {
        if(this->table[i].size() > 0)
          {
            stream << "Hash table entry " << i << " is not empty:\n";
            for(typename std::deque<container>::iterator ptr = this->table[i].begin();
                ptr != this->table[i].end(); ptr++)
              {
                (*ptr).print(stream);
              }
            stream << "\n";
          }
      }
  }


template <class container>
bool symbol_table<container>::find(std::string const name, container*& obj)
  {
    // search for existing identifier associated with 'name'
    size_t h = this->hash_object(name) % this->size;

    bool found = false;

    if(this->table[h].size() > 0)
      {
        typename std::deque<container>::iterator ptr;

        for(ptr = this->table[h].begin(); found == false && ptr != this->table[h].end(); ptr++)
          {
            if((*ptr).get_name() == name)
              {
                found = true;
                break;
              }
          }

        if(found)
          {
            obj = &(*ptr);
          }
      }

    return(found);
  }


template <class container>
bool symbol_table<container>::insert(container* obj)
  {
    container* p;
    bool       exists = this->find(obj->get_name(), p);

    if(exists == false)
      {
        size_t h = this->hash_object(obj->get_name()) % this->size;

        this->table[h].push_front(*obj);      // takes a copy of the container object; need to be careful about GiNaC::symbols contained within
      }

    return(!exists);
  }


#endif //__symbol_table_H_
