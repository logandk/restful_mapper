// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <model.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Declarations
// --------------------------------------------------------------------------------
class Todo : public Model<Todo>
{
public:
  Primary id;
  Field<string> task;
  Field<int> priority;
  Field<double> time;
  Field<bool> completed;
  Field<time_t> completed_on;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("task", task);
    mapper.set("priority", priority);
    mapper.set("time", time);
    mapper.set("completed", completed);
    mapper.set("completed_on", completed_on);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("task", task);
    mapper.get("priority", priority);
    mapper.get("time", time);
    mapper.get("completed", completed);
    mapper.get("completed_on", completed_on);
  }

  virtual std::string endpoint() const
  {
    return "/todo";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

class City;
class Zipcode;
class Citizen;
class PhoneNumber;

class Country : public Model<Country>
{
public:
  Primary id;
  Field<string> name;
  HasMany<City> cities;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("name", name);
    mapper.set("cities", cities);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("name", name);
    mapper.get("cities", cities);
  }

  virtual std::string endpoint() const
  {
    return "/country";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

class City : public Model<City>
{
public:
  Primary id;
  Field<int> country_id;
  Field<string> name;
  HasOne<Country> country;
  HasOne<Zipcode> zipcode;
  HasMany<Citizen> citizens;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("country_id", country_id);
    mapper.set("name", name);
    mapper.set("country", country);
    mapper.set("zipcode", zipcode);
    mapper.set("citizens", citizens);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("country_id", country_id);
    mapper.get("name", name);
    mapper.get("country", country);
    mapper.get("zipcode", zipcode);
    mapper.get("citizens", citizens);
  }

  virtual std::string endpoint() const
  {
    return "/city";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

class Zipcode : public Model<Zipcode>
{
public:
  Primary id;
  Field<int> city_id;
  Field<string> code;
  HasOne<City> city;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("city_id", city_id);
    mapper.set("code", code);
    mapper.set("city", city);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("city_id", city_id);
    mapper.get("code", code);
    mapper.get("city", city);
  }

  virtual std::string endpoint() const
  {
    return "/zipcode";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

class Citizen : public Model<Citizen>
{
public:
  Primary id;
  Field<int> city_id;
  Field<string> first_name;
  Field<string> last_name;
  HasOne<City> city;
  HasMany<PhoneNumber> phone_numbers;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("city_id", city_id);
    mapper.set("first_name", first_name);
    mapper.set("last_name", last_name);
    mapper.set("city", city);
    mapper.set("phone_numbers", phone_numbers);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("city_id", city_id);
    mapper.get("first_name", first_name);
    mapper.get("last_name", last_name);
    mapper.get("city", city);
    mapper.get("phone_numbers", phone_numbers);
  }

  virtual std::string endpoint() const
  {
    return "/citizen";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

class PhoneNumber : public Model<PhoneNumber>
{
public:
  Primary id;
  Field<int> citizen_id;
  Field<int> number;
  HasOne<Citizen> citizen;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("citizen_id", citizen_id);
    mapper.set("number", number);
    mapper.set("citizen", citizen);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("citizen_id", citizen_id);
    mapper.get("number", number);
    mapper.get("citizen", citizen);
  }

  virtual std::string endpoint() const
  {
    return "/phone_number";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
class ModelTest : public ::testing::Test
{
protected:
  virtual void SetUp()
  {
    Api::set_url("http://localhost:5000/api");
    Api::set_username("admin");
    Api::set_password("test");
    Api::get("/reload");
  }
};

TEST_F(ModelTest, GetItem)
{
  Todo t = Todo::find(2);

  ASSERT_EQ(2, int(t.id));
  ASSERT_STREQ("???", string(t.task).c_str());
  ASSERT_EQ(2, int(t.priority));
  ASSERT_DOUBLE_EQ(4.54, double(t.time));
  ASSERT_FALSE(bool(t.completed));
  ASSERT_EQ(2013, t.completed_on.utc_year());
  ASSERT_EQ(3, t.completed_on.utc_month());
  ASSERT_EQ(13, t.completed_on.utc_day());
  ASSERT_EQ(11, t.completed_on.utc_hour());
  ASSERT_EQ(53, t.completed_on.utc_minute());
  ASSERT_EQ(21, t.completed_on.utc_second());
  ASSERT_STREQ("2013-03-13T11:53:21Z", string(t.completed_on).c_str());
}

TEST_F(ModelTest, GetNonExistingItem)
{
  try
  {
    Todo t = Todo::find(10);
  }
  catch (ApiError &e)
  {
    ASSERT_EQ(404, e.code());
  }
}

TEST_F(ModelTest, CreateItem)
{
  Todo t;
  t.id = 8345; // Should be ignored
  t.task = "Newly created...";
  t.save();

  Todo t2 = Todo::find(4);
  ASSERT_STREQ("Newly created...", string(t2.task).c_str());
}

TEST_F(ModelTest, UpdateItem)
{
  Todo t = Todo::find(2);
  t.task = "It works!";
  t.save();

  Todo t2 = Todo::find(2);
  ASSERT_STREQ("It works!", string(t2.task).c_str());

  time_t now = utc_time();

  t2.completed_on = now;

  ASSERT_TRUE(t2.is_dirty());

  t2.save();

  Todo t3 = Todo::find(2);
  ASSERT_EQ(int(now), int(t2.completed_on));
}

TEST_F(ModelTest, DeleteItem)
{
  Todo t = Todo::find(2);
  t.destroy();

  ASSERT_THROW(Todo::find(2), ApiError);

  ASSERT_TRUE(t.id.is_null());
  ASSERT_STREQ("???", string(t.task).c_str());
  ASSERT_EQ(2, int(t.priority));
  ASSERT_DOUBLE_EQ(4.54, double(t.time));
  ASSERT_FALSE(bool(t.completed));

  t.time.clear();

  ASSERT_THROW(t.id = 3, runtime_error);

  ASSERT_TRUE(t.id.is_dirty());
  ASSERT_TRUE(t.task.is_dirty());
  ASSERT_TRUE(t.priority.is_dirty());
  ASSERT_TRUE(t.time.is_dirty());
  ASSERT_TRUE(t.completed.is_dirty());

  t.save();

  ASSERT_EQ(4, int(t.id));
  ASSERT_STREQ("???", string(t.task).c_str());
  ASSERT_EQ(2, int(t.priority));
  ASSERT_TRUE(t.time.is_null());
  ASSERT_FALSE(bool(t.completed));

  ASSERT_FALSE(t.id.is_dirty());
  ASSERT_FALSE(t.task.is_dirty());
  ASSERT_FALSE(t.priority.is_dirty());
  ASSERT_FALSE(t.time.is_dirty());
  ASSERT_FALSE(t.completed.is_dirty());
}

TEST_F(ModelTest, GetCollection)
{
  Todo::Collection todos = Todo::find_all();

  ASSERT_EQ(3, todos.size());

  ASSERT_STREQ("Build an API", string(todos[0].task).c_str());
  ASSERT_STREQ("???", string(todos[1].task).c_str());
  ASSERT_STREQ("Profit!!!", string(todos[2].task).c_str());
}

TEST_F(ModelTest, GetHasOne)
{
  City c = City::find(2);

  ASSERT_EQ(2, c.zipcode->city_id.get());
  ASSERT_EQ(2, c.zipcode->id.get());
  ASSERT_STREQ("8000", c.zipcode->code.get().c_str());

  City c2 = City::find(3);
  ASSERT_TRUE(c2.zipcode.is_null());
}

TEST_F(ModelTest, SaveHasOne)
{
  City c = City::find(2);
  c.zipcode->code = "3453";

  ASSERT_TRUE(c.is_dirty());
  ASSERT_TRUE(c.zipcode.is_dirty());
  ASSERT_FALSE(c.zipcode->city.is_dirty());
  ASSERT_FALSE(c.citizens.is_dirty());

  c.save();

  City c2 = City::find(2);
  ASSERT_STREQ("3453", c.zipcode->code.get().c_str());
}

TEST_F(ModelTest, GetHasMany)
{
  City c = City::find(1);

  ASSERT_FALSE(c.citizens.empty());
  ASSERT_EQ(2, c.citizens.size());
  ASSERT_STREQ("Jane", c.citizens[1].first_name.get().c_str());
  ASSERT_EQ(1, c.citizens[0].city_id.get());
  ASSERT_EQ(1, c.citizens[0].id.get());
  ASSERT_TRUE(c.citizens[1].exists());

  PhoneNumber ph = PhoneNumber::find(2);
  ASSERT_EQ(5678, ph.number.get());
  ASSERT_STREQ("John", ph.citizen->first_name.get().c_str());

  Country ct = Country::find(3);
  ASSERT_STREQ("Norway", ct.name.get().c_str());
  ASSERT_TRUE(ct.cities.empty());
}

TEST_F(ModelTest, SaveHasMany)
{
  Country c = Country::find(1);

  c.cities[0].name = "Gothenburg";
  c.save();

  Country c2 = Country::find(1);

  ASSERT_STREQ("Gothenburg", c2.cities[0].name.get().c_str());

  ASSERT_TRUE(c.id.get() == c2.id.get());
  ASSERT_TRUE(c.name.get() == c2.name.get());
}

TEST_F(ModelTest, QuerySingle)
{
  Query q;
  q("task").like("%?%");

  Todo todo = Todo::find(q);

  ASSERT_STREQ("???", todo.task.get().c_str());

  q.clear();
  q("time").gt("1.45");

  try
  {
    Todo::find(q);
  }
  catch (BadRequestError &e)
  {
    ASSERT_STREQ("Multiple results found", e.what());
  }
}

TEST_F(ModelTest, QueryMany)
{
  Query q;
  q("time").gt("1.45");
  q.order_by_asc(q.field("priority"));

  Todo::Collection todos = Todo::find_all(q);

  ASSERT_EQ(2, todos.size());
  ASSERT_STREQ("???", todos[0].task.get().c_str());
  ASSERT_STREQ("Build an API", todos[1].task.get().c_str());

  q.clear();
  q("time").gt("100000");

  Todo::Collection todos2 = Todo::find_all(q);

  ASSERT_TRUE(todos2.empty());
}

TEST_F(ModelTest, IsDirty)
{
  Todo t;

  ASSERT_FALSE(t.is_dirty());

  t.task = "Some task...";

  ASSERT_TRUE(t.is_dirty());
}

TEST_F(ModelTest, FailedAuthentication)
{
  Api::set_username("admin2");

  ASSERT_THROW(Todo::find(1), AuthenticationError);
}

TEST_F(ModelTest, FailedValidation)
{
  Zipcode z;
  z.code = "42";

  try
  {
    z.save();
  }
  catch (ValidationError &e)
  {
    ASSERT_STREQ("Code must have 4 digits", e.what());
  }
}

TEST_F(ModelTest, Encoding)
{
  Todo t;
  t.task = "Strange characters \xF8 here";
  t.save();

  Todo t2 = Todo::find(4);
  ASSERT_STREQ("Strange characters \xF8 here", string(t2.task).c_str());
}

// TODO: Fixes in Flask-restless
// TODO: Authorization (Flask-Principal)
// TODO: More clean dependency compilation/requirement

