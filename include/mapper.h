#ifndef MAPPER_H_20130307
#define MAPPER_H_20130307

#include "field.h"
#include "relation.h"

namespace restful_mapper
{

enum MapperConfig
{
  IGNORE_MISSING = 1,
  IS_RELATION    = 2,
  FORCE_DIRTY    = 4,
  NO_CLEAN       = 8
};

class Mapper
{
public:
  Mapper(const int &flags = 0);
  Mapper(std::string json_struct, const int &flags = 0);
  ~Mapper();

  void set_flags(const int &flags)
  {
    flags_ = flags;
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
    if ((flags_ & FORCE_DIRTY) != FORCE_DIRTY && !attr.is_dirty()) return;

    set(key, attr.to_json(flags_ | IS_RELATION));

    if ((flags_ & NO_CLEAN) != NO_CLEAN) attr.clean();
  }

  template <class T> void get(const char *key, HasMany<T> &attr) const
  {
    if (!exists(key)) return;

    attr.from_json(get(key), flags_ | IS_RELATION);
  }

  template <class T> void set(const char *key, const HasMany<T> &attr)
  {
    if ((flags_ & FORCE_DIRTY) != FORCE_DIRTY && !attr.is_dirty()) return;

    set(key, attr.to_json(flags_ | IS_RELATION));

    if ((flags_ & NO_CLEAN) != NO_CLEAN) attr.clean();
  }

private:
  void *json_gen_ptr_;
  void *json_tree_ptr_;

  int flags_;

  // Disallow copy
  Mapper(Mapper const &);         // Don't Implement
  void operator=(Mapper const &); // Don't implement
};

}

#endif // MAPPER_H_20130307

