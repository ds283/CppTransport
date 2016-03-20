//
// Created by David Seery on 28/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
        mgr.commit();
      }


    template <typename number>
    void repository_integration_writer_abort<number>::operator()(integration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.abort_integration_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_postintegration_writer_commit<number>::operator()(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.close_postintegration_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_postintegration_writer_abort<number>::operator()(postintegration_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.abort_postintegration_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_derived_content_writer_commit<number>::operator()(derived_content_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.close_derived_content_writer(writer, mgr);
        mgr.commit();
      }


    template <typename number>
    void repository_derived_content_writer_abort<number>::operator()(derived_content_writer<number>& writer)
      {
        transaction_manager mgr = this->repo.transaction_factory();
        this->repo.abort_derived_content_writer(writer, mgr);
        mgr.commit();
      }

  }   // namespace transport


#endif //CPPTRANSPORT_WRITER_REPO_IMPL_H
