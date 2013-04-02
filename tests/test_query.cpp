// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <restful_mapper/query.h>
#include <restful_mapper/internal/iso8601.h>
#include <restful_mapper/internal/utf8.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
TEST(QueryTest, GenerateQuery)
{
  local_charset = "latin1";

  putenv("TZ=CET-1");
  tzset();

  Query q;

  q("name").eq("Flaf");
  q("address").neq("England");
  q("id").gt(3).lt(q.field("zipcode"));
  q("zipcode").gte(1000);
  q("zipcode").lte(7000);

  vector<int> numbers;
  numbers.push_back(4);
  numbers.push_back(6);
  numbers.push_back(11);
  q("house").in(numbers);

  numbers.clear();
  q("house").not_in(numbers);

  vector<bool> choices;
  choices.push_back(true);
  choices.push_back(false);

  q("existing").in(choices);

  vector<string> districts;
  districts.push_back("Outer \xf9 city");
  districts.push_back("Inner city");

  q("district").in(districts);

  q("city").is_null();
  q("country").is_not_null();

  q("region").like("%somewh%");
  q("county").ilike("% Amt");

  q("residents__name").any("Jimi Hendrix");
  q("residents__id").has(2);

  q("voided").eq(false);
  q("profile_completion").gt(3.0);
  q("created_on").lte(to_iso8601(10000, false));

  q.limit(10).offset(2);
  q.order_by_asc(q.field("id"));
  q.order_by_desc(q.field("created_on"));
  q.single();

  ostringstream s;
  s << "{\"filters\":"
    << "["
    << "{\"name\":\"name\",\"op\":\"eq\",\"val\":\"Flaf\"},"
    << "{\"name\":\"address\",\"op\":\"neq\",\"val\":\"England\"},"
    << "{\"name\":\"id\",\"op\":\"gt\",\"val\":3},"
    << "{\"name\":\"id\",\"op\":\"lt\",\"field\":\"zipcode\"},"
    << "{\"name\":\"zipcode\",\"op\":\"gte\",\"val\":1000},"
    << "{\"name\":\"zipcode\",\"op\":\"lte\",\"val\":7000},"
    << "{\"name\":\"house\",\"op\":\"in\",\"val\":[4,6,11]},"
    << "{\"name\":\"house\",\"op\":\"not_in\",\"val\":[]},"
    << "{\"name\":\"existing\",\"op\":\"in\",\"val\":[true,false]},"
    << "{\"name\":\"district\",\"op\":\"in\",\"val\":[\"Outer \xc3\xb9 city\",\"Inner city\"]},"
    << "{\"name\":\"city\",\"op\":\"is_null\"},"
    << "{\"name\":\"country\",\"op\":\"is_not_null\"},"
    << "{\"name\":\"region\",\"op\":\"like\",\"val\":\"%somewh%\"},"
    << "{\"name\":\"county\",\"op\":\"ilike\",\"val\":\"% Amt\"},"
    << "{\"name\":\"residents__name\",\"op\":\"any\",\"val\":\"Jimi Hendrix\"},"
    << "{\"name\":\"residents__id\",\"op\":\"has\",\"val\":2},"
    << "{\"name\":\"voided\",\"op\":\"eq\",\"val\":false},"
    << "{\"name\":\"profile_completion\",\"op\":\"gt\",\"val\":3.0},"
    << "{\"name\":\"created_on\",\"op\":\"lte\",\"val\":\"1970-01-01T02:46:40\"}"
    << "],"
    << "\"limit\":10,"
    << "\"offset\":2,"
    << "\"order_by\":[{\"field\":\"id\",\"direction\":\"asc\"},{\"field\":\"created_on\",\"direction\":\"desc\"}],"
    << "\"single\":true"
    << "}";

  ASSERT_STREQ(s.str().c_str(), q.dump().c_str());
}

TEST(QueryTest, ClearQuery)
{
  Query q;

  q("name").eq("Flaf");
  q.clear();

  ASSERT_STREQ("{}", q.dump().c_str());
}
