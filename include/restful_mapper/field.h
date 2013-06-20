#ifndef RESTFUL_MAPPER_FIELD_H
#define RESTFUL_MAPPER_FIELD_H

#include <restful_mapper/api.h>
#include <restful_mapper/helpers.h>
#include <restful_mapper/internal/iso8601.h>
#include <cstdio>
#include <sstream>

namespace restful_mapper
{

template <class T>
class FieldBase
{
public:
  FieldBase() : is_dirty_(false), is_null_(true) {}

  virtual const T &get() const
  {
    return value_;
  }

  virtual const T &set(const T &value, const bool &keep_clean = false)
  {
    if (!keep_clean && (is_null_ || value != value_))
    {
      touch();
    }

    is_null_ = false;

    return value_ = value;
  }

  virtual void touch() const
  {
    is_dirty_ = true;
  }

  virtual void clean() const
  {
    is_dirty_ = false;
  }

  virtual void clear(const bool &keep_clean = false) = 0;

  virtual const bool &is_dirty() const
  {
    return is_dirty_;
  }

  virtual const bool &is_null() const
  {
    return is_null_;
  }

  virtual std::string name() = 0;

  operator T() const
  {
    return get();
  }

protected:
  T value_;
  mutable bool is_dirty_;
  bool is_null_;

  virtual void clear_(const T &null_value, const bool &keep_clean = false)
  {
    set(null_value, keep_clean);
    is_null_ = true;
  }

};

template <class T>
std::ostream &operator<<(std::ostream &out, const FieldBase<T> &field)
{
  return out << field.get();
}

template <class T>
class Field : public FieldBase<T> {};

template <>
class Field<bool> : public FieldBase<bool>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<bool>() { value_ = false; }
  const bool &operator=(const bool &value) { return set(value); }
  const Field<bool> &operator=(const FieldBase<bool> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(bool)); }
  virtual void clear(const bool &keep_clean = false) { clear_(false, keep_clean); };
};

template <>
class Field<int> : public FieldBase<int>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<int>() { value_ = 0; }
  const int &operator=(const int &value) { return set(value); }
  const Field<int> &operator=(const FieldBase<int> &value) { set(value); is_null_ = value.is_null(); return *this; }
  const Field<int> &operator=(const FieldBase<long long> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(int)); }
  virtual void clear(const bool &keep_clean = false) { clear_(0, keep_clean); };
};

template <>
class Field<long long> : public FieldBase<long long>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<long long>() { value_ = 0; }
  const long long &operator=(const long long &value) { return set(value); }
  const Field<long long> &operator=(const FieldBase<long long> &value) { set(value); is_null_ = value.is_null(); return *this; }
  const Field<long long> &operator=(const FieldBase<int> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(long long)); }
  virtual void clear(const bool &keep_clean = false) { clear_(0, keep_clean); };
};

template <>
class Field<double> : public FieldBase<double>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<double>() { value_ = 0.0; }
  const double &operator=(const double &value) { return set(value); }
  const Field<double> &operator=(const FieldBase<double> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(double)); }
  virtual void clear(const bool &keep_clean = false) { clear_(0.0, keep_clean); };
};

