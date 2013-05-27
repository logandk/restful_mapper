#ifndef RESTFUL_MAPPER_HELPERS_H
#define RESTFUL_MAPPER_HELPERS_H

#include <string>
#include <typeinfo>

#ifdef __GNUG__
#  include <cxxabi.h>
#endif

inline std::string type_info_name(const std::type_info &info)
{
# ifdef __GNUG__
  int status;
  char *res = abi::__cxa_demangle(info.name(), NULL, NULL, &status);

  const char *const demangled_name = (status==0) ? res : info.name();
  std::string ret_val(demangled_name);
  free(res);

  return ret_val;
# else
  return info.name();
# endif
}

#endif // RESTFUL_MAPPER_HELPERS_H

