#include <restful_mapper/json.h>
#include <restful_mapper/internal/utf8.h>
#include <cstring>
#include <sstream>

extern "C" {
#include <yajl/yajl_gen.h>
#include <yajl/yajl_tree.h>
}

using namespace std;
using namespace restful_mapper;

// Helper macros
#define JSON_GEN_HANDLE static_cast<yajl_gen>(json_gen_ptr_)
#define JSON_TREE_HANDLE static_cast<yajl_val>(json_tree_ptr_)
#define YAJL_IS_BOOLEAN(v)  (((v) != NULL) && ((v)->type == yajl_t_true || (v)->type == yajl_t_false))
#define YAJL_GET_BOOLEAN(v) ((v)->type == yajl_t_true)

// yajl helpers
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

void yajl_wrong_type(const string &key, yajl_val value, const string &expected_type)
{
  ostringstream s;

  s << "Expected JSON node \"" << key << "\" to be " << expected_type << ", found";

  if (YAJL_IS_STRING(value)) s << " STRING: " << YAJL_GET_STRING(value);
  else if (YAJL_IS_INTEGER(value)) s << " INTEGER: " << YAJL_GET_INTEGER(value);
  else if (YAJL_IS_DOUBLE(value)) s << " DOUBLE: " << YAJL_GET_DOUBLE(value);
  else if (YAJL_IS_OBJECT(value)) s << " OBJECT";
  else if (YAJL_IS_ARRAY(value)) s << " ARRAY";
  else if (YAJL_IS_BOOLEAN(value)) s << " BOOLEAN: " << (YAJL_GET_BOOLEAN(value) ? "true" : "false");

  throw runtime_error(s.str());
}

void Json::not_found(const string &name)
{
  ostringstream s;
  s << "JSON node \"" << name <<  "\" not found";
  throw runtime_error(s.str());
}

Json::Emitter::Emitter()
{
  json_gen_ptr_ = NULL;
  reset();
}

Json::Emitter::~Emitter()
{
  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
    json_gen_ptr_ = NULL;
  }
}

void Json::Emitter::reset()
{
  output_.clear();

  if (json_gen_ptr_)
  {
    yajl_gen_free(JSON_GEN_HANDLE);
  }

  // Allocate JSON_GEN_HANDLE
  json_gen_ptr_ = static_cast<void *>(yajl_gen_alloc(NULL));
  yajl_gen_config(JSON_GEN_HANDLE, yajl_gen_validate_utf8, 1);
}

const string &Json::Emitter::dump() const
{
  if (output_.empty())
  {
    const unsigned char *buf;
    size_t len;

    yajl_gen_error(yajl_gen_get_buf(JSON_GEN_HANDLE, &buf, &len));

    output_ = string(reinterpret_cast<const char *>(buf), len);
  }

  return output_;
}

void Json::Emitter::emit(const int &value)
{
  yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, value));
}

void Json::Emitter::emit(const long long &value)
{
  yajl_gen_error(yajl_gen_integer(JSON_GEN_HANDLE, value));
}

void Json::Emitter::emit(const double &value)
{
  yajl_gen_error(yajl_gen_double(JSON_GEN_HANDLE, value));
}

void Json::Emitter::emit(const bool &value)
{
  yajl_gen_error(yajl_gen_bool(JSON_GEN_HANDLE, value));
}

void Json::Emitter::emit(const string &value)
{
  string converted = local_to_utf8(value);
  yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(converted.c_str()), converted.size()));
}

void Json::Emitter::emit(const char *value)
{
  emit(string(value));
}

void Json::Emitter::emit(const vector<int> &value)
{
  emit_array_open();

  vector<int>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(*i);
  }

  emit_array_close();
}

void Json::Emitter::emit(const vector<long long> &value)
{
  emit_array_open();

  vector<long long>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(*i);
  }

  emit_array_close();
}

void Json::Emitter::emit(const vector<double> &value)
{
  emit_array_open();

  vector<double>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(*i);
  }

  emit_array_close();
}

void Json::Emitter::emit(const vector<bool> &value)
{
  emit_array_open();

  vector<bool>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(*i);
  }

  emit_array_close();
}

void Json::Emitter::emit(const vector<string> &value)
{
  emit_array_open();

  vector<string>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(*i);
  }

  emit_array_close();
}

