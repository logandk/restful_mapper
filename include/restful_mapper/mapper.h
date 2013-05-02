#ifndef RESTFUL_MAPPER_MAPPER_H
#define RESTFUL_MAPPER_MAPPER_H

#include <restful_mapper/field.h>
#include <restful_mapper/relation.h>

namespace restful_mapper
{

enum MapperConfig
{
  IGNORE_MISSING = 1,
  IS_RELATION    = 2,
  FORCE_DIRTY    = 4,
  NO_CLEAN       = 8,
  SINGLE_FIELD   = 16
};

class Mapper
{
public:
  Mapper(const int &flags = 0);
  Mapper(std::string json_struct, const int &flags = 0);
  ~Mapper();

  const int &flags() const
  {
    return flags_;
  }

  void set_flags(const int &flags)
  {
    flags_ = flags;
  }

  const std::string &field_filter() const
  {
    return field_filter_;
  }

  void set_field_filter(const std::string &field_filter)
  {
    field_filter_ = field_filter;
  }

  std::string dump() const;

  bool exists(const char *key) const;

  std::string get(const char *key) const;
  void set(const char *key, std::string json_struct);

  void get(const char *key, Field<int> &attr) const;
  void set(const char *key, const Field<int> &attr);

  void get(const char *key, Field<double> &attr) const;
  void set(const char *key, const Field<double> &attr);

  void get(const char *key, Field<bool> &attr) const;
  void set(const char *key, const Field<bool> &attr);

  void get(const char *key, Field<std::string> &attr) const;
  void set(const char *key, const Field<std::string> &attr);

  void get(const char *key, Field<std::time_t> &attr) const;
  void set(const char *key, const Field<std::time_t> &attr);

  void get(const char *key, Primary &attr) const;
  void set(const char *key, const Primary &attr);

  template <class T> void get(const char *key, HasOne<T> &attr) const
  {
    if (!exists(key)) return;

    attr.from_json(get(key), flags_ | IS_RELATION);
  }

  template <class T> void set(const char *key, const HasOne<T> &attr)
  {
    if (should_output_single_field() && field_filter_ != key) return;
    if (should_check_dirty() && !attr.is_dirty()) return;

    set(key, attr.to_json(flags_ | IS_RELATION));

    if (should_clean()) attr.clean();
  }

  template <class T> void get(const char *key, HasMany<T> &attr) const
  {
    if (!exists(key)) return;

    attr.from_json(get(key), flags_ | IS_RELATION);
  }

  template <class T> void set(const char *key, const HasMany<T> &attr)
  {
    if (should_output_single_field() && field_filter_ != key) return;
    if (should_check_dirty() && !attr.is_dirty()) return;

    set(key, attr.to_json(flags_ | IS_RELATION));

    if (should_clean()) attr.clean();
  }

private:
  void *json_gen_ptr_;
  void *json_tree_ptr_;

  int flags_;
  std::string field_filter_;

  // Disallow copy
  Mapper(Mapper const &);         // Don't Implement
  void operator=(Mapper const &); // Don't implement

  inline bool should_clean() const
  {
    return (flags_ & NO_CLEAN) != NO_CLEAN;
  }

  inline bool should_check_dirty() const
  {
    return (flags_ & FORCE_DIRTY) != FORCE_DIRTY;
  }

  inline bool should_output_single_field() const
  {
    return (flags_ & SINGLE_FIELD) == SINGLE_FIELD;
  }
};

}

#endif // RESTFUL_MAPPER_MAPPER_H

