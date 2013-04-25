#ifndef RELATION_H_20130318
#define RELATION_H_20130318

#include <restful_mapper/collection.h>
#include <restful_mapper/collector.h>

namespace restful_mapper
{

template <class T>
class HasMany : public Collection<T>
{
public:
  HasMany() : Collection<T>(), is_dirty_(false) {}

  void from_json(std::string values, const int &flags = 0)
  {
    Collection<T>::clear();

    Collector collector(values);

    std::vector<std::string> partials = collector.get();
    std::vector<std::string>::const_iterator i, i_end = partials.end();

    for (i = partials.begin(); i != i_end; ++i)
    {
      T instance;
      instance.from_json(*i, flags);

      Collection<T>::push_back(instance);
    }
  }

  std::string to_json(const int &flags = 0) const
  {
    std::ostringstream s;
    s << "[";

    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      s << i->to_json(flags);
      if ((i + 1) != i_end) s << ",";
    }

    s << "]";

    return s.str();
  }

  T &build()
  {
    push_back(T());
    return back();
  }

  bool is_dirty() const
  {
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->is_dirty()) return true;
    }

    return is_dirty_;
  }

  void clean() const
  {
    is_dirty_ = false;
  }

  // Inherit typedefs
  typedef typename Collection<T>::value_type value_type;
  typedef typename Collection<T>::allocator_type allocator_type;
  typedef typename Collection<T>::reference reference;
  typedef typename Collection<T>::const_reference const_reference;
  typedef typename Collection<T>::pointer pointer;
  typedef typename Collection<T>::const_pointer const_pointer;
  typedef typename Collection<T>::iterator iterator;
  typedef typename Collection<T>::const_iterator const_iterator;
  typedef typename Collection<T>::reverse_iterator reverse_iterator;
  typedef typename Collection<T>::const_reverse_iterator const_reverse_iterator;
  typedef typename Collection<T>::difference_type difference_type;
  typedef typename Collection<T>::size_type size_type;

  void resize(size_type n, value_type val = value_type()) { is_dirty_ = true; Collection<T>::resize(n, val); }
  template <class InputIterator> void assign(InputIterator first, InputIterator last) { is_dirty_ = true; Collection<T>::assign(first, last); }
  void assign(size_type n, const value_type val) { is_dirty_ = true; Collection<T>::assign(n, val); }
  void push_back(const value_type val) { is_dirty_ = true; Collection<T>::push_back(val); }
  void pop_back() { is_dirty_ = true; Collection<T>::pop_back(); }
  iterator insert(iterator position, const value_type val) { is_dirty_ = true; return Collection<T>::insert(position, val); }
  void insert(iterator position, size_type n, const value_type val) { is_dirty_ = true; Collection<T>::insert(position, n, val); }
  template <class InputIterator> void insert(iterator position, InputIterator first, InputIterator last) { is_dirty_ = true; Collection<T>::insert(position, first, last); }
  iterator erase(iterator position) { is_dirty_ = true; return Collection<T>::erase(position); }
  iterator erase(iterator first, iterator last) { is_dirty_ = true; return Collection<T>::erase(first, last); }
  void swap(HasMany& x) { is_dirty_ = true; Collection<T>::swap(x); }
  void clear() { is_dirty_ = true; Collection<T>::clear(); }

private:
  mutable bool is_dirty_;
};

template <class T>
class HasOne
{
public:
  HasOne() : item_(NULL), is_dirty_(false) {}

  HasOne(const HasOne &other) : item_(NULL), is_dirty_(other.is_dirty_)
  {
    if (other.item_)
    {
      item_ = new T(*other.item_);
    }
  }

  ~HasOne()
  {
    clear();
  }

  bool is_null() const
  {
    return !item_;
  }

  bool is_dirty() const
  {
    if (item_ && item_->is_dirty())
    {
      return true;
    }

    return is_dirty_;
  }

  void clean() const
  {
    is_dirty_ = false;
  }

  const T &build()
  {
    clear();

    item_ = new T();

    return *item_;
  }

  const T &get() const
  {
    check_null();

    return *item_;
  }

  const T &set(const T &value)
  {
    clear();

    item_ = new T(value);

    return *item_;
  }

  void clear()
  {
    if (item_)
    {
      delete item_;
      item_ = NULL;
    }

    is_dirty_ = true;
  }

  void from_json(std::string values, const int &flags = 0)
  {
    build();
    clean();

    item_->from_json(values, flags);
  }

  std::string to_json(const int &flags = 0) const
  {
    if (item_)
    {
      return item_->to_json(flags);
    }
    else
    {
      return "null";
    }
  }

  T *operator->() const
  {
    check_null();

    return item_;
  }

  operator T() const
  {
    check_null();

    return get();
  }

  const T &operator=(const T &value)
  {
    return set(value);
  }

  const HasOne &operator=(const HasOne &value)
  {
    set(value.get());
    return *this;
  }

private:
  T *item_;
  mutable bool is_dirty_;

  void check_null() const
  {
    if (!item_)
    {
      throw std::runtime_error(std::string("Related object \"") + typeid(T).name() + "\" does not exist");
    }
  }
};

}

#endif // RELATION_H_20130318

