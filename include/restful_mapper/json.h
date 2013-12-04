#ifndef RESTFUL_MAPPER_JSON_H
#define RESTFUL_MAPPER_JSON_H

#include <string>
#include <vector>
#include <map>

namespace restful_mapper
{

class Json
{
public:
  Json();
  ~Json();

  static std::string encode(const int &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const long long &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const double &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const bool &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::string &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const char *value) { Emitter e; e.emit(value); return e.dump(); }

  static std::string encode(const std::vector<int> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::vector<long long> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::vector<double> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::vector<bool> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::vector<std::string> &value) { Emitter e; e.emit(value); return e.dump(); }

  static std::string encode(const std::map<std::string, int> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::map<std::string, long long> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::map<std::string, double> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::map<std::string, bool> &value) { Emitter e; e.emit(value); return e.dump(); }
  static std::string encode(const std::map<std::string, std::string> &value) { Emitter e; e.emit(value); return e.dump(); }

  static void not_found(const std::string &name);

  template <class T> static T decode(const std::string &json_struct) { Parser p(json_struct); return p.root(); }

  class Emitter
  {
  public:
    Emitter();
    ~Emitter();

    void reset();
    const std::string &dump() const;

    void emit(const int &value);
    void emit(const long long &value);
    void emit(const double &value);
    void emit(const bool &value);
    void emit(const std::string &value);

    void emit(const std::vector<int> &value);
    void emit(const std::vector<long long> &value);
    void emit(const std::vector<double> &value);
    void emit(const std::vector<bool> &value);
    void emit(const std::vector<std::string> &value);
    void emit(const char *value);

    void emit(const std::map<std::string, int> &value);
    void emit(const std::map<std::string, long long> &value);
    void emit(const std::map<std::string, double> &value);
    void emit(const std::map<std::string, bool> &value);
    void emit(const std::map<std::string, std::string> &value);

    void emit(const std::string &key, const int &value);
    void emit(const std::string &key, const long long &value);
    void emit(const std::string &key, const double &value);
    void emit(const std::string &key, const bool &value);
    void emit(const std::string &key, const std::string &value);
    void emit(const std::string &key, const char *value);

    void emit(const std::string &key, const std::vector<int> &value);
    void emit(const std::string &key, const std::vector<long long> &value);
    void emit(const std::string &key, const std::vector<double> &value);
    void emit(const std::string &key, const std::vector<bool> &value);
    void emit(const std::string &key, const std::vector<std::string> &value);

    void emit(const std::string &key, const std::map<std::string, int> &value);
    void emit(const std::string &key, const std::map<std::string, long long> &value);
    void emit(const std::string &key, const std::map<std::string, double> &value);
    void emit(const std::string &key, const std::map<std::string, bool> &value);
    void emit(const std::string &key, const std::map<std::string, std::string> &value);

    void emit_tree(void *json_tree);

    void emit_json(const std::string &value);
    void emit_json(const std::string &key, const std::string &value);

    void emit_null();
    void emit_null(const std::string &key);

    void emit_map_open();
    void emit_map_close();

    void emit_array_open();
    void emit_array_close();

  private:
    void *json_gen_ptr_;
    mutable std::string output_;

    // Disallow copy
    Emitter(Emitter const &);        // Don't Implement
    void operator=(Emitter const &); // Don't implement
  };

  class Node
  {
  public:
    Node(const std::string &name, void *json_node);

    const std::string &name()
    {
      return name_;
    }

    std::string dump() const;
    std::vector<std::string> dump_array() const;
    std::map<std::string, std::string> dump_map() const;

    bool is_null() const;
    bool is_string() const;
    bool is_int() const;
    bool is_double() const;
    bool is_bool() const;
    bool is_array() const;
    bool is_map() const;

    std::string to_string() const;
    long long to_int() const;
    double to_double() const;
    bool to_bool() const;

    std::vector<Node> to_array() const;
    std::vector<std::string> to_string_array() const;
    std::vector<long long> to_int_array() const;
    std::vector<double> to_double_array() const;
    std::vector<bool> to_bool_array() const;

    std::map<std::string, Node> to_map() const;
    std::map<std::string, std::string> to_string_map() const;
    std::map<std::string, long long> to_int_map() const;
    std::map<std::string, double> to_double_map() const;
    std::map<std::string, bool> to_bool_map() const;

    operator std::string() const { return to_string(); }
    operator int() const { return to_int(); }
    operator long long() const { return to_int(); }
    operator double() const { return to_double(); }
    operator bool() const { return to_bool(); }

    operator std::vector<Node>() const { return to_array(); }
    operator std::vector<std::string>() const { return to_string_array(); }
    operator std::vector<long long>() const { return to_int_array(); }
    operator std::vector<double>() const { return to_double_array(); }
    operator std::vector<bool>() const { return to_bool_array(); }

    operator std::map<std::string, Node>() const { return to_map(); }
    operator std::map<std::string, std::string>() const { return to_string_map(); }
    operator std::map<std::string, long long>() const { return to_int_map(); }
    operator std::map<std::string, double>() const { return to_double_map(); }
    operator std::map<std::string, bool>() const { return to_bool_map(); }

    void *json_tree_ptr() const
    {
      return json_tree_ptr_;
    }

  private:
    std::string name_;
    void *json_tree_ptr_;
  };

  class Parser
  {
  public:
    Parser();
    Parser(const std::string &json_struct);
    ~Parser();

    bool is_loaded() const;
    void load(const std::string &json_struct);
    Node root() const;
    bool exists(const std::string &key) const;
    bool empty(const std::string &key) const;
    Node find(const std::string &key) const;
    Node find(const char **key) const;

    void *json_tree_ptr() const
    {
      return json_tree_ptr_;
    }

  private:
    void *json_tree_ptr_;

    // Disallow copy
    Parser(Parser const &);        // Don't Implement
    void operator=(Parser const &); // Don't implement
  };
};

}

#endif // RESTFUL_MAPPER_JSON_H

