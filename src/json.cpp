#include <restful_mapper/json.h>
#include <restful_mapper/internal/yajl_helpers.h>

using namespace std;
using namespace restful_mapper;

// Helper macros
#define JSON_GEN_HANDLE static_cast<yajl_gen>(json_gen_ptr_)

Json::Json()
{
  json_gen_ptr_ = NULL;
}

Json::~Json()
{
  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
  }
}

void Json::reset_json()
{
  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
  }

  // Allocate JSON_GEN_HANDLE
  json_gen_ptr_ = static_cast<void *>(yajl_gen_alloc(NULL));
  yajl_gen_config(JSON_GEN_HANDLE, yajl_gen_validate_utf8, 1);
}

string Json::encode_(const int &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, value));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Json::encode_(const double &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_double(JSON_GEN_HANDLE, value));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Json::encode_(const bool &value)
{
  reset_json();
  yajl_gen_error(yajl_gen_bool(JSON_GEN_HANDLE, value));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Json::encode_(const string &value)
{
  reset_json();
  string converted = local_to_utf8(value);
  yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(converted.c_str()), converted.size()));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Json::encode_(const char *value)
{
  reset_json();
  string converted = local_to_utf8(value);
  yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(converted.c_str()), converted.size()));
  return yajl_gen_dump(JSON_GEN_HANDLE);
}

string Json::encode_(const std::vector<int> &value)
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

string Json::encode_(const std::vector<double> &value)
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

string Json::encode_(const std::vector<bool> &value)
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

string Json::encode_(const std::vector<string> &value)
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
