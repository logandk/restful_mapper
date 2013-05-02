#include <restful_mapper/mapper.h>
#include <restful_mapper/internal/yajl_helpers.h>

using namespace std;
using namespace restful_mapper;

// Helper macros
#define JSON_GEN_HANDLE static_cast<yajl_gen>(json_gen_ptr_)
#define JSON_TREE_HANDLE static_cast<yajl_val>(json_tree_ptr_)

Mapper::Mapper(const int &flags)
{
  flags_ = flags;

  // Allocate generator
  json_gen_ptr_ = static_cast<void *>(yajl_gen_alloc(NULL));
  yajl_gen_config(JSON_GEN_HANDLE, yajl_gen_validate_utf8, 1);

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_map_open(JSON_GEN_HANDLE));
  }

  json_tree_ptr_ = NULL;
}

Mapper::Mapper(string json_struct, const int &flags)
{
  flags_ = flags;

  // Allocate generator
  json_gen_ptr_ = static_cast<void *>(yajl_gen_alloc(NULL));
  yajl_gen_config(JSON_GEN_HANDLE, yajl_gen_validate_utf8, 1);

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_map_open(JSON_GEN_HANDLE));
  }

  char errors[1024];
  json_tree_ptr_ = static_cast<void *>(yajl_tree_parse(json_struct.c_str(), errors, sizeof(errors)));

  if (json_tree_ptr_ == NULL)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
    throw runtime_error(string("JSON parse error:\n") + errors);
  }
}

Mapper::~Mapper()
{
  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
  }

  if (json_tree_ptr_)
  {
    yajl_tree_free(JSON_TREE_HANDLE);
  }
}

string Mapper::dump() const
{
  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_map_close(JSON_GEN_HANDLE));
  }

  return yajl_gen_dump(JSON_GEN_HANDLE);
}

bool Mapper::exists(const char *key) const
{
  const char * path[] = { key, (const char *) 0 };
  yajl_val v = yajl_tree_get(JSON_TREE_HANDLE, path, yajl_t_any);

  if (v && !YAJL_IS_NULL(v))
  {
    return true;
  }

  return false;
}

string Mapper::get(const char *key) const
{
  const char * path[] = { key, (const char *) 0 };
  yajl_val v = yajl_tree_get(JSON_TREE_HANDLE, path, yajl_t_any);

  if (v)
  {
    return yajl_encode(v);
  }
  else
  {
    YAJL_NOT_FOUND(key);
  }
}

void Mapper::set(const char *key, string json_struct)
{
  if (should_output_single_field() && field_filter_ != key) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  yajl_gen_json(JSON_GEN_HANDLE, json_struct.c_str());
}

void Mapper::get(const char *key, Field<int> &attr) const
{
  YAJL_GET(key, attr, INTEGER);
}

void Mapper::set(const char *key, const Field<int> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  if (attr.is_null())
  {
    yajl_gen_error(yajl_gen_null(JSON_GEN_HANDLE));
  }
  else
  {
    yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, attr));
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<double> &attr) const
{
  YAJL_GET(key, attr, DOUBLE);
}

void Mapper::set(const char *key, const Field<double> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  if (attr.is_null())
  {
    yajl_gen_error(yajl_gen_null(JSON_GEN_HANDLE));
  }
  else
  {
    yajl_gen_error(yajl_gen_double(JSON_GEN_HANDLE, attr));
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<bool> &attr) const
{
  YAJL_GET(key, attr, BOOLEAN);
}

void Mapper::set(const char *key, const Field<bool> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  if (attr.is_null())
  {
    yajl_gen_error(yajl_gen_null(JSON_GEN_HANDLE));
  }
  else
  {
    yajl_gen_error(yajl_gen_bool(JSON_GEN_HANDLE, attr));
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<string> &attr) const
{
  YAJL_GET(key, attr, UNICODE);
}

void Mapper::set(const char *key, const Field<string> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  if (attr.is_null())
  {
    yajl_gen_error(yajl_gen_null(JSON_GEN_HANDLE));
  }
  else
  {
    string value = local_to_utf8(attr);
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(value.c_str()), value.size()));
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<time_t> &attr) const
{
  YAJL_GET(key, attr, STRING);
}

void Mapper::set(const char *key, const Field<time_t> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  if (attr.is_null())
  {
    yajl_gen_error(yajl_gen_null(JSON_GEN_HANDLE));
  }
  else
  {
    string value = local_to_utf8(attr.to_iso8601(true));
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(value.c_str()), value.size()));
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Primary &attr) const
{
  YAJL_GET(key, attr, INTEGER);
}

void Mapper::set(const char *key, const Primary &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if ((flags_ & IS_RELATION) != IS_RELATION || attr.is_null()) return;

  if (!should_output_single_field())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(key), strlen(key)));
  }

  yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, attr));

  if (should_clean()) attr.clean();
}

