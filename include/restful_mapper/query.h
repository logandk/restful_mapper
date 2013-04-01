#ifndef QUERY_H_20130320
#define QUERY_H_20130320

#include <string>
#include <vector>

// Some macros. For the sake of brevity...
#define _OP_PAR_NONE(op) \
  Query &op() { return filter(cur_field_, #op); }

#define _OP_PAR_SINGLE(op) \
  Query &op(const int         &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const double      &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const bool        &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const std::string &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const char        *value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const Query       &value) { return filter(cur_field_, #op, value);         }

#define _OP_PAR_LIST(op) \
  Query &op(const std::vector<int>         &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const std::vector<double>      &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const std::vector<bool>        &value) { return filter(cur_field_, #op, encode(value)); } \
  Query &op(const std::vector<std::string> &value) { return filter(cur_field_, #op, encode(value)); }

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
  Query();
  ~Query();

  std::string dump();

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
  void *json_gen_ptr_;

  std::string cur_field_;
  std::string cur_reference_;

  std::vector<QueryFilter> filters_;
  std::vector<int> limit_;
  std::vector<int> offset_;
  std::vector<QueryOrderBy> order_by_;
  std::vector<bool> single_;

  // Disallow copy
  Query(Query const &);          // Don't Implement
  void operator=(Query const &); // Don't implement

  void reset_json();

  std::string encode(const int &value);
  std::string encode(const double &value);
  std::string encode(const bool &value);
  std::string encode(const std::string &value);
  std::string encode(const char *value);
  std::string encode(const std::vector<int> &value);
  std::string encode(const std::vector<double> &value);
  std::string encode(const std::vector<bool> &value);
  std::string encode(const std::vector<std::string> &value);
};

}

#endif // QUERY_H_20130320