template <>
class Field<std::string> : public FieldBase<std::string>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<std::string>() { value_ = ""; }
  const std::string &operator=(const std::string &value) { return set(value); }
  const Field<std::string> &operator=(const FieldBase<std::string> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(std::string)); }
  virtual void clear(const bool &keep_clean = false) { clear_("", keep_clean); };

  // Reimplement std::string
  typedef std::string::value_type             value_type;
  typedef std::string::traits_type            traits_type;
  typedef std::string::allocator_type         allocator_type;
  typedef std::string::reference              reference;
  typedef std::string::const_reference        const_reference;
  typedef std::string::pointer                pointer;
  typedef std::string::const_pointer          const_pointer;
  typedef std::string::iterator               iterator;
  typedef std::string::const_iterator         const_iterator;
  typedef std::string::reverse_iterator       reverse_iterator;
  typedef std::string::const_reverse_iterator const_reverse_iterator;
  typedef std::string::difference_type        difference_type;
  typedef std::string::size_type              size_type;

  iterator begin() { return value_.begin(); }
  const_iterator begin() const { return value_.begin(); }
  iterator end() { return value_.end(); }
  const_iterator end() const { return value_.end(); }
  reverse_iterator rbegin() { return value_.rbegin(); }
  const_reverse_iterator rbegin() const { return value_.rbegin(); }
  reverse_iterator rend() { return value_.rend(); }
  const_reverse_iterator rend() const { return value_.rend(); }
  size_t size() const { return value_.size(); }
  size_t length() const { return value_.length(); }
  size_t max_size() const { return value_.max_size(); }
  void resize(size_t n) { touch(); value_.resize(n); }
  void resize(size_t n, char c) { touch(); value_.resize(n, c); }
  size_t capacity() const { return value_.capacity(); }
  void reserve(size_t n = 0) { touch(); value_.reserve(n); }
  bool empty() const { return value_.empty(); }
  char &operator[](size_t pos) { return value_[pos]; }
  const char &operator[](size_t pos) const { return value_[pos]; }
  char &at(size_t pos) { return value_.at(pos); }
  const char &at(size_t pos) const { return value_.at(pos); }
  Field<std::string> &operator+=(const std::string &str) { touch(); value_ += str; return *this; }
  Field<std::string> &operator+=(const char* s) { touch(); value_ += s; return *this; }
  Field<std::string> &operator+=(char c) { touch(); value_ += c; return *this; }
  Field<std::string> &append(const std::string &str) { touch(); value_.append(str); return *this; }
  Field<std::string> &append(const std::string &str, size_t subpos, size_t sublen) { touch(); value_.append(str, subpos, sublen); return *this; }
  Field<std::string> &append(const char* s) { touch(); value_.append(s); return *this; }
  Field<std::string> &append(const char* s, size_t n) { touch(); value_.append(s, n); return *this; }
  Field<std::string> &append(size_t n, char c) { touch(); value_.append(n, c); return *this; }
  template <class InputIterator> Field<std::string> &append(InputIterator first, InputIterator last) { value_.append(first, last); return *this; }
  void push_back(char c) { touch(); value_.push_back(c); }
  Field<std::string> &assign(const std::string &str) { touch(); value_.assign(str); return *this; }
  Field<std::string> &assign(const std::string &str, size_t subpos, size_t sublen) { touch(); value_.assign(str, subpos, sublen); return *this; }
  Field<std::string> &assign(const char* s) { touch(); value_.assign(s); return *this; }
  Field<std::string> &assign(const char* s, size_t n) { touch(); value_.assign(s, n); return *this; }
  Field<std::string> &assign(size_t n, char c) { touch(); value_.assign(n, c); return *this; }
  template <class InputIterator> Field<std::string> &assign(InputIterator first, InputIterator last) { value_.assign(first, last); return *this; }
  Field<std::string> &insert(size_t pos, const std::string &str) { touch(); value_.insert(pos, str); return *this; }
  Field<std::string> &insert(size_t pos, const std::string &str, size_t subpos, size_t sublen) { touch(); value_.insert(pos, str, subpos, sublen); return *this; }
  Field<std::string> &insert(size_t pos, const char* s) { touch(); value_.insert(pos, s); return *this; }
  Field<std::string> &insert(size_t pos, const char* s, size_t n) { touch(); value_.insert(pos, s, n); return *this; }
  Field<std::string> &insert(size_t pos, size_t n, char c) { touch(); value_.insert(pos, n, c); return *this; }
  void insert(iterator p, size_t n, char c) { touch(); value_.insert(p, n, c); }
  iterator insert(iterator p, char c) { touch(); return value_.insert(p, c); }
  template <class InputIterator> void insert(iterator p, InputIterator first, InputIterator last) { return value_.insert(first, last); }
  Field<std::string> &erase(size_t pos = 0, size_t len = npos) { touch(); value_.erase(pos, len); return *this; }
  iterator erase(iterator p) { touch(); return value_.erase(p); }
  iterator erase(iterator first, iterator last) { touch(); return value_.erase(first, last); }
  Field<std::string> &replace(size_t pos,  size_t len,  const std::string &str) { touch(); value_.replace(pos, len, str); return *this; }
  Field<std::string> &replace(iterator i1, iterator i2, const std::string &str) { touch(); value_.replace(i1, i2, str); return *this; }
  Field<std::string> &replace(size_t pos,  size_t len,  const std::string &str, size_t subpos, size_t sublen) { touch(); value_.replace(pos, len, str, subpos, sublen); return *this; }
  Field<std::string> &replace(size_t pos,  size_t len,  const char* s) { touch(); value_.replace(pos, len, s); return *this; }
  Field<std::string> &replace(iterator i1, iterator i2, const char* s) { touch(); value_.replace(i1, i2, s); return *this; }
  Field<std::string> &replace(size_t pos,  size_t len,  const char* s, size_t n) { touch(); value_.replace(pos, len, s, n); return *this; }
  Field<std::string> &replace(iterator i1, iterator i2, const char* s, size_t n) { touch(); value_.replace(i1, i2, s, n); return *this; }
  Field<std::string> &replace(size_t pos,  size_t len,  size_t n, char c) { touch(); value_.replace(pos, len, n, c); return *this; }
  Field<std::string> &replace(iterator i1, iterator i2, size_t n, char c) { touch(); value_.replace(i1, i2, n, c); return *this; }
  template <class InputIterator> Field<std::string> &replace(iterator i1, iterator i2, InputIterator first, InputIterator last) { value_.replace(i1, i2, first, last); return *this; }
  void swap(std::string &str) { touch(); value_.swap(str); }
  const char* c_str() const { return value_.c_str(); }
  const char* data() const { return value_.data(); }
  allocator_type get_allocator() const { return value_.get_allocator(); }
  size_t copy(char* s, size_t n, size_t pos = 0) const { return value_.copy(s, n, pos); }
  size_t find(const std::string &str, size_t pos = 0) const { return value_.find(str, pos); }
  size_t find(const char* s, size_t pos = 0) const { return value_.find(s, pos); }
  size_t find(const char* s, size_t pos, size_t n) const { return value_.find(s, pos, n); }
  size_t find(char c, size_t pos = 0) const { return value_.find(c, pos); }
  size_t rfind(const std::string &str, size_t pos = npos) const { return value_.rfind(str, pos); }
  size_t rfind(const char* s, size_t pos = npos) const { return value_.rfind(s, pos); }
  size_t rfind(const char* s, size_t pos, size_t n) const { return value_.rfind(s, pos, n); }
  size_t rfind(char c, size_t pos = npos) const { return value_.rfind(c, pos); }
  size_t find_first_of(const std::string &str, size_t pos = 0) const { return value_.find_first_of(str, pos); }
  size_t find_first_of(const char* s, size_t pos = 0) const { return value_.find_first_of(s, pos); }
  size_t find_first_of(const char* s, size_t pos, size_t n) const { return value_.find_first_of(s, pos, n); }
  size_t find_first_of(char c, size_t pos = 0) const { return value_.find_first_of(c, pos); }
  size_t find_last_of(const std::string &str, size_t pos = npos) const { return value_.find_last_of(str, pos); }
  size_t find_last_of(const char* s, size_t pos = npos) const { return value_.find_last_of(s, pos); }
  size_t find_last_of(const char* s, size_t pos, size_t n) const { return value_.find_last_of(s, pos, n); }
  size_t find_last_of(char c, size_t pos = npos) const { return value_.find_last_of(c, pos); }
  size_t find_first_not_of(const std::string &str, size_t pos = 0) const { return value_.find_first_not_of(str, pos); }
  size_t find_first_not_of(const char* s, size_t pos = 0) const { return value_.find_first_not_of(s, pos); }
  size_t find_first_not_of(const char* s, size_t pos, size_t n) const { return value_.find_first_not_of(s, pos, n); }
  size_t find_first_not_of(char c, size_t pos = 0) const { return value_.find_first_not_of(c, pos); }
  size_t find_last_not_of(const std::string &str, size_t pos = npos) const { return value_.find_last_not_of(str, pos); }
  size_t find_last_not_of(const char* s, size_t pos = npos) const { return value_.find_last_not_of(s, pos); }
  size_t find_last_not_of(const char* s, size_t pos, size_t n) const { return value_.find_last_not_of(s, pos, n); }
  size_t find_last_not_of(char c, size_t pos = npos) const { return value_.find_last_not_of(c, pos); }
  std::string substr(size_t pos = 0, size_t len = npos) const { return value_.substr(pos, len); }
  int compare(const std::string &str) const { return value_.compare(str); }
  int compare(size_t pos, size_t len, const std::string &str) const { return value_.compare(pos, len, str); }
  int compare(size_t pos, size_t len, const std::string &str, size_t subpos, size_t sublen) const { return value_.compare(pos, len, str, subpos, sublen); }
  int compare(const char* s) const { return value_.compare(s); }
  int compare(size_t pos, size_t len, const char* s) const { return value_.compare(pos, len, s); }
  int compare(size_t pos, size_t len, const char* s, size_t n) const { return value_.compare(pos, len, s, n); }

  static const size_t npos = -1;
};

