// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <restful_mapper/collector.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
TEST(CollectorTest, GetObjects)
{
  Collector c("{ \"objects\": [{ \"task\": \"Build an API\", \"id\": 1 }, { \"task\": \"???\", \"id\": 2 }, { \"task\": \"Profit!!!\", \"id\": 3 }] }");
  vector<string> objects = c.get("objects");

  ASSERT_EQ(3, objects.size());

  ASSERT_STREQ("{\"task\":\"Build an API\",\"id\":1}", objects[0].c_str());
  ASSERT_STREQ("{\"task\":\"???\",\"id\":2}", objects[1].c_str());
  ASSERT_STREQ("{\"task\":\"Profit!!!\",\"id\":3}", objects[2].c_str());

  Collector c2("[{ \"task\": \"Build an API\", \"id\": 1 }, { \"task\": \"???\", \"id\": 2 }, { \"task\": \"Profit!!!\", \"id\": 3 }]");
  vector<string> objects2 = c2.get();

  ASSERT_EQ(3, objects2.size());

  ASSERT_STREQ("{\"task\":\"Build an API\",\"id\":1}", objects2[0].c_str());
  ASSERT_STREQ("{\"task\":\"???\",\"id\":2}", objects2[1].c_str());
}

TEST(CollectorTest, ComplexNested)
{
  Collector c("{ \"objects\": [{ \"task\": \"Build an API\", \"id\": 1, \"codes\": { \"pages\": 2.0345, \"objects\": [{\"name\": \"flaf\", \"id\": null}] } }, { \"task\": \"???\", \"id\": 2 }, { \"task\": \"Profit!!!\", \"id\": 3 }] }");
  vector<string> objects = c.get("objects");

  ASSERT_EQ(3, objects.size());

  ASSERT_STREQ("{\"task\":\"Build an API\",\"id\":1,\"codes\":{\"pages\":2.0345,\"objects\":[{\"name\":\"flaf\",\"id\":null}]}}", objects[0].c_str());
  ASSERT_STREQ("{\"task\":\"???\",\"id\":2}", objects[1].c_str());
}

TEST(CollectorTest, InvalidKey)
{
  Collector c("{ \"objects\": [{ \"task\": \"Build an API\", \"id\": 1 }, { \"task\": \"???\", \"id\": 2 }, { \"task\": \"Profit!!!\", \"id\": 3 }] }");

  ASSERT_THROW(c.get("objects2"), runtime_error);

  try
  {
    vector<string> objects = c.get("objects2");
  }
  catch (runtime_error &e)
  {
    ASSERT_STREQ("JSON node \"objects2\" not found", e.what());
  }
}

TEST(CollectorTest, InvalidType)
{
  Collector c("{ \"objects\": 523.01 }");

  ASSERT_THROW(c.get("objects"), runtime_error);

  try
  {
    vector<string> objects = c.get("objects");
  }
  catch (runtime_error &e)
  {
    ASSERT_STREQ("Expected JSON node \"objects\" to be ARRAY, found DOUBLE: 523.01", e.what());
  }
}

