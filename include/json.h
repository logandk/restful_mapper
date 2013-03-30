#ifndef JSON_H_20130315
#define JSON_H_20130315

#include <stdexcept>
#include <sstream>
#include <cstring>
#include <string>

extern "C" {
#include <yajl/yajl_gen.h>
#include <yajl/yajl_tree.h>
}

#include "utf8.h"

// Helper macros
#define YAJL_IS_UNICODE(v)  YAJL_IS_STRING(v)
#define YAJL_IS_BOOLEAN(v)  (((v) != NULL) && ((v)->type == yajl_t_true || (v)->type == yajl_t_false))
#define YAJL_GET_BOOLEAN(v) ((v)->type == yajl_t_true)
#define YAJL_GET_UNICODE(v) utf8_to_local(YAJL_GET_STRING(v))

#define YAJL_NOT_FOUND(key) \
  std::ostringstream s; \
  s << "JSON node \"" << key <<  "\" not found"; \
  throw std::runtime_error(s.str()); \

#define YAJL_WRONG_TYPE(key, value, json_type) \
  std::ostringstream s; \
  \
  s << "Expected JSON node \"" << key << "\" to be " << #json_type << ", found"; \
  \
  if (YAJL_IS_STRING(value)) s << " STRING: " << YAJL_GET_STRING(value); \
  else if (YAJL_IS_INTEGER(value)) s << " INTEGER: " << YAJL_GET_INTEGER(value); \
  else if (YAJL_IS_DOUBLE(value)) s << " DOUBLE: " << YAJL_GET_DOUBLE(value); \
  else if (YAJL_IS_OBJECT(value)) s << " OBJECT"; \
  else if (YAJL_IS_ARRAY(value)) s << " ARRAY"; \
  else if (YAJL_IS_BOOLEAN(value)) s << " BOOLEAN: " << (YAJL_GET_BOOLEAN(value) ? "true" : "false"); \
  \
  throw std::runtime_error(s.str()); \


#define YAJL_GET(key, target, json_type) \
  const char * path[] = { key, (const char *) 0 }; \
  yajl_val v = yajl_tree_get(static_cast<yajl_val>(json_tree_ptr_), path, yajl_t_any); \
  \
  if (v) \
  { \
    if (YAJL_IS_##json_type(v)) \
    { \
      target.set(YAJL_GET_##json_type(v), true); \
    } \
    else if (YAJL_IS_NULL(v)) \
    { \
      target.clear(true); \
    } \
    else \
    { \
      YAJL_WRONG_TYPE(key, v, json_type); \
    } \
  } \
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING) \
  { \
    YAJL_NOT_FOUND(key); \
  } \
  \
  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY) \
  { \
    target.touch(); \
  }

// Global helper functions
void yajl_gen_error(const yajl_gen_status &status);
std::string yajl_gen_dump(yajl_gen generator);
void yajl_gen_value(yajl_gen generator, yajl_val value);
void yajl_gen_json(yajl_gen generator, const char *value);
std::string yajl_encode(yajl_val value);

#endif // JSON_H_20130315

