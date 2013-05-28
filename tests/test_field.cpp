// --------------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <restful_mapper/field.h>

using namespace std;
using namespace restful_mapper;

// --------------------------------------------------------------------------------
// Definitions
// --------------------------------------------------------------------------------
TEST(FieldTest, IntField)
{
  Field<int> f;

  ASSERT_TRUE(f.is_null());
  ASSERT_FALSE(f.is_dirty());
  ASSERT_EQ(0, (int) f);

  f = 5;

  ASSERT_FALSE(f.is_null());
  ASSERT_TRUE(f.is_dirty());
  ASSERT_EQ(5, int(f));
  ASSERT_TRUE(f == 5);
  ASSERT_FALSE(f == 3);
}

TEST(FieldTest, DoubleField)
{
  Field<double> f;

  ASSERT_DOUBLE_EQ(0.0, f.get());

  f = 5.45;

  ASSERT_FLOAT_EQ(5.45, float(f));

  Field<double> f2;

  f2 = 5.45;

  ASSERT_TRUE(f == f2);
}

TEST(FieldTest, BoolField)
{
  Field<bool> f;

  ASSERT_FALSE(f.get());

  f = true;

  ASSERT_TRUE(bool(f));
  ASSERT_TRUE(f);

  f.clear();

  ASSERT_FALSE(bool(f));
  ASSERT_FALSE(f);
}

TEST(FieldTest, StringField)
{
  Field<string> f;

  ASSERT_STREQ("", string(f).c_str());

  ASSERT_TRUE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f = "Test";

  ASSERT_FALSE(f.is_null());
  ASSERT_TRUE(f.is_dirty());
  ASSERT_STREQ("Test", string(f).c_str());

  string s = "Lorem ipsum";
  f = s;

  ASSERT_STREQ("Lorem ipsum", string(f).c_str());

  f.clear();

  ASSERT_STREQ("", string(f).c_str());

  ASSERT_TRUE(f.is_null());
  ASSERT_TRUE(f.is_dirty());

  f.set("Something else...");

  ASSERT_FALSE(f.is_null());
  ASSERT_TRUE(f.is_dirty());

  string s2 = f;

  ASSERT_EQ(s2.size(), f.size());

  ASSERT_STREQ("Something else...", s2.c_str());
  ASSERT_STREQ("Something else...", f.c_str());

  ASSERT_TRUE(f == string("Something else..."));
  ASSERT_TRUE(f == "Something else...");
  ASSERT_FALSE(f != "Something else...");
  ASSERT_FALSE(f == "Something...");
  ASSERT_TRUE(s2 == f);

  Field<string> f2;
  f2 = "Something else...";

  ASSERT_TRUE(f2 == f);

  string s3 = f + f2;

  ASSERT_STREQ("Something else...Something else...", s3.c_str());
}

TEST(FieldTest, TimeField)
{
  putenv("TZ=EST5");
  tzset();

  Field<time_t> f;

  ASSERT_TRUE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f = 1234567890;
  ASSERT_STREQ("2009-02-13T23:31:30", f.to_iso8601(false).c_str());

  f = "2013-03-14T09:33:59Z";

  ASSERT_FALSE(f.is_null());
  ASSERT_TRUE(f.is_dirty());

  ASSERT_EQ(2013, f.local_year());
  ASSERT_EQ(3, f.local_month());
  ASSERT_EQ(14, f.local_day());
  ASSERT_EQ(4, f.local_hour());
  ASSERT_EQ(33, f.local_minute());
  ASSERT_EQ(59, f.local_second());

  ASSERT_STREQ("2013-03-14T09:33:59", f.to_iso8601(false).c_str());

  f = "2013-02-28T13:45:02";

  ASSERT_EQ(2013, f.local_year());
  ASSERT_EQ(2, f.local_month());
  ASSERT_EQ(28, f.local_day());
  ASSERT_EQ(8, f.local_hour());
  ASSERT_EQ(45, f.local_minute());
  ASSERT_EQ(2, f.local_second());

  f = "2013-02-28";

  ASSERT_EQ(2013, f.local_year());
  ASSERT_EQ(2, f.local_month());
  ASSERT_EQ(27, f.local_day());
  ASSERT_EQ(19, f.local_hour());
  ASSERT_EQ(0.0, f.local_minute());
  ASSERT_EQ(0.0, f.local_second());

  f = "2013-02-28T13:45:02Z+0100";

  ASSERT_EQ(2013, f.local_year());
  ASSERT_EQ(2, f.local_month());
  ASSERT_EQ(28, f.local_day());
  ASSERT_EQ(7, f.local_hour());
  ASSERT_EQ(45, f.local_minute());
  ASSERT_EQ(2, f.local_second());

  f = "2013-02-28T13:45:02Z+07:30";
  ASSERT_STREQ("2013-02-28T06:15:02", f.to_iso8601(false).c_str());

  f = "2013-02-28T13:45:02Z-03";
  ASSERT_STREQ("2013-02-28T16:45:02", f.to_iso8601(false).c_str());

  f = "2013-12-31T23:59";
  ASSERT_STREQ("2013-12-31T23:59:00", f.to_iso8601(false).c_str());

  f.clear();

  ASSERT_TRUE(f.is_null());
  ASSERT_TRUE(f.is_dirty());

  ASSERT_THROW(f = "203-12-31T23:59", runtime_error);

  f = "2013-12-31T23:59:00.068484";
  ASSERT_STREQ("2013-12-31T23:59:00", f.to_iso8601(false).c_str());

  f = "2013-12-31T23:59:00+0200";
  ASSERT_STREQ("2013-12-31T21:59:00", f.to_iso8601(false).c_str());
  ASSERT_STREQ("31/12/2013 16:59", f.to_local("%d/%m/%Y %H:%M").c_str());

  f = "2013-12-31T23:59:00-02";
  ASSERT_STREQ("2014-01-01T01:59:00", f.to_iso8601(false).c_str());

  f = "2013-12-31T23:59:00.005-02:45";
  ASSERT_STREQ("2014-01-01T02:44:00", f.to_iso8601(false).c_str());

  time_t now_ts = time(NULL);
  ASSERT_EQ(now_ts - 18000, utc_time());
}

