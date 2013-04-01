#ifndef RELATION_H_20130318
#define RELATION_H_20130318

#include <restful_mapper/collector.h>

namespace restful_mapper
{

template <class T>
class HasMany
{
public:
  HasMany() : is_dirty_(false) {}

  void from_json(std::string values, const int &flags = 0)
  {
    items_.clear();

    Collector collector(values);

    std::vector<std::string> partials = collector.get();
    std::vector<std::string>::const_iterator i, i_end = partials.end();

    for (i = partials.begin(); i != i_end; ++i)
    {
      T instance;
      instance.from_json(*i, flags);

      items_.push_back(instance);
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

  const std::vector<T> &items() const
  {
    return items_;
  }

  T &build()
  {
    push_back(T());
    return back();
  }

  bool is_dirty() const
  {
    const_iterator i, i_end = items_.end();

    for (i = items_.begin(); i != i_end; ++i)
    {
      if (i->is_dirty()) return true;
    }

    return is_dirty_;
  }

  void clean() const
  {
    is_dirty_ = false;
  }

  // Reimplement std::vector for convenience
  typedef typename std::vector<T>::value_type value_type;
  typedef typename std::vector<T>::allocator_type allocator_type;
  typedef typename std::vector<T>::reference reference;
  typedef typename std::vector<T>::const_reference const_reference;
  typedef typename std::vector<T>::pointer pointer;
  typedef typename std::vector<T>::const_pointer const_pointer;
  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;
  typedef typename std::vector<T>::reverse_iterator reverse_iterator;
  typedef typename std::vector<T>::const_reverse_iterator const_reverse_iterator;
  typedef typename std::vector<T>::difference_type difference_type;
  typedef typename std::vector<T>::size_type size_type;

  iterator begin() { return items_.begin(); }
  const_iterator begin() const { return items_.begin(); }
  iterator end() { return items_.end(); }
  const_iterator end() const { return items_.end(); }
  reverse_iterator rbegin() { return items_.rbegin(); }
  const_reverse_iterator rbegin() const { return items_.rbegin(); }
  reverse_iterator rend() { return items_.rend(); }
  const_reverse_iterator rend() const { return items_.rend(); }
  size_type size() const { return items_.size(); }
  size_type max_size() const { return items_.max_size(); }
  void resize(size_type n, value_type val = value_type()) { is_dirty_ = true; items_.resize(n, val); }
  size_type capacity() const { return items_.capacity(); }
  bool empty() const { return items_.empty(); }
  void reserve(size_type n) { items_.reserve(n); }
  reference operator[](size_type n) { return items_[n]; }
  const_reference operator[](size_type n) const { return items_[n]; }
  reference at(size_type n) { return items_.at(n); }
  const_reference at(size_type n) const { return items_.at(n); }
  reference front() { return items_.front(); }
  const_reference front() const { return items_.front(); }
  reference back() { return items_.back(); }
  const_reference back() const { return items_.back(); }
  template <class InputIterator> void assign(InputIterator first, InputIterator last) { is_dirty_ = true; items_.assign(first, last); }
  void assign(size_type n, const value_type val) { is_dirty_ = true; items_.assign(n, val); }
  void push_back(const value_type val) { is_dirty_ = true; items_.push_back(val); }
  void pop_back() { is_dirty_ = true; items_.pop_back(); }
  iterator insert(iterator position, const value_type val) { is_dirty_ = true; return items_.insert(position, val); }
  void insert(iterator position, size_type n, const value_type val) { is_dirty_ = true; items_.insert(position, n, val); }
  template <class InputIterator> void insert(iterator position, InputIterator first, InputIterator last) { is_dirty_ = true; items_.insert(position, first, last); }
  iterator erase(iterator position) { is_dirty_ = true; return items_.erase(position); }
  iterator erase(iterator first, iterator last) { is_dirty_ = true; return items_.erase(first, last); }
  void swap(HasMany& x) { is_dirty_ = true; items_.swap(x); }
  void clear() { is_dirty_ = true; items_.clear(); }
  allocator_type get_allocator() const { return items_.get_allocator(); }

private:
  std::vector<T> items_;
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

