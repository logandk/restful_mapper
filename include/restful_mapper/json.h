#ifndef RESTFUL_MAPPER_JSON_H
#define RESTFUL_MAPPER_JSON_H

#include <string>
#include <vector>

namespace restful_mapper
{

/**
 * Lazy evaluated singleton class providing json encoding.
 */
class Json
{
public:
  Json();
  ~Json();

  static std::string encode(const int &value) { return instance().encode_(value); }
  static std::string encode(const double &value) { return instance().encode_(value); }
  static std::string encode(const bool &value) { return instance().encode_(value); }
  static std::string encode(const std::string &value) { return instance().encode_(value); }
  static std::string encode(const char *value) { return instance().encode_(value); }
  static std::string encode(const std::vector<int> &value) { return instance().encode_(value); }
  static std::string encode(const std::vector<double> &value) { return instance().encode_(value); }
  static std::string encode(const std::vector<bool> &value) { return instance().encode_(value); }
  static std::string encode(const std::vector<std::string> &value) { return instance().encode_(value); }

private:
  void *json_gen_ptr_;

  // Dont forget to declare these two. You want to make sure they
  // are unaccessable otherwise you may accidently get copies of
  // your singleton appearing.
  Json(Json const &);    // Don't Implement
  void operator=(Json const &); // Don't implement

  /**
   * Return the singleton instance
   */
  static Json &instance()
  {
      static Json instance;  // Guaranteed to be destroyed, instantiated on first use

      return instance;
  }

  void reset_json();

  std::string encode_(const int &value);
  std::string encode_(const double &value);
  std::string encode_(const bool &value);
  std::string encode_(const std::string &value);
  std::string encode_(const char *value);
  std::string encode_(const std::vector<int> &value);
  std::string encode_(const std::vector<double> &value);
  std::string encode_(const std::vector<bool> &value);
  std::string encode_(const std::vector<std::string> &value);
};

}

#endif // RESTFUL_MAPPER_JSON_H