inline std::string operator+(const std::string &lhs, const Field<std::string> &rhs) { return lhs + rhs.get(); }
inline std::string operator+(const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() + rhs.get(); }
inline std::string operator+(const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() + rhs; }

inline bool operator==(const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() == rhs.get(); }
inline bool operator!=(const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() != rhs.get(); }
inline bool operator< (const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() <  rhs.get(); }
inline bool operator<=(const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() <= rhs.get(); }
inline bool operator> (const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() >  rhs.get(); }
inline bool operator>=(const Field<std::string> &lhs, const Field<std::string> &rhs) { return lhs.get() >= rhs.get(); }

inline bool operator==(const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() == rhs; }
inline bool operator!=(const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() != rhs; }
inline bool operator< (const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() <  rhs; }
inline bool operator<=(const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() <= rhs; }
inline bool operator> (const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() >  rhs; }
inline bool operator>=(const Field<std::string> &lhs, const std::string &rhs) { return lhs.get() >= rhs; }

inline bool operator==(const std::string &lhs, const Field<std::string> &rhs) { return lhs == rhs.get(); }
inline bool operator!=(const std::string &lhs, const Field<std::string> &rhs) { return lhs != rhs.get(); }
inline bool operator< (const std::string &lhs, const Field<std::string> &rhs) { return lhs <  rhs.get(); }
inline bool operator<=(const std::string &lhs, const Field<std::string> &rhs) { return lhs <= rhs.get(); }
inline bool operator> (const std::string &lhs, const Field<std::string> &rhs) { return lhs >  rhs.get(); }
inline bool operator>=(const std::string &lhs, const Field<std::string> &rhs) { return lhs >= rhs.get(); }

