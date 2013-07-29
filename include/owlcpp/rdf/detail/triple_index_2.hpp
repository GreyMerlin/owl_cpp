/** @file "/owlcpp/include/owlcpp/rdf/detail/triple_index_2.hpp"
part of owlcpp2 project.
@n @n Distributed under the Boost Software License, Version 1.0; see doc/license.txt.
@n Copyright Mikhail K Levin 2013
*******************************************************************************/
#ifndef TRIPLE_INDEX_2_HPP_
#define TRIPLE_INDEX_2_HPP_
#include <functional>
#include "boost/foreach.hpp"
#include "boost/iterator/iterator_facade.hpp"
#include "boost/iterator/filter_iterator.hpp"
#include "boost/iterator/transform_iterator.hpp"
#include "boost/mpl/assert.hpp"
#include "boost/mpl/at.hpp"
#include "boost/mpl/contains.hpp"
#include "boost/mpl/remove.hpp"
#include "boost/mpl/equal.hpp"
#include "boost/mpl/sort.hpp"
#include "owlcpp/rdf/exception.hpp"
#include "owlcpp/rdf/detail/convert_fragment.hpp"
#include "owlcpp/rdf/detail/fragment_set.hpp"

namespace owlcpp{ namespace map_triple_detail{

/**@brief Invoke appropriate search algorithm
@tparam Tag0 numerical tag indicating which triple element is indexed first
@tparam Tag1 numerical tag indicating which triple element is indexed second
@tparam Tag2 numerical tag indicating which triple element is indexed third
@tparam Tag3 numerical tag indicating which triple element is indexed fourth
@tparam Q0 type of first-indexed query element
@tparam Q1 type of second-indexed query element
@tparam Q2 type of third-indexed query element
@tparam Q3 type of fourth-indexed query element
*******************************************************************************/
template<
   template<class,class> class Map,
   class Tag0, class Tag1, class Tag2, class Tag3,
   class Q0, class Q1, class Q2, class Q3
> class Triple_find_dispatch;

/**@brief
*******************************************************************************/
template<
   class Conv, /**< convert fragment to triple */
   class Iter /**< iterator over fragments */
> class Triple_from_fragment_iterator {
   typedef typename Conv::el0 el0;

   class Convert : public std::unary_function<typename Conv::fragment, Triple> {
   public:
      Convert() {}
      explicit Convert(const el0 id) : id_(id) {}
      Triple operator()(typename Conv::fragment const& f) const {
         return Conv::get_triple(id_, f);
      }
   private:
      el0 id_;
   };

   struct Iterator
            : public boost::transform_iterator<Convert,Iter,Triple,Triple> {

      Iterator() {}

      Iterator(Iter const& i, const el0 id)
      : boost::transform_iterator<Convert,Iter,Triple,Triple>(i, Convert(id))
        {}
   };

public:
   typedef Iterator type;
};

/**@brief
*******************************************************************************/
template<
   class Converter,
   class Fs_Iter,
   class Q1, class Q2, class Q3
> class Triple_merge_iterator
         : public boost::iterator_facade<
              Triple_merge_iterator<Converter, Fs_Iter, Q1, Q2, Q3>,
              Triple,
              boost::forward_traversal_tag,
              Triple
           > {
   typedef typename Converter::el0 el0;
   typedef typename Converter::el1 el1;
   typedef typename Converter::el2 el2;
   typedef typename Converter::el3 el3;
   typedef Fragment_set<el1, el2, el3> fragment_set;
   typedef typename fragment_set::template result<Q1,Q2,Q3> fragment_search;
   typedef typename fragment_search::iterator f_iter;
   typedef typename fragment_search::range f_range;
   typedef typename Triple_from_fragment_iterator<Converter,f_iter>::type t_iter;
   typedef boost::iterator_range<t_iter> t_range;

public:
   Triple_merge_iterator(const Fs_Iter begin, const Fs_Iter end,
            Q1 const& q1, Q2 const& q2, Q3 const& q3)
   : begin_(begin),
     end_(end),
     q1_(q1),
     q2_(q2),
     q3_(q3),
     tr_(get_fragment_range())
     {
      ensure_end_or_match();
     }

private:
   Fs_Iter begin_;
   Fs_Iter end_;
   Q1 q1_;
   Q2 q2_;
   Q3 q3_;
   t_range tr_;

   friend class boost::iterator_core_access;

   void increment() {
      tr_.advance_begin(1);
      ensure_end_or_match();
   }

   bool equal(Triple_merge_iterator const& i) const {
      return begin_ == i.begin_ && tr_.begin() == i.tr_.begin();
   }

   Triple dereference() const {
      return tr_.front();
   }

   void ensure_end_or_match() {
      while( ! tr_ && (begin_ != end_) ) {
         ++begin_;
         tr_ = get_fragment_range();
      }
   }

   t_range get_fragment_range() {
      if( begin_ == end_ ) return t_range();
      const f_range r = begin_->second.find(q1_,q2_,q3_);
      const el0 id = begin_->first;
      return t_range(
               t_iter(r.begin(), id),
               t_iter(r.end(), id)
      );
   }
};

/**@brief
*******************************************************************************/
template<
   template<class,class> class Map,
   class Tag0, class Tag1, class Tag2, class Tag3,
   class Q0, class Q1, class Q2, class Q3
> class Triple_find_dispatch {
   typedef Convert_fragment<Tag0,Tag1,Tag2,Tag3> convert;
   typedef typename convert::el0 el0;
   typedef typename convert::el1 el1;
   typedef typename convert::el2 el2;
   typedef typename convert::el3 el3;
   typedef Fragment_set<el1, el2, el3> fragment_set;
   typedef Map<el0, fragment_set> storage;
   typedef typename storage::template result<Q0>::iterator fs_iter;
   typedef typename storage::template result<Q0>::range fs_range;

public:
   typedef Triple_merge_iterator<convert, fs_iter, Q1, Q2, Q3> iterator;
   typedef boost::iterator_range<iterator> range;

   static range find(
            storage const& v,
            Q0 const& q0,
            Q1 const& q1,
            Q2 const& q2,
            Q3 const& q3
   ) {
      const fs_range r = v.find(q0);
      return range(
               iterator(r.begin(), r.end(), q1,q2,q3),
               iterator(r.end(), r.end(), q1,q2,q3)
      );
   }
};

/**@brief Specialize to search within single set
*******************************************************************************/
template<
   template<class,class> class Map,
   class Tag0, class Tag1, class Tag2, class Tag3,
   class Q1, class Q2, class Q3
> class Triple_find_dispatch<
   Map,
   Tag0,Tag1,Tag2,Tag3,
   typename boost::mpl::at<Triple, Tag0>::type,Q1,Q2,Q3
> {
   typedef typename boost::mpl::at<Triple, Tag0>::type Q0;
   typedef Convert_fragment<Tag0,Tag1,Tag2,Tag3> convert;
   typedef typename convert::el0 el0;
   typedef typename convert::el1 el1;
   typedef typename convert::el2 el2;
   typedef typename convert::el3 el3;
   typedef Fragment_set<el1, el2, el3> fragment_set;
   typedef Map<el0, fragment_set> storage;
   typedef typename fragment_set::template result<Q1,Q2,Q3> fs_result;
   typedef typename fs_result::iterator fragment_iter;
   typedef typename fs_result::range fragment_range;

public:
   typedef typename Triple_from_fragment_iterator<convert,fragment_iter>::type iterator;
   typedef boost::iterator_range<iterator> range;

   static range find(
            storage const& v,
            Q0 const& q0,
            Q1 const& q1,
            Q2 const& q2,
            Q3 const& q3
   ) {
      fragment_range fr = v[q0].find(q1,q2,q3);
      return range(
               iterator(boost::begin(fr), q0),
               iterator(boost::end(fr), q0)
      );
   }
};

/**@brief 
*******************************************************************************/
template<
   template<class,class> class Map,
   class Tag0,
   class Tag1,
   class Tag2,
   class Tag3
> class Triple_index {
public:
   typedef boost::mpl::vector4<Tag0,Tag1,Tag2,Tag3> sort_order;
private:
   BOOST_MPL_ASSERT((
            boost::mpl::equal<typename boost::mpl::sort<sort_order>::type, triple_tags>
   ));
   typedef Convert_fragment<Tag0,Tag1,Tag2,Tag3> converter;
   typedef typename converter::el0 el0;
   typedef typename converter::el1 el1;
   typedef typename converter::el2 el2;
   typedef typename converter::el3 el3;
   typedef typename converter::fragment fragment;
   typedef Fragment_set<el1, el2, el3> fragment_set;
   typedef Map<el0, fragment_set> storage;

public:

   template<class Subj, class Pred, class Obj, class Doc> class result {
      typedef boost::fusion::vector4<Subj,Pred,Obj,Doc> vector;
      typedef typename boost::mpl::at<vector, Tag0>::type qt0;
      typedef typename boost::mpl::at<vector, Tag1>::type qt1;
      typedef typename boost::mpl::at<vector, Tag2>::type qt2;
      typedef typename boost::mpl::at<vector, Tag3>::type qt3;
   public:
      typedef Triple_find_dispatch<
                  Map,Tag0,Tag1,Tag2,Tag3, qt0, qt1, qt2, qt3
               > dispatch;
      typedef typename dispatch::range range;
      typedef typename dispatch::iterator iterator;

      static range find(
               storage const& v,
               Subj const& subj, Pred const& pred,
               Obj const& obj, Doc const& doc
      ) {
         BOOST_MPL_ASSERT((boost::has_equal_to<el0,qt0,bool>));
         BOOST_MPL_ASSERT((boost::has_equal_to<el1,qt1,bool>));
         BOOST_MPL_ASSERT((boost::has_equal_to<el2,qt2,bool>));
         BOOST_MPL_ASSERT((boost::has_equal_to<el3,qt3,bool>));
         const vector q(subj, pred, obj, doc);
         return dispatch::find(
                  v,
                  boost::fusion::at<Tag0>(q),
                  boost::fusion::at<Tag1>(q),
                  boost::fusion::at<Tag2>(q),
                  boost::fusion::at<Tag3>(q)
         );
      }
   };

   typedef typename result<any,any,any,any>::iterator iterator;
   typedef iterator const_iterator;

   const_iterator begin() const {
      return const_iterator(v_.begin(), v_.end(), any(),any(),any());
   }

   const_iterator end() const {
      return const_iterator(v_.end(), v_.end(), any(),any(),any());
   }

   void insert(Triple const& t) {
      v_.insert(converter::get_index(t), converter::get_fragment(t));
   }

   std::size_t size() const {return v_.n_fragments();}

   bool empty() const {return ! v_.n_fragments();}

   template<class Subj, class Pred, class Obj, class Doc>
   typename result<Subj,Pred,Obj,Doc>::range
   find(Subj const& subj, Pred const& pred, Obj const& obj, Doc const& doc) const {
      return result<Subj,Pred,Obj,Doc>::find(v_, subj, pred, obj, doc);
   }

private:
   storage v_;
};

}//namespace map_triple_detail
}//namespace owlcpp
#endif /* TRIPLE_INDEX_2_HPP_ */
