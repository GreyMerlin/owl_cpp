/** @file "/owlcpp/binding/python/node_store_api.hpp"
part of owlcpp project.
@n @n Distributed under the Boost Software License, Version 1.0; see doc/license.txt.
@n Copyright Mikhail K Levin 2013
*******************************************************************************/
#ifndef NODE_STORE_API_HPP_
#define NODE_STORE_API_HPP_
#include "boost/python.hpp"
namespace bp = boost::python;

#include "owlcpp/node_id.hpp"
#include "owlcpp/doc_id.hpp"
#include "owlcpp/ns_id.hpp"
#include "owlcpp/rdf/node.hpp"
#include "owlcpp/rdf/doc_meta.hpp"
#include "owlcpp/rdf/ns_iri.hpp"
using owlcpp::Node_id;
using owlcpp::Doc_id;
using owlcpp::Doc_meta;
using owlcpp::Ns_iri;
using owlcpp::Ns_id;

namespace owlcpp{ namespace py{

template<class Wrapped>
Doc_id insert_doc(
         Wrapped& ts,
         std::string const& iri,
         std::string const& path,
         std::string const& vers
         ) {
   return ts.insert_doc(iri, path, vers).first;
}

template<class Wrapped> Node_id insert_node_iri(
         Wrapped& ts,
         std::string const& str
) {
   return ts.insert_node_iri(str);
}

/**@brief 
*******************************************************************************/
template<class BP_Class> inline BP_Class& export_node_store_api(BP_Class& bpc) {
   typedef typename BP_Class::wrapped_type wrapped_type;
   typedef typename wrapped_type::node_type node_type;
   bpc
      .def(
               "map_ns",
               &wrapped_type::map_ns,
               bp::return_internal_reference<>(),
               "map of namespace IRIs"
      )

      .def(
               "map_node",
               &wrapped_type::map_node,
               bp::return_internal_reference<>(),
               "map of RDF nodes"
      )

      .def(
               "map_doc",
               &wrapped_type::map_doc,
               bp::return_internal_reference<>(),
               "map of ontology documents"
      )

      .def(
               "__getitem__",
               static_cast<
                  Ns_iri const&
                  (wrapped_type::*)(Ns_id const) const
               >(&wrapped_type::at),
               bp::return_internal_reference<>(),
               (bp::arg("ns_id")),
               "retrieve namespace IRI"
      )

      .def(
               "__getitem__",
               static_cast<
               node_type const&
                  (wrapped_type::*)(Node_id const) const
               >(&wrapped_type::at),
               bp::return_internal_reference<>(),
               (bp::arg("node_id")),
               "retrieve RDF node"
      )

      .def(
               "__getitem__",
               static_cast<
                  Doc_meta const&
                  (wrapped_type::*)(Doc_id const) const
               >(&wrapped_type::at),
               bp::return_internal_reference<>(),
               (bp::arg("doc_id")),
               "retrieve ontology document info"
      )

      .def(
               "insert",
               static_cast<
                  Ns_id (wrapped_type::*)(Ns_iri const&)
               >(&wrapped_type::insert),
               (bp::arg("ns_iri")),
               "insert namespace IRI"
      )

      .def(
               "insert_node_iri",
               &insert_node_iri<wrapped_type>,
               (bp::arg("iri")),
               "insert node IRI"
      )

      .def(
               "insert_doc",
               &insert_doc<wrapped_type>,
               (bp::arg("iri"), bp::arg("path")="", bp::arg("vers")=""),
               "insert ontology document information"
      )

      .def(
               "find_doc_iri",
               &wrapped_type::find_doc_iri,
               (bp::arg("iri")),
               "find documents with specified ontology IRI"
      )

      .def(
               "find_doc_version",
               &wrapped_type::find_doc_version,
               (bp::arg("iri")),
               "find documents with specified version IRI"
      )
      ;

   return bpc;
}

}//namespace py
}//namespace owlcpp
#endif /* NODE_STORE_API_HPP_ */
