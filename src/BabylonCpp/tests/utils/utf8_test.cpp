#include <gtest/gtest.h>

#include <babylon/utils/utf8.h>

TEST(TestUTF8, append)
{
  unsigned char u[5] = {0, 0, 0, 0, 0};
  unsigned char* end = utf8::append(0x0448, u);
  EXPECT_TRUE(end);
  EXPECT_EQ(u[0], 0xd1);
  EXPECT_EQ(u[1], 0x88);
  EXPECT_EQ(u[2], 0);
  EXPECT_EQ(u[3], 0);
  EXPECT_EQ(u[4], 0);
}

TEST(TestUTF8, next)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars;
  auto cp              = static_cast<int>(utf8::next(w, twochars + 6));
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars + 3);
}

TEST(TestUTF8, peek_next)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars;
  auto cp              = static_cast<int>(utf8::peek_next(w, twochars + 6));
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars);
}

TEST(TestUTF8, prior)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars + 3;
  auto cp              = static_cast<int>(utf8::prior(w, twochars));
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars);
}

TEST(TestUTF8, previous)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars + 3;
  auto cp              = static_cast<int>(utf8::previous(w, twochars - 1));
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars);
}

TEST(TestUTF8, advance)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars;
  utf8::advance(w, 2, twochars + 6);
  EXPECT_EQ(w, twochars + 5);
}

TEST(TestUTF8, distance)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  auto dist            = utf8::distance(twochars, twochars + 5);
  EXPECT_EQ(dist, 2);
}

TEST(TestUTF8, utf16to8)
{
  unsigned short utf16string[] = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
  std::vector<unsigned char> utf8result;
  utf8::utf16to8(utf16string, utf16string + 5, back_inserter(utf8result));
  EXPECT_EQ(utf8result.size(), 10);
}

TEST(TestUTF8, utf8to16)
{
  char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
  std::vector<unsigned short> utf16result;
  utf8::utf8to16(utf8_with_surrogates, utf8_with_surrogates + 9,
                 back_inserter(utf16result));
  EXPECT_EQ(utf16result.size(), 4);
  EXPECT_EQ(utf16result[2], 0xd834);
  EXPECT_EQ(utf16result[3], 0xdd1e);
}

TEST(TestUTF8, utf32to8)
{
  int utf32string[] = {0x448, 0x65E5, 0x10346, 0};
  std::vector<unsigned char> utf8result;
  utf8::utf32to8(utf32string, utf32string + 3, back_inserter(utf8result));
  EXPECT_EQ(utf8result.size(), 9);
}

TEST(TestUTF8, utf8to32)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  std::vector<int> utf32result;
  utf8::utf8to32(twochars, twochars + 5, back_inserter(utf32result));
  EXPECT_EQ(utf32result.size(), 2);
}

TEST(TestUTF8, find_invalid)
{
  const char utf_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
  const char* invalid      = utf8::find_invalid(utf_invalid, utf_invalid + 6);
  EXPECT_EQ(invalid, utf_invalid + 5);
}

TEST(TestUTF8, is_valid)
{
  char utf_invalid[] = "\xe6\x97\xa5\xd1\x88\xfa";
  bool bvalid        = utf8::is_valid(utf_invalid, utf_invalid + 6);
  EXPECT_FALSE(bvalid);
}

TEST(TestUTF8, replace_invalid)
{
  char invalid_sequence[] = "a\x80\xe0\xa0\xc0\xaf\xed\xa0\x80z";
  std::vector<char> replace_invalid_result;
  utf8::replace_invalid(invalid_sequence,
                        invalid_sequence + sizeof(invalid_sequence),
                        back_inserter(replace_invalid_result), '?');
  bool bvalid = utf8::is_valid(replace_invalid_result.begin(),
                               replace_invalid_result.end());
  EXPECT_TRUE(bvalid);
  const char* fixed_invalid_sequence = "a????z";
  EXPECT_TRUE(std::equal(replace_invalid_result.begin(),
                         replace_invalid_result.end(), fixed_invalid_sequence));
}

TEST(TestUTF8, starts_with_bom)
{
  unsigned char byte_order_mark[] = {0xef, 0xbb, 0xbf};
  bool bbom                       = utf8::starts_with_bom(byte_order_mark,
                                    byte_order_mark + sizeof(byte_order_mark));
  EXPECT_TRUE(bbom);
}

TEST(TestUTF8, is_bom)
{
  unsigned char byte_order_mark[] = {0xef, 0xbb, 0xbf};
  bool bbom                       = utf8::is_bom(byte_order_mark);
  EXPECT_TRUE(bbom);
}

