#include "CardSet.h"

#include <sstream>

std::ostream &operator <<(std::ostream &os, const __uint128_t x) {
  os << "0x" << std::hex << (uint64_t)(x >> 64) << "." << (uint64_t)x << std::dec;
  return os;
}

std::ostream &operator <<(std::ostream &os, const CardSet &cardSet) {
  const char *numbers[] = { "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
  const char *suits[] = { "a", "b", "c", "d", "e" };
  os << "{ ";
  bool any = false;
  for (int i = 0; i < 11; ++i) {
    for (int j = 0; j < 5; ++j) {
      const uint32_t numCards = (cardSet.data_ >> ((i * 5 + j) * 2)) & 0x3;
      if (numCards > 0) {
        os << numCards << "x" << numbers[i] << suits[j] << ", ";
        any = true;
      }
    }
  }
  const uint32_t numJokers = cardSet.data_ >> 110;
  if (numJokers > 0) {
    os << numJokers << "xJoker, ";
    any = true;
  }
  if (!any) {
    os << "NULL ";
  }
  os << "}";
  return os;
}

static __uint128_t seed = 0xaaaaaaaaaaaaaaaa;
const CardSet CardSet::FULL_DECK = CardSet(seed << 46 | seed | ((__uint128_t)6 << 110));

std::string CardSet::str() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

CardSet CardSet::add(const CardSet &other) const {
  if (detectOverflowAdd(other)) {
    std::stringstream ss;
    ss << "Overflow: adding " << other << " to " << *this;
    throw std::overflow_error(ss.str());
  }
  return CardSet(data_ + other.data_);
}

CardSet CardSet::sub(const CardSet &other) const {
  if (detectOverflowSub(other)) {
    std::stringstream ss;
    ss << "Overflow: subtracting " << other << " from " << *this;
    throw std::overflow_error(ss.str());
  }
  return CardSet(data_ - other.data_);
}

bool CardSet::contains(const CardSet &other) const {
  return !detectOverflowSub(other);
}

bool CardSet::empty() const {
  return data_ == 0;
}

uint32_t CardSet::score(const uint32_t wildNumber) const {
  uint32_t output;
  for (uint32_t number = 0; number < 11; ++number) {
    uint32_t count = sumFiveBitPairs(data_ >> (number * 10));
    output += count * ((number == wildNumber) ? 50 : (number + 3));
  }
  output += (data_ >> 110) * 20;
  return output;
}

CardSet CardSet::optimalRemainder(const uint32_t wildNumber) const {
  auto cacheIt = cache_[wildNumber].find(*this);
  if (cacheIt != cache_[wildNumber].end()) {
    return cacheIt->second;
  }
  std::set<CardSet> matches_ = matches(wildNumber);
  if (matches_.empty()) {
    cache_[wildNumber].emplace(*this, *this);
    return *this;
  }
  uint32_t bestScore = score(wildNumber);
  CardSet bestSet = *this;
  for (const CardSet &match : matches_) {
    assert(!match.empty());
    CardSet matchRemainder = sub(match).optimalRemainder(wildNumber);
    uint32_t matchScore = matchRemainder.score(wildNumber);
    if (matchScore < bestScore) {
      bestScore = matchScore;
      bestSet = matchRemainder;
    }
  }
  cache_[wildNumber].emplace(*this, bestSet);
  return bestSet;
}

float CardSet::expectedScore(const CardSet &unseenCards, const uint32_t wildNumber, const uint32_t depth) const {
  assert(depth >= 1);
  float totalScore = 0;
  uint32_t totalCards = 0;
  __uint128_t unseenMask = unseenCards.getMask();
  for (uint32_t i = 0; i < 110; i += 2) {
    if ((unseenMask >> i) & 0x3) {
      uint32_t numCards = (unseenCards.data_ >> i) & 0x3;
      CardSet augmentedSet = add(CardSet((__uint128_t)1 << i));
      __uint128_t augmentedMask = augmentedSet.getMask();
      uint32_t bestScore = 1000000;
      CardSet bestSet;
      for (uint32_t j = 0; j < 110; j += 2) {
        if ((augmentedMask >> j) & 0x3) {
          CardSet diminishedSet = augmentedSet.sub(CardSet((__uint128_t)1 << j));
          float score = depth == 1 ?
            diminishedSet.optimalRemainder(wildNumber).score(wildNumber) :
            diminishedSet.expectedScore(FULL_DECK.sub(diminishedSet), wildNumber, depth - 1);
          if (score < bestScore) {
            bestScore = score;
            bestSet = diminishedSet;
          }
        }
      }
      totalScore += bestScore * numCards;
      totalCards += numCards;
    }
  }
  return (float)totalScore / totalCards;
}

uint32_t CardSet::numWild(const uint32_t wildNumber) const {
  return ((data_ >> 110) & 0x3) + countBook(wildNumber);
}

__uint128_t CardSet::getMask() const {
  const __uint128_t temp = 0x5555555555555555;
  const __uint128_t mask1 = (temp << 46) | temp;
  const __uint128_t mask2 = mask1 << 1;
  return data_ | ((data_ & mask1) << 1) | ((data_ & mask2) >> 1);
}

bool CardSet::detectOverflowAdd(const CardSet &other) const {
  const __uint128_t temp = 0x3333333333333333;
  const __uint128_t mask1 = (temp << 64) | temp;
  const __uint128_t mask2 = mask1 << 2;
  __uint128_t filteredData = data_ & (((__uint128_t)1 << 110) - 1);
  __uint128_t filteredOther = other.data_ & (((__uint128_t)1 << 110) - 1);
  return
    (((filteredData & mask1) + (filteredOther & mask1)) & ~mask1) != 0 ||
    (((filteredData & mask2) + (filteredOther & mask2)) & ~mask2) != 0 ||
    ((data_ >> 110) + (other.data_ >> 110)) > 0xf;
}

bool CardSet::detectOverflowSub(const CardSet &other) const {
  const __uint128_t temp = 0x3333333333333333;
  const __uint128_t mask1 = (temp << 64) | temp;
  const __uint128_t mask2 = mask1 << 2;
  __uint128_t filteredData = data_ & (((__uint128_t)1 << 110) - 1);
  __uint128_t filteredOther = other.data_ & (((__uint128_t)1 << 110) - 1);
  return
    (((filteredData & mask1) - (filteredOther & mask1)) & ~mask1) != 0 ||
    (((filteredData & mask2) - (filteredOther & mask2)) & ~mask2) != 0 ||
    (data_ & ~(((__uint128_t)1 << 110) - 1)) < (other.data_ & ~(((__uint128_t)1 << 110) - 1));
}

uint32_t CardSet::sumFiveBitPairs(const uint32_t bits) {
  return
    ((bits >> 8) & 0x3) +
    ((bits >> 6) & 0x3) +
    ((bits >> 4) & 0x3) +
    ((bits >> 2) & 0x3) +
    (bits & 0x3);
}

uint32_t CardSet::popcount() const {
  return
    __builtin_popcountll((uint64_t)(data_ >> 64)) +
    __builtin_popcountll((uint64_t)data_);
}

uint32_t CardSet::countBook(const uint32_t number) const {
  assert(number < 11);
  uint32_t book = (data_ >> (number * 10)) & 0x3ff;
  return sumFiveBitPairs(book);
}

uint32_t CardSet::countRun(const uint32_t suit) const {
  assert(suit < 5);
  const __uint128_t temp = 0x000c0300c0300c03;
  const __uint128_t mask = (temp << 50) | temp;
  __uint128_t run = (data_ >> (suit * 2)) & mask;
  uint64_t sum1 = (run + (run >> 60)) & 0x0fffffffffffffff;
  uint32_t sum2 = (sum1 + (sum1 >> 30)) & 0x3fffffff;
  return
    ((sum2 >> 20) & 0xf) +
    ((sum2 >> 10) & 0xf) +
    (sum2 & 0xf);
}

std::vector<CardSet> CardSet::possibleWildCombos(const uint32_t wildNumber) const {
  std::vector<CardSet> possibleSingles;
  possibleSingles.push_back(CardSet((__uint128_t)1 << 110));
  for (int i = 0; i < 5; ++i) {
    possibleSingles.push_back(
      CardSet((__uint128_t)1 << ((wildNumber * 5 + i) * 2)));
  }
  std::vector<CardSet> output;
  CardSet combo;
  CardSet remainder(data_);
  output.push_back(combo);
  while (remainder.numWild(wildNumber) > 0) {
    for (const CardSet &single : possibleSingles) {
      if (remainder.contains(single)) {
        combo = combo.add(single);
        remainder = remainder.sub(single);
        output.push_back(combo);
        break;
      }
    }
  }
  return output;
}

std::set<CardSet> CardSet::matches(const uint32_t wildNumber) const {
  std::vector<CardSet> wildCombos = possibleWildCombos(wildNumber);
  std::set<CardSet> output;

  // Book matches
  for (int number = 0; number < 11; ++number) {
    uint32_t maxBookSize = countBook(number);
    uint32_t maxMatchSize = maxBookSize + wildCombos.size() - 1;
    if (maxMatchSize < 3) {
      continue;
    }
    if (number != wildNumber) {
      for (uint32_t bookCombo = 1; bookCombo < 0x400; ++bookCombo) {
        CardSet bookComboSet((__uint128_t)bookCombo << (number * 10));
        uint32_t bookSize = sumFiveBitPairs(bookCombo);
        if (bookSize > maxBookSize || !contains(bookComboSet)) {
          continue;
        }
        uint32_t wildComboSize = 0;
        for (auto wildComboIt = wildCombos.begin();
             wildComboIt < wildCombos.end();
             wildComboIt++, wildComboSize++) {
          if (bookSize + wildComboSize >= 3) {
            output.insert(bookComboSet.add(*wildComboIt));
          }
        }
      }
    }
    else {
      for (auto wildComboIt = wildCombos.begin() + 3;
           wildComboIt < wildCombos.end();
           ++wildComboIt) {
        output.insert(*wildComboIt);
      }
    }
  }

  // Run matches
  CardSet thisWithoutWild(sub(*(wildCombos.end() - 1)));
  __uint128_t thisWithoutWildMask = ~thisWithoutWild.getMask();
  for (int suit = 0; suit < 5; ++suit) {
    __uint128_t seed = (
        ((__uint128_t)1 << 100) |
        ((__uint128_t)1 << 90) |
        ((__uint128_t)1 << 80)
      ) << (suit * 2);
    for (uint32_t runSize = 3; runSize <= 11; ++runSize, seed |= seed >> 10) {
      for (uint32_t shift = 0; shift <= (11 - runSize) * 10; shift += 10) {
        CardSet runSet(seed >> shift);
        uint32_t wildComboSize = 0;
        for (auto wildComboIt = wildCombos.begin();
             wildComboIt < wildCombos.end();
             ++wildComboIt, ++wildComboSize) {
          CardSet missing(runSet.data_ & thisWithoutWildMask);
          uint32_t numMissing = missing.popcount();
          if (numMissing == wildComboSize) {
            output.insert(runSet.sub(missing).add(*wildComboIt));
          }
        }
      }
    }
  }

  return output;
}

void CardSet::clearCache() {
  for (int i = 0; i < 11; ++i) {
    cache_[i].clear();
  }
}

std::map<CardSet, CardSet> CardSet::cache_[11];