template <>
class Field<std::time_t> : public FieldBase<std::time_t>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<std::time_t>() { value_ = 0; }
  const std::time_t &operator=(const std::time_t &value) { return set(value); }
  const Field<std::time_t> &operator=(const FieldBase<std::time_t> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(std::time_t)); }
  virtual void clear(const bool &keep_clean = false) { clear_(0, keep_clean); };

  const std::time_t &set(const std::string &value, const bool &keep_clean = false)
  {
    return FieldBase<std::time_t>::set(from_iso8601(value), keep_clean);
  }
  using FieldBase<std::time_t>::set;

  std::string to_iso8601(const bool &include_timezone = true) const
  {
    return ::to_iso8601(get(), include_timezone);
  }

  std::string to_local(const std::string &format) const
  {
    std::tm exploded_time(*std::localtime(&get()));

    char buf[1024];
    std::strftime(buf, sizeof buf, format.c_str(), &exploded_time);

    return std::string(buf);
  }

  operator std::string() const
  {
    return to_iso8601();
  }

  const std::time_t &operator=(const std::string &value)
  {
    return set(value);
  }

  int local_year() const
  {
    std::tm value(*std::localtime(&get()));

    return value.tm_year + 1900;
  }

  int local_month() const
  {
    std::tm value(*std::localtime(&get()));

    return value.tm_mon + 1;
  }

  int local_day() const
  {
    std::tm value(*std::localtime(&get()));

    return value.tm_mday;
  }

  int local_hour() const
  {
    std::tm value(*std::localtime(&get()));

    return value.tm_hour;
  }

  int local_minute() const
  {
    std::tm value(*std::localtime(&get()));

    return value.tm_min;
  }

  int local_second() const
  {
    std::tm value(*std::localtime(&get()));

    return value.tm_sec;
  }

  int utc_year() const
  {
    std::tm value(*std::gmtime(&get()));

    return value.tm_year + 1900;
  }

  int utc_month() const
  {
    std::tm value(*std::gmtime(&get()));

    return value.tm_mon + 1;
  }

  int utc_day() const
  {
    std::tm value(*std::gmtime(&get()));

    return value.tm_mday;
  }

  int utc_hour() const
  {
    std::tm value(*std::gmtime(&get()));

    return value.tm_hour;
  }

  int utc_minute() const
  {
    std::tm value(*std::gmtime(&get()));

    return value.tm_min;
  }

  int utc_second() const
  {
    std::tm value(*std::gmtime(&get()));

    return value.tm_sec;
  }
};

