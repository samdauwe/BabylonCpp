#include <gtest/gtest.h>

#include <babylon/utils/picojson.h>

TEST(TestPicoJSON, Erase)
{
  picojson::value v1, v2;
  const char* s;
  std::string err;
  s   = "{ \"b\": true, \"a\": [1,2,\"three\"], \"d\": 2 }";
  err = picojson::parse(v1, s, s + strlen(s));
  EXPECT_TRUE(err.empty());
  picojson::object& o = v1.get<picojson::object>();
  o.erase("b");
  picojson::array& a = o["a"].get<picojson::array>();
  picojson::array::iterator i;
  i = std::remove(a.begin(), a.end(), picojson::value(std::string("three")));
  a.erase(i, a.end());
  s   = "{ \"a\": [1,2], \"d\": 2 }";
  err = picojson::parse(v2, s, s + strlen(s));
  EXPECT_TRUE(err.empty());
  EXPECT_TRUE(v1 == v2);
}

TEST(TestPicoJSON, Parse1)
{
  picojson::value v;
  const char* json
    = "{\"string\": \"Test PicoJSON\", \"number\": 3.14, \"integer\":5}";
  std::string err = picojson::parse(v, json, json + strlen(json));
  EXPECT_TRUE(err.empty());
  EXPECT_EQ(v.get("string").get<std::string>(), "Test PicoJSON");
  EXPECT_EQ(v.get("number").get<double>(), 3.14);
  EXPECT_EQ(static_cast<int>(v.get("integer").get<double>()), 5);
}

TEST(TestPicoJSON, Parse2)
{
  picojson::value v;
  const char* json
    = "{\"object\": {"
      "\"id\": \"123\","
      "\"properties\": {"
      "  \"values\": ["
      "    {\"v\": 0},"
      "    {\"v\": 1},"
      "    {\"v\": 2}"
      "   ]"
      "  }"
      "}"
      "}";

  std::string err = picojson::parse(v, json, json + strlen(json));
  EXPECT_TRUE(err.empty());
  EXPECT_EQ(v.get("object").get("id").get<std::string>(), "123");

  int i = 0;
  picojson::array list
    = v.get("object").get("properties").get("values").get<picojson::array>();
  for (picojson::array::iterator iter = list.begin(); iter != list.end();
       ++iter) {
    int value = static_cast<int>((*iter).get("v").get<double>());
    EXPECT_EQ(value, i++);
  }
}

