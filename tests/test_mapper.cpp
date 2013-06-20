// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <restful_mapper/mapper.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
class Item
{
public:
  Primary id;
  Field<int> revision;
  Field<string> task;
  BelongsTo<Item> parent;
  Foreign<Item> parent_id;

  static const std::string &class_name()
  {
    static std::string class_name = "Item";
    return class_name;
  }

  void from_json(string values, const int &flags = 0, const bool &exists = false)
  {
    Mapper m(values, flags);

    m.get("id", id);
    m.get("revision", revision);
    m.get("task", task);
    m.get("parent", parent);
    m.get("parent_id", parent_id);
  }

  std::string to_json(const int &flags = 0, const std::string &parent_model = "") const
  {
    Mapper m(flags);
    m.set_current_model("Item");
    m.set_parent_model(parent_model);

    m.set("id", id);
    m.set("revision", revision);
    m.set("task", task);
    m.set("parent", parent);
    m.set("parent_id", parent_id);

    return m.dump();
  }

  bool is_dirty() const
  {
    return id.is_dirty() || revision.is_dirty() || task.is_dirty();
  }
};

TEST(MapperTest, ParseJson)
{
  Field<int> f_int;
  Field<double> f_double;
  Field<bool> f_bool;
  Field<string> f_string;
  Field<time_t> f_time;
  Primary f_primary;

  Mapper m("{ \"priority\": 994, \"task\": \"Profit!!!\", \"completed_on\": \"2013-12-05T00:00:00\", \"time\": 0.001, \"completed\": false, \"id\": 3 }");

  m.get("priority", f_int);
  ASSERT_EQ(994, int(f_int));

  m.get("time", f_double);
  ASSERT_DOUBLE_EQ(0.001, double(f_double));

  m.get("completed", f_bool);
  ASSERT_FALSE(f_bool);

  m.get("task", f_string);
  ASSERT_STREQ("Profit!!!", f_string.c_str());

  m.get("completed_on", f_time);
  ASSERT_EQ(2013, f_time.local_year());

  m.get("id", f_primary);
  ASSERT_EQ(3, int(f_primary));

  ASSERT_FALSE(f_int.is_dirty());
  ASSERT_FALSE(f_double.is_dirty());
  ASSERT_FALSE(f_bool.is_dirty());
  ASSERT_FALSE(f_string.is_dirty());
  ASSERT_FALSE(f_time.is_dirty());
  ASSERT_FALSE(f_primary.is_dirty());
}

TEST(MapperTest, EmitJson)
{
  Field<int> f_int;
  Field<double> f_double;
  Field<bool> f_bool;
  Field<string> f_string;
  Field<time_t> f_time;
  Primary f_primary;

  Mapper m;

  f_int = 654;
  f_double = 8.0;
  f_bool = true;
  f_string = "Stoned and starving";
  f_time = "2021-12-31";
  f_primary = 7;

  ASSERT_TRUE(f_int.is_dirty());
  ASSERT_TRUE(f_double.is_dirty());
  ASSERT_TRUE(f_bool.is_dirty());
  ASSERT_TRUE(f_string.is_dirty());
  ASSERT_TRUE(f_time.is_dirty());
  ASSERT_TRUE(f_primary.is_dirty());

  m.set("priority", f_int);
  m.set("time", f_double);
  m.set("completed", f_bool);
  m.set("task", f_string);
  m.set("completed_on", f_time);
  m.set("id", f_primary);

  ASSERT_FALSE(f_int.is_dirty());
  ASSERT_FALSE(f_double.is_dirty());
  ASSERT_FALSE(f_bool.is_dirty());
  ASSERT_FALSE(f_string.is_dirty());
  ASSERT_FALSE(f_time.is_dirty());
  ASSERT_TRUE(f_primary.is_dirty());

  ASSERT_STREQ("{\"priority\":654,\"time\":8.0,\"completed\":true,\"task\":\"Stoned and starving\",\"completed_on\":\"2021-12-31T00:00:00Z\"}", m.dump().c_str());
}

