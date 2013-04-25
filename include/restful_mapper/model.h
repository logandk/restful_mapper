#ifndef RESTFUL_MAPPER_MODEL_H
#define RESTFUL_MAPPER_MODEL_H

#include <restful_mapper/api.h>
#include <restful_mapper/mapper.h>
#include <restful_mapper/query.h>

namespace restful_mapper
{

template <class T>
class Model
{
public:
  typedef ModelCollection<T> Collection;

  Model() : exists_(false) {}

  virtual void map_set(Mapper &mapper) const
  {
    throw std::logic_error(std::string("map_set not implemented for ") + typeid(T).name());
  }

  virtual void map_get(const Mapper &mapper)
  {
    throw std::logic_error(std::string("map_get not implemented for ") + typeid(T).name());
  }

  virtual std::string endpoint() const
  {
    throw std::logic_error(std::string("endpoint not implemented for ") + typeid(T).name());
  }

  virtual const Primary &primary() const
  {
    throw std::logic_error(std::string("primary not implemented for ") + typeid(T).name());
  }

  void from_json(std::string values, const int &flags = 0)
  {
    const_cast<Primary &>(primary()) = Primary(); // Reset primary

    Mapper mapper(values, flags);
    map_get(mapper);

    exists_ = true;
  }

  std::string to_json(const int &flags = 0) const
  {
    Mapper request_mapper(flags);
    map_set(request_mapper);

    return request_mapper.dump();
  }

  bool is_dirty() const
  {
    return to_json(NO_CLEAN).size() > 2;
  }

  void reload()
  {
    if (exists())
    {
      from_json(Api::get(url()));
    }
  }

  void destroy()
  {
    if (exists())
    {
      Api::del(url());

      // Reload all attributes
      from_json(to_json(FORCE_DIRTY), FORCE_DIRTY | IGNORE_MISSING);
      const_cast<Primary &>(primary()).clear();

      exists_ = false;
    }
  }

  void save()
  {
    if (exists())
    {
      from_json(Api::put(url(), to_json()), IGNORE_MISSING);
    }
    else
    {
      from_json(Api::post(url(), to_json()), IGNORE_MISSING);
    }
  }

  virtual T clone() const
  {
    T cloned;

    cloned.from_json(to_json(NO_CLEAN | FORCE_DIRTY), FORCE_DIRTY | IGNORE_MISSING);

    return cloned;
  }

  static T find(const int &id)
  {
    T instance;
    const_cast<Primary &>(instance.primary()).set(id, true);

    instance.exists_ = true;
    instance.reload();

    return instance;
  }

  static Collection find_all()
  {
    Collection objects;

    Collector collector(Api::get(T().url()));

    std::vector<std::string> partials = collector.get("objects");
    std::vector<std::string>::const_iterator i, i_end = partials.end();

    for (i = partials.begin(); i != i_end; ++i)
    {
      T instance;
      instance.from_json(*i);

      objects.push_back(instance);
    }

    return objects;
  }

  static T find(Query &query)
  {
    T instance;

    std::string url = Api::query_param(instance.url(), "q", query.single().dump());
    instance.from_json(Api::get(url));

    return instance;
  }

  static Collection find_all(Query &query)
  {
    Collection objects;

    std::string url = Api::query_param(T().url(), "q", query.dump());
    Collector collector(Api::get(url));

    std::vector<std::string> partials = collector.get("objects");
    std::vector<std::string>::const_iterator i, i_end = partials.end();

    for (i = partials.begin(); i != i_end; ++i)
    {
      T instance;
      instance.from_json(*i);

      objects.push_back(instance);
    }

    return objects;
  }

  std::string url() const
  {
    if (exists())
    {
      return endpoint() + "/" + std::string(primary());
    }
    else
    {
      return endpoint();
    }
  }

  const bool &exists() const
  {
    return exists_;
  }

protected:
  bool exists_;
};

}

#endif // RESTFUL_MAPPER_MODEL_H

