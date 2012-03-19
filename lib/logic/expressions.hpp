/** @file "/owlcpp/lib/logic/expressions.hpp" 
part of owlcpp project.
@n @n Distributed under the Boost Software License, Version 1.0; see doc/license.txt.
@n Copyright Mikhail K Levin 2012
*******************************************************************************/
#ifndef EXPRESSIONS_HPP_
#define EXPRESSIONS_HPP_
#include <vector>
#include <memory>

#include "factpp/Kernel.hpp"

#include "owlcpp/rdf/triple_store.hpp"
#include "owlcpp/terms/node_tags_owl.hpp"
#include "owlcpp/terms/node_tags_system.hpp"
#include "owlcpp/logic/exception.hpp"
#include "expression_args.hpp"

namespace owlcpp{ namespace logic{ namespace factpp{

struct Obj_type {
   typedef TDLConceptExpression* fact_type;
};

struct Obj_prop {
   typedef TDLObjectRoleExpression* fact_type;
};

struct Obj_inst {
   typedef TDLObjectRoleExpression* fact_type;
};

struct Data_type {
   typedef TDLDataTypeExpression* fact_type;
};

struct Data_prop {
   typedef TDLDataRoleExpression* fact_type;
};

/**@brief 
*******************************************************************************/
template<class T> struct Fact_expression {
   struct Err : public Logic_err {};
   typedef Fact_expression self_t;
   typedef T expression_type;
   typedef std::auto_ptr<self_t> ptr_t;
   typedef typename expression_type::fact_type generated_t;
   virtual generated_t get(ReasoningKernel& k) const  = 0;
   virtual ~Fact_expression() {}
};

template<class T> typename Fact_expression<T>::ptr_t
make_fact_expression(Expression_args const& ea, Triple_store const& ts);

template<> typename Fact_expression<Obj_type>::ptr_t
make_fact_expression<Obj_type>(Expression_args const& ea, Triple_store const& ts);

template<> typename Fact_expression<Obj_prop>::ptr_t
make_fact_expression<Obj_prop>(Expression_args const& ea, Triple_store const& ts);

template<> typename Fact_expression<Data_type>::ptr_t
make_fact_expression<Data_type>(Expression_args const& ea, Triple_store const& ts);

template<> typename Fact_expression<Data_prop>::ptr_t
make_fact_expression<Data_prop>(Expression_args const& ea, Triple_store const& ts);

template<class T> inline typename Fact_expression<T>::ptr_t
make_fact_expression(const Node_id h, Triple_store const& ts) {
   return make_fact_expression<T>(Expression_args(h, ts), ts);
}

}//namespace factpp
}//namespace logic
}//namespace owlcpp
#endif /* EXPRESSIONS_HPP_ */
