#include <gtest/gtest.h>

#include <babylon/core/json.h>

TEST(JSON, NameValuePair)
{
  using namespace BABYLON;

  // String
  Json::value v = Json::NameValuePair("string", "Test JSON");
  EXPECT_EQ(v.get("name").get<::std::string>(), "string");
  EXPECT_EQ(v.get("value").get<::std::string>(), "Test JSON");
  // Floating point number
  v = Json::NameValuePair("number", 3.14);
  EXPECT_EQ(v.get("name").get<::std::string>(), "number");
  EXPECT_EQ(v.get("value").get<double>(), 3.14);
  // Integer
  v = Json::NameValuePair<int>("integer", 5);
  EXPECT_EQ(v.get("name").get<::std::string>(), "integer");
  EXPECT_EQ(static_cast<int>(v.get("value").get<double>()), 5);
  // Boolean value
  v = Json::NameValuePair("bool", true);
  EXPECT_EQ(v.get("name").get<::std::string>(), "bool");
  EXPECT_EQ(v.get("value").get<bool>(), true);
}

TEST(JSON, Pair)
{
  using namespace BABYLON;

  // String
  ::std::pair<::std::string, Json::value> v = Json::Pair("string", "Test JSON");
  EXPECT_EQ(v.first, "string");
  EXPECT_EQ(v.second.get<::std::string>(), "Test JSON");
  // Floating point number
  v = Json::Pair("number", 3.14);
  EXPECT_EQ(v.first, "number");
  EXPECT_EQ(v.second.get<double>(), 3.14);
  // Integer
  v = Json::Pair<int>("integer", 5);
  EXPECT_EQ(v.first, "integer");
  EXPECT_EQ(static_cast<int>(v.second.get<double>()), 5);
  // Boolean value
  v = Json::Pair("bool", false);
  EXPECT_EQ(v.first, "bool");
  EXPECT_EQ(v.second.get<bool>(), false);
}
