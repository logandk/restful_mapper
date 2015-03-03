// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <restful_mapper/json.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
TEST(JsonTest, EncodeLiteral)
{
  long long a = 1;
  double b = 3.1415926535897931;
  bool c = false;
  string d = "a string";

  ASSERT_STREQ("1", Json::encode(a).c_str());
  ASSERT_STREQ("3.14", Json::encode(b).substr(0, 4).c_str());
  ASSERT_STREQ("false", Json::encode(c).c_str());
  ASSERT_STREQ("\"a string\"", Json::encode(d).c_str());
  ASSERT_STREQ("\"another string\"", Json::encode("another string").c_str());
}

TEST(JsonTest, EncodeVector)
{
  vector<long long> a;
  vector<double> b;
  vector<bool> c;
  vector<string> d;
  vector<string> e;

  a.push_back(3);
  a.push_back(8);

  b.push_back(3.1415926535897931);

  c.push_back(true);
  c.push_back(false);
  c.push_back(false);

  d.push_back("hello");
  d.push_back("world");

  ASSERT_STREQ("[3,8]", Json::encode(a).c_str());
  ASSERT_STREQ("[3.14", Json::encode(b).substr(0, 5).c_str());
  ASSERT_STREQ("[true,false,false]", Json::encode(c).c_str());
  ASSERT_STREQ("[\"hello\",\"world\"]", Json::encode(d).c_str());
  ASSERT_STREQ("[]", Json::encode(e).c_str());
}

TEST(JsonTest, EncodeMap)
{
  map<string, long long> a;
  map<string, double> b;
  map<string, bool> c;
  map<string, string> d;
  map<string, string> e;

  a["test"] = 3;
  a["flaf"] = 8;

  b["amount"] = 3.1415926535897931;

  c["1"] = true;
  c["2"] = false;
  c["3"] = false;

  d["hello"] = "";
  d["to"] = "world";

  ASSERT_STREQ("{\"flaf\":8,\"test\":3}", Json::encode(a).c_str());
  ASSERT_STREQ("{\"amount\":3.14", Json::encode(b).substr(0, 14).c_str());
  ASSERT_STREQ("{\"1\":true,\"2\":false,\"3\":false}", Json::encode(c).c_str());
  ASSERT_STREQ("{\"hello\":\"\",\"to\":\"world\"}", Json::encode(d).c_str());
  ASSERT_STREQ("{}", Json::encode(e).c_str());
}

TEST(JsonTest, Emit)
{
  Json::Emitter emitter;

  emitter.emit_map_open();

  emitter.emit("test", 4ll);
  emitter.emit_null("hello");

  std::vector<string> v;
  v.push_back("hello");
  v.push_back("world");

  emitter.emit("strings", v);

  std::map<string, long long> m;
  m["flaf"] = 6;
  m["abc"] = 8;

  emitter.emit("numbers", m);

  emitter.emit_map_close();

  ASSERT_STREQ("{\"test\":4,\"hello\":null,\"strings\":[\"hello\",\"world\"],\"numbers\":{\"abc\":8,\"flaf\":6}}", emitter.dump().c_str());
  ASSERT_NO_THROW(emitter.dump());

  emitter.reset();

  ASSERT_STREQ("", emitter.dump().c_str());
}

TEST(JsonTest, DecodeLiteral)
{
  ASSERT_EQ(1, Json::decode<long long>("1"));
  ASSERT_DOUBLE_EQ(3.1415926535897931, Json::decode<double>("3.1415926535897931"));
  ASSERT_EQ(true, Json::decode<bool>("true"));
  ASSERT_STREQ("a string", Json::decode<string>("\"a string\"").c_str());
}

TEST(JsonTest, DecodeVector)
{
  vector<long long> a(Json::decode<vector<long long> >("[3,8]"));
  vector<double> b(Json::decode<vector<double> >("[3.1415926535897931]"));
  vector<bool> c(Json::decode<vector<bool> >("[true,false,false]"));
  vector<string> d(Json::decode<vector<string> >("[\"hello\",\"world\"]"));

  ASSERT_EQ(2, a.size());
  ASSERT_EQ(1, b.size());
  ASSERT_EQ(3, c.size());
  ASSERT_EQ(2, d.size());

  ASSERT_STREQ("hello", d[0].c_str());
  ASSERT_STREQ("world", d[1].c_str());
}

TEST(JsonTest, DecodeMap)
{
  map<string, long long> a(Json::decode<map<string, long long> >("{\"flaf\":8,\"test\":3}"));
  map<string, double> b(Json::decode<map<string, double> >("{\"amount\":3.1415926535897931}"));
  map<string, bool> c(Json::decode<map<string, bool> >("{\"1\":true,\"2\":false,\"3\":false}"));
  map<string, string> d(Json::decode<map<string, string> >("{\"hello\":\"\",\"to\":\"world\"}"));

  ASSERT_EQ(2, a.size());
  ASSERT_EQ(1, b.size());
  ASSERT_EQ(3, c.size());
  ASSERT_EQ(2, d.size());

  ASSERT_STREQ("", d["hello"].c_str());
  ASSERT_STREQ("world", d["to"].c_str());
}

