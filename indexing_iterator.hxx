#ifndef _indexing_iterator_hxx_
#define _indexing_iterator_hxx_

#include <iterator>
#include "index_policy.hxx"


typedef size_t _default_index_type;
//typedef int _default_index_type;

template <typename Iterator, typename Index = _default_index_type, 
	  typename IndexPolicy = default_index_policy<Index> >
class indexing_iterator 
  : public std::iterator 
<typename std::iterator_traits<Iterator>::iterator_category,
 typename std::iterator_traits<Iterator>::value_type,
 typename std::iterator_traits<Iterator>::difference_type,
 typename std::iterator_traits<Iterator>::pointer,
 typename std::iterator_traits<Iterator>::reference>
{
public:
  typedef Iterator iterator_type;
  typedef Index index_type;
  typedef IndexPolicy index_policy_type;
  typedef std::iterator_traits<Iterator> iterator_traits;
  typedef typename iterator_traits::difference_type difference_type;
  typedef typename iterator_traits::value_type value_type;
  typedef typename iterator_traits::pointer pointer;
  typedef typename iterator_traits::reference reference;
  typedef typename iterator_traits::iterator_category iterator_category;

  class IndexProxy
  {
  protected:
    index_type const idx;
    index_policy_type const idx_policy;
    
  public:
    IndexProxy (index_type const & i, index_policy_type const & pol)
      : idx (i), idx_policy (pol)
    { }

    IndexProxy (IndexProxy const & p)
      : idx (p.idx), idx_policy (p.idx_policy)
    { }

    index_type index () const
    {
      return idx;
    }

    operator index_type () const
    {
      return idx;
    }

    // IndexProxy operators:
    template <typename U>
    friend inline IndexProxy 
    operator + (IndexProxy const & p, U const & u)
    {
      return IndexProxy (p.idx_policy.plus_left (p.index (), u), p.idx_policy);
    }

    template <typename U>
    friend inline IndexProxy 
    operator + (U const & u, IndexProxy const & p)
    {
      return IndexProxy (p.idx_policy.plus_right (u, p.index ()), 
			 p.idx_policy);
    }

    template <typename U>
    friend inline IndexProxy 
    operator - (IndexProxy const & p, U const & u)
    {
      return IndexProxy (p.idx_policy.minus_left (p.index (), u), 
			 p.idx_policy);
    }
  };

  typedef IndexProxy index_proxy_type;

protected:
  iterator_type current;
  index_type idx;
  index_policy_type idx_policy;

public:    
  indexing_iterator ()
  { }

  explicit
  indexing_iterator (iterator_type const & it, 
		     index_type const & i = index_type (), 
		     index_policy_type const & p = index_policy_type ())
    : current (it), idx (i), idx_policy (p)
  { }

  indexing_iterator (indexing_iterator const & it)
    : current (it.current), idx (it.idx), idx_policy (it.idx_policy)
  { }

  template <typename It, typename Ind, typename IdxPol>
  indexing_iterator (indexing_iterator<It, Ind, IdxPol> const & it)
    : current (it.current), idx (it.idx), idx_policy (it.idx_policy)
  { }

  iterator_type
  base () const
  {
    return current;
  }

  index_proxy_type
  index () const
  {
    return IndexProxy (idx, idx_policy);
  }

  index_policy_type
  policy () const
  {
    return idx_policy;
  }

  reference
  operator * () const
  {
    return *current;
  }

  pointer
  operator -> () const
  { 
    return &(operator * ());
  }

  indexing_iterator &
  operator ++ ()
  {
    ++current;
    idx_policy.pre_increment (idx);
    return *this;
  }

  indexing_iterator
  operator ++ (int)
  {
    indexing_iterator tmp (*this);
    ++current;
    idx_policy.post_increment (idx);
    return tmp;
  }

  indexing_iterator &
  operator -- ()
  {
    --current;
    idx_policy.pre_decrement (idx);
    return *this;
  }

  indexing_iterator
  operator -- (int)
  {
    indexing_iterator tmp (*this);
    --current;
    idx_policy.post_decrement (idx);
    return tmp;
  }

  indexing_iterator
  operator + (difference_type n) const
  {
    return indexing_iterator (current + n, idx_policy.plus_left (idx, n));
  }

  indexing_iterator &
  operator += (difference_type n)
  {
    current += n;
    idx_policy.plus_eq (idx, n);
    return *this;
  }

  indexing_iterator
  operator - (difference_type n) const
  {
    return indexing_iterator (current - n, idx_policy.minus_left (idx, n));
  }

  indexing_iterator &
  operator -= (difference_type n)
  {
    current -= n;
    idx_policy.minus_eq (idx, n);
    return *this;
  }

  reference
  operator [] (difference_type n) const
  {
    return *(*this + n);
  }

  operator Iterator const & () const
  {
    return current;
  }

  operator Iterator & () 
  {
    return current;
  }
};


