#include <restful_mapper/mapper.h>

using namespace std;
using namespace restful_mapper;

Mapper::Mapper(const int &flags)
{
  flags_ = flags;

  if (!should_output_single_field())
  {
    emitter_.emit_map_open();
  }
}

Mapper::Mapper(string json_struct, const int &flags)
{
  flags_ = flags;

  if (!should_output_single_field())
  {
    emitter_.emit_map_open();
  }

  parser_.load(json_struct);
}

string Mapper::dump()
{
  if (!should_output_single_field())
  {
    emitter_.emit_map_close();
  }

  return emitter_.dump();
}

bool Mapper::exists(const char *key) const
{
  return parser_.exists(key) && !parser_.find(key).is_null();
}

string Mapper::get(const char *key) const
{
  return parser_.find(key).dump();
}

void Mapper::set(const char *key, string json_struct)
{
  if (should_output_single_field() && field_filter_ != key) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  emitter_.emit_json(json_struct);
}

void Mapper::get(const char *key, Field<int> &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_int(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("int");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Field<int> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  if (attr.is_null())
  {
    emitter_.emit_null();
  }
  else
  {
    emitter_.emit(attr);
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<long long> &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_int(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("int");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Field<long long> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  if (attr.is_null())
  {
    emitter_.emit_null();
  }
  else
  {
    emitter_.emit(attr);
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<double> &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_double(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("hej");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Field<double> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  if (attr.is_null())
  {
    emitter_.emit_null();
  }
  else
  {
    emitter_.emit(attr);
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<bool> &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_bool(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("hej");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Field<bool> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  if (attr.is_null())
  {
    emitter_.emit_null();
  }
  else
  {
    emitter_.emit(attr);
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<string> &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_string(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("hej");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Field<string> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  if (attr.is_null())
  {
    emitter_.emit_null();
  }
  else
  {
    emitter_.emit(attr);
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Field<time_t> &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_string(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("hej");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Field<time_t> &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if (should_check_dirty() && !attr.is_dirty()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  if (attr.is_null())
  {
    emitter_.emit_null();
  }
  else
  {
    emitter_.emit(attr.to_iso8601(true));
  }

  if (should_clean()) attr.clean();
}

void Mapper::get(const char *key, Primary &attr) const
{
  if (parser_.exists(key))
  {
    Json::Node node = parser_.find(key);

    if (node.is_null())
    {
      attr.clear(true);
    }
    else
    {
      attr.set(node.to_int(), true);
    }
  }
  else if ((flags_ & IGNORE_MISSING) != IGNORE_MISSING)
  {
    throw runtime_error("hej");
  }

  if ((flags_ & FORCE_DIRTY) == FORCE_DIRTY)
  {
    attr.touch();
  }
}

void Mapper::set(const char *key, const Primary &attr)
{
  if (should_output_single_field() && field_filter_ != key) return;
  if ((flags_ & IS_RELATION) != IS_RELATION || attr.is_null()) return;

  if (!should_output_single_field())
  {
    emitter_.emit(key);
  }

  emitter_.emit(attr.get());

  if (should_clean()) attr.clean();
}

