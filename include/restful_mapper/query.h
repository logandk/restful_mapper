#ifndef RESTFUL_MAPPER_QUERY_H
#define RESTFUL_MAPPER_QUERY_H

#include <restful_mapper/json.h>

// Some macros. For the sake of brevity...
#define _OP_PAR_NONE(op) \
  Query &op() { return filter(cur_field_, #op); }

#define _OP_PAR_SINGLE(op) \
  Query &op(const int         &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const long long   &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const double      &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const bool        &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const std::string &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const char        *value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const Query       &value) { return filter(cur_field_, #op, value);         }

#define _OP_PAR_LIST(op) \
  Query &op(const std::vector<int>         &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const std::vector<long long>   &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const std::vector<double>      &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const std::vector<bool>        &value) { return filter(cur_field_, #op, Json::encode(value)); } \
  Query &op(const std::vector<std::string> &value) { return filter(cur_field_, #op, Json::encode(value)); }

namespace restful_mapper
{

struct QueryFilter
{
  std::string field;
  std::string operation;
  std::string value;
  bool has_value;
  bool is_reference;
};

struct QueryOrderBy
{
  std::string field;
  std::string direction;
};

class Query
{
public:
  const std::string &dump()
  {
    if (output_.empty())
    {
      json_.emit_map_open();

      if (!filters_.empty())
      {
        json_.emit("filters");
        json_.emit_array_open();

        std::vector<QueryFilter>::const_iterator i, i_end = filters_.end();
        for (i = filters_.begin(); i != i_end; ++i)
        {
          json_.emit_map_open();

          json_.emit("name", i->field);
          json_.emit("op", i->operation);

          if (i->is_reference)
          {
            json_.emit("field", i->value);
          }
          else if (i->has_value)
          {
            json_.emit_json("val", i->value);
          }

          json_.emit_map_close();
        }

        json_.emit_array_close();
      }

      if (!limit_.empty())
      {
        json_.emit("limit", limit_.back());
      }

      if (!offset_.empty())
      {
        json_.emit("offset", offset_.back());
      }

      if (!order_by_.empty())
      {
        json_.emit("order_by");
        json_.emit_array_open();

        std::vector<QueryOrderBy>::const_iterator i, i_end = order_by_.end();
        for (i = order_by_.begin(); i != i_end; ++i)
        {
          json_.emit_map_open();

          json_.emit("field", i->field);
          json_.emit("direction", i->direction);

          json_.emit_map_close();
        }

        json_.emit_array_close();
      }

      if (!single_.empty())
      {
        json_.emit("single", single_.back());
      }

      json_.emit_map_close();

      output_ = json_.dump();
    }

    return output_;
  }

  Query &operator()(const std::string &name)
  {
    cur_field_ = name;
    return *this;
  }

  Query &field(const std::string &name)
  {
    cur_reference_ = name;
    return *this;
  }

  Query &filter(std::string name, std::string operation)
  {
    QueryFilter filter;

    filter.field        = name;
    filter.operation    = operation;
    filter.value        = "";
    filter.has_value    = false;
    filter.is_reference = false;

    filters_.push_back(filter);

    return *this;
  }

  Query &filter(std::string name, std::string operation, std::string value)
  {
    QueryFilter filter;

    filter.field        = name;
    filter.operation    = operation;
    filter.value        = value;
    filter.has_value    = true;
    filter.is_reference = false;

    filters_.push_back(filter);

    return *this;
  }

  Query &filter(std::string name, std::string operation, const Query &value)
  {
    QueryFilter filter;

    filter.field        = name;
    filter.operation    = operation;
    filter.value        = value.cur_reference_;
    filter.has_value    = true;
    filter.is_reference = true;

    filters_.push_back(filter);

    return *this;
  }

  Query &limit(const int &value)
  {
    limit_.push_back(value);
    return *this;
  }

  Query &offset(const int &value)
  {
    offset_.push_back(value);
    return *this;
  }

  Query &order_by_asc(const Query &value)
  {
    QueryOrderBy order_by;

    order_by.field     = value.cur_reference_;
    order_by.direction = "asc";

    order_by_.push_back(order_by);

    return *this;
  }

  Query &order_by_desc(const Query &value)
  {
    QueryOrderBy order_by;

    order_by.field     = value.cur_reference_;
    order_by.direction = "desc";

    order_by_.push_back(order_by);

    return *this;
  }

  Query &single()
  {
    single_.push_back(true);
    return *this;
  }

  void clear()
  {
    json_.reset();
    output_.clear();
    cur_field_.clear();
    cur_reference_.clear();
    filters_.clear();
    limit_.clear();
    offset_.clear();
    order_by_.clear();
    single_.clear();
  }

  // Convenience operators
  _OP_PAR_NONE(is_null);
  _OP_PAR_NONE(is_not_null);

  _OP_PAR_SINGLE(eq);
  _OP_PAR_SINGLE(neq);
  _OP_PAR_SINGLE(gt);
  _OP_PAR_SINGLE(lt);
  _OP_PAR_SINGLE(gte);
  _OP_PAR_SINGLE(lte);
  _OP_PAR_SINGLE(like);
  _OP_PAR_SINGLE(ilike);
  _OP_PAR_SINGLE(any);
  _OP_PAR_SINGLE(has);

  _OP_PAR_LIST(in);
  _OP_PAR_LIST(not_in);

private:
  Json::Emitter json_;
  std::string output_;

  std::string cur_field_;
  std::string cur_reference_;

  std::vector<QueryFilter> filters_;
  std::vector<int> limit_;
  std::vector<int> offset_;
  std::vector<QueryOrderBy> order_by_;
  std::vector<bool> single_;
};

}

#endif // RESTFUL_MAPPER_QUERY_H

