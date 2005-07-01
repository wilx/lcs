#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>

#include "index_policy.hxx"
#include "indexing_iterator.hxx"

template <typename _OutputIter, typename _InputIter1, typename _InputIter2, 
          typename _Eq, typename _IndexType, typename _IndexPolicy>
void
lcs_b (_OutputIter ll, _InputIter1 ai, _InputIter1 const & end1,
       indexing_iterator<_InputIter2, _IndexType, _IndexPolicy> 
       const & start2, 
       indexing_iterator<_InputIter2, _IndexType, _IndexPolicy> const & end2, 
       _Eq const & eq)
{
  // step 1
  size_t const m = std::distance (ai, end1);
  size_t const n = std::distance (start2, end2);
  std::vector<size_t> k1 (n + 1, 0), k0 (n + 1);
  
  // step 2
  for (; ai != end1; ++ai)
    {
      // step 3
      std::copy (k1.begin (), k1.end (), k0.begin ());
      // step 4
      for (indexing_iterator<_InputIter2, _IndexType, _IndexPolicy> 
	     bj (start2); bj != end2; ++bj)
        {
          if (eq (*ai, *bj))
            k1[bj.index ()] = k0[bj.index () - 1] + 1;
          else
            k1[bj.index ()] = std::max (k1[bj.index () - 1], k0[bj.index ()]);
        }
    }
  // step 5
  std::copy (k1.begin (), k1.end (), ll);
}


template <typename _OutputIter, typename _InputIter1, typename _InputIter2, 
          typename _Eq>
size_t
lcs_c (_OutputIter cc, _InputIter1 const & start1, _InputIter1 const & end1,
       _InputIter2 const & start2, _InputIter2 const & end2, _Eq const & eq)
{
  size_t const m = std::distance (start1, end1);
  size_t const n = std::distance (start2, end2);
  
  // step 1
  if (n == 0)
    return 0;
  else if (m == 1)
    {
      _InputIter2 const bj (std::find_if (start2, end2, 
					  std::bind1st (eq, *start1)));
      if (bj != end2)
        {
          *cc = *bj;
          return 1;
        }
      else
        return 0;
    }   
  // step 2
  else
    {
      // step 3
      size_t const i = m / 2;
      std::vector<size_t> l1 (n + 1);
      // i-1: count->offset; +1: end is one-past-the-end.
      lcs_b (l1.begin (), start1, start1 + i - 1 + 1, 
	     indexing_iterator<_InputIter2> (start2, 1), 
	     indexing_iterator<_InputIter2> (end2), eq);
      std::vector<size_t> l2 (n + 1);
      lcs_b (l2.begin (), std::reverse_iterator<_InputIter1> (end1), 
             std::reverse_iterator<_InputIter1> (start1 + i - 1 + 1),
             indexing_iterator<std::reverse_iterator<_InputIter2>, size_t, 
	     reverse_index_policy<default_index_policy<size_t> > > 
	     (std::reverse_iterator<_InputIter2> (end2), n), 
             indexing_iterator<std::reverse_iterator<_InputIter2>, size_t,
	     reverse_index_policy<default_index_policy<size_t> > >
	     (std::reverse_iterator<_InputIter2> (start2)), eq);
      // step 4
      size_t const mm = std::inner_product 
        (l1.begin (), l1.end (), l2.rbegin (), static_cast<size_t>(0),
         std::ptr_fun<size_t const &, size_t const &, size_t const &> 
         (std::max), std::plus<size_t> ());
      size_t k;
      for (k = 0; k <= n; ++k)
        if (l1[k] + l2[n - k] == mm)
          break;
      // step 5
      _OutputIter & tmp = cc;
      size_t const c1len = lcs_c (tmp, start1, start1 + i - 1 + 1, start2, 
                                  start2 + k - 1 + 1, eq);
      return c1len + lcs_c (tmp , start1 + i - 1 + 1, end1, start2 + k - 1 + 1,
			    end2, eq);
    }
}



int main ()
{
  std::string s1 ("ab");
  std::string s2 ("axb");

  lcs_c (std::ostream_iterator<char> (std::cout, " "), s1.begin (), s1.end (),
         s2.begin (), s2.end (), std::equal_to<char>());
}

