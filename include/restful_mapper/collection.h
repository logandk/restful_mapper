#ifndef COLLECTION_H_20130315
#define COLLECTION_H_20130315

#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace restful_mapper
{

template <class T>
class Collection
{
public:
  Collection() {}
  Collection(const Collection &other) : items_(other.items_) {}
  Collection &operator=(Collection other) { std::swap(*this, other); return *this; }
  virtual ~Collection() {}

  const std::vector<T> &items() const
  {
    return items_;
  }

  T &find(const int &id)
  {
    iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->primary() == id) return *i;
    }

    ostringstream s;
    s << "Cannot find " << typeid(T).name() << " with id " << id;
    throw std::out_of_range(s.str());
  }

  const T &find(const int &id) const
  {
    iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->primary() == id) return *i;
    }

    ostringstream s;
    s << "Cannot find " << typeid(T).name() << " with id " << id;
    throw std::out_of_range(s.str());
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
  void resize(size_type n, value_type val = value_type()) { items_.resize(n, val); }
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
  template <class InputIterator> void assign(InputIterator first, InputIterator last) { items_.assign(first, last); }
  void assign(size_type n, const value_type val) { items_.assign(n, val); }
  void push_back(const value_type val) { items_.push_back(val); }
  void pop_back() { items_.pop_back(); }
  iterator insert(iterator position, const value_type val) { return items_.insert(position, val); }
  void insert(iterator position, size_type n, const value_type val) { items_.insert(position, n, val); }
  template <class InputIterator> void insert(iterator position, InputIterator first, InputIterator last) { items_.insert(position, first, last); }
  iterator erase(iterator position) { return items_.erase(position); }
  iterator erase(iterator first, iterator last) { return items_.erase(first, last); }
  void swap(Collection& x) { items_.swap(x); }
  void clear() { items_.clear(); }
  allocator_type get_allocator() const { return items_.get_allocator(); }

private:
  std::vector<T> items_;
};

}

#endif // COLLECTION_H_20130315