/**
 * @brief Represents the primary key column of a database object.
 *
 * Can only be set once, otherwise it will throw.
 */
class Primary : public FieldBase<long long>
{
public:
  // Inherit from FieldBase
  Primary() : FieldBase<long long>(), is_assigned_(false) { value_ = 0; }
  const long long &operator=(const long long &value) { return set(value); }
  virtual std::string name() { return type_info_name(typeid(long long)); }
  virtual void clear(const bool &keep_clean = false) { clear_(0, keep_clean); };

  operator std::string() const
  {
    std::ostringstream s;
    s << get();
    return s.str();
  }

  const long long &set(const long long &value, const bool &keep_clean = false)
  {
    if (is_assigned_ && value != value_)
    {
      throw std::runtime_error("Primary field is read-only");
    }

    is_assigned_ = true;
    return FieldBase<long long>::set(value, keep_clean);
  }

protected:
  bool is_assigned_;
};

/**
 * @brief Represents the foreign key column of a database object.
 */
template <class T>
class Foreign : public FieldBase<long long>
{
public:
  // Inherit from FieldBase
  Foreign() : FieldBase<long long>() { value_ = 0; }
  const long long &operator=(const long long &value) { return set(value); }
  const Foreign &operator=(const FieldBase<long long> &value) { set(value); is_null_ = value.is_null(); return *this; }
  const Foreign &operator=(const FieldBase<int> &value) { set(value); is_null_ = value.is_null(); return *this; }
  virtual std::string name() { return type_info_name(typeid(long long)); }
  virtual void clear(const bool &keep_clean = false) { clear_(0, keep_clean); };

  const std::string &class_name() const
  {
    return T::class_name();
  }
};

}

#endif // RESTFUL_MAPPER_FIELD_H

