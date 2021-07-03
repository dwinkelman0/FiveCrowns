#pragma once

#include "CardSet.h"

class Player {
 public:
  Player(const CardSet &hand);

  void reveal(const CardSet &cards);
  CardSet pickUpAndDiscard(const CardSet &card, const uint32_t wildNumber);
  bool chooseToDraw(const CardSet &topCard, const uint32_t wildNumber) const;
  CardSet optimalRemainder(const uint32_t wildNumber) const;
  CardSet getHand() const;

 private:
  std::pair<CardSet, uint32_t> chooseDiscard(const CardSet &newCard, const uint32_t wildNumber) const;

  CardSet hand_;
  CardSet unseen_;
};
