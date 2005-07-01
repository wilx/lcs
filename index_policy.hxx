#ifndef _index_policy_h_
#define _index_policy_h_

template <typename T>
struct default_index_policy
{
    typedef T index_type;

    index_type & 
    pre_increment (index_type & x) const
    {
       return ++x;
    }

    index_type 
    post_increment (index_type & x) const 
    {
        index_type tmp (x);
        ++x;
        return tmp;
    }

    index_type &
    pre_decrement (index_type & x) const 
    {
        return --x;
    }

    index_type
    post_decrement (index_type & x) const 
    {
        index_type tmp (x);
        --x;
        return tmp;
    }
    
    template <typename U>
    index_type
    plus_left (index_type const & t, U const & u) const 
    {
        return t + u;
    }

    template <typename U>
    index_type
    plus_right (U const & u, index_type const & t) const
    {
        return u + t;
    }

    template <typename U>
    index_type
    minus_left (index_type const & t, U const & u) const 
    {
        return t - u;
    }

    template <typename U>
    index_type
    minus_right (U const & u, index_type const & t) const
    {
        return u - t;
    }

    template <typename U>
    index_type &
    plus_eq (index_type & t, U const & u) const 
    {
        return t += u;
    }

    template <typename U>
    index_type &
    minus_eq (index_type & t, U const & u) const
    { 
        return t -= u;
    }
};


template <typename Policy>
struct reverse_index_policy
    : public Policy
{
    typedef Policy base_policy;
    typedef typename Policy::index_type index_type;

    index_type & 
    pre_increment (index_type & x) const
    {
        return Policy::pre_decrement (x);
    }

    index_type 
    post_increment (index_type & x) const
    {
        return Policy::post_decrement (x);
    }

    index_type &
    pre_decrement (index_type & x) const
    {
        return Policy::pre_increment (x);
    }

    index_type
    post_decrement (index_type & x) const
    {
        return Policy::post_increment (x);
    }
    
    template <typename U>
    index_type
    plus_left (index_type const & t, U const & u) const
    {
        return Policy::minus_left (t, u);
    }

    template <typename U>
    index_type
    plus_right (U const & u, index_type const & t) const
    {
        return Policy::minus_right (u, t);
    }

    template <typename U>
    index_type
    minus_left (index_type const & t, U const & u) const
    {
        return Policy::plus_left (t, u);
    }

    template <typename U>
    index_type
    minus_right (U const & u, index_type const & t) const
    {
        return Policy::plus_right (u, t);
    }

    template <typename U>
    index_type &
    plus_eq (index_type & t, U const & u) const
    {
        return Policy::minus_eq (t, u);
    }

    template <typename U>
    index_type &
    minus_eq (index_type & t, U const & u) const
    { 
        return Policy::plus_eq (t, u);
    }
};


template <typename T>
struct reverse_default_index_policy
    : public reverse_index_policy<default_index_policy<T> >
{ };


template <typename Policy>
inline reverse_index_policy<Policy>
make_reverse_policy (Policy const &)
{
    return reverse_index_policy<Policy> ();
}


template <typename Policy>
inline Policy
make_reverse_policy (reverse_index_policy<Policy> const &)
{
    return Policy ();
}


#endif // _index_policy_h_
