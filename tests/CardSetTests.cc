#include <CardSet.h>

#include <gtest/gtest.h>

TEST(CardSetTest, Print) {
  CardSet set(
    (__uint128_t)1 |
    (__uint128_t)1 << 10 |
    (__uint128_t)2 << 22 |
    (__uint128_t)1 << 44 |
    (__uint128_t)1 << 66 |
    (__uint128_t)1 << 88 |
    (__uint128_t)1 << 110);
  ASSERT_EQ(set.str(), "{ 1x3a, 1x4a, 2x5b, 1x7c, 1x9d, 1xJe, 1xJoker, }");
}

TEST(CardSetTest, Add) {
  CardSet set1(1 | (1 << 2) | (1 << 4));
  CardSet set2(1 | (1 << 2) | (1 << 6));
  CardSet expected(2 | (2 << 2) | (1 << 4) | (1 << 6));
  CardSet sum = set1.add(set2);
  ASSERT_EQ(sum.str(), expected.str());
}

TEST(CardSetTest, AddHigh) {
  CardSet set1(1 | ((__uint128_t)1 << 92) | ((__uint128_t)1 << 104));
  CardSet set2(1 | ((__uint128_t)1 << 92) | ((__uint128_t)1 << 106));
  CardSet expected(2 | ((__uint128_t)2 << 92) | ((__uint128_t)1 << 104) | ((__uint128_t)1 << 106));
  CardSet sum = set1.add(set2);
  ASSERT_EQ(sum.str(), expected.str());
}

TEST(CardSetTest, AddOverflow) {
  CardSet set1(1 | (1 << 2) | (1 << 4));
  CardSet set2(3 | (1 << 2) | (1 << 6));
  ASSERT_ANY_THROW(set1.add(set2));
}

TEST(CardSetTest, Sub) {
  CardSet set1(1 | (1 << 2) | (1 << 4));
  CardSet set2(1 | (1 << 2));
  CardSet expected(1 << 4);
  CardSet diff = set1.sub(set2);
  ASSERT_EQ(diff.str(), expected.str());
}

TEST(CardSetTest, SubOverflow) {
  CardSet set1(1 | (1 << 2) | (1 << 4));
  CardSet set2(1 | (1 << 2) | (1 << 6));
  ASSERT_ANY_THROW(set1.sub(set2));
}

TEST(CardSetTest, NumWild) {
  CardSet set1(1 | (1 << 10) | (1 << 14) | (1 << 18) | (1 << 22));
  ASSERT_EQ(set1.numWild(1), 3);
  CardSet set2 = set1.add(CardSet((__uint128_t)3 << 110));
  ASSERT_EQ(set2.numWild(1), 6);
}

TEST(CardSetTest, CountRun) {
  CardSet set1((1 << 4) | (1 << 14) | (2 << 24) | ((__uint128_t)3 << 34));
  ASSERT_EQ(set1.countRun(2), 7);
  CardSet set2 = set1.add(CardSet(((__uint128_t)1 << 44) | ((__uint128_t)1 << 54) | ((__uint128_t)1 << 64)));
  ASSERT_EQ(set2.countRun(2), 10);
  CardSet set3 = set2.add(CardSet(((__uint128_t)1 << 74) | ((__uint128_t)1 << 84) | ((__uint128_t)1 << 104)));
  ASSERT_EQ(set3.countRun(2), 13);
  CardSet set4 = set3.add(CardSet(((__uint128_t)1 << 66) | ((__uint128_t)1 << 32) | ((__uint128_t)1 << 100)));
  ASSERT_EQ(set4.countRun(2), 13);
}

TEST(CardSetTest, PossibleWildCombos) {
  std::vector<CardSet> expected = {
    CardSet((__uint128_t)1 << 110),
    CardSet((__uint128_t)1 << 110),
    CardSet((__uint128_t)1 << 110),
    CardSet((__uint128_t)1),
    CardSet((__uint128_t)1 << 2),
    CardSet((__uint128_t)1 << 2),
    CardSet((__uint128_t)1 << 4),
    CardSet((__uint128_t)1 << 4),
    CardSet((__uint128_t)1 << 4),
    CardSet((__uint128_t)1 << 6)
  };
  CardSet set(1 | (2 << 2) | (3 << 4) | (1 << 6) | (1 << 14) | ((__uint128_t)3 << 110));
  std::vector<CardSet> combos = set.possibleWildCombos(0);
  ASSERT_EQ(combos.begin()->str(), CardSet().str());
  CardSet combo;
  auto expectedIt = expected.begin();
  auto combosIt = combos.begin() + 1;
  for (; expectedIt != expected.end(); ++expectedIt, ++combosIt) {
    combo = combo.add(*expectedIt);
    ASSERT_EQ(combosIt->str(), combo.str());
  }
}

TEST(CardSetTest, MatchThreeBook) {
  CardSet set(1 | (1 << 2) | (1 << 4));
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 1);
  ASSERT_EQ(matches.begin()->str(), set.str());
}

TEST(CardSetTest, MatchThreeBookSameAsWild) {
  CardSet set(1 | (1 << 2) | (1 << 4));
  std::set<CardSet> matches = set.matches(0);
  ASSERT_EQ(matches.size(), 1);
  ASSERT_EQ(matches.begin()->str(), set.str());
}

