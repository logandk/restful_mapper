#include "collector.h"
#include "json.h"

using namespace std;
using namespace restful_mapper;

// Helper macros
#define JSON_TREE_HANDLE static_cast<yajl_val>(json_tree_ptr_)

Collector::Collector(const string &json_struct)
{
  char errors[1024];
  json_tree_ptr_ = static_cast<void *>(yajl_tree_parse(json_struct.c_str(), errors, sizeof(errors)));

  if (json_tree_ptr_ == NULL)
  {
    throw runtime_error(string("JSON parse error:\n") + errors);
  }
}

Collector::~Collector()
{
  if (json_tree_ptr_)
  {
    yajl_tree_free(JSON_TREE_HANDLE);
  }
}

vector<string> Collector::get() const
{
  vector<string> objects;

  yajl_val v = JSON_TREE_HANDLE;

  if (YAJL_IS_ARRAY(v))
  {
    yajl_val *values = YAJL_GET_ARRAY(v)->values;
    size_t count     = YAJL_GET_ARRAY(v)->len;

    for (size_t i = 0; i < count; i++)
    {
      objects.push_back(yajl_encode(values[i]));
    }
  }
  else if (!YAJL_IS_NULL(v))
  {
    YAJL_WRONG_TYPE("root", v, ARRAY);
  }

  return objects;
}

vector<string> Collector::get(const string &key) const
{
  vector<string> objects;

  const char * path[] = { key.c_str(), (const char *) 0 };
  yajl_val v = yajl_tree_get(JSON_TREE_HANDLE, path, yajl_t_any);

  if (v)
  {
    if (YAJL_IS_ARRAY(v))
    {
      yajl_val *values = YAJL_GET_ARRAY(v)->values;
      size_t count     = YAJL_GET_ARRAY(v)->len;

      for (size_t i = 0; i < count; i++)
      {
        objects.push_back(yajl_encode(values[i]));
      }
    }
    else if (!YAJL_IS_NULL(v))
    {
      YAJL_WRONG_TYPE(key, v, ARRAY);
    }
  }
  else
  {
    YAJL_NOT_FOUND(key);
  }

  return objects;
}