TEST(FieldTest, PrimaryField)
{
  Primary f;

  ASSERT_TRUE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f = 3;

  ASSERT_FALSE(f.is_null());
  ASSERT_TRUE(f.is_dirty());

  ASSERT_THROW(f = 5, runtime_error);

  f = 3;

  ASSERT_STREQ("3", string(f).c_str());

  Primary f2;
  f2.set(3, true);

  ASSERT_FALSE(f2.is_null());
  ASSERT_FALSE(f2.is_dirty());

  ASSERT_THROW(f2 = 5, runtime_error);
  ASSERT_THROW(f2.clear(), runtime_error);

  f2 = Primary();

  ASSERT_TRUE(f2.is_null());
  ASSERT_FALSE(f2.is_dirty());
}

TEST(FieldTest, CleanSet)
{
  Field<int> f;

  ASSERT_TRUE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f.set(3, true);

  ASSERT_FALSE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f.clear();

  ASSERT_TRUE(f.is_null());
  ASSERT_TRUE(f.is_dirty());

  Field<string> f2;

  ASSERT_TRUE(f2.is_null());
  ASSERT_FALSE(f2.is_dirty());

  f2.set("flaf", true);

  ASSERT_FALSE(f2.is_null());
  ASSERT_FALSE(f2.is_dirty());

  Field<int> f3;
  f3.clear(true);
  ASSERT_FALSE(f3.is_dirty());
}

TEST(FieldTest, DirtyFlag)
{
  Field<int> f;

  ASSERT_TRUE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f.set(3, true);

  ASSERT_FALSE(f.is_null());
  ASSERT_FALSE(f.is_dirty());

  f = 3;
  ASSERT_FALSE(f.is_dirty());

  f = 5;
  ASSERT_TRUE(f.is_dirty());

  Field<string> f2;

  ASSERT_TRUE(f2.is_null());
  ASSERT_FALSE(f2.is_dirty());

  f2.set("flaf", true);

  ASSERT_FALSE(f2.is_null());
  ASSERT_FALSE(f2.is_dirty());

  f2 = "flaf";
  ASSERT_FALSE(f2.is_dirty());

  f2 = "blob";
  ASSERT_TRUE(f2.is_dirty());

  Field<time_t> f3;

  ASSERT_TRUE(f3.is_null());
  ASSERT_FALSE(f3.is_dirty());

  f3.set("2030-01-02", true);

  ASSERT_FALSE(f3.is_null());
  ASSERT_FALSE(f3.is_dirty());

  f3 = "2030-01-02";
  ASSERT_FALSE(f3.is_dirty());

  f3 = "2030-01-02T01:14:52";
  ASSERT_TRUE(f3.is_dirty());

  Field<int> f5;
  f5.clear(true);
  ASSERT_FALSE(f5.is_dirty());
  f5 = 5;
  f5.clear(true);
  ASSERT_TRUE(f5.is_dirty());

  Field<string> f6;

  ASSERT_TRUE(f6.is_null());
  ASSERT_FALSE(f6.is_dirty());

  f6.touch();

  ASSERT_TRUE(f6.is_null());
  ASSERT_TRUE(f6.is_dirty());
}

TEST(FieldTest, Stream)
{
  Field<string> f1;
  f1 = "flaf";

  Field<double> f2;
  f2 = 3.1;

  ostringstream s;

  s << f1;
  s << f2;

  ASSERT_STREQ("flaf3.1", s.str().c_str());
}

TEST(FieldTest, FieldAssignment)
{
  Field<long long> f1;
  Field<int> f2;
  Field<bool> f3;
  Field<bool> f4;
  Field<double> f5;
  Field<double> f6;
  Primary f7;
  Primary f8;

  ASSERT_TRUE(f1.is_null());
  ASSERT_TRUE(f2.is_null());

  f1 = f2;

  ASSERT_TRUE(f1.is_null());
  ASSERT_TRUE(f2.is_null());

  f2 = f1;

  ASSERT_TRUE(f1.is_null());
  ASSERT_TRUE(f2.is_null());

  f3 = f4;
  f5 = f6;

  ASSERT_TRUE(f3.is_null());
  ASSERT_TRUE(f5.is_null());

  f7 = f8;

  ASSERT_TRUE(f7.is_null());

  f1 = f8;
  f2 = f7;

  ASSERT_TRUE(f1.is_null());
  ASSERT_TRUE(f2.is_null());
}

