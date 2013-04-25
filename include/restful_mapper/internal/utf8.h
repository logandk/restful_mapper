#ifndef RESTFUL_MAPPER_UTF8_H_20130326
#define RESTFUL_MAPPER_UTF8_H_20130326

#include <string>
#include <stdexcept>

extern std::string local_charset;
std::string iconv_string(const std::string &value, const char *to, const char *from);
std::string local_to_utf8(const std::string &value);
std::string utf8_to_local(const std::string &value);

#endif // RESTFUL_MAPPER_UTF8_H_20130326