TEST(TestPicoJSON, ParseArray)
{
  {
    picojson::value v;
    const char* s   = "[1,true,\"string\"]";
    std::string err = picojson::parse(v, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    EXPECT_TRUE(v.is<picojson::array>());
    EXPECT_EQ(v.get<picojson::array>().size(), size_t(3));
    EXPECT_TRUE(v.contains(0));
    EXPECT_TRUE(v.get(0).is<double>());
    EXPECT_EQ(v.get(0).get<double>(), 1.0);
    EXPECT_TRUE(v.contains(1));
    EXPECT_TRUE(v.get(1).is<bool>());
    EXPECT_TRUE(v.get(1).get<bool>());
    EXPECT_TRUE(v.contains(2));
    EXPECT_TRUE(v.get(2).is<std::string>());
    EXPECT_EQ(v.get(2).get<std::string>(), std::string("string"));
    EXPECT_TRUE(!v.contains(3));
  }
  {
    picojson::value v;
    const char* s   = "[0,1,2,3,4]";
    std::string err = picojson::parse(v, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    EXPECT_TRUE(v.is<picojson::array>());
    EXPECT_EQ(v.get<picojson::array>().size(), size_t(5));
    size_t ctr           = 0;
    picojson::array list = v.get<picojson::array>();
    for (auto& item : list) {
      EXPECT_TRUE(v.contains(ctr));
      EXPECT_EQ(static_cast<size_t>(item.get<double>()), ctr++);
    }
  }
}

TEST(TestPicoJSON, ParseObject)
{
  picojson::value v;
  const char* s   = "{ \"a\": true }";
  std::string err = picojson::parse(v, s, s + strlen(s));
  EXPECT_TRUE(err.empty());
  EXPECT_TRUE(v.is<picojson::object>());
  EXPECT_EQ(v.get<picojson::object>().size(), size_t(1));
  EXPECT_TRUE(v.contains("a"));
  EXPECT_TRUE(v.get("a").is<bool>());
  EXPECT_EQ(v.get("a").get<bool>(), true);
  EXPECT_EQ(v.serialize(), std::string("{\"a\":true}"));
  EXPECT_TRUE(!v.contains("z"));
}

TEST(TestPicoJSON, DeepComparison)
{
  // Check == operator
  {
    picojson::value v1, v2;
    const char* s;
    std::string err;
    s   = "{ \"b\": true, \"a\": [1,2,\"three\"], \"d\": 2 }";
    err = picojson::parse(v1, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    s   = "{ \"d\": 2.0, \"b\": true, \"a\": [1,2,\"three\"] }";
    err = picojson::parse(v2, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    EXPECT_TRUE(v1 == v2);
  }

  // Check != operator for array
  {
    picojson::value v1, v2;
    const char* s;
    std::string err;
    s   = "{ \"b\": true, \"a\": [1,2,\"three\"], \"d\": 2 }";
    err = picojson::parse(v1, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    s   = "{ \"d\": 2.0, \"a\": [1,\"three\"], \"b\": true }";
    err = picojson::parse(v2, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    EXPECT_TRUE(v1 != v2);
  }

  // Check != operator for object
  {
    picojson::value v1, v2;
    const char* s;
    std::string err;
    s   = "{ \"b\": true, \"a\": [1,2,\"three\"], \"d\": 2 }";
    err = picojson::parse(v1, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    s   = "{ \"d\": 2.0, \"a\": [1,2,\"three\"], \"b\": false }";
    err = picojson::parse(v2, s, s + strlen(s));
    EXPECT_TRUE(err.empty());
    EXPECT_TRUE(v1 != v2);
  }
}

TEST(TestPicoJSON, NullParseContext)
{
  const char* s = "{ \"a\": [1,2], \"d\": 2 }";
  picojson::null_parse_context ctx;
  std::string err;
  picojson::_parse(ctx, s, s + strlen(s), &err);
  EXPECT_TRUE(err.empty());
}

TEST(TestPicoJSON, Prettify)
{
  picojson::value v;
  const char* s
    = "{ \"a\": 1, \"b\": [ 2, { \"b1\": \"abc\" } ], \"c\": {}, \"d\": [] }";
  std::string err;
  err = picojson::parse(v, s, s + strlen(s));
  EXPECT_TRUE(err.empty());
  std::string nonPrettified
    = "{\"a\":1,\"b\":[2,{\"b1\":\"abc\"}],\"c\":{},\"d\":[]}";
  EXPECT_EQ(v.serialize(), nonPrettified);
  std::string prettified
    = "{\n  \"a\": 1,\n  \"b\": [\n    2,\n    {\n      \"b1\": \"abc\"\n    "
      "}\n  ],\n  \"c\": {},\n  \"d\": []\n}\n";
  EXPECT_EQ(v.serialize(true), prettified);
}

TEST(TestPicoJSON, Serialize)
{
  // Serialize json object to string
  picojson::object v, inner;
  v["string"]      = picojson::value("Test PicoJSON");
  v["number"]      = picojson::value(1.5);
  inner["boolean"] = picojson::value(true);
  inner["integer"] = picojson::value(1.0);
  v["inner"]       = picojson::value(inner);
  std::string str  = picojson::value(v).serialize();
  // Set expected result
  std::string expected
    = "{\"inner\":{\"boolean\":true,\"integer\":1},\"number\":1.5,\"string\":"
      "\"Test PicoJSON\"}";
  // Perform comparison
  EXPECT_EQ(str, expected);
}

TEST(TestPicoJSON, SwapObject)
{
  picojson::value v1, v2;
  v1 = picojson::value(true);
  std::swap(v1, v2);
  EXPECT_TRUE(v1.is<picojson::null>());
  EXPECT_TRUE(v2.get<bool>() == true);

  v1 = picojson::value("a");
  v2 = picojson::value(1.0);
  std::swap(v1, v2);
  EXPECT_TRUE(v1.get<double>() == 1.0);
  EXPECT_TRUE(v2.get<std::string>() == "a");

  v1 = picojson::value(picojson::object());
  v2 = picojson::value(picojson::array());
  std::swap(v1, v2);
  EXPECT_TRUE(v1.is<picojson::array>());
  EXPECT_TRUE(v2.is<picojson::object>());
}