TEST(MapperTest, IsRelation)
{
  Field<string> f_string;
  Primary f_primary;
  Primary f_primary2;

  Mapper m("{ \"task\": \"Profit!!!\", \"id\": 3 }", INCLUDE_PRIMARY_KEY);

  m.get("id", f_primary);
  m.get("task", f_string);

  ASSERT_EQ(3, int(f_primary));
  ASSERT_STREQ("Profit!!!", string(f_string).c_str());

  Mapper m2(INCLUDE_PRIMARY_KEY);

  f_primary2 = 10;
  f_string = "Cool";

  m2.set("task", f_string);
  m2.set("id", f_primary2);

  ASSERT_STREQ("{\"task\":\"Cool\",\"id\":10}", m2.dump().c_str());

  Mapper m3(INCLUDE_PRIMARY_KEY);

  f_string.touch();
  m3.set("task", f_string);
  m3.set("id", f_primary);

  ASSERT_STREQ("{\"task\":\"Cool\",\"id\":3}", m3.dump().c_str());

  Mapper m4(INCLUDE_PRIMARY_KEY);
  Primary f_primary3;

  f_string.touch();
  m4.set("task", f_string);
  m4.set("id", f_primary3);

  ASSERT_STREQ("{\"task\":\"Cool\"}", m4.dump().c_str());
}

TEST(MapperTest, IgnoreMissing)
{
  Field<int> f_int;
  Field<double> f_double;
  Field<bool> f_bool;
  Field<string> f_string;
  Field<time_t> f_time;
  Primary f_primary;

  Mapper m("{ \"priority\": 994, \"completed_on\": \"2013-12-05T00:00:00\" }", IGNORE_MISSING_FIELDS);

  m.get("priority", f_int);
  ASSERT_EQ(994, int(f_int));

  m.get("time", f_double);
  ASSERT_TRUE(f_double.is_null());

  m.get("completed", f_bool);
  ASSERT_TRUE(f_bool.is_null());

  m.get("task", f_string);
  ASSERT_TRUE(f_string.is_null());

  m.get("completed_on", f_time);
  ASSERT_EQ(2013, f_time.local_year());

  m.get("id", f_primary);
  ASSERT_TRUE(f_primary.is_null());

  Mapper m2("{ \"priority\": 994, \"completed_on\": \"2013-12-05T00:00:00\" }");
  m2.set_flags(IGNORE_MISSING_FIELDS);

  m2.get("priority", f_int);
  ASSERT_EQ(994, int(f_int));

  m2.get("time", f_double);
  ASSERT_TRUE(f_double.is_null());
}

TEST(MapperTest, MissingKey)
{
  Field<int> f_int;

  Mapper m("{ \"priority\": 994 }");

  ASSERT_THROW(m.get("priority2", f_int), runtime_error);
}

TEST(MapperTest, InvalidType)
{
  Field<int> f_int;

  Mapper m("{ \"priority\": 994.354 }");

  ASSERT_THROW(m.get("priority", f_int), runtime_error);

  try
  {
    m.get("priority", f_int);
  }
  catch (runtime_error &e)
  {
    ASSERT_STREQ("Expected JSON node \"priority\" to be INTEGER, found DOUBLE: 994.354", e.what());
  }
}

TEST(MapperTest, InvalidJson)
{
  ASSERT_THROW(Mapper m(""), runtime_error);
  ASSERT_THROW(Mapper m("xxx"), runtime_error);
}

TEST(MapperTest, GetJsonField)
{
  Mapper m("{ \"priority\": [1, 2, 3] }");

  ASSERT_STREQ("[1,2,3]", m.get("priority").c_str());
  ASSERT_THROW(m.get("id"), runtime_error);

  Mapper m2("{ \"priority\": null }");

  ASSERT_STREQ("null", m2.get("priority").c_str());
}

TEST(MapperTest, SetJsonField)
{
  Mapper m;

  m.set("child", "{ \"name\": \"Flaf\", \"id\": 2 }");
  ASSERT_STREQ("{\"child\":{\"name\":\"Flaf\",\"id\":2}}", m.dump().c_str());
}