TEST(JsonTest, Parse)
{
  Json::Parser parser("{\"test\":4,\"hello\":null,\"strings\":[\"hello\",\"world\"],\"numbers\":{\"abc\":8,\"flaf\":6}}");

  ASSERT_TRUE(parser.is_loaded());
  ASSERT_TRUE(parser.exists("test"));
  ASSERT_FALSE(parser.exists("non-existing"));
  ASSERT_TRUE(parser.exists("hello"));
  ASSERT_TRUE(parser.empty("hello"));

  ASSERT_EQ(4, parser.find("test").to_int());
  ASSERT_STREQ("test", parser.find("test").name().c_str());

  const char *path[] = { "numbers", "flaf", (const char *) 0 };
  ASSERT_EQ(6, parser.find(path).to_int());
  ASSERT_STREQ("flaf", parser.find(path).name().c_str());

  ASSERT_STREQ("[\"hello\",\"world\"]", parser.find("strings").dump().c_str());

  vector<string> vdump = parser.find("strings").dump_array();
  ASSERT_EQ(2, vdump.size());
  ASSERT_STREQ("\"hello\"", vdump[0].c_str());

  map<string, string> mdump = parser.find("numbers").dump_map();
  ASSERT_EQ(2, mdump.size());
  ASSERT_STREQ("8", mdump["abc"].c_str());

  vector<Json::Node> sarray = parser.find("strings").to_array();
  ASSERT_STREQ("strings[1]", sarray[1].name().c_str());

  map<string, Json::Node> nmap = parser.find("numbers").to_map();
  ASSERT_STREQ("numbers[\"abc\"]", nmap.find("abc")->second.name().c_str());
}

TEST(JsonTest, AddValueToMap)
{
    Json::Parser parser("{}");
    Json::Node node;
    
    ASSERT_TRUE(parser.is_loaded());
    
    ASSERT_FALSE(parser.exists("an-int"));
    ASSERT_TRUE(parser.root().add_int("an-int", 3, node));
    ASSERT_EQ("an-int", node.name());
    ASSERT_EQ(3, node.to_int());
    ASSERT_TRUE(parser.exists("an-int"));
    ASSERT_EQ(3, parser.find("an-int").to_int());
    
    ASSERT_FALSE(parser.exists("a-double"));
    ASSERT_TRUE(parser.root().add_double("a-double", 4.5, node));
    ASSERT_EQ("a-double", node.name());
    ASSERT_EQ(4.5, node.to_double());
    ASSERT_TRUE(parser.exists("a-double"));
    ASSERT_EQ(4.5, parser.find("a-double").to_double());
    
    ASSERT_FALSE(parser.exists("a-bool"));
    ASSERT_TRUE(parser.root().add_bool("a-bool", true, node));
    ASSERT_EQ("a-bool", node.name());
    ASSERT_EQ(true, node.to_bool());
    ASSERT_TRUE(parser.exists("a-bool"));
    ASSERT_EQ(true, parser.find("a-bool").to_bool());
    
    ASSERT_FALSE(parser.exists("a-string"));
    ASSERT_TRUE(parser.root().add_string("a-string", "foo", node));
    ASSERT_EQ("a-string", node.name());
    ASSERT_EQ("foo", node.to_string());
    ASSERT_TRUE(parser.exists("a-string"));
    ASSERT_EQ("foo", parser.find("a-string").to_string());
    
    ASSERT_FALSE(parser.exists("an-array"));
    ASSERT_TRUE(parser.root().add_array("an-array", node));
    ASSERT_EQ("an-array", node.name());
    ASSERT_EQ(0, node.to_array().size());
    ASSERT_TRUE(parser.exists("an-array"));
    ASSERT_EQ(0, parser.find("an-array").to_array().size());
    
    ASSERT_FALSE(parser.exists("a-map"));
    ASSERT_TRUE(parser.root().add_map("a-map", node));
    ASSERT_EQ("a-map", node.name());
    ASSERT_EQ(0, node.to_map().size());
    ASSERT_TRUE(parser.exists("a-map"));
    ASSERT_EQ(0, parser.find("a-map").to_map().size());
}

TEST(JsonTest, AddValueToArray)
{
    Json::Parser parser("{}");
    Json::Node array;
    Json::Node node;
    
    ASSERT_TRUE(parser.is_loaded());
    
    ASSERT_FALSE(parser.exists("an-array"));
    ASSERT_TRUE(parser.root().add_array("an-array", array));
    ASSERT_EQ(0, array.to_array().size());
    
    ASSERT_TRUE(array.add_int(3, node));
    ASSERT_EQ(3, node.to_int());
    ASSERT_EQ(3, array.to_array()[0].to_int());
    
    ASSERT_TRUE(array.add_double(4.5, node));
    ASSERT_EQ(4.5, node.to_double());
    ASSERT_EQ(4.5, array.to_array()[1].to_double());
    
    ASSERT_TRUE(array.add_bool(true, node));
    ASSERT_EQ(true, node.to_bool());
    ASSERT_EQ(true, array.to_array()[2].to_bool());
    
    ASSERT_TRUE(array.add_string("foo", node));
    ASSERT_EQ("foo", node.to_string());
    ASSERT_EQ("foo", array.to_array()[3].to_string());
    
    ASSERT_TRUE(array.add_array(node));
    ASSERT_EQ(0, node.to_array().size());
    ASSERT_EQ(0, array.to_array()[4].to_array().size());
    
    ASSERT_TRUE(array.add_map(node));
    ASSERT_EQ(0, node.to_map().size());
    ASSERT_EQ(0, array.to_array()[5].to_map().size());
}
