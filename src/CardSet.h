#pragma once

#include <iostream>
#include <stdint.h>
#include <sstream>

std::ostream &operator <<(std::ostream &os, const __uint128_t x);

class CardSet {
 public:
  CardSet() : data_(0) {}
  CardSet(const __uint128_t data) : data_(data) {
    assert((data >> 112) == 0);
  }

  CardSet add(const CardSet &other) const {
    if (detectOverflowAdd(other)) {
      std::stringstream ss;
      ss << "Overflow: adding " << other << " to " << *this;
      throw std::overflow_error(ss.str());
    }
    return CardSet(data_ + other.data_);
  }

  CardSet sub(const CardSet &other) const {
    if (detectOverflowSub(other)) {
      std::stringstream ss;
      ss << "Overflow: subtracting " << other << " from " << *this;
      throw std::overflow_error(ss.str());
    }
    return CardSet(data_ - other.data_);
  }

  bool contains(const CardSet &other) const {
    return !detectOverflowSub(other);
  }

  friend std::ostream &operator <<(std::ostream &os, const CardSet &cardSet);

 private:
  __uint128_t getMask() const {
    const __uint128_t temp = 0x5555555555555555;
    const __uint128_t mask1 = temp << 24 | temp;
    const __uint128_t mask2 = mask1 << 1;
    return data_ | ((data_ & mask1) << 1) | ((data_ & mask2) >> 1);
  }

  bool detectOverflowAdd(const CardSet &other) const {
    const __uint128_t temp = 0x3333333333333333;
    const __uint128_t mask1 = temp << 24 | temp;
    const __uint128_t mask2 = mask1 << 2;
    return
      (((data_ & mask1) + (other.data_ & mask1)) & ~mask1) != 0 ||
      (((data_ & mask2) + (other.data_ & mask2)) & ~mask2) != 0;
  }

  bool detectOverflowSub(const CardSet &other) const {
    const __uint128_t temp = 0x3333333333333333;
    const __uint128_t mask1 = temp << 24 | temp;
    const __uint128_t mask2 = mask1 << 2;
    return
      (((data_ & mask1) - (other.data_ & mask1)) & ~mask1) != 0 ||
      (((data_ & mask2) - (other.data_ & mask2)) & ~mask2) != 0;
  }

  __uint128_t data_;
};