TEST(MapperTest, NullValue)
{
  Field<string> f_string;
  Field<int> f_int;
  Primary f_primary;

  Mapper m;

  f_string = "Stoned and starving";
  f_primary.clear();

  f_int.clear();
  f_string.clear();

  m.set("task", f_string);
  m.set("priority", f_int);
  m.set("id", f_primary);

  ASSERT_STREQ("{\"task\":null,\"priority\":null}", m.dump().c_str());
}

TEST(MapperTest, SingleField)
{
  Field<string> f_string;
  Field<int> f_int;
  Field<bool> f_bool;
  Field<double> f_double;
  Field<time_t> f_time;
  Primary f_primary;

  f_int = 6;
  f_double = 3.0;
  f_primary = 4;

  Mapper m(OUTPUT_SINGLE_FIELD);
  m.set_field_filter("priority");

  m.set("task", f_string);
  m.set("priority", f_int);
  m.set("completed", f_bool);
  m.set("time", f_double);
  m.set("due", f_time);
  m.set("id", f_primary);

  ASSERT_STREQ("6", m.dump().c_str());

  HasOne<Item> parent;
  parent.build();
  parent->revision = 4;

  Mapper m2(OUTPUT_SINGLE_FIELD);
  m2.set_field_filter("parent");

  m2.set("parent", parent);

  ASSERT_STREQ("{\"revision\":4}", m2.dump().c_str());
}

TEST(MapperTest, SetForceDirty)
{
  Field<string> f_string;
  Field<int> f_int;
  Field<bool> f_bool;
  Field<double> f_double;
  Field<time_t> f_time;
  Primary f_primary;

  Mapper m(IGNORE_DIRTY_FLAG);

  f_int.clear();
  f_double = 3.0;
  f_primary = 4;

  m.set("task", f_string);
  m.set("priority", f_int);
  m.set("completed", f_bool);
  m.set("time", f_double);
  m.set("due", f_time);
  m.set("id", f_primary);

  ASSERT_STREQ("{\"task\":null,\"priority\":null,\"completed\":null,\"time\":3.0,\"due\":null}", m.dump().c_str());
}

TEST(MapperTest, GetTouchFields)
{
  Field<string> f_string;
  Field<int> f_int;
  Field<bool> f_bool;
  Field<double> f_double;
  Primary f_primary;

  Mapper m("{\"task\":null,\"priority\":null,\"completed\":null,\"time\":3.0}", TOUCH_FIELDS | IGNORE_MISSING_FIELDS);

  f_int.clear();
  f_double = 3.0;

  m.get("task", f_string);
  m.get("priority", f_int);
  m.get("completed", f_bool);
  m.get("time", f_double);
  m.get("id", f_primary);

  ASSERT_TRUE(f_string.is_null());
  ASSERT_TRUE(f_int.is_null());
  ASSERT_TRUE(f_bool.is_null());
  ASSERT_FALSE(f_double.is_null());
  ASSERT_TRUE(f_primary.is_null());

  ASSERT_DOUBLE_EQ(3.0, double(f_double));

  ASSERT_TRUE(f_int.is_dirty());
  ASSERT_TRUE(f_double.is_dirty());
  ASSERT_TRUE(f_bool.is_dirty());
  ASSERT_TRUE(f_string.is_dirty());
  ASSERT_TRUE(f_primary.is_dirty());
}

TEST(MapperTest, HasOneNull)
{
  Item item;
  HasOne<Item> child;
  ASSERT_FALSE(child.is_dirty());

  Mapper m("{\"revision\":5,\"id\":3,\"task\":\"do something\",\"child\":null}");

  m.get("id", item.id);
  m.get("revision", item.revision);
  m.get("task", item.task);
  m.get("child", child);

  ASSERT_TRUE(child.is_null());
  ASSERT_FALSE(child.is_dirty());

  child.build();

  ASSERT_FALSE(child.is_null());
  ASSERT_TRUE(child.is_dirty());

  HasOne<Item> child2;
  ASSERT_FALSE(child2.is_dirty());

  Mapper m2;
  m2.set("child", child2);
  ASSERT_STREQ("{}", m2.dump().c_str());

  Mapper m3(IGNORE_DIRTY_FLAG);
  m3.set("child", child2);
  ASSERT_STREQ("{\"child\":null}", m3.dump().c_str());
}

