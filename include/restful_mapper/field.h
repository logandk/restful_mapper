#ifndef RESTFUL_MAPPER_FIELD_H
#define RESTFUL_MAPPER_FIELD_H

#include <restful_mapper/api.h>
#include <restful_mapper/internal/iso8601.h>
#include <cstdio>
#include <sstream>
#include <typeinfo>

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

  virtual void clear(const bool &keep_clean = false)
  {
    set(0, keep_clean);
    is_null_ = true;
  }

  virtual void touch() const
  {
    is_dirty_ = true;
  }

  virtual void clean() const
  {
    is_dirty_ = false;
  }

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

  const T &operator=(const T &value)
  {
    return set(value);
  }

protected:
  T value_;
  mutable bool is_dirty_;
  bool is_null_;
};

template <class T>
class Field : public FieldBase<T>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<T>() {}
  const T &operator=(const T &value) { return FieldBase<T>::operator=(value); }
  virtual std::string name() { return typeid(T).name(); }
};

template <>
class Field<std::string> : public FieldBase<std::string>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<std::string>() {}
  const std::string &operator=(const std::string &value) { return FieldBase<std::string>::operator=(value); }
  virtual std::string name() { return typeid(std::string).name(); }

  void clear(const bool &keep_clean = false)
  {
    set("", keep_clean);
    is_null_ = true;
  }

  const char *c_str() const
  {
    return get().c_str();
  }
};

template <>
class Field<std::time_t> : public FieldBase<std::time_t>
{
public:
  // Inherit from FieldBase
  Field() : FieldBase<std::time_t>() {}
  const std::time_t &operator=(const std::time_t &value) { return FieldBase<std::time_t>::operator=(value); }
  virtual std::string name() { return typeid(std::time_t).name(); }

  const std::time_t &set(const char *value, const bool &keep_clean = false)
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

  const std::time_t &operator=(const char *value)
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
class Primary : public FieldBase<int>
{
public:
  // Inherit from FieldBase
  Primary() : FieldBase<int>(), is_assigned_(false) {}
  virtual std::string name() { return typeid(int).name(); }

  operator std::string() const
  {
    std::ostringstream s;
    s << get();
    return s.str();
  }

  const int &set(const int &value, const bool &keep_clean = false)
  {
    if (is_assigned_ && value != value_)
    {
      throw std::runtime_error("Primary field is read-only");
    }

    is_assigned_ = true;
    return FieldBase<int>::set(value, keep_clean);
  }

  const int &operator=(const int &value)
  {
    return set(value);
  }

protected:
  bool is_assigned_;
};

}

#endif // RESTFUL_MAPPER_FIELD_H