void Json::Emitter::emit(const map<string, int> &value)
{
  emit_map_open();

  map<string, int>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(i->first, i->second);
  }

  emit_map_close();
}

void Json::Emitter::emit(const map<string, long long> &value)
{
  emit_map_open();

  map<string, long long>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(i->first, i->second);
  }

  emit_map_close();
}

void Json::Emitter::emit(const map<string, double> &value)
{
  emit_map_open();

  map<string, double>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(i->first, i->second);
  }

  emit_map_close();
}

void Json::Emitter::emit(const map<string, bool> &value)
{
  emit_map_open();

  map<string, bool>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(i->first, i->second);
  }

  emit_map_close();
}

void Json::Emitter::emit(const map<string, string> &value)
{
  emit_map_open();

  map<string, string>::const_iterator i, i_end = value.end();
  for (i = value.begin(); i != i_end; ++i)
  {
    emit(i->first, i->second);
  }

  emit_map_close();
}

void Json::Emitter::emit(const string &key, const int &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const long long &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const double &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const bool &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const string &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const char *value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const vector<int> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const vector<long long> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const vector<double> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const vector<bool> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const vector<string> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const map<string, int> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const map<string, long long> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const map<string, double> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const map<string, bool> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit(const string &key, const map<string, string> &value)
{
  emit(key);
  emit(value);
}

void Json::Emitter::emit_tree(void *json_tree)
{
  yajl_val value = static_cast<yajl_val>(json_tree);

  if (YAJL_IS_STRING(value))
  {
    yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(YAJL_GET_STRING(value)), strlen(YAJL_GET_STRING(value))));
  }
  else if (YAJL_IS_INTEGER(value))
  {
    emit(YAJL_GET_INTEGER(value));
  }
  else if (YAJL_IS_DOUBLE(value))
  {
    emit(YAJL_GET_DOUBLE(value));
  }
  else if (YAJL_IS_OBJECT(value))
  {
    emit_map_open();

    for (size_t i = 0; i < YAJL_GET_OBJECT(value)->len; i++)
    {
      yajl_gen_error(yajl_gen_string(JSON_GEN_HANDLE, reinterpret_cast<const unsigned char *>(YAJL_GET_OBJECT(value)->keys[i]), strlen(YAJL_GET_OBJECT(value)->keys[i])));
      emit_tree(static_cast<void *>(YAJL_GET_OBJECT(value)->values[i]));
    }

    emit_map_close();
  }
  else if (YAJL_IS_ARRAY(value))
  {
    emit_array_open();

    for (size_t i = 0; i < YAJL_GET_ARRAY(value)->len; i++)
    {
      emit_tree(static_cast<void *>(YAJL_GET_ARRAY(value)->values[i]));
    }

    emit_array_close();
  }
  else if (YAJL_IS_TRUE(value))
  {
    emit(true);
  }
  else if (YAJL_IS_FALSE(value))
  {
    emit(false);
  }
  else if (YAJL_IS_NULL(value))
  {
    emit_null();
  }
}

void Json::Emitter::emit_json(const string &value)
{
  Parser p(value);

  emit_tree(p.json_tree_ptr());
}

void Json::Emitter::emit_json(const string &key, const string &value)
{
  emit(key);
  emit_json(value);
}

void Json::Emitter::emit_null()
{
  yajl_gen_error(yajl_gen_null(JSON_GEN_HANDLE));
}

void Json::Emitter::emit_null(const string &key)
{
  emit(key);
  emit_null();
}

void Json::Emitter::emit_map_open()
{
  yajl_gen_error(yajl_gen_map_open(JSON_GEN_HANDLE));
}

void Json::Emitter::emit_map_close()
{
  yajl_gen_error(yajl_gen_map_close(JSON_GEN_HANDLE));
}

void Json::Emitter::emit_array_open()
{
  yajl_gen_error(yajl_gen_array_open(JSON_GEN_HANDLE));
}

void Json::Emitter::emit_array_close()
{
  yajl_gen_error(yajl_gen_array_close(JSON_GEN_HANDLE));
}

Json::Node::Node(const string &name, void *json_node)
{
  name_ = name;
  json_tree_ptr_ = json_node;

  if (!json_tree_ptr_)
  {
    Json::not_found(name_);
  }
}

string Json::Node::dump() const
{
  Emitter e;

  e.emit_tree(json_tree_ptr_);

  return e.dump();
}