TEST(MapperTest, HasOneWithValue)
{
  Item item;
  HasOne<Item> child;

  Mapper m("{\"revision\":5,\"id\":3,\"task\":\"do something\",\"child\":{\"revision\":8,\"id\":1,\"task\":\"do something first\",\"parent_id\":2}}");

  m.get("id", item.id);
  m.get("revision", item.revision);
  m.get("task", item.task);
  m.get("child", child);

  ASSERT_FALSE(child.is_null());
  ASSERT_FALSE(child.is_dirty());

  ASSERT_EQ(1, child->id.get());
  ASSERT_EQ(8, child->revision.get());

  Mapper m2;
  m2.set("child", child);
  ASSERT_STREQ("{}", m2.dump().c_str());

  Mapper m3(IGNORE_DIRTY_FLAG);
  m3.set("child", child);
  ASSERT_STREQ("{\"child\":{\"id\":1,\"revision\":8,\"task\":\"do something first\",\"parent\":null,\"parent_id\":2}}", m3.dump().c_str());

  ASSERT_FALSE(child.is_dirty());
  ASSERT_FALSE(child->parent.is_dirty());

  child->revision = 9;

  ASSERT_TRUE(child.is_dirty());

  Mapper m4;
  m4.set("child", child);
  ASSERT_STREQ("{\"child\":{\"id\":1,\"revision\":9}}", m4.dump().c_str());
}

TEST(MapperTest, HasManyNull)
{
  Item item;
  HasMany<Item> children;
  ASSERT_FALSE(children.is_dirty());

  Mapper m("{\"revision\":5,\"id\":3,\"task\":\"do something\",\"children\":null}");

  m.get("id", item.id);
  m.get("revision", item.revision);
  m.get("task", item.task);
  m.get("children", children);

  ASSERT_FALSE(children.is_dirty());

  Item i = children.build();

  ASSERT_TRUE(children.is_dirty());

  Mapper m4;
  m4.set("children", children);
  ASSERT_STREQ("{\"children\":[{}]}", m4.dump().c_str());
}

TEST(MapperTest, HasManyWithValue)
{
  Item item;
  HasMany<Item> children;

  Mapper m("{\"revision\":5,\"id\":3,\"task\":\"do something\",\"children\":[{\"revision\":1,\"id\":1},{\"revision\":4,\"id\":7},{\"revision\":51,\"id\":3}]}", IGNORE_MISSING_FIELDS);

  m.get("id", item.id);
  m.get("revision", item.revision);
  m.get("task", item.task);
  m.get("children", children);

  ASSERT_FALSE(children.is_dirty());
  ASSERT_EQ(3, children.size());

  HasMany<Item> children2(children);

  ASSERT_FALSE(children2.is_dirty());
  ASSERT_EQ(3, children2.size());

  children[1].task = "Sometask";

  ASSERT_TRUE(children.is_dirty());

  Item i;
  children.push_back(i);

  ASSERT_TRUE(children.is_dirty());

  Mapper m4;
  m4.set("children", children);
  ASSERT_STREQ("{\"children\":[{\"id\":1},{\"id\":7,\"task\":\"Sometask\"},{\"id\":3},{}]}", m4.dump().c_str());
}

TEST(MapperTest, OmitParentKeys)
{
  Item item;

  Mapper m(OMIT_PARENT_KEYS);
  m.set_parent_model("Item");

  item.id = 2;
  item.task = "Play";
  item.parent_id = 1;

  m.set("id", item.id);
  m.set("revision", item.revision);
  m.set("task", item.task);
  m.set("parent", item.parent);
  m.set("parent_id", item.parent_id);

  ASSERT_STREQ("{\"task\":\"Play\"}", m.dump().c_str());
}