// indexing_iterator operators:
template <typename Iterator, typename Index, typename IndexPolicy>
inline bool
operator == (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
             indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return x.base () == y.base ();
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline bool
operator != (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
             indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return !(x == y);
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline bool
operator < (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
            indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return x.base () < y.base ();
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline bool
operator > (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
            indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return x.base () > y.base ();
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline bool
operator <= (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
	     indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return !(x > y);
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline bool
operator >= (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
	     indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return !(x < y);
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline 
typename indexing_iterator<Iterator, Index, IndexPolicy>::difference_type
operator - (indexing_iterator<Iterator, Index, IndexPolicy> const & x,
            indexing_iterator<Iterator, Index, IndexPolicy> const & y)
{
  return x.base () - y.base ();
}


template <typename Iterator, typename Index, typename IndexPolicy>
inline indexing_iterator<Iterator, Index, IndexPolicy>
operator + (typename indexing_iterator<Iterator, Index, IndexPolicy>
	    ::difference_type const & n,
            indexing_iterator<Iterator, Index, IndexPolicy> const & x)
{
  return indexing_iterator<Iterator, Index, IndexPolicy> (x.base () + n);
}


template <typename BaseIterator, typename Index, typename IndexPolicy>
inline indexing_iterator<BaseIterator, Index, IndexPolicy>
make_indexing_iterator (BaseIterator const & base, Index const & index, 
                        IndexPolicy const & policy)
{
  return indexing_iterator<BaseIterator, Index, IndexPolicy> 
    (base, index, policy);
}


template <typename BaseIterator, typename Index>
inline indexing_iterator<BaseIterator, Index, default_index_policy<Index> >
make_indexing_iterator (BaseIterator const & base, Index const & index)
{
  return make_indexing_iterator (base, index, default_index_policy<Index> ());
}


template <typename BaseIterator>
inline indexing_iterator<BaseIterator, _default_index_type, 
			 default_index_policy<_default_index_type> >
make_indexing_iterator (BaseIterator const & base)
{
  return make_indexing_iterator (base, static_cast<_default_index_type>(0));
}


template <typename BaseIterator, typename Index, typename IndexPolicy>
inline 
indexing_iterator<std::reverse_iterator<BaseIterator>, Index, IndexPolicy>
make_indexing_iterator (std::reverse_iterator<BaseIterator> const & base, 
			Index const & index, IndexPolicy const & policy)
{
  return indexing_iterator<std::reverse_iterator<BaseIterator>, Index, 
    IndexPolicy> (base, index, policy);
}


template <typename BaseIterator, typename Index>
inline indexing_iterator<std::reverse_iterator<BaseIterator>, Index, 
			 reverse_default_index_policy<Index> >
make_indexing_iterator (std::reverse_iterator<BaseIterator> const & base, 
			Index const & index)
{
  return indexing_iterator<std::reverse_iterator<BaseIterator>, Index, 
    reverse_default_index_policy<Index> > 
    (base, index, reverse_default_index_policy<Index> ());
}


template <typename BaseIterator>
inline 
indexing_iterator<std::reverse_iterator<BaseIterator>, _default_index_type, 
		  reverse_default_index_policy<_default_index_type> >
make_indexing_iterator (std::reverse_iterator<BaseIterator> const & base)
{
  return indexing_iterator<std::reverse_iterator<BaseIterator>, 
    _default_index_type, reverse_default_index_policy<_default_index_type> > 
    (base, static_cast<_default_index_type>(-1));
}


#endif // _indexing_iterator_hxx_
