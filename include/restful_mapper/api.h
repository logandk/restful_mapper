#ifndef API_H_20130306
#define API_H_20130306

#include <stdexcept>
#include <string>
#include <map>

namespace restful_mapper
{

enum RequestType { GET, POST, PUT, DEL };

/**
 * Lazy evaluated singleton class holding global API configuration.
 */
class Api
{
public:
  Api();
  ~Api();

  static std::string get(const std::string &endpoint)
  {
    return instance().get_(endpoint);
  }

  static std::string post(const std::string &endpoint, const std::string &body)
  {
    return instance().post_(endpoint, body);
  }

  static std::string put(const std::string &endpoint, const std::string &body)
  {
    return instance().put_(endpoint, body);
  }

  static std::string del(const std::string &endpoint)
  {
    return instance().del_(endpoint);
  }

  static std::string escape(const std::string &value)
  {
    return instance().escape_(value);
  }

  static std::string query_param(const std::string &url, const std::string &param, const std::string &value)
  {
    return instance().query_param_(url, param, value);
  }

  static std::string url()
  {
    return instance().url_;
  }

  static std::string url(const std::string &endpoint)
  {
    return instance().url_ + endpoint;
  }

  static std::string set_url(const std::string &url)
  {
    return instance().url_ = url;
  }

  static std::string proxy()
  {
    return instance().proxy_;
  }

  static std::string set_proxy(const std::string &proxy)
  {
    return instance().proxy_ = proxy;
  }

  static void clear_proxy()
  {
    instance().read_environment_proxy();
  }

  static std::string username()
  {
    return instance().username_;
  }

  static std::string set_username(const std::string &username)
  {
    return instance().username_ = username;
  }

  static std::string password()
  {
    return instance().password_;
  }

  static std::string set_password(const std::string &password)
  {
    return instance().password_ = password;
  }

private:
  std::string url_;
  std::string proxy_;
  std::string username_;
  std::string password_;
  static const char *user_agent_;
  static const char *content_type_;
  void *curl_handle_;

  // Dont forget to declare these two. You want to make sure they
  // are unaccessable otherwise you may accidently get copies of
  // your singleton appearing.
  Api(Api const &);            // Don't Implement
  void operator=(Api const &); // Don't implement

  /**
   * Return the singleton instance
   */
  static Api &instance()
  {
      static Api instance;  // Guaranteed to be destroyed, instantiated on first use

      return instance;
  }

  // Curl read callback function
  std::string send_request(const RequestType &type, const std::string &endpoint, const std::string &body) const;

  // Curl write callback function
  static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);

  // Curl read callback function
  static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userdata);

  // Check whether an error occurred
  static void check_http_error(const RequestType &type, const std::string &endpoint, long &http_code, const std::string &response_body);

  // Get environment proxy into proxy_
  void read_environment_proxy();

  // Request methods
  std::string get_(const std::string &endpoint) const;
  std::string post_(const std::string &endpoint, const std::string &body) const;
  std::string put_(const std::string &endpoint, const std::string &body) const;
  std::string del_(const std::string &endpoint) const;

  // String methods
  std::string escape_(const std::string &value) const;
  std::string query_param_(const std::string &url, const std::string &param, const std::string &value) const;
};

class ApiError : public std::runtime_error
{
public:
  explicit ApiError(const std::string &what, const int &code)
    : std::runtime_error(what), code_(code) {}
  virtual ~ApiError() throw() {}

  virtual const int &code() const
  {
    return code_;
  }

private:
  int code_;
};

class AuthenticationError : public ApiError
{
public:
  explicit AuthenticationError()
    : ApiError("Unable to authenticate using the specified credentials", 401) {}
  virtual ~AuthenticationError() throw() {}
};

class ResponseError : public ApiError
{
public:
  explicit ResponseError(const std::string &what, const int &code, const std::string &details)
    : ApiError(what, code), details_(details) {}
  virtual ~ResponseError() throw() {}

  virtual const char *details() const
  {
    return details_.c_str();
  }

private:
  std::string details_;
};

class BadRequestError : public ApiError
{
public:
  explicit BadRequestError(const std::string &json_struct);
  virtual ~BadRequestError() throw() {}

  virtual const char *what() const throw()
  {
    return what_.c_str();
  }

private:
  std::string what_;
};

class ValidationError : public ApiError
{
public:
  typedef std::map<std::string, std::string> FieldMap;

  explicit ValidationError(const std::string &json_struct);
  virtual ~ValidationError() throw() {}

  const FieldMap &errors() const
  {
    return errors_;
  }

  virtual const char *what() const throw()
  {
    return what_.c_str();
  }

  std::string operator[](const std::string &field) const
  {
    FieldMap::const_iterator i = errors_.find(field);

    if (i != errors_.end())
    {
      return i->second;
    }

    return "";
  }

private:
  FieldMap errors_;
  std::string what_;
};

}

#endif // API_H_20130306
