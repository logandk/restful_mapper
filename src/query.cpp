#include "query.h"
#include "json.h"

using namespace std;
using namespace restful_mapper;

// Helper macros
#define JSON_GEN_HANDLE static_cast<yajl_gen>(json_gen_ptr_)

Query::Query()
{
  cur_field_    = "";
  json_gen_ptr_ = NULL;
}

Query::~Query()
{
  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
  }
}

void Query::reset_json()
{
  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
  }

  // Allocate JSON_GEN_HANDLE
  json_gen_ptr_ = static_cast<void *>(yajl_gen_alloc(NULL));
  yajl_gen_config(JSON_GEN_HANDLE, yajl_gen_validate_utf8, 1);
}

string Query::dump()
{
  reset_json();
  yajl_gen_error(yajl_gen_map_open(JSON_GEN_HANDLE));

  if (!filters_.empty())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("filters"), strlen("filters")));
    yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));

    vector<QueryFilter>::const_iterator i, i_end = filters_.end();
    for (i = filters_.begin(); i != i_end; ++i)
    {
      yajl_gen_error(yajl_gen_map_open(JSON_GEN_HANDLE));

      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("name"), strlen("name")));
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(i->field.c_str()), i->field.size()));
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("op"), strlen("op")));
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(i->operation.c_str()), i->operation.size()));

      if (i->is_reference)
      {
        yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("field"), strlen("field")));
        yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(i->value.c_str()), i->value.size()));
      }
      else if (i->has_value)
      {
        yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("val"), strlen("val")));
        yajl_gen_json(JSON_GEN_HANDLE, i->value.c_str());
      }

      yajl_gen_error(yajl_gen_map_close(JSON_GEN_HANDLE));
    }

    yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
  }

  if (!limit_.empty())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("limit"), strlen("limit")));
    yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, limit_.back()));
  }

  if (!offset_.empty())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("offset"), strlen("offset")));
    yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, offset_.back()));
  }

  if (!order_by_.empty())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("order_by"), strlen("order_by")));
    yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));

    vector<QueryOrderBy>::const_iterator i, i_end = order_by_.end();
    for (i = order_by_.begin(); i != i_end; ++i)
    {
      yajl_gen_error(yajl_gen_map_open(JSON_GEN_HANDLE));

      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("field"), strlen("field")));
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(i->field.c_str()), i->field.size()));
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("direction"), strlen("direction")));
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(i->direction.c_str()), i->direction.size()));

      yajl_gen_error(yajl_gen_map_close(JSON_GEN_HANDLE));
    }

    yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
  }

  if (!single_.empty())
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>("single"), strlen("single")));
    yajl_gen_error(yajl_gen_bool(JSON_GEN_HANDLE, single_.back()));
  }

  yajl_gen_error(yajl_gen_map_close(JSON_GEN_HANDLE));

  // Convert to string
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const int &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, value));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const double &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_double(JSON_GEN_HANDLE, value));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const bool &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_bool(JSON_GEN_HANDLE, value));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const string &value)
{
  reset_json();
  string converted = local_to_utf8(value);
  yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(converted.c_str()), converted.size()));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const char *value)
{
  reset_json();
  string converted = local_to_utf8(value);
  yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(converted.c_str()), converted.size()));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const std::vector<int> &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));

  std::vector<int>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, *i));
  }

  yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const std::vector<double> &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));

  std::vector<double>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    yajl_gen_error(yajl_gen_double(JSON_GEN_HANDLE, *i));
  }

  yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const std::vector<bool> &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));

  std::vector<bool>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    yajl_gen_error(yajl_gen_bool(JSON_GEN_HANDLE, *i));
  }

  yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Query::encode(const std::vector<string> &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));

  std::vector<string>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    string converted = local_to_utf8(*i);
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(converted.c_str()), converted.size()));
  }

  yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

