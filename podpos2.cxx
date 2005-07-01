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
lcs_b (_OutputIter ll, _InputIter1 ai, _InputIter1 end1,
       indexing_iterator<_InputIter2, _IndexType, _IndexPolicy> start2,
       _InputIter2 end2, _Eq const & eq)
{
  // step 1
  size_t const n = std::distance (start2.base (), end2);
  std::vector<size_t> k1 (n + 1, 0), k0 (n + 1);

  // step 2
  for (; ai != end1; ++ai)
    {
      // step 3
      std::copy (k1.begin (), k1.end (), k0.begin ());
      // step 4
      for (indexing_iterator<_InputIter2, _IndexType, _IndexPolicy> 
	     bj (start2); bj.base () != end2; ++bj)
	if (eq (*ai, *bj))
	  k1[bj.index ()] = k0[bj.index () - 1] + 1;
	else
	  k1[bj.index ()] = std::max (k1[bj.index () - 1], k0[bj.index ()]);
    }
  // step 5
  std::copy (k1.begin (), k1.end (), ll);
}


template <typename _OutputIter, typename _InputIter1, typename _InputIter2,
          typename _Eq>
size_t
lcs_c (_OutputIter cc, _InputIter1 start1, _InputIter1 end1,
       _InputIter2 start2, _InputIter2 end2, _Eq const & eq)
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
      lcs_b (l1.begin (), start1,
	     // i-1: count->offset; +1: end is one-past-the-end.
	     start1 + i - 1 + 1, make_indexing_iterator (start2, 1), end2, eq);
      
      std::vector<size_t> l2 (n + 1);
      lcs_b (l2.rbegin (), std::reverse_iterator<_InputIter1> (end1),
             std::reverse_iterator<_InputIter1> (start1 + i - 1 + 1),
             make_indexing_iterator (std::reverse_iterator<_InputIter2> (end2),
				     n - 1),
             std::reverse_iterator<_InputIter2> (start2), eq);
      // step 4
      size_t const mm = std::inner_product 
	(l1.begin (), l1.end (), l2.rbegin (), static_cast<size_t>(0),
	 std::ptr_fun<size_t const &, size_t const &, size_t const &>
         (std::max), std::plus<size_t> ());

      size_t k = static_cast<size_t>(-1);
      for (size_t j = 0; j <= n; ++j)
        if (l1[j] + l2[n - j] == mm)
          {
            k = j;
            break;
          }

      // step 5
      std::vector<typename std::iterator_traits<_InputIter1>::value_type> 
	c1 (std::min (i, n));
      size_t const c1len = lcs_c (c1.begin (), start1, start1 + i - 1 + 1, 
				  start2, start2 + k - 1 + 1, eq);

      std::vector<typename std::iterator_traits<_InputIter1>::value_type> 
	c2 (std::min (m - i, n - k));
      size_t const c2len = lcs_c (c2.begin (), start1 + i - 1 + 1, end1, 
				  start2 + k - 1 + 1, end2, eq);

      typename std::vector<typename std::iterator_traits
	<_InputIter1>::value_type>::const_iterator it;
      size_t u;
      for (it = c1.begin (), u = 0; u < c1len; ++u, ++it, ++cc)
	*cc = *it;
      for (it = c2.begin (), u = 0; u < c2len; ++u, ++it, ++cc)
	*cc = *it;
      return c1len + c2len;
    }
}


int main ()
{
  {
    std::string s1 ("axbyc");
    std::string s2 ("abzc");
    //std::string s1 ("axbc");
    //std::string s2 ("abzc");
    
    std::cout << "A: " << s1 << std::endl
	      << "B: " << s2 << std::endl;
    lcs_c (std::ostream_iterator<std::string::value_type> (std::cout, " "), 
	   s1.begin (), s1.end (),
	   s2.begin (), s2.end (), 
	   std::equal_to<std::string::value_type> ());
    std::cout << std::endl;
  }

  {
    std::vector<std::string> v1, v2;
    v1.push_back ("test\n");
    v1.push_back ("bbbb\n");
    v1.push_back ("xxxx\n");
    v1.push_back ("cccc\n");

    v2.push_back ("BAFF!!\n");
    v2.push_back ("test\n");
    v2.push_back ("bbbb\n");
    v2.push_back ("dummy\n");
    v2.push_back ("cccc\n");

    
    lcs_c (std::ostream_iterator<std::string> (std::cout, ""), 
	   v1.begin (), v1.end (),
	   v2.begin (), v2.end (), 
	   std::equal_to<std::string> ());
  }
}
