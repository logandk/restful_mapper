// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <restful_mapper/internal/utf8.h>

using namespace std;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
TEST(Utf8Test, Encode)
{
  local_charset = "latin1";

  string latin1 = "Special \xf8 chars \xc6\xc6\xc6";
  ASSERT_STREQ("Special \xc3\xb8 chars \xc3\x86\xc3\x86\xc3\x86", local_to_utf8(latin1).c_str());

  latin1 = "\xf9";
  ASSERT_STREQ("\xc3\xb9", local_to_utf8(latin1).c_str());
}

TEST(Utf8Test, Decode)
{
  local_charset = "latin1";

  string utf8 = "Special \xc3\xb8 chars";
  ASSERT_STREQ("Special \xf8 chars", utf8_to_local(utf8).c_str());
}
