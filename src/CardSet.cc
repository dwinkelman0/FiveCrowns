#include "CardSet.h"

std::ostream &operator <<(std::ostream &os, const __uint128_t x) {
  os << "0x" << std::hex << (uint64_t)(x >> 64) << "." << (uint64_t)x << std::dec;
  return os;
}

std::ostream &operator <<(std::ostream &os, const CardSet &cardSet) {
  const char *numbers[] = { "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "Joker" };
  const char *suits[] = { "a", "b", "c", "d", "e" };
  os << "{ ";
  bool any = false;
  for (int i = 0; i < 12; ++i) {
    for (int j = 0; j < 5; ++j) {
      const uint32_t numCards = (cardSet.data_ >> ((i * 5 + j) * 2)) & 0x3;
      if (numCards > 0) {
        os << numCards << "x" << numbers[i] << suits[j] << ", ";
        any = true;
      }
    }
  }
  if (!any) {
    os << "NULL ";
  }
  os << "}";
  return os;
}
