#include <restful_mapper/api.h>
#include <restful_mapper/meta.h>
#include <restful_mapper/internal/yajl_helpers.h>
#include <curl/curl.h>
#include <sstream>
#include <cstdlib>

using namespace std;
using namespace restful_mapper;

// Initialize user agent string
const char *Api::user_agent_ = "RestfulMapper/" VERSION;

// Initialize content type string
const char *Api::content_type_ = "application/json";

// Struct used for sending data
typedef struct
{
  const char *data;
  size_t length;
} RequestBody;

// Helper macros
#define MAKE_HEADER(name, value) (std::string(name) + ": " + std::string(value)).c_str()
#define CURL_HANDLE static_cast<CURL *>(instance().curl_handle_)

// Initialize curl
Api::Api()
{
  curl_handle_ = static_cast<void *>(curl_easy_init());

  if (!curl_handle_)
  {
    throw ApiError("Unable to initialize libcurl", 0);
  }

  // Read environment proxy
  read_environment_proxy();
}

// Free curl
Api::~Api()
{
  if (curl_handle_)
  {
    curl_easy_cleanup(CURL_HANDLE);
  }
}

/**
 * @brief HTTP GET method
 *
 * @param endpoint url to query
 *
 * @return response body
 */
string Api::get_(const string &endpoint) const
{
  return send_request(GET, endpoint, "");
}

/**
 * @brief HTTP POST method
 *
 * @param endpoint url to query
 * @param data HTTP POST body
 *
 * @return response body
 */
string Api::post_(const string &endpoint, const string &body) const
{
  return send_request(POST, endpoint, body);
}

/**
 * @brief HTTP PUT method
 *
 * @param endpoint url to query
 * @param data HTTP PUT body
 *
 * @return response body
 */
string Api::put_(const string &endpoint, const string &body) const
{
  return send_request(PUT, endpoint, body);
}

/**
 * @brief HTTP DEL method
 *
 * @param endpoint url to query
 *
 * @return response body
 */
string Api::del_(const string &endpoint) const
{
  return send_request(DEL, endpoint, "");
}

/**
 * @brief URL encode the specified string
 *
 * @param value the string to URL encode
 *
 * @return the URL encoded string
 */
string Api::escape_(const string &value) const
{
  char *curl_esc = curl_easy_escape(CURL_HANDLE, value.c_str(), value.size());
  string escaped(curl_esc);
  curl_free(curl_esc);

  return escaped;
}

/**
 * @brief Add a query parameter to the specified URL
 *
 * @param url the URL to add the parameter to
 * @param param the parameter name
 * @param value the parameter value, will be URL encoded
 *
 * @return the new URL that includes the query parameter
 */
string Api::query_param_(const string &url, const string &param, const string &value) const
{
  string query_part;

  if (url.find("http://") == 0 && url.find('/', 7) == string::npos)
  {
    query_part += "/";
  }

  if (url.find('?') != string::npos)
  {
    query_part += "&";
  }
  else
  {
    query_part += "?";
  }

  query_part += param;
  query_part += "=";
  query_part += escape_(value);

  return url + query_part;
}

/**
 * @brief wrapper to perform curl request
 *
 * @param type the request type
 * @param endpoint url to query
 * @param data HTTP PUT body
 *
 * @return
 */
