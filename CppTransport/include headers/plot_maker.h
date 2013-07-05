//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __plot_maker_H_
#define __plot_maker_H_

#include <iostream>
#include <fstream>

#include <vector>
#include <string>


template <typename number>
class plot_maker
  {
    public:
      virtual void plot(std::string output, std::string title,
        const std::vector<number>& x, const std::vector< std::vector<number> >& ys, const std::vector<std::string>& labels,
        std::string xlabel, std::string ylabel, bool logx, bool logy) = 0;
  };


inline void open_tempfile(std::string& path)
  {
    path += "/XXXXXX";
    std::vector<char> dst_path(path.begin(), path.end());
    dst_path.push_back('\0');

    int fd = mkstemp(&dst_path[0]);
    if(fd != -1)
      {
        path.assign(dst_path.begin(), dst_path.end() - 1);

        std::ofstream f;
        f.open(path.c_str(), std::ios_base::trunc | std::ios_base::out);
        f.close();
      }
  }


#endif //__plot_maker_H_