TEST(TestUTF8, iterator)
{
  const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
  utf8::iterator<const char*> it(threechars, threechars, threechars + 9);
  utf8::iterator<const char*> it2 = it;
  EXPECT_EQ(it2, it);
  EXPECT_EQ(*it, 0x10346);
  EXPECT_EQ(*(++it), 0x65e5);
  EXPECT_EQ((*it++), 0x65e5);
  EXPECT_EQ(*it, 0x0448);
  EXPECT_NE(it, it2);
  utf8::iterator<const char*> endit(threechars + 9, threechars, threechars + 9);
  EXPECT_EQ(++it, endit);
  EXPECT_EQ(*(--it), 0x0448);
  EXPECT_EQ((*it--), 0x0448);
  EXPECT_EQ(*it, 0x65e5);
  EXPECT_EQ(
    --it, utf8::iterator<const char*>(threechars, threechars, threechars + 9));
  EXPECT_EQ(*it, 0x10346);
}

TEST(TestUTF8, unchecked_append)
{
  unsigned char u[5] = {0, 0, 0, 0, 0};
  unsigned char* end = utf8::unchecked::append(0x0448, u);
  EXPECT_TRUE(end);
  EXPECT_EQ(u[0], 0xd1);
  EXPECT_EQ(u[1], 0x88);
  EXPECT_EQ(u[2], 0);
  EXPECT_EQ(u[3], 0);
  EXPECT_EQ(u[4], 0);
}

TEST(TestUTF8, unchecked_next)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars;
  auto cp              = utf8::unchecked::next(w);
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars + 3);
}

TEST(TestUTF8, unchecked_peek_next)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars;
  auto cp              = utf8::unchecked::peek_next(w);
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars);
}

TEST(TestUTF8, unchecked_prior)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars + 3;
  auto cp              = utf8::unchecked::prior(w);
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars);
}

TEST(TestUTF8, unchecked_previous)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars + 3;
  auto cp              = utf8::unchecked::previous(w);
  EXPECT_EQ(cp, 0x65e5);
  EXPECT_EQ(w, twochars);
}

TEST(TestUTF8, unchecked_advance)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  const char* w        = twochars;
  utf8::unchecked::advance(w, 2);
  EXPECT_EQ(w, twochars + 5);
}

TEST(TestUTF8, unchecked_distance)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  auto dist            = utf8::unchecked::distance(twochars, twochars + 5);
  EXPECT_EQ(dist, 2);
}

TEST(TestUTF8, unchecked_utf16to8)
{
  unsigned short utf16string[] = {0x41, 0x0448, 0x65e5, 0xd834, 0xdd1e};
  std::vector<unsigned char> utf8result;
  utf8::unchecked::utf16to8(utf16string, utf16string + 5,
                            back_inserter(utf8result));
  EXPECT_EQ(utf8result.size(), 10);
}

TEST(TestUTF8, unchecked_utf8to16)
{
  char utf8_with_surrogates[] = "\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e";
  std::vector<unsigned short> utf16result;
  utf8::unchecked::utf8to16(utf8_with_surrogates, utf8_with_surrogates + 9,
                            back_inserter(utf16result));
  EXPECT_EQ(utf16result.size(), 4);
  EXPECT_EQ(utf16result[2], 0xd834);
  EXPECT_EQ(utf16result[3], 0xdd1e);
}

TEST(TestUTF8, unchecked_utf32to8)
{
  int utf32string[] = {0x448, 0x65e5, 0x10346, 0};
  std::vector<unsigned char> utf8result;
  utf8::utf32to8(utf32string, utf32string + 3, back_inserter(utf8result));
  EXPECT_EQ(utf8result.size(), 9);
}

TEST(TestUTF8, unchecked_utf8to32)
{
  const char* twochars = "\xe6\x97\xa5\xd1\x88";
  std::vector<int> utf32result;
  utf8::unchecked::utf8to32(twochars, twochars + 5, back_inserter(utf32result));
  EXPECT_EQ(utf32result.size(), 2);
}

TEST(TestUTF8, unchecked_iterator)
{
  const char* threechars = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88";
  utf8::unchecked::iterator<const char*> un_it(threechars);
  utf8::unchecked::iterator<const char*> un_it2 = un_it;
  EXPECT_EQ(un_it2, un_it);
  EXPECT_EQ(*un_it, 0x10346);
  EXPECT_EQ(*(++un_it), 0x65e5);
  EXPECT_EQ((*un_it++), 0x65e5);
  EXPECT_EQ(*un_it, 0x0448);
  EXPECT_NE(un_it, un_it2);
  utf8::unchecked::iterator<const char*> un_endit(threechars + 9);
  EXPECT_EQ(++un_it, un_endit);
  EXPECT_EQ(*(--un_it), 0x0448);
  EXPECT_EQ((*un_it--), 0x0448);
  EXPECT_EQ(*un_it, 0x65e5);
  EXPECT_EQ(--un_it, utf8::unchecked::iterator<const char*>(threechars));
  EXPECT_EQ(*un_it, 0x10346);
}
