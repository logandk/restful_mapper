#include <restful_mapper/query.h>

using namespace std;
using namespace restful_mapper;

string Query::dump()
{
  json_.emit_map_open();

  if (!filters_.empty())
  {
    json_.emit("filters");
    json_.emit_array_open();

    vector<QueryFilter>::const_iterator i, i_end = filters_.end();
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

    vector<QueryOrderBy>::const_iterator i, i_end = order_by_.end();
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

  return json_.dump();
}