TEST(CardSetTest, MatchThreeBookCompleteWithWild) {
  CardSet set(1 | (1 << 2) | (1 << 14));
  std::set<CardSet> matches = set.matches(1);
  ASSERT_EQ(matches.size(), 1);
  ASSERT_EQ(matches.begin()->str(), set.str());
}

TEST(CardSetTest, MatchThreeBookAllSame) {
  CardSet set(3 << 8);
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 1);
  ASSERT_EQ(matches.begin()->str(), set.str());
}

TEST(CardSetTest, MatchFourBook) {
  CardSet set(1 | (1 << 2) | (1 << 4) | (1 << 6));
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 5);
  ASSERT_NE(matches.find(CardSet(1 | (1 << 2) | (1 << 4))), matches.end());
  ASSERT_NE(matches.find(CardSet(1 | (1 << 2) | (1 << 6))), matches.end());
  ASSERT_NE(matches.find(CardSet(1 | (1 << 4) | (1 << 6))), matches.end());
  ASSERT_NE(matches.find(CardSet((1 << 2) | (1 << 4) | (1 << 6))), matches.end());
  ASSERT_NE(matches.find(CardSet(1 | (1 << 2) | (1 << 4) | (1 << 6))), matches.end());
}

TEST(CardSetTest, MatchFourBookCompleteWithWild) {
  CardSet set(1 | (1 << 2) | (1 << 4) | ((__uint128_t)1 << 110));
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 5);
  ASSERT_NE(matches.find(CardSet(1 | (1 << 2) | ((__uint128_t)1 << 110))), matches.end());
  ASSERT_NE(matches.find(CardSet(1 | (1 << 4) | ((__uint128_t)1 << 110))), matches.end());
  ASSERT_NE(matches.find(CardSet((1 << 2) | (1 << 4) | ((__uint128_t)1 << 110))), matches.end());
  ASSERT_NE(matches.find(CardSet(1 | (1 << 2) | (1 << 4))), matches.end());
  ASSERT_NE(matches.find(CardSet(1 | (1 << 2) | (1 << 4) | ((__uint128_t)1 << 110))), matches.end());
}

TEST(CardSetTest, MatchNone) {
  CardSet set(1 | (1 << 2));
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 0);
}

TEST(CardSetTest, MatchThreeRun) {
  CardSet set((1 << 4) | (1 << 14) | (1 << 24));
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 1);
  ASSERT_EQ(matches.begin()->str(), set.str());
}

TEST(CardSetTest, MatchThreeRunCompleteWithWild) {
  CardSet set((1 << 14) | (1 << 24) | ((__uint128_t)1 << 110));
  std::set<CardSet> matches = set.matches(4);
  ASSERT_EQ(matches.size(), 1);
  ASSERT_EQ(matches.begin()->str(), set.str());
}

TEST(CardSetTest, MatchFourRun) {
  CardSet set((1 << 14) | (1 << 24) | ((__uint128_t)1 << 34) | ((__uint128_t)1 << 44));
  std::set<CardSet> matches1 = set.matches(6);
  ASSERT_EQ(matches1.size(), 3);
  std::set<CardSet> matches2 = set.matches(4);
  ASSERT_EQ(matches2.size(), 4);
  std::set<CardSet> matches3 = set.matches(2);
  ASSERT_EQ(matches3.size(), 3);
}

TEST(CardSetTest, Score) {
  CardSet set1;
  ASSERT_EQ(set1.score(0), 0);
  CardSet set2(1);
  ASSERT_EQ(set2.score(1), 3);
  ASSERT_EQ(set2.score(0), 50);
}

TEST(CardSetTest, WinningHand) {
  CardSet set(
    ((__uint128_t)1 << 22) |
    ((__uint128_t)2 << 32) |
    ((__uint128_t)1 << 42) |
    ((__uint128_t)1 << 52) |
    ((__uint128_t)1 << 34) |
    ((__uint128_t)2 << 38));
  ASSERT_EQ(set.optimalRemainder(5), CardSet());
  ASSERT_EQ(set.optimalRemainder(0), CardSet());
}

TEST(CardSetTest, ThreeWithNothing) {
  CardSet set(
    ((__uint128_t)1 << 22) |
    ((__uint128_t)1 << 42) |
    ((__uint128_t)1 << 52));
  ASSERT_EQ(set.optimalRemainder(9), set);
}

TEST(CardSetTest, FiveWithPartial) {
  CardSet set(
    ((__uint128_t)1 << 22) |
    ((__uint128_t)1 << 42) |
    ((__uint128_t)1 << 52) |
    ((__uint128_t)1 << 53) |
    ((__uint128_t)1 << 54));
  ASSERT_EQ(set.optimalRemainder(9), CardSet(((__uint128_t)1 << 22) | ((__uint128_t)1 << 42)));
}

TEST(CardSetTest, ExpectedScore) {
  CardSet set(
    ((__uint128_t)1 << 22) |
    ((__uint128_t)1 << 42) |
    ((__uint128_t)1 << 52) |
    ((__uint128_t)1 << 54) |
    ((__uint128_t)1 << 56));
  std::cout << set.expectedScore(CardSet::FULL_DECK.sub(set), 9, 1) << std::endl;
}
