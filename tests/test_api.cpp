// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <api.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
TEST(ApiTest, Escape)
{
  string escaped = Api::escape("Hej!#&?der");
  ASSERT_STREQ("Hej%21%23%26%3Fder", escaped.c_str());
}

TEST(ApiTest, QueryParams)
{
  string url = "http://google.com";
  url = Api::query_param(url, "q", "test");
  ASSERT_STREQ("http://google.com/?q=test", url.c_str());

  url = "/todo";
  url = Api::query_param(url, "q", "test");
  ASSERT_STREQ("/todo?q=test", url.c_str());

  url = "http://m";
  url = Api::query_param(url, "q", "test");
  ASSERT_STREQ("http://m/?q=test", url.c_str());

  url = "http://";
  url = Api::query_param(url, "q", "test");
  ASSERT_STREQ("http:///?q=test", url.c_str());

  url = "http://google.com/search";
  url = Api::query_param(url, "q", "Min s\xD8gning"); // iso-8859-1
  ASSERT_STREQ("http://google.com/search?q=Min%20s%D8gning", url.c_str());

  url = "http://google.com/search";
  url = Api::query_param(url, "q", "Min s\xC3\xB8gning"); // utf-8
  ASSERT_STREQ("http://google.com/search?q=Min%20s%C3%B8gning", url.c_str());
}

TEST(ApiTest, BadRequestErrorJson)
{
  BadRequestError err("{\"message\":\"Some error message...\"}");
  ASSERT_EQ(400, err.code());
  ASSERT_STREQ("Some error message...", err.what());

  BadRequestError err2("broken json");
  ASSERT_STREQ("", err2.what());
}

TEST(ApiTest, ValidationErrorJson)
{
  ValidationError err("{\"validation_errors\":{\"age\":\"some error message...\",\"name\":\"not provided\"}}");

  ASSERT_EQ(400, err.code());
  ASSERT_STREQ("Age some error message...\nName not provided", err.what());
  ASSERT_STREQ("not provided", err["name"].c_str());

  ValidationError err2("broken json");
  ASSERT_STREQ("", err2.what());
}
