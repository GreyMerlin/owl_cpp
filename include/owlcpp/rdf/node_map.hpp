/** @file "/owlcpp/include/owlcpp/rdf/node_map.hpp" 
part of owlcpp project.
@n @n Distributed under the Boost Software License, Version 1.0; see doc/license.txt.
@n Copyright Mikhail K Levin 2012
*******************************************************************************/
#ifndef NODE_MAP_HPP_
#define NODE_MAP_HPP_
#include "boost/assert.hpp"
#include "boost/foreach.hpp"
#include "boost/multi_index_container.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/global_fun.hpp"
#include "boost/unordered_map.hpp"
#include "boost/range.hpp"

#include "owlcpp/node.hpp"
#include "owlcpp/doc_id.hpp"
#include "owlcpp/terms/iri_tags.hpp"
#include "owlcpp/terms/node_tags_system.hpp"
#include "owlcpp/node_id.hpp"
#include "owlcpp/rdf/config.hpp"
#include "owlcpp/rdf/node_iri_map.hpp"
#include "owlcpp/exception.hpp"
#include "owlcpp/detail/id_tracker.hpp"

namespace owlcpp{

/**@brief Store RDF nodes
@details
Validity of node IDs is assumed and asserted in debug mode.
Calling node map methods with invalid node IDs results in undefined behavior.
*******************************************************************************/
class OWLCPP_RDF_DECL Node_map : public Node_iri_map {

   typedef boost::unordered_map<Node_id,Node_id> datatypes_t;
   typedef datatypes_t::const_iterator datatype_iter_t;

   typedef boost::unordered_map<Node_id,const std::string> langs_t;
   typedef langs_t::const_iterator lang_iter_t;

   typedef boost::unordered_map<Node_id,Doc_id> docs_t;
   typedef docs_t::const_iterator doc_iter_t;

public:
   explicit Node_map(const Node_id id0) : Node_iri_map(id0) {}

   /**@brief Find the datatype of the literal node
    @param id literal node's ID
    @return pointer to the datatype node's ID for the literal node or
    NULL if the node is not literal or no datatype is defined for it.
   */
   Node_id const* datatype(const Node_id id) const {
      const datatype_iter_t i = dtypes_.find(id);
      if( i == dtypes_.end() ) return 0;
      return &i->second;
   }

   /**@brief Find the language of the literal node
    @param id literal node's ID
    @return language tag string for the literal node or
    "" if the node is not literal or no language is defined for it.
    @details The tag string format MAY be according to RFC 5646
    (http://tools.ietf.org/html/rfc5646).
    This is not, however, currently enforced by the library.
   */
   std::string language(const Node_id id) const {
      const lang_iter_t i = langs_.find(id);
      if( i == langs_.end() ) return "";
      return i->second;
   }

   /**@brief Insert blank node
    @param did document ID
    @param name blank node name (MUST be unique within the document)
    @return node ID
   */
   Node_id insert_blank(const Doc_id did, std::string const& name) {
      node_index_t const& n_index = nodes_.get<node_tag>();
      const Node node(terms::N_blank::id(), name);
      const node_iter_t n_iter = n_index.find(node);
      //TODO use find()
      if( n_iter != n_index.end() ) {
         const Node_id id = n_iter->first;
         BOOST_ASSERT(docs_.find(id) != docs_.end());
         if( docs_[id] == did ) return id;
      }
      const Node_id id = insert(terms::N_blank::id(), name);
      docs_.insert(std::make_pair(id, did));
      return id;
   }

   /**@brief Insert literal node
    @param value
    @param datatype
    @param lang language tag string for the literal node or
    "" if the language is not defined.
    The tag string format SHOULD be according to RFC 5646
    (http://tools.ietf.org/html/rfc5646).
    This is not, however, currently enforced by the library.
    @return node ID
   */
   Node_id insert_literal(
            std::string const& value,
            const Node_id datatype = terms::T_empty_::id(),
            std::string const& lang = ""
   ) {
      const Node node(terms::N_empty::id(), value);
      node_index_t const& n_index = nodes_.get<node_tag>();
      const node_iter_t n_iter = n_index.find(node);
      if( n_iter == n_index.end() ) return insert_literal_private(node, dt, lang);
      const Node_id id = n_iter->first;
      Node_id const* dtp = datatype(id);
      Node_id dt0 = dtp ? *dtp : terms::T_empty_::id();
      if( dt != dt0 ) return insert_literal_private(node, dt, lang);
      if( lang != language(id) ) return insert_literal_private(node, dt, lang);
      return id;
   }

   /**
    @param id ID of a blank node
    @return ID of the document to which the blank node belongs
    @throw Err if @b id is not for a blank node
   */
   Doc_id blank_node_doc(const Node_id id) const {
      //BOOST_ASSERT(docs_.find(id) != docs_.end());
      const doc_iter_t i = docs_.find(id);
      if( i == docs_.end() ) BOOST_THROW_EXCEPTION(
               Err()
               << Err::msg_t("document unknown")
               << Err::int1_t(id())
      );
      return i->second;
   }

private:
   docs_t docs_;
   datatypes_t dtypes_;
   langs_t langs_;

   Node_id insert_literal_private(
            Node const& node,
            const Node_id dt,
            std::string const& lang
   ) {
      const Node_id id = tracker_.get();
      insert(id, node);
      if( dt != terms::T_empty_::id() ) dtypes_.insert(std::make_pair(id, dt));
      if( ! lang.empty() ) langs_.insert(std::make_pair(id, lang));
      return id;
   }

};

}//namespace owlcpp
#endif /* NODE_MAP_HPP_ */
