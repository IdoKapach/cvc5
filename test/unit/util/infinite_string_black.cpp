/******************************************************************************
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2026 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * Black box testing of cvc5::InfiniteString.
 */

#include <sstream>

#include "test.h"
#include "util/inf_string.h"

namespace cvc5::internal {
namespace test {

class TestUtilInfiniteString : public TestInternal
{
 protected:
  void SetUp() override
  {
    d_a_s = String("a");
    d_b_s = String("b");
    d_a = InfiniteString(d_a_s);
    d_b = InfiniteString(d_b_s);
    d_ab = InfiniteString(d_a_s, d_b_s);
  }

  String d_a_s;
  String d_b_s;
  String d_e_s;
  InfiniteString d_a;
  InfiniteString d_b;
  InfiniteString d_ab;
};

TEST_F(TestUtilInfiniteString, concat)
{
  InfiniteString a1 = d_b.concat(d_a_s);
  InfiniteString a2 = d_b.concat(d_b_s);
  InfiniteString a3 = d_b.concat(d_e_s);
  ASSERT_EQ(a1, d_ab);
  ASSERT_EQ(a2, d_b);
  ASSERT_EQ(a3, d_b);
}

TEST_F(TestUtilInfiniteString, comparison)
{
  InfiniteString a1(d_a_s);
  InfiniteString b1(d_b_s);
  InfiniteString ab(d_a_s, d_b_s);

  ASSERT_TRUE(a1 == d_a);
  ASSERT_TRUE(a1 != b1);
  ASSERT_TRUE(a1 < ab);
  ASSERT_TRUE(b1 > a1);
  ASSERT_TRUE(a1 <= d_a);
  ASSERT_TRUE(b1 >= d_b);
}

TEST_F(TestUtilInfiniteString, frontAndBack)
{
  ASSERT_EQ(d_a.front(), d_a_s.front());
  ASSERT_EQ(d_a.back(), d_a_s.front());

  ASSERT_EQ(d_ab.front(), d_a_s.front());
  ASSERT_EQ(d_ab.back(), d_b_s.front());
}

TEST_F(TestUtilInfiniteString, hasPrefix)
{
  ASSERT_TRUE(d_a.hasPrefix(d_e_s));
  ASSERT_TRUE(d_a.hasPrefix(d_a_s));

  String ab_s("abbb");
  String ac_s("accc");

  ASSERT_TRUE(d_ab.hasPrefix(ab_s));
  ASSERT_FALSE(d_ab.hasPrefix(ac_s));
}

TEST_F(TestUtilInfiniteString, prefix)
{
  String empty;
  String aaa("aaa");
  String abbb("abbb");

  ASSERT_EQ(d_a.prefix(0), empty);
  ASSERT_EQ(d_a.prefix(3), aaa);
  ASSERT_EQ(d_ab.prefix(4), abbb);
}

TEST_F(TestUtilInfiniteString, substr)
{
  String abbb("abbb");
  String bbb("bbb");
  String bb("bb");

  ASSERT_EQ(d_ab.substr(0, 4), abbb);
  ASSERT_EQ(d_ab.substr(1, 3), bbb);
  ASSERT_EQ(d_ab.substr(2, 2), bb);
  ASSERT_EQ(d_ab.substr(0, 0), d_e_s);
}

TEST_F(TestUtilInfiniteString, strncmp)
{
  String aaa("aaa");
  String aab("aab");
  String aa("aa");

  ASSERT_TRUE(d_a.strncmp(aaa, 3));
  ASSERT_FALSE(d_a.strncmp(aab, 3));
  ASSERT_FALSE(d_a.strncmp(aa, 3));

  ASSERT_TRUE(d_a.strncmp(d_a, 10));
  ASSERT_FALSE(d_a.strncmp(d_b, 1));
  ASSERT_TRUE(d_a.strncmp(d_b, 0));
}

TEST_F(TestUtilInfiniteString, find)
{
  String bbb("bbb");
  String c("c");
  String ab("ab");

  ASSERT_EQ(d_ab.find(bbb), 1u);
  ASSERT_EQ(d_ab.find(c), std::string::npos);
  ASSERT_EQ(d_ab.find(ab), 0u);

  ASSERT_EQ(d_ab.find(d_b), 1u);
  ASSERT_EQ(d_a.find(d_b), std::string::npos);
}

TEST_F(TestUtilInfiniteString, update)
{
  String cc("cc");
  InfiniteString expected(cc, d_b_s);

  InfiniteString result = d_ab.update(0, cc);
  ASSERT_EQ(result, expected);
}

TEST_F(TestUtilInfiniteString, replace)
{
  String cc("cc");
  InfiniteString expected(cc, d_b_s);

  InfiniteString result = d_ab.replace(d_a_s, cc);
  ASSERT_EQ(result, expected);
}

TEST_F(TestUtilInfiniteString, roverlap)
{
  String bbba("bbba");
  String abbb("abbb");
  String defabcde("defabcde");

  ASSERT_EQ(d_ab.roverlap(bbba), 1u);
  ASSERT_EQ(d_ab.roverlap(abbb), 4u);
  ASSERT_EQ(d_a.roverlap(defabcde), 0u);
}

TEST_F(TestUtilInfiniteString, isRepeated)
{
  ASSERT_TRUE(d_a.isRepeated());
  ASSERT_TRUE(d_b.isRepeated());
  ASSERT_FALSE(d_ab.isRepeated());
}

TEST_F(TestUtilInfiniteString, isRatNumber)
{
  InfiniteString decimal(String("7."), String("5"));
  InfiniteString nonNumber(String("abc"), String("d"));
  InfiniteString integer(String("12"), String("0"));

  ASSERT_TRUE(decimal.isRatNumber());
  ASSERT_FALSE(nonNumber.isRatNumber());
  ASSERT_FALSE(integer.isRatNumber());
}

TEST_F(TestUtilInfiniteString, toRatNumber)
{
  InfiniteString decimal(String("7."), String("5"));

  // Consider three digits after the decimal point: 7.555.
  ASSERT_EQ(decimal.toRatNumber(3),
            Rational(Integer(7555), Integer(1000)));

  // Consider five digits after the decimal point: 7.55555.
  ASSERT_EQ(decimal.toRatNumber(5),
            Rational(Integer(755555), Integer(100000)));

  InfiniteString zeros(String("12.6"), String("0"));

  // The repeating zero suffix gives 12.600.
  ASSERT_EQ(zeros.toRatNumber(2),
            Rational(Integer(1260), Integer(100)));

  // Zero fractional digits: 7
  ASSERT_EQ(decimal.toRatNumber(0), Rational(7));
}

TEST_F(TestUtilInfiniteString, getNChars)
{
  String empty;
  String aaa("aaa");
  String abbb("abbb");

  ASSERT_EQ(d_a.d_stream.getNChars(0), empty);
  ASSERT_EQ(d_a.d_stream.getNChars(3), aaa);
  ASSERT_EQ(d_ab.d_stream.getNChars(4), abbb);
}

// prefix and suffix may pass some sort of optimization in the future

// TEST_F(TestUtilInfiniteString, getPrefixAndSuffix)
// {
//   ASSERT_EQ(d_a.getPrefix(), d_e_s);
//   ASSERT_EQ(d_a.getSuffix(), d_a_s);

//   ASSERT_EQ(d_ab.getPrefix(), d_a_s);
//   ASSERT_EQ(d_ab.getSuffix(), d_b_s);
// }

// TEST_F(TestUtilInfiniteString, getVecPair)
// {
//   auto pair = d_ab.getVecPair();

//   ASSERT_EQ(pair.first, d_a_s.getVec());
//   ASSERT_EQ(pair.second, d_b_s.getVec());
// }

}  // namespace test
}  // namespace cvc5::internal