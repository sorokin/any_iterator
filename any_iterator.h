#pragma once

#include <cassert>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace any_iterator_impl
{
struct bad_any_iterator : std::exception
{
    bad_any_iterator()
    {}

    virtual char const* what() const noexcept
    {
        return "bad any_iterator";
    }
};

template <typename ValueType, typename Category>
struct any_iterator;

constexpr size_t small_storage_size = sizeof(void*);
constexpr size_t small_storage_alignment = alignof(void*);
using small_storage_type = std::aligned_storage<small_storage_size, small_storage_alignment>::type;

template <typename ValueType, typename Category>
struct any_iterator_ops;

template <typename ValueType>
struct any_iterator_ops<ValueType, std::forward_iterator_tag>
{
    using copy_t = void (*)(small_storage_type& dst, small_storage_type const& src);
    using move_t  = void (*)(small_storage_type& dst, small_storage_type& src);
    using assign_t = void (*)(any_iterator_ops<ValueType, std::forward_iterator_tag> const* dst_ops,
                              small_storage_type& dst, small_storage_type const& src);
    using destroy_t = void (*)(small_storage_type& obj);

    using deref_t = ValueType& (*)(small_storage_type const& obj);
    using preinc_t = void (*)(small_storage_type& obj);
    using postinc_t = void (*)(small_storage_type& dst, small_storage_type& src);

    using eq_t = bool (*)(small_storage_type const& lhs, small_storage_type const& rhs);

    copy_t copy;
    move_t move;
    assign_t assign;
    destroy_t destroy;

    deref_t deref;
    preinc_t preinc;
    postinc_t postinc;

    eq_t eq;

    constexpr any_iterator_ops(copy_t copy, move_t move, assign_t assign,
                               destroy_t destroy,
                               deref_t deref, preinc_t preinc, postinc_t postinc,
                               eq_t eq)
        : copy(copy)
        , move(move)
        , assign(assign)
        , destroy(destroy)
        , deref(deref)
        , preinc(preinc)
        , postinc(postinc)
        , eq(eq)
    {}
};

template <typename ValueType>
struct any_iterator_ops<ValueType, std::bidirectional_iterator_tag> : any_iterator_ops<ValueType, std::forward_iterator_tag>
{
    using base = any_iterator_ops<ValueType, std::forward_iterator_tag>;
    using typename base::copy_t;
    using typename base::move_t;
    using typename base::assign_t;
    using typename base::destroy_t;
    using typename base::deref_t;
    using typename base::preinc_t;
    using typename base::postinc_t;
    using typename base::eq_t;

    using predec_t = void (*)(small_storage_type& obj);
    using postdec_t = void (*)(small_storage_type& dst, small_storage_type& src);

    predec_t predec;
    postdec_t postdec;

    constexpr any_iterator_ops(copy_t copy, move_t move, assign_t assign,
                               destroy_t destroy,
                               deref_t deref, preinc_t preinc, postinc_t postinc,
                               eq_t eq, predec_t predec, postdec_t postdec)
        : any_iterator_ops<ValueType, std::forward_iterator_tag>(copy, move, assign,
                                                                 destroy,
                                                                 deref, preinc, postinc, eq)
        , predec(predec)
        , postdec(postdec)
    {}
};

template <typename ValueType>
struct any_iterator_ops<ValueType, std::random_access_iterator_tag> : any_iterator_ops<ValueType, std::bidirectional_iterator_tag>
{
    typedef any_iterator_ops<ValueType, std::bidirectional_iterator_tag> base;
    using typename base::copy_t;
    using typename base::move_t;
    using typename base::assign_t;
    using typename base::destroy_t;
    using typename base::deref_t;
    using typename base::preinc_t;
    using typename base::postinc_t;
    using typename base::eq_t;

    using typename base::predec_t;
    using typename base::postdec_t;

    using add_t = void (*)(small_storage_type& obj, size_t n);
    using sub_t = void (*)(small_storage_type& obj, size_t n);
    using diff_t = std::ptrdiff_t (*)(small_storage_type const& lhs, small_storage_type const& rhs);
    using lt_t = bool (*)(small_storage_type const& lhs, small_storage_type const& rhs);

    add_t add;
    sub_t sub;
    diff_t diff;
    lt_t lt;

    constexpr any_iterator_ops(copy_t copy, move_t move, assign_t assign,
                               destroy_t destroy,
                               deref_t deref, preinc_t preinc, postinc_t postinc,
                               eq_t eq, predec_t predec, postdec_t postdec,
                               add_t add, sub_t sub, diff_t diff, lt_t lt)
        : any_iterator_ops<ValueType, std::bidirectional_iterator_tag>(copy, move, assign,
                                                                       destroy,
                                                                       deref, preinc, postinc,
                                                                       eq, predec, postdec)
        , add(add)
        , sub(sub)
        , diff(diff)
        , lt(lt)
    {}
};

void null_clone(small_storage_type&, small_storage_type const&)
{}

void null_move(small_storage_type&, small_storage_type&)
{}

template <typename ValueType>
void null_assign(any_iterator_ops<ValueType, std::forward_iterator_tag> const* dst_ops, small_storage_type& dst, small_storage_type const&)
{
    dst_ops->destroy(dst);
}

void null_destroy(small_storage_type&)
{}

template <typename ValueType>
ValueType& null_deref(small_storage_type const&)
{
    throw bad_any_iterator();
}

void null_preinc(small_storage_type&)
{
    throw bad_any_iterator();
}

void null_postinc(small_storage_type&, small_storage_type&)
{
    throw bad_any_iterator();
}

bool null_eq(small_storage_type const&, small_storage_type const&)
{
    throw bad_any_iterator();
}

void null_predec(small_storage_type&)
{
    throw bad_any_iterator();
}

void null_postdec(small_storage_type&, small_storage_type&)
{
    throw bad_any_iterator();
}

void null_add(small_storage_type&, size_t)
{
    throw bad_any_iterator();
}

void null_sub(small_storage_type&, size_t)
{
    throw bad_any_iterator();
}

std::ptrdiff_t null_diff(small_storage_type const&, small_storage_type const&)
{
    throw bad_any_iterator();
}

bool null_lt(small_storage_type const&, small_storage_type const&)
{
    throw bad_any_iterator();
}

template <typename ValueType>
inline any_iterator_ops<ValueType, std::random_access_iterator_tag> const* make_null_ops()
{
    static constexpr any_iterator_ops<ValueType, std::random_access_iterator_tag> instance
    (
        &null_clone,
        &null_move,
        &null_assign,
        &null_destroy,

        &null_deref<ValueType>,
        &null_preinc,
        &null_postinc,

        &null_eq,

        &null_predec,
        &null_postdec,

        &null_add,
        &null_sub,
        &null_diff,
        &null_lt
    );

    return &instance;
}

template <typename InnerIterator>
static constexpr bool fits_small_storage
    = sizeof(InnerIterator) <= small_storage_size
   && alignof(InnerIterator) <= small_storage_alignment
   && std::is_trivially_move_constructible<InnerIterator>::value;

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>, InnerIterator&>::type access(small_storage_type& stg)
{
    return reinterpret_cast<InnerIterator&>(stg);
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>, InnerIterator&>::type access(small_storage_type& stg)
{
    return *reinterpret_cast<InnerIterator*&>(stg);
}

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>, InnerIterator const&>::type access(small_storage_type const& stg)
{
    return reinterpret_cast<InnerIterator const&>(stg);
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>, InnerIterator const&>::type access(small_storage_type const& stg)
{
    return *reinterpret_cast<InnerIterator* const&>(stg);
}

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>>::type inner_copy(small_storage_type& dst, small_storage_type const& src)
{
    new (&dst) InnerIterator(access<InnerIterator>(src));
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>>::type inner_copy(small_storage_type& dst, small_storage_type const& src)
{
    new (&dst) InnerIterator*(new InnerIterator(access<InnerIterator>(src)));
}

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>>::type inner_move(small_storage_type& dst, small_storage_type& src)
{
    new (&dst) InnerIterator(std::move(access<InnerIterator>(src)));
    access<InnerIterator>(src).~InnerIterator();
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>>::type inner_move(small_storage_type& dst, small_storage_type& src)
{
    reinterpret_cast<InnerIterator*&>(dst) = reinterpret_cast<InnerIterator*&>(src);
}

template <typename ValueType, typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>>::type inner_assign(any_iterator_ops<ValueType, std::forward_iterator_tag> const* dst_ops, small_storage_type& dst, small_storage_type const& src)
{
    dst_ops->destroy(dst);
    new (&dst) InnerIterator(access<InnerIterator>(src));
}

template <typename ValueType, typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator> >::type inner_assign(any_iterator_ops<ValueType, std::forward_iterator_tag> const* dst_ops, small_storage_type& dst, small_storage_type const& src)
{
    auto p = std::make_unique<InnerIterator>(access<InnerIterator>(src));
    dst_ops->destroy(dst);
    new (&dst) InnerIterator*(p.release());
}

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>>::type inner_destroy(small_storage_type& obj)
{
    access<InnerIterator>(obj).~InnerIterator();
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>>::type inner_destroy(small_storage_type& obj)
{
    delete &access<InnerIterator>(obj);
}

template <typename ValueType, typename InnerIterator>
ValueType& inner_deref(small_storage_type const& obj)
{
    return *access<InnerIterator>(obj);
}

template <typename InnerIterator>
void inner_preinc(small_storage_type& obj)
{
    ++access<InnerIterator>(obj);
}

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>>::type inner_postinc(small_storage_type& dst, small_storage_type& src)
{
    new (&dst) InnerIterator(access<InnerIterator>(src));
    ++access<InnerIterator>(src);
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>>::type inner_postinc(small_storage_type& dst, small_storage_type& src)
{
    auto p = std::make_unique<InnerIterator>(std::move(access<InnerIterator>(src)));
    ++access<InnerIterator>(src);
    new (&dst) InnerIterator*(p.release());
}

template <typename InnerIterator>
bool inner_eq(small_storage_type const& lhs, small_storage_type const& rhs)
{
    return access<InnerIterator>(lhs) == access<InnerIterator>(rhs);
}

template <typename InnerIterator>
void inner_predec(small_storage_type& obj)
{
    --access<InnerIterator>(obj);
}

template <typename InnerIterator>
typename std::enable_if<fits_small_storage<InnerIterator>>::type inner_postdec(small_storage_type& dst, small_storage_type& src)
{
    new (&dst) InnerIterator(access<InnerIterator>(src));
    --access<InnerIterator>(src);
}

template <typename InnerIterator>
typename std::enable_if<!fits_small_storage<InnerIterator>>::type inner_postdec(small_storage_type& dst, small_storage_type& src)
{
    auto p = std::make_unique<InnerIterator>(std::move(access<InnerIterator>(src)));
    --access<InnerIterator>(src);
    new (&dst) InnerIterator*(p.release());
}

template <typename InnerIterator>
void inner_add(small_storage_type& obj, size_t n)
{
    access<InnerIterator>(obj) += n;
}

template <typename InnerIterator>
void inner_sub(small_storage_type& obj, size_t n)
{
    access<InnerIterator>(obj) -= n;
}

template <typename InnerIterator>
std::ptrdiff_t inner_diff(small_storage_type const& lhs, small_storage_type const& rhs)
{
    return access<InnerIterator>(lhs) - access<InnerIterator>(rhs);
}

template <typename InnerIterator>
bool inner_lt(small_storage_type const& lhs, small_storage_type const& rhs)
{
    return access<InnerIterator>(lhs) < access<InnerIterator>(rhs);
}

template <typename ValueType, typename InnerIterator, typename IteratorCategory>
struct iterator_ops_impl;

template <typename ValueType, typename InnerIterator>
struct iterator_ops_impl<ValueType, InnerIterator, std::forward_iterator_tag>
{
    static constexpr any_iterator_ops<ValueType, std::forward_iterator_tag> make()
    {
        return
        {
            &inner_copy<InnerIterator>,
            &inner_move<InnerIterator>,
            &inner_assign<ValueType, InnerIterator>,
            &inner_destroy<InnerIterator>,
            &inner_deref<ValueType, InnerIterator>,
            &inner_preinc<InnerIterator>,
            &inner_postinc<InnerIterator>,
            &inner_eq<InnerIterator>
        };
    }
};

template <typename ValueType, typename InnerIterator>
struct iterator_ops_impl<ValueType, InnerIterator, std::bidirectional_iterator_tag>
{
    static constexpr any_iterator_ops<ValueType, std::bidirectional_iterator_tag> make()
    {
        return
        {
            &inner_copy<InnerIterator>,
            &inner_move<InnerIterator>,
            &inner_assign<ValueType, InnerIterator>,
            &inner_destroy<InnerIterator>,
            &inner_deref<ValueType, InnerIterator>,
            &inner_preinc<InnerIterator>,
            &inner_postinc<InnerIterator>,
            &inner_eq<InnerIterator>,
            &inner_predec<InnerIterator>,
            &inner_postdec<InnerIterator>
        };
    }
};

template <typename ValueType, typename InnerIterator>
struct iterator_ops_impl<ValueType, InnerIterator, std::random_access_iterator_tag>
{
    static constexpr any_iterator_ops<ValueType, std::random_access_iterator_tag> make()
    {
        return
        {
            &inner_copy<InnerIterator>,
            &inner_move<InnerIterator>,
            &inner_assign<ValueType, InnerIterator>,
            &inner_destroy<InnerIterator>,
            &inner_deref<ValueType, InnerIterator>,
            &inner_preinc<InnerIterator>,
            &inner_postinc<InnerIterator>,
            &inner_eq<InnerIterator>,
            &inner_predec<InnerIterator>,
            &inner_postdec<InnerIterator>,
            &inner_add<InnerIterator>,
            &inner_sub<InnerIterator>,
            &inner_diff<InnerIterator>,
            &inner_lt<InnerIterator>
        };
    }
};

template <typename ValueType, typename InnerIterator>
any_iterator_ops<ValueType, typename std::iterator_traits<InnerIterator>::iterator_category> const* make_inner_iterator_ops()
{
    static constexpr any_iterator_ops<ValueType, typename std::iterator_traits<InnerIterator>::iterator_category> instance
        = iterator_ops_impl<ValueType, InnerIterator, typename std::iterator_traits<InnerIterator>::iterator_category>::make();

    return &instance;
}

template <typename ValueType, typename InnerIterator>
any_iterator_ops<ValueType, typename std::iterator_traits<InnerIterator>::iterator_category> const* make_big_iterator_ops();

template <typename ValueType, typename Category>
ValueType& operator*(any_iterator<ValueType, Category> const& it);

template <typename ValueType, typename Category>
any_iterator<ValueType, Category>& operator++(any_iterator<ValueType, Category>& it);

template <typename ValueType, typename Category>
any_iterator<ValueType, Category> operator++(any_iterator<ValueType, Category>& it, int);

template <typename ValueType, typename Category>
bool operator==(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs);

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::bidirectional_iterator_tag*>::value,
    any_iterator<ValueType, Category>&
>::type operator--(any_iterator<ValueType, Category>& it);

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::bidirectional_iterator_tag*>::value,
    any_iterator<ValueType, Category>
>::type operator--(any_iterator<ValueType, Category>& it, int);

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>&
>::type operator+=(any_iterator<ValueType, Category>& it, std::size_t);

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>&
>::type operator-=(any_iterator<ValueType, Category>& it, std::size_t);

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    std::ptrdiff_t
>::type operator-(any_iterator<ValueType, Category> const&, any_iterator<ValueType, Category> const&);

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    bool
>::type operator<(any_iterator<ValueType, Category> const&, any_iterator<ValueType, Category> const&);

template <typename ValueType, typename Category>
struct any_iterator_base;

template <typename ValueType>
struct any_iterator_base<ValueType, std::forward_iterator_tag>
{
};

template <typename ValueType>
struct any_iterator_base<ValueType, std::bidirectional_iterator_tag>
{
    any_iterator_ops<ValueType, std::bidirectional_iterator_tag> const*& get_ops()
    {
        return static_cast<any_iterator<ValueType, std::bidirectional_iterator_tag>&>(*this).ops;
    }

    any_iterator_ops<ValueType, std::bidirectional_iterator_tag> const* const& get_ops() const
    {
        return static_cast<any_iterator<ValueType, std::bidirectional_iterator_tag> const&>(*this).ops;
    }

    small_storage_type& get_stg()
    {
        return static_cast<any_iterator<ValueType, std::bidirectional_iterator_tag>&>(*this).stg;
    }

    small_storage_type const& get_stg() const
    {
        return static_cast<any_iterator<ValueType, std::bidirectional_iterator_tag> const&>(*this).stg;
    }

    friend typename std::enable_if<
        true,
        any_iterator<ValueType, std::bidirectional_iterator_tag>&
    >::type operator--<>(any_iterator<ValueType, std::bidirectional_iterator_tag>&);

    friend typename std::enable_if<
        true,
        any_iterator<ValueType, std::bidirectional_iterator_tag>
    >::type operator--<>(any_iterator<ValueType, std::bidirectional_iterator_tag>&, int);
};

template <typename ValueType>
struct any_iterator_base<ValueType, std::random_access_iterator_tag>
{
    ValueType& operator[](std::ptrdiff_t n) const
    {
        return *(static_cast<any_iterator<ValueType, std::random_access_iterator_tag> const&>(*this) + n);
    }

    any_iterator_ops<ValueType, std::random_access_iterator_tag> const*& get_ops()
    {
        return static_cast<any_iterator<ValueType, std::random_access_iterator_tag>&>(*this).ops;
    }

    any_iterator_ops<ValueType, std::random_access_iterator_tag> const* const& get_ops() const
    {
        return static_cast<any_iterator<ValueType, std::random_access_iterator_tag> const&>(*this).ops;
    }

    small_storage_type& get_stg()
    {
        return static_cast<any_iterator<ValueType, std::random_access_iterator_tag>&>(*this).stg;
    }

    small_storage_type const& get_stg() const
    {
        return static_cast<any_iterator<ValueType, std::random_access_iterator_tag> const&>(*this).stg;
    }

    friend typename std::enable_if<
        true,
        any_iterator<ValueType, std::random_access_iterator_tag>&
    >::type operator+=<>(any_iterator<ValueType, std::random_access_iterator_tag>& it, std::size_t);

    friend typename std::enable_if<
        true,
        any_iterator<ValueType, std::random_access_iterator_tag>&
    >::type operator-=<>(any_iterator<ValueType, std::random_access_iterator_tag>& it, std::size_t);

    friend typename std::enable_if<
        true,
        std::ptrdiff_t
    >::type operator-<>(any_iterator<ValueType, std::random_access_iterator_tag> const&, any_iterator<ValueType, std::random_access_iterator_tag> const&);

    friend typename std::enable_if<
        true,
        bool
    >::type operator< <>(any_iterator<ValueType, std::random_access_iterator_tag> const&, any_iterator<ValueType, std::random_access_iterator_tag> const&);
};

template <typename ValueType, typename Category>
struct any_iterator : any_iterator_base<ValueType, Category>
{
    using value_type = ValueType;
    using iterator_category = Category;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using reference = ValueType&;

    any_iterator() noexcept
        : ops(make_null_ops<ValueType>())
    {}

    template <typename InnerIterator>
    any_iterator(InnerIterator ii,
                 typename std::enable_if<
                     std::is_convertible<typename std::iterator_traits<InnerIterator>::iterator_category, Category>::value
                  && fits_small_storage<InnerIterator> >::type* = nullptr)
        : ops(make_inner_iterator_ops<ValueType, InnerIterator>())
    {
        new (&stg) InnerIterator(std::move(ii));
    }

    template <typename InnerIterator>
    any_iterator(InnerIterator ii,
                 typename std::enable_if<
                     std::is_convertible<typename std::iterator_traits<InnerIterator>::iterator_category, Category>::value
                  && !fits_small_storage<InnerIterator> >::type* = nullptr)
        : ops(make_inner_iterator_ops<ValueType, InnerIterator>())
    {
        new (&stg) InnerIterator*(new InnerIterator(std::move(ii)));
    }

    any_iterator(any_iterator const& other)
        : ops(other.ops)
    {
        ops->copy(stg, other.stg);
    }

    any_iterator(any_iterator&& other) noexcept
        : ops(other.ops)
    {
        ops->move(stg, other.stg);
        other.ops = make_null_ops<ValueType>();
    }

    ~any_iterator()
    {
        ops->destroy(stg);
    }

    any_iterator& operator=(any_iterator const& rhs)
    {
        rhs.ops->assign(ops, stg, rhs.stg);
        return *this;
    }

    any_iterator& operator=(any_iterator&& rhs) noexcept
    {
        ops->destroy(stg);
        rhs.ops->move(stg, rhs.stg);
        rhs.ops = make_null_ops<ValueType>();
        return *this;
    }
private:
    any_iterator_ops<ValueType, Category> const* ops;
    small_storage_type stg;

    friend struct any_iterator_base<ValueType, Category>;
    friend ValueType& operator*<>(any_iterator<ValueType, Category> const&);
    friend any_iterator& operator++<>(any_iterator& it);
    friend any_iterator operator++<>(any_iterator& it, int);
    friend bool operator==<>(any_iterator const& lhs, any_iterator const& rhs);
};

template <typename ValueType, typename Category>
ValueType& operator*(any_iterator<ValueType, Category> const& it)
{
    return it.ops->deref(it.stg);
}

template <typename ValueType, typename Category>
any_iterator<ValueType, Category>& operator++(any_iterator<ValueType, Category>& it)
{
    it.ops->preinc(it.stg);
    return it;
}

template <typename ValueType, typename Category>
any_iterator<ValueType, Category> operator++(any_iterator<ValueType, Category>& it, int)
{
    any_iterator<ValueType, Category> copy;
    it.ops->postinc(copy.stg, it.stg);
    copy.ops = it.ops;
    return copy;
}

template <typename ValueType, typename Category>
bool operator==(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    assert(lhs.ops == rhs.ops);
    return lhs.ops->eq(lhs.stg, rhs.stg);
}

template <typename ValueType, typename Category>
bool operator!=(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    return !(lhs == rhs);
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::bidirectional_iterator_tag*>::value,
    any_iterator<ValueType, Category>&
>::type operator--(any_iterator<ValueType, Category>& it)
{
    it.get_ops()->predec(it.get_stg());
    return it;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::bidirectional_iterator_tag*>::value,
    any_iterator<ValueType, Category>
>::type operator--(any_iterator<ValueType, Category>& it, int)
{
    any_iterator<ValueType, Category> copy;
    it.get_ops()->postdec(copy.get_stg(), it.get_stg());
    copy.get_ops() = it.get_ops();
    return copy;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>&
>::type operator+=(any_iterator<ValueType, Category>& it, std::size_t n)
{
    it.get_ops()->add(it.get_stg(), n);
    return it;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>&
>::type operator-=(any_iterator<ValueType, Category>& it, std::size_t n)
{
    it.get_ops()->sub(it.get_stg(), n);
    return it;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    std::ptrdiff_t
>::type operator-(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    assert(lhs.get_ops() == rhs.get_ops());
    return lhs.get_ops()->diff(lhs.get_stg(), rhs.get_stg());
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    bool
>::type operator<(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    assert(lhs.get_ops() == rhs.get_ops());
    return lhs.get_ops()->lt(lhs.get_stg(), rhs.get_stg());
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    bool
>::type operator<=(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    return !(rhs < lhs);
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    bool
>::type operator>(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    return rhs < lhs;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    bool
>::type operator>=(any_iterator<ValueType, Category> const& lhs, any_iterator<ValueType, Category> const& rhs)
{
    return !(lhs < rhs);
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>
>::type operator+(any_iterator<ValueType, Category> it, std::size_t n)
{
    it += n;
    return it;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>
>::type operator+(std::size_t n, any_iterator<ValueType, Category> it)
{
    it += n;
    return it;
}

template <typename ValueType, typename Category>
typename std::enable_if<
    std::is_convertible<Category*, std::random_access_iterator_tag*>::value,
    any_iterator<ValueType, Category>
>::type operator-(any_iterator<ValueType, Category> it, std::size_t n)
{
    it -= n;
    return it;
}

}

using any_iterator_impl::bad_any_iterator;
using any_iterator_impl::any_iterator;

template <typename ValueType>
using any_forward_iterator = any_iterator<ValueType, std::forward_iterator_tag>;

template <typename ValueType>
using any_bidirectional_iterator = any_iterator<ValueType, std::bidirectional_iterator_tag>;

template <typename ValueType>
using any_random_access_iterator = any_iterator<ValueType, std::random_access_iterator_tag>;