vector<string> Json::Node::dump_array() const
{
  vector<Node> nodes = to_array();
  vector<string> r;
  r.reserve(nodes.size());

  vector<Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.push_back(i->dump());
  }

  return r;
}

map<string, string> Json::Node::dump_map() const
{
  map<string, Node> nodes = to_map();
  map<string, string> r;

  map<string, Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.insert(pair<string, string>(i->first, i->second.dump()));
  }

  return r;
}

bool Json::Node::is_null() const
{
  return YAJL_IS_NULL(JSON_TREE_HANDLE);
}

bool Json::Node::is_string() const
{
  return YAJL_IS_STRING(JSON_TREE_HANDLE);
}

bool Json::Node::is_int() const
{
  return YAJL_IS_INTEGER(JSON_TREE_HANDLE);
}

bool Json::Node::is_double() const
{
  return YAJL_IS_DOUBLE(JSON_TREE_HANDLE);
}

bool Json::Node::is_bool() const
{
  return YAJL_IS_BOOLEAN(JSON_TREE_HANDLE);
}

bool Json::Node::is_array() const
{
  return YAJL_IS_ARRAY(JSON_TREE_HANDLE);
}

bool Json::Node::is_map() const
{
  return YAJL_IS_OBJECT(JSON_TREE_HANDLE);
}

string Json::Node::to_string() const
{
  if (!YAJL_IS_STRING(JSON_TREE_HANDLE))
  {
    yajl_wrong_type(name_, JSON_TREE_HANDLE, "STRING");
  }

  return utf8_to_local(YAJL_GET_STRING(JSON_TREE_HANDLE));
}

long long Json::Node::to_int() const
{
  if (!YAJL_IS_INTEGER(JSON_TREE_HANDLE))
  {
    yajl_wrong_type(name_, JSON_TREE_HANDLE, "INTEGER");
  }

  return YAJL_GET_INTEGER(JSON_TREE_HANDLE);
}

double Json::Node::to_double() const
{
  if (!YAJL_IS_DOUBLE(JSON_TREE_HANDLE))
  {
    yajl_wrong_type(name_, JSON_TREE_HANDLE, "DOUBLE");
  }

  return YAJL_GET_DOUBLE(JSON_TREE_HANDLE);
}

bool Json::Node::to_bool() const
{
  if (!YAJL_IS_BOOLEAN(JSON_TREE_HANDLE))
  {
    yajl_wrong_type(name_, JSON_TREE_HANDLE, "BOOLEAN");
  }

  return YAJL_GET_BOOLEAN(JSON_TREE_HANDLE);
}

vector<Json::Node> Json::Node::to_array() const
{
  if (!YAJL_IS_ARRAY(JSON_TREE_HANDLE))
  {
    yajl_wrong_type(name_, JSON_TREE_HANDLE, "ARRAY");
  }

  vector<Node> r;
  r.reserve(YAJL_GET_ARRAY(JSON_TREE_HANDLE)->len);

  for (size_t i = 0; i < YAJL_GET_ARRAY(JSON_TREE_HANDLE)->len; i++)
  {
    void *node_ptr = static_cast<void *>(YAJL_GET_ARRAY(JSON_TREE_HANDLE)->values[i]);

    ostringstream name;
    name << name_ << "[" << i << "]";

    r.push_back(Node(name.str(), node_ptr));
  }

  return r;
}

vector<string> Json::Node::to_string_array() const
{
  vector<Node> nodes = to_array();
  vector<string> r;
  r.reserve(nodes.size());

  vector<Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.push_back(i->to_string());
  }

  return r;
}

vector<long long> Json::Node::to_int_array() const
{
  vector<Node> nodes = to_array();
  vector<long long> r;
  r.reserve(nodes.size());

  vector<Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.push_back(i->to_int());
  }

  return r;
}

vector<double> Json::Node::to_double_array() const
{
  vector<Node> nodes = to_array();
  vector<double> r;
  r.reserve(nodes.size());

  vector<Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.push_back(i->to_double());
  }

  return r;
}

vector<bool> Json::Node::to_bool_array() const
{
  vector<Node> nodes = to_array();
  vector<bool> r;
  r.reserve(nodes.size());

  vector<Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.push_back(i->to_bool());
  }

  return r;
}

