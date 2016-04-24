//
// Created by David Seery on 28/01/2016.
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

#ifndef CPPTRANSPORT_WRITER_REPO_IMPL_H
#define CPPTRANSPORT_WRITER_REPO_IMPL_H


namespace transport
  {

    template <typename number>
    void repository_integration_writer_commit<number>::operator()(integration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.close_integration_writer(writer, mgr);
        this->repo.deregister_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_integration_writer_abort<number>::operator()(integration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.abort_integration_writer(writer, mgr);
        this->repo.deregister_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_postintegration_writer_commit<number>::operator()(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.close_postintegration_writer(writer, mgr);
        this->repo.deregister_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_postintegration_writer_abort<number>::operator()(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.abort_postintegration_writer(writer, mgr);
        this->repo.deregister_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_derived_content_writer_commit<number>::operator()(derived_content_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.close_derived_content_writer(writer, mgr);
        this->repo.deregister_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_derived_content_writer_abort<number>::operator()(derived_content_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.abort_derived_content_writer(writer, mgr);
        this->repo.deregister_writer(writer, mgr);
        mgr.commit();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_WRITER_REPO_IMPL_H
