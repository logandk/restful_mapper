#ifndef RESTFUL_MAPPER_RELATION_H
#define RESTFUL_MAPPER_RELATION_H

#include <restful_mapper/model_collection.h>

namespace restful_mapper
{

template <class T>
class HasMany : public ModelCollection<T>
{
public:
  HasMany() : ModelCollection<T>(), is_dirty_(false) {}

  void from_json(std::string values, const int &flags = 0)
  {
    ModelCollection<T>::clear();

    Json::Parser collector(values);

    std::vector<std::string> partials = collector.root().dump_array();
    std::vector<std::string>::const_iterator i, i_end = partials.end();

    for (i = partials.begin(); i != i_end; ++i)
    {
      T instance;
      instance.from_json(*i, flags, true);

      ModelCollection<T>::push_back(instance);
    }

    clean();
  }

  std::string to_json(const int &flags = 0) const
  {
    std::ostringstream s;
    s << "[";

    const_iterator i, i_end = ModelCollection<T>::end();

    for (i = ModelCollection<T>::begin(); i != i_end; ++i)
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
    return ModelCollection<T>::back();
  }

  bool is_dirty() const
  {
    const_iterator i, i_end = ModelCollection<T>::end();

    for (i = ModelCollection<T>::begin(); i != i_end; ++i)
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
  typedef typename ModelCollection<T>::value_type value_type;
  typedef typename ModelCollection<T>::allocator_type allocator_type;
  typedef typename ModelCollection<T>::reference reference;
  typedef typename ModelCollection<T>::const_reference const_reference;
  typedef typename ModelCollection<T>::pointer pointer;
  typedef typename ModelCollection<T>::const_pointer const_pointer;
  typedef typename ModelCollection<T>::iterator iterator;
  typedef typename ModelCollection<T>::const_iterator const_iterator;
  typedef typename ModelCollection<T>::reverse_iterator reverse_iterator;
  typedef typename ModelCollection<T>::const_reverse_iterator const_reverse_iterator;
  typedef typename ModelCollection<T>::difference_type difference_type;
  typedef typename ModelCollection<T>::size_type size_type;

  void resize(size_type n, value_type val = value_type()) { is_dirty_ = true; ModelCollection<T>::resize(n, val); }
  template <class InputIterator> void assign(InputIterator first, InputIterator last) { is_dirty_ = true; ModelCollection<T>::assign(first, last); }
  void assign(size_type n, const value_type val) { is_dirty_ = true; ModelCollection<T>::assign(n, val); }
  void push_back(const value_type val) { is_dirty_ = true; ModelCollection<T>::push_back(val); }
  void pop_back() { is_dirty_ = true; ModelCollection<T>::pop_back(); }
  iterator insert(iterator position, const value_type val) { is_dirty_ = true; return ModelCollection<T>::insert(position, val); }
  void insert(iterator position, size_type n, const value_type val) { is_dirty_ = true; ModelCollection<T>::insert(position, n, val); }
  template <class InputIterator> void insert(iterator position, InputIterator first, InputIterator last) { is_dirty_ = true; ModelCollection<T>::insert(position, first, last); }
  iterator erase(iterator position) { is_dirty_ = true; return ModelCollection<T>::erase(position); }
  iterator erase(iterator first, iterator last) { is_dirty_ = true; return ModelCollection<T>::erase(first, last); }
  void swap(HasMany& x) { is_dirty_ = true; ModelCollection<T>::swap(x); }
  void clear() { is_dirty_ = true; ModelCollection<T>::clear(); }

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

    item_->from_json(values, flags, true);
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
    if (value.is_null())
    {
      clear();
    }
    else
    {
      set(value.get());
    }

    is_dirty_ = value.is_dirty_;

    return *this;
  }

private:
  T *item_;
  mutable bool is_dirty_;

  void check_null() const
  {
    if (!item_)
    {
      std::ostringstream s;
      s << "Related object \"" << type_info_name(typeid(T)) << "\" does not exist";
      throw std::runtime_error(s.str());
    }
  }
};

}

#endif // RESTFUL_MAPPER_RELATION_H

