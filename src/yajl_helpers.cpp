#include <restful_mapper/internal/yajl_helpers.h>

using namespace std;

void yajl_gen_error(const yajl_gen_status &status)
{
  switch (status)
  {
    case yajl_gen_keys_must_be_strings:
      throw runtime_error("A non-string JSON key was passed");

    case yajl_max_depth_exceeded:
      throw runtime_error("YAJL's maximum generation depth was exceeded");

    case yajl_gen_in_error_state:
      throw runtime_error("A JSON generator function was called while in an error state");

    case yajl_gen_generation_complete:
      throw runtime_error("A complete JSON document has been generated");

    case yajl_gen_invalid_number:
      throw runtime_error("An invalid floating point value was passed to the JSON generator (infinity or NaN)");

    case yajl_gen_no_buf:
      throw runtime_error("A print callback was passed in, so there is no internal buffer to get from");

    case yajl_gen_invalid_string:
      throw runtime_error("An invalid UTF-8 string was passed to the JSON generator");
  }
}

string yajl_gen_dump(yajl_gen generator)
{
  const unsigned char *buf;
  size_t len;

  yajl_gen_error(yajl_gen_get_buf(generator, &buf, &len));

  return string(reinterpret_cast<const char *>(buf), len);
}

void yajl_gen_value(yajl_gen generator, yajl_val value)
{
  if (YAJL_IS_STRING(value))
  {
    yajl_gen_error(yajl_gen_string(generator, reinterpret_cast<const unsigned char *>(YAJL_GET_STRING(value)), strlen(YAJL_GET_STRING(value))));
  }
  else if (YAJL_IS_NUMBER(value))
  {
    yajl_gen_error(yajl_gen_number(generator, YAJL_GET_NUMBER(value), strlen(YAJL_GET_NUMBER(value))));
  }
  else if (YAJL_IS_OBJECT(value))
  {
    yajl_gen_error(yajl_gen_map_open(generator));

    for (size_t i = 0; i < YAJL_GET_OBJECT(value)->len; i++)
    {
      yajl_gen_error(yajl_gen_string(generator, reinterpret_cast<const unsigned char *>(YAJL_GET_OBJECT(value)->keys[i]), strlen(YAJL_GET_OBJECT(value)->keys[i])));
      yajl_gen_value(generator, YAJL_GET_OBJECT(value)->values[i]);
    }

    yajl_gen_error(yajl_gen_map_close(generator));
  }
  else if (YAJL_IS_ARRAY(value))
  {
    yajl_gen_error(yajl_gen_array_open(generator));

    for (size_t i = 0; i < YAJL_GET_ARRAY(value)->len; i++)
    {
      yajl_gen_value(generator, YAJL_GET_ARRAY(value)->values[i]);
    }

    yajl_gen_error(yajl_gen_array_close(generator));
  }
  else if (YAJL_IS_TRUE(value))
  {
    yajl_gen_error(yajl_gen_bool(generator, true));
  }
  else if (YAJL_IS_FALSE(value))
  {
    yajl_gen_error(yajl_gen_bool(generator, false));
  }
  else if (YAJL_IS_NULL(value))
  {
    yajl_gen_error(yajl_gen_null(generator));
  }
}

void yajl_gen_json(yajl_gen generator, const char *value)
{
  char errors[1024];
  yajl_val json_tree = yajl_tree_parse(value, errors, sizeof(value));

  if (json_tree == NULL)
  {
    throw runtime_error(string("JSON parse error:\n") + errors);
  }

  yajl_gen_value(generator, json_tree);
  yajl_tree_free(json_tree);
}

string yajl_encode(yajl_val value)
{
  string dump;

  // Allocate generator
  yajl_gen generator = yajl_gen_alloc(NULL);
  yajl_gen_config(generator, yajl_gen_validate_utf8, 1);

  try
  {
    yajl_gen_value(generator, value);

    // Convert to string
    dump = yajl_gen_dump(generator);
  }
  catch (...)
  {
    // Free generator
    yajl_gen_free(generator);

    throw;
  }

  // Free generator
  yajl_gen_free(generator);

  return dump;
}

