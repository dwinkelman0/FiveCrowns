#include "Player.h"

Player::Player(const CardSet &hand) : hand_(hand), unseen_(CardSet::FULL_DECK) {
  reveal(hand);
}

void Player::reveal(const CardSet &cards) {
  unseen_ = unseen_.sub(cards);
}

CardSet Player::pickUpAndDiscard(const CardSet &card, const uint32_t wildNumber) {
  std::pair<CardSet, uint32_t> discard = chooseDiscard(card, wildNumber);
  hand_ = hand_.add(card).sub(discard.first);
  return discard.first;
}

bool Player::chooseToDraw(const CardSet &topCard, const uint32_t wildNumber) const {
  std::pair<CardSet, uint32_t> bestDiscard = chooseDiscard(topCard, wildNumber);
  float drawScore = hand_.expectedScore(unseen_, wildNumber, 2);
  std::cout << "Draw: " << drawScore << "; Top Card: " << bestDiscard.second << std::endl;
  return drawScore < bestDiscard.second;
}

CardSet Player::optimalRemainder(const uint32_t wildNumber) const {
  return hand_.optimalRemainder(wildNumber);
}

CardSet Player::getHand() const {
  return hand_;
}

std::pair<CardSet, uint32_t> Player::chooseDiscard(const CardSet &newCard, const uint32_t wildNumber) const {
  CardSet augmentedSet = hand_.add(newCard);
  __uint128_t augmentedMask = augmentedSet.getMask();
  float bestScore = 1000000;
  CardSet bestDiscard;
  for (uint32_t i = 0; i < 110; i += 2) {
    if ((augmentedMask >> i) & 0x3) {
      CardSet discard((__uint128_t)1 << i);
      CardSet diminishedSet = augmentedSet.sub(discard);
      float diminishedScore = diminishedSet.expectedScore(unseen_, wildNumber, 1);
      if (diminishedScore < bestScore) {
        bestScore = diminishedScore;
        bestDiscard = discard;
      }
    }
  }
  return { bestDiscard, bestScore };
}
