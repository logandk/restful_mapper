#ifndef RESTFUL_MAPPER_COLLECTOR_H
#define RESTFUL_MAPPER_COLLECTOR_H

#include <vector>
#include <string>
#include <stdexcept>

namespace restful_mapper
{

class Collector
{
public:
  Collector(const std::string &json_struct);
  ~Collector();

  std::vector<std::string> get() const;
  std::vector<std::string> get(const std::string &key) const;

private:
  void *json_tree_ptr_;

  // Disallow copy
  Collector(Collector const &);      // Don't Implement
  void operator=(Collector const &); // Don't implement
};

}

#endif // RESTFUL_MAPPER_COLLECTOR_H

