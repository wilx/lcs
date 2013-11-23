#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>

#include "index_policy.hxx"
#include "indexing_iterator.hxx"


namespace internal 
{
  
  template <
    typename OutputIter, typename InputIter1, typename InputIter2, 
    typename Eq, typename IndexType, typename IndexPolicy>
  void
  lcs_length (OutputIter ll, InputIter1 ai, InputIter1 end1,
              indexing_iterator<InputIter2, IndexType, 
              IndexPolicy> start2, InputIter2 end2, Eq eq)
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
        for (indexing_iterator<InputIter2, IndexType, IndexPolicy> 
               bj (start2); bj.base () != end2; ++bj)
          if (eq (*ai, *bj))
            k1[bj.index ()] = k0[bj.index () - 1] + 1;
          else
            k1[bj.index ()] = std::max (k1[bj.index () - 1], k0[bj.index ()]);
      }
    // step 5
    std::copy (k1.begin (), k1.end (), ll);
  }


  
  template <
    typename OutputIter, typename InputIter1, typename InputIter2, typename Eq>
  size_t
  lcs_c (OutputIter & cc, InputIter1 start1, InputIter1 end1,
         InputIter2 start2, InputIter2 end2, Eq eq)
  {
    size_t const m = std::distance (start1, end1);
    size_t const n = std::distance (start2, end2);

    // step 1
    if (n == 0)
      return 0;
    else if (m == 1)
      {
        InputIter2 const bj (std::find_if (start2, end2,
                                           std::bind1st (eq, *start1)));
        if (bj != end2)
          {
            *cc++ = *bj;
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
        lcs_length (l1.begin (), start1,
                    // i-1: count->offset; +1: end is one-past-the-end.
                    start1 + i - 1 + 1, make_indexing_iterator (start2, 1), 
                    end2, eq);
      
        std::vector<size_t> l2 (n + 1);
        lcs_length (l2.rbegin (), std::reverse_iterator<InputIter1> (end1),
                    std::reverse_iterator<InputIter1> (start1 + i - 1 + 1),
                    make_indexing_iterator (std::reverse_iterator<InputIter2> 
                                            (end2), n - 1),
                    std::reverse_iterator<InputIter2> (start2), eq);
        // step 4
        size_t const mm = std::inner_product 
          (l1.begin (), l1.end (), l2.rbegin (), 0,
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
        size_t const c1len = lcs_c (cc, start1, start1 + i - 1 + 1, 
                                    start2, start2 + k - 1 + 1, eq);

        size_t const c2len = lcs_c (cc, start1 + i - 1 + 1, end1, 
                                    start2 + k - 1 + 1, end2, eq);

        return c1len + c2len;
      }
  }
  
} // namespace internal


template <
  typename OutputIter, typename InputIter1, typename InputIter2, typename Eq>
void
lcs_length (OutputIter ll, InputIter1 start1, InputIter1 end1,
            InputIter2 start2, InputIter2 end2, Eq eq)
{
  size_t const n = std::distance (start2, end2);
  internal::lcs_length (ll, start1, end1, 
                        make_indexing_iterator (start2, n - 1), end2, eq);
}


template <
  typename OutputIter, typename InputIter1, typename InputIter2, typename Eq>
inline size_t
lcs (OutputIter cc, InputIter1 start1, InputIter1 end1,
     InputIter2 start2, InputIter2 end2, Eq eq)
{
  return internal::lcs_c (cc, start1, end1, start2, end2, eq);
}


int main ()
{
  {
    std::string s1 ("This is a test.");
    std::string s2 ("Txhis iys jyuyyst anjkjhothjkher tsdfest.");

    std::vector<size_t> l (s2.size () + 1, 0);
    lcs_length (l.begin (), s1.begin (), s1.end (),s2.begin (), s2.end (),
                std::equal_to<std::string::value_type> ());
    //std::cout << "lcs_length:" << l.back () << std::endl;
    std::copy (l.begin (), l.end (), 
               std::ostream_iterator<size_t> (std::cout, " "));
    
    std::cout << "A: " << s1 << std::endl
              << "B: " << s2 << std::endl;
    lcs (std::ostream_iterator<std::string::value_type> (std::cout, " "), 
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

    lcs (std::ostream_iterator<std::string> (std::cout, ""), 
         v1.begin (), v1.end (),
         v2.begin (), v2.end (), 
         std::equal_to<std::string> ());
  }
}
