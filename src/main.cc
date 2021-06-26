#include "CardSet.h"

#include <iostream>

int main() {
  CardSet set1(2 | 1 << 2 | 1 << 4);
  CardSet set2(1 | 1 << 2);
  std::cout << set1.add(set2) << std::endl;
  return 0;
}
