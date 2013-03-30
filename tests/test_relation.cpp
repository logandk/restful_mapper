// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <relation.h>
#include <algorithm>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
class Task
{
public:
  int id;
  string task;

  void from_json(string values, const int &flags = 0)
  {
  }

  string to_json(const int &flags = 0) const
  {
    ostringstream s;

    s << "{\"id\":" << id << ",\"task\":\"" << task << "\"}";

    return s.str();
  }

  bool is_dirty() const
  {
    return true;
  }
};

TEST(RelationTest, HasOne)
{
  HasOne<Task> r;

  ASSERT_TRUE(r.is_null());
  ASSERT_FALSE(r.is_dirty());
  ASSERT_THROW(r.get(), runtime_error);
  ASSERT_THROW((Task) r, runtime_error);
  ASSERT_THROW(r->id, runtime_error);

  ASSERT_STREQ("null", r.to_json().c_str());

  HasOne<Task> r2(r);
  ASSERT_TRUE(r2.is_null());
  ASSERT_FALSE(r2.is_dirty());
  ASSERT_THROW(r2.get(), runtime_error);
  ASSERT_THROW((Task) r2, runtime_error);
  ASSERT_THROW(r2->id, runtime_error);

  Task t;
  t.id = 3;
  t.task = "Play!";

  r2 = t;
  ASSERT_FALSE(r2.is_null());
  ASSERT_TRUE(r2.is_dirty());
  ASSERT_EQ(3, r2.get().id);
  ASSERT_STREQ("Play!", r2->task.c_str());

  ASSERT_STREQ("{\"id\":3,\"task\":\"Play!\"}", r2.to_json().c_str());

  r2->task = "Testing...";
  ASSERT_STREQ("Play!", t.task.c_str());

  r2.clear();
  ASSERT_TRUE(r2.is_null());
  ASSERT_THROW(r2.get(), runtime_error);

  r.build();
  ASSERT_TRUE(r2.is_dirty());
  r->id = 8;
  r->task = "flaf";
  ASSERT_FALSE(r.is_null());
  ASSERT_EQ(8, r.get().id);
  ASSERT_STREQ("flaf", r->task.c_str());

  r2 = r;
  ASSERT_FALSE(r2.is_null());
  ASSERT_EQ(8, r2.get().id);
  ASSERT_STREQ("flaf", r2->task.c_str());

  r->id = 3;
  ASSERT_EQ(8, r2.get().id);
}

TEST(RelationTest, HasMany)
{
  HasMany<Task> r;

  ASSERT_FALSE(r.is_dirty());
  ASSERT_TRUE(r.empty());
  ASSERT_EQ(0.0, r.size());

  Task t;
  t.id = 3;
  t.task = "Play!";

  r.push_back(t);
  ASSERT_TRUE(r.is_dirty());
  ASSERT_EQ(1, r.size());

  Task &t2 = r.build();
  t2.id = 5;

  ASSERT_EQ(5, r.back().id);

  reverse(r.begin(), r.end());

  ASSERT_EQ(5, r.front().id);

  Task t3;
  t3.id = 9;
  t3.task = "Profit!!!";

  r.assign(3, t3);
  ASSERT_EQ(3, r.size());

  ASSERT_EQ(9, (r.begin())->id);
  ASSERT_EQ(9, (r.begin() + 1)->id);
  ASSERT_EQ(9, (r.begin() + 2)->id);

  r.erase(r.begin(), r.end());
  ASSERT_TRUE(r.is_dirty());

  ASSERT_TRUE(r.empty());
}

