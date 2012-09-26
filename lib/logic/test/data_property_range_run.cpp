/** @file "/owlcpp/lib/logic/test/data_property_range_run.cpp" 
part of owlcpp project.
@n @n Distributed under the Boost Software License, Version 1.0; see doc/license.txt.
@n Copyright Mikhail K Levin 2012
*******************************************************************************/
#define BOOST_TEST_MODULE data_property_range_run
#include "boost/test/unit_test.hpp"
#include "test/exception_fixture.hpp"
#include "test/sample_data.hpp"
#include "owlcpp/rdf/triple_store.hpp"
#include "owlcpp/io/input.hpp"
#include "owlcpp/logic/triple_to_fact.hpp"
#include "factpp/Kernel.hpp"

namespace owlcpp{ namespace test{

BOOST_GLOBAL_FIXTURE( Exception_fixture );

/**
*******************************************************************************/
BOOST_AUTO_TEST_CASE( case01 ) {
   Triple_store ts(Node_map_std::get(Nodes_owl()));
   load_file(sample_file_path("data_property_range.owl"), ts);
   ReasoningKernel k;
   submit_triples(ts, k, false);
   BOOST_CHECK( k.isKBConsistent() );
}

}//namespace test
}//namespace owlcpp