string Api::send_request(const RequestType &type, const string &endpoint, const string &body) const
{
  curl_slist *header = NULL;

  // Create return struct
  string response_body;

  // Initialize request body
  RequestBody request_body;
  request_body.data   = body.c_str();
  request_body.length = body.size();

  // Reset libcurl
  curl_easy_reset(CURL_HANDLE);

  // Debug output
  //curl_easy_setopt(CURL_HANDLE, CURLOPT_VERBOSE, 1);

  // Set user agent
  curl_easy_setopt(CURL_HANDLE, CURLOPT_USERAGENT, Api::user_agent_);

  // Set query URL
  curl_easy_setopt(CURL_HANDLE, CURLOPT_URL, url(endpoint).c_str());

  // Set proxy
  curl_easy_setopt(CURL_HANDLE, CURLOPT_PROXY, proxy_.c_str());

  switch (type)
  {
    case POST:
      // Now specify we want to POST data
      curl_easy_setopt(CURL_HANDLE, CURLOPT_POST, 1L);

      // Set data size
      curl_easy_setopt(CURL_HANDLE, CURLOPT_POSTFIELDSIZE, request_body.length);

      break;

    case PUT:
      // Now specify we want to PUT data
      curl_easy_setopt(CURL_HANDLE, CURLOPT_POST, 1L);
      curl_easy_setopt(CURL_HANDLE, CURLOPT_CUSTOMREQUEST, "PUT");

      // Set data size
      curl_easy_setopt(CURL_HANDLE, CURLOPT_POSTFIELDSIZE, request_body.length);

      break;

    case DEL:
      // Set HTTP DEL METHOD
      curl_easy_setopt(CURL_HANDLE, CURLOPT_CUSTOMREQUEST, "DELETE");

      break;
  }

  switch (type)
  {
    case POST:
    case PUT:
      // Set read callback function
      curl_easy_setopt(CURL_HANDLE, CURLOPT_READFUNCTION, Api::read_callback);

      // Set data object to pass to callback function
      curl_easy_setopt(CURL_HANDLE, CURLOPT_READDATA, &request_body);

      // Set content-type header
      header = curl_slist_append(header, MAKE_HEADER("Content-Type", content_type_));

      break;
  }

  // Set callback function
  curl_easy_setopt(CURL_HANDLE, CURLOPT_WRITEFUNCTION, Api::write_callback);

  // Set data object to pass to callback function
  curl_easy_setopt(CURL_HANDLE, CURLOPT_WRITEDATA, &response_body);

  // Specify authentication information
  if (!username().empty())
  {
    curl_easy_setopt(CURL_HANDLE, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(CURL_HANDLE, CURLOPT_USERNAME, username().c_str());
    curl_easy_setopt(CURL_HANDLE, CURLOPT_PASSWORD, password().c_str());
  }

  // Set content negotiation header
  header = curl_slist_append(header, MAKE_HEADER("Accept", content_type_));
  curl_easy_setopt(CURL_HANDLE, CURLOPT_HTTPHEADER, header);

  // Prepare buffer for error messages
  char errors[CURL_ERROR_SIZE];
  curl_easy_setopt(CURL_HANDLE, CURLOPT_ERRORBUFFER, &errors);

  // Perform the actual query
  CURLcode res = curl_easy_perform(CURL_HANDLE);

  // Free header list
  curl_slist_free_all(header);

  // Handle unexpected internal errors
  if (res != 0)
  {
    throw ResponseError(curl_easy_strerror(res), res, errors);
  }

  // Handle server-side erros
  long http_code = 0;
  curl_easy_getinfo(CURL_HANDLE, CURLINFO_RESPONSE_CODE, &http_code);

  check_http_error(type, endpoint, http_code, response_body);

  return response_body;
}

/**
 * @brief write callback function for libcurl
 *
 * @param data returned data of size (size*nmemb)
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to save/work with return data
 *
 * @return (size * nmemb)
 */
size_t Api::write_callback(void *data, size_t size, size_t nmemb, void *userdata)
{
  string *r = reinterpret_cast<string *>(userdata);
  r->append(reinterpret_cast<char *>(data), size * nmemb);

  return (size * nmemb);
}

/**
 * @brief read callback function for libcurl
 *
 * @param pointer of max size (size*nmemb) to write data to
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to read data from
 *
 * @return (size * nmemb)
 */
size_t Api::read_callback(void *data, size_t size, size_t nmemb, void *userdata)
{
  // Get upload struct
  RequestBody *body = reinterpret_cast<RequestBody *>(userdata);

  // Set correct sizes
  size_t curl_size = size * nmemb;
  size_t copy_size = (body->length < curl_size) ? body->length : curl_size;

  // Copy data to buffer
  memcpy(data, body->data, copy_size);

  // Decrement length and increment data pointer
  body->length -= copy_size;
  body->data   += copy_size;

  // Return copied size
  return copy_size;
}

/**
 * @brief Check whether an error occurred
 *
 * @param type the request type
 * @param http_code the response code
 * @param response_body the response body
 */
void Api::check_http_error(const RequestType &type, const string &endpoint, long &http_code, const string &response_body)
{
  // Handle BadRequestError and ValidationError
  if (http_code == 400)
  {
    if (response_body.find("validation_errors") != string::npos)
    {
      throw ValidationError(response_body);
    }
    else
    {
      throw BadRequestError(response_body);
    }
  }

  // Handle AuthenticationError
  if (http_code == 401)
  {
    throw AuthenticationError();
  }

  // Handle other errors
  bool http_ok   = false;

  switch (type)
  {
    case GET:
      http_ok = (http_code == 200);

      break;

    case POST:
      http_ok = (http_code == 201);

      break;

    case PUT:
      http_ok = (http_code == 200 || http_code == 201);

      break;

    case DEL:
      http_ok = (http_code == 204);

      break;
  }

  if (!http_ok)
  {
    ostringstream s;
    s << "Server at \"" << url(endpoint) << "\" responded with code " << http_code;

    throw ResponseError(s.str(), http_code, response_body);
  }
}

void Api::read_environment_proxy()
{
  if (getenv("HTTP_PROXY"))
  {
    proxy_ = getenv("HTTP_PROXY");
  }
  else
  {
    proxy_.clear();
  }
}

BadRequestError::BadRequestError(const string &json_struct)
  : ApiError("", 400), what_("")
{
  yajl_val json_tree = yajl_tree_parse(json_struct.c_str(), NULL, json_struct.size());

  if (json_tree != NULL)
  {
    const char * path[] = { "message", (const char *) 0 };
    yajl_val v = yajl_tree_get(json_tree, path, yajl_t_string);

    if (v && YAJL_IS_STRING(v))
    {
      what_ = YAJL_GET_STRING(v);
    }

    yajl_tree_free(json_tree);
  }
}

ValidationError::ValidationError(const string &json_struct)
  : ApiError("", 400), what_("")
{
  string separator = "";

  yajl_val json_tree = yajl_tree_parse(json_struct.c_str(), NULL, json_struct.size());

  if (json_tree != NULL)
  {
    const char * path[] = { "validation_errors", (const char *) 0 };
    yajl_val v = yajl_tree_get(json_tree, path, yajl_t_object);

    if (v && YAJL_IS_OBJECT(v))
    {
      for (size_t i = 0; i < YAJL_GET_OBJECT(v)->len; i++)
      {
        if (YAJL_IS_STRING(YAJL_GET_OBJECT(v)->values[i]))
        {
          string field_name  = YAJL_GET_OBJECT(v)->keys[i];
          string field_value = YAJL_GET_STRING(YAJL_GET_OBJECT(v)->values[i]);

          errors_[field_name] = field_value;

          // Capitalize field name
          field_name[0] = toupper(field_name[0]);
          what_ += separator;
          what_ += field_name;
          what_ += " ";
          what_ += field_value;
          separator = "\n";
        }
      }
    }

    yajl_tree_free(json_tree);
  }
}