map<string, Json::Node> Json::Node::to_map() const
{
  if (!YAJL_IS_OBJECT(JSON_TREE_HANDLE))
  {
    yajl_wrong_type(name_, JSON_TREE_HANDLE, "OBJECT");
  }

  map<string, Node> r;

  for (size_t i = 0; i < YAJL_GET_OBJECT(JSON_TREE_HANDLE)->len; i++)
  {
    const char *key = YAJL_GET_OBJECT(JSON_TREE_HANDLE)->keys[i];
    void *node_ptr = static_cast<void *>(YAJL_GET_OBJECT(JSON_TREE_HANDLE)->values[i]);

    ostringstream name;
    name << name_ << "[\"" << key << "\"]";

    r.insert(pair<string, Node>(key, Node(name.str(), node_ptr)));
  }

  return r;
}

map<string, string> Json::Node::to_string_map() const
{
  map<string, Node> nodes = to_map();
  map<string, string> r;

  map<string, Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.insert(pair<string, string>(i->first, i->second.to_string()));
  }

  return r;
}

map<string, long long> Json::Node::to_int_map() const
{
  map<string, Node> nodes = to_map();
  map<string, long long> r;

  map<string, Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.insert(pair<string, long long>(i->first, i->second.to_int()));
  }

  return r;
}

map<string, double> Json::Node::to_double_map() const
{
  map<string, Node> nodes = to_map();
  map<string, double> r;

  map<string, Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.insert(pair<string, double>(i->first, i->second.to_double()));
  }

  return r;
}

map<string, bool> Json::Node::to_bool_map() const
{
  map<string, Node> nodes = to_map();
  map<string, bool> r;

  map<string, Node>::const_iterator i, i_end = nodes.end();
  for (i = nodes.begin(); i != i_end; ++i)
  {
    r.insert(pair<string, bool>(i->first, i->second.to_bool()));
  }

  return r;
}

Json::Parser::Parser()
{
  json_tree_ptr_ = NULL;
}

Json::Parser::Parser(const string &json_struct)
{
  json_tree_ptr_ = NULL;

  load(json_struct);
}

Json::Parser::~Parser()
{
  if (json_tree_ptr_)
  {
    yajl_tree_free(JSON_TREE_HANDLE);
    json_tree_ptr_ = NULL;
  }
}

bool Json::Parser::is_loaded() const
{
  return json_tree_ptr_ != NULL;
}

void Json::Parser::load(const string &json_struct)
{
  if (json_tree_ptr_)
  {
    yajl_tree_free(JSON_TREE_HANDLE);
  }

  char errors[1024];
  json_tree_ptr_ = static_cast<void *>(yajl_tree_parse(json_struct.c_str(), errors, sizeof(errors)));

  if (json_tree_ptr_ == NULL)
  {
    throw runtime_error(string("JSON parse error:\n") + errors);
  }
}

Json::Node Json::Parser::root() const
{
  if (!is_loaded())
  {
    throw runtime_error("No JSON loaded in parser");
  }

  return Node("root", json_tree_ptr_);
}

bool Json::Parser::exists(const string &key) const
{
  if (!is_loaded())
  {
    throw runtime_error("No JSON loaded in parser");
  }

  const char *path[] = { key.c_str(), (const char *) 0 };
  return yajl_tree_get(JSON_TREE_HANDLE, path, yajl_t_any) != NULL;
}

bool Json::Parser::empty(const string &key) const
{
  if (!is_loaded())
  {
    throw runtime_error("No JSON loaded in parser");
  }

  const char *path[] = { key.c_str(), (const char *) 0 };
  yajl_val v = yajl_tree_get(JSON_TREE_HANDLE, path, yajl_t_any);

  if (!v)
  {
    return true;
  }

  return YAJL_IS_NULL(v);
}

Json::Node Json::Parser::find(const string &key) const
{
  const char *path[] = { key.c_str(), (const char *) 0 };

  return find(path);
}

Json::Node Json::Parser::find(const char **key) const
{
  if (!is_loaded())
  {
    throw runtime_error("No JSON loaded in parser");
  }

  yajl_val v = yajl_tree_get(JSON_TREE_HANDLE, key, yajl_t_any);

  // Find name
  size_t i = 0;
  string name;

  while (key[i] != (const char *) 0)
  {
    name = key[i++];
  }

  return Node(name, static_cast<void *>(v));
}

