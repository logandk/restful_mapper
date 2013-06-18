#ifndef RESTFUL_MAPPER_MODEL_COLLECTION_H
#define RESTFUL_MAPPER_MODEL_COLLECTION_H

#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <restful_mapper/helpers.h>
#include <restful_mapper/json.h>

namespace restful_mapper
{

template <class T>
class ModelCollection
{
public:
  ModelCollection() {}
  virtual ~ModelCollection() {}

  const std::vector<T> &items() const
  {
    return items_;
  }

  ModelCollection<T> clone() const
  {
    ModelCollection<T> cloned_list;
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      cloned_list.push_back(i->clone());
    }

    return cloned_list;
  }

  T &find(const long long &id)
  {
    iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->primary() == id) return *i;
    }

    std::ostringstream s;
    s << "Cannot find " << type_info_name(typeid(T)) << " with id " << id;
    throw std::out_of_range(s.str());
  }

  T &find(const int &id)
  {
    return find((long long) id);
  }

  const T &find(const long long &id) const
  {
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->primary() == id) return *i;
    }

    std::ostringstream s;
    s << "Cannot find " << type_info_name(typeid(T)) << " with id " << id;
    throw std::out_of_range(s.str());
  }

  const T &find(const int &id) const
  {
    return find((long long) id);
  }

  ModelCollection<T> find(const std::string &field, const int &value) const
  {
    return find_by_field(field, Json::encode(value));
  }

  T &find_first(const std::string &field, const int &value)
  {
    return find_first_by_field(field, Json::encode(value));
  }

  const T &find_first(const std::string &field, const int &value) const
  {
    return find_first_by_field(field, Json::encode(value));
  }

  ModelCollection<T> find(const std::string &field, const long long &value) const
  {
    return find_by_field(field, Json::encode(value));
  }

  T &find_first(const std::string &field, const long long &value)
  {
    return find_first_by_field(field, Json::encode(value));
  }

  const T &find_first(const std::string &field, const long long &value) const
  {
    return find_first_by_field(field, Json::encode(value));
  }

  ModelCollection<T> find(const std::string &field, const double &value) const
  {
    return find_by_field(field, Json::encode(value));
  }

  T &find_first(const std::string &field, const double &value)
  {
    return find_first_by_field(field, Json::encode(value));
  }

  const T &find_first(const std::string &field, const double &value) const
  {
    return find_first_by_field(field, Json::encode(value));
  }

  ModelCollection<T> find(const std::string &field, const bool &value) const
  {
    return find_by_field(field, Json::encode(value));
  }

  T &find_first(const std::string &field, const bool &value)
  {
    return find_first_by_field(field, Json::encode(value));
  }

  const T &find_first(const std::string &field, const bool &value) const
  {
    return find_first_by_field(field, Json::encode(value));
  }

  ModelCollection<T> find(const std::string &field, const std::string &value) const
  {
    return find_by_field(field, Json::encode(value));
  }

  T &find_first(const std::string &field, const std::string &value)
  {
    return find_first_by_field(field, Json::encode(value));
  }

  const T &find_first(const std::string &field, const std::string &value) const
  {
    return find_first_by_field(field, Json::encode(value));
  }

  ModelCollection<T> find(const std::string &field, const char *value) const
  {
    return find_by_field(field, Json::encode(value));
  }

  T &find_first(const std::string &field, const char *value)
  {
    return find_first_by_field(field, Json::encode(value));
  }

  const T &find_first(const std::string &field, const char *value) const
  {
    return find_first_by_field(field, Json::encode(value));
  }

  bool contains(const long long &id) const
  {
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->primary() == id) return true;
    }

    return false;
  }

  bool contains(const int &id) const
  {
    return contains((long long) id);
  }

  bool contains(const std::string &field, const int &value) const
  {
    return contains_by_field(field, Json::encode(value));
  }

  bool contains(const std::string &field, const long long &value) const
  {
    return contains_by_field(field, Json::encode(value));
  }

  bool contains(const std::string &field, const double &value) const
  {
    return contains_by_field(field, Json::encode(value));
  }

  bool contains(const std::string &field, const bool &value) const
  {
    return contains_by_field(field, Json::encode(value));
  }

  bool contains(const std::string &field, const std::string &value) const
  {
    return contains_by_field(field, Json::encode(value));
  }

  bool contains(const std::string &field, const char *value) const
  {
    return contains_by_field(field, Json::encode(value));
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
  void swap(ModelCollection& x) { items_.swap(x); }
  void clear() { items_.clear(); }
  allocator_type get_allocator() const { return items_.get_allocator(); }

protected:
  std::vector<T> items_;

  ModelCollection<T> find_by_field(const std::string &field, const std::string &json_value) const
  {
    ModelCollection<T> results;
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->read_field(field) == json_value) results.push_back(*i);
    }

    return results;
  }

  T &find_first_by_field(const std::string &field, const std::string &json_value)
  {
    iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->read_field(field) == json_value) return *i;
    }

    std::ostringstream s;
    s << "Cannot find " << type_info_name(typeid(T)) << " with " << field << " " << json_value;
    throw std::out_of_range(s.str());
  }

  const T &find_first_by_field(const std::string &field, const std::string &json_value) const
  {
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->read_field(field) == json_value) return *i;
    }

    std::ostringstream s;
    s << "Cannot find " << type_info_name(typeid(T)) << " with " << field << " " << json_value;
    throw std::out_of_range(s.str());
  }

  bool contains_by_field(const std::string &field, const std::string &json_value) const
  {
    const_iterator i, i_end = end();

    for (i = begin(); i != i_end; ++i)
    {
      if (i->read_field(field) == json_value) return true;
    }

    return false;
  }
};

}

#endif // RESTFUL_MAPPER_MODEL_COLLECTION_H

