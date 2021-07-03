#pragma once

#include <iostream>
#include <stdint.h>
#include <set>
#include <vector>

#include <gtest/gtest.h>

std::ostream &operator <<(std::ostream &os, const __uint128_t x);

class CardSet {
  FRIEND_TEST(CardSetTest, Add);
  FRIEND_TEST(CardSetTest, AddHigh);
  FRIEND_TEST(CardSetTest, AddOverflow);
  FRIEND_TEST(CardSetTest, Sub);
  FRIEND_TEST(CardSetTest, SubOverflow);
  FRIEND_TEST(CardSetTest, NumWild);
  FRIEND_TEST(CardSetTest, CountRun);
  FRIEND_TEST(CardSetTest, PossibleWildCombos);

 public:
  CardSet() : data_(0) {}
  CardSet(const __uint128_t data) : data_(data) {
    assert((data >> 114) == 0);
  }

  bool operator ==(const CardSet &other) const {
    return data_ == other.data_;
  }

  bool operator <(const CardSet &other) const {
    return data_ < other.data_;
  }

  friend std::ostream &operator <<(std::ostream &os, const CardSet &cardSet);
  std::string str() const;

  CardSet add(const CardSet &other) const;
  CardSet sub(const CardSet &other) const;
  bool contains(const CardSet &other) const;
  bool empty() const;
  uint32_t score(const uint32_t wildNumber) const;
  __uint128_t getMask() const;

  std::set<CardSet> matches(const uint32_t wildNumber) const;
  CardSet optimalRemainder(const uint32_t wildNumber) const;

  float expectedScore(const CardSet &unseenCards, const uint32_t wildNumber, const uint32_t depth) const;

  static const CardSet FULL_DECK;

 private:
  uint32_t numWild(const uint32_t wildNumber) const;
  bool detectOverflowAdd(const CardSet &other) const;
  bool detectOverflowSub(const CardSet &other) const;
  static uint32_t sumFiveBitPairs(const uint32_t bits);
  uint32_t popcount() const;
  uint32_t countBook(const uint32_t number) const;
  uint32_t countRun(const uint32_t suit) const;

  std::vector<CardSet> possibleWildCombos(const uint32_t wildNumber) const;

 private:
  __uint128_t data_;
};
