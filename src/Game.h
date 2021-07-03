#pragma once

#include "Player.h"

#include <algorithm>
#include <random>

class Game {
 public:
  struct Stats {
    std::vector<uint32_t> scores;
    uint32_t numPlays;
    uint32_t numTopCards;
  };

  Game(const uint32_t numPlayers, const uint32_t roundNumber) :
      wildNumber_(roundNumber),
      randomGen_(std::mt19937::result_type(std::random_device()())) {
    for (uint32_t i = 0; i < 110; i += 2) {
      deck_.push_back(CardSet((__uint128_t)1 << i));
      deck_.push_back(CardSet((__uint128_t)1 << i));
    }
    deck_.push_back(CardSet((__uint128_t)1 << 110));
    deck_.push_back(CardSet((__uint128_t)1 << 110));
    deck_.push_back(CardSet((__uint128_t)1 << 110));
    deck_.push_back(CardSet((__uint128_t)1 << 110));
    deck_.push_back(CardSet((__uint128_t)1 << 110));
    deck_.push_back(CardSet((__uint128_t)1 << 110));
    topCard_ = deal();
    for (uint32_t i = 0; i < numPlayers; ++i) {
      CardSet hand;
      for (uint32_t j = 0; j < roundNumber + 3; ++j) {
        hand = hand.add(deal());
      }
      Player player(hand);
      players_.push_back(player);
    }
  }

  Stats play() {
    Stats stats;
    uint32_t numPlays = 0;
    uint32_t numTopCards = 0;
    std::vector<Player>::iterator winner;
    for (auto it = players_.begin();;
         it = (it + 1 == players_.end()) ? players_.begin() : it + 1) {
      numPlays++;
      bool willDraw = it->chooseToDraw(topCard_, wildNumber_);
      numTopCards += !willDraw;
      topCard_ = it->pickUpAndDiscard(willDraw ? deal() : topCard_, wildNumber_);
      if (it->optimalRemainder(wildNumber_).empty()) {
        winner = it;
        stats.scores.push_back(0);
        break;
      }
    }
    for (auto it = (winner + 1 == players_.end()) ? players_.begin() : winner + 1;
         it != winner;
         it = (it + 1 == players_.end()) ? players_.begin() : it + 1) {
      bool willDraw = it->chooseToDraw(topCard_, wildNumber_);
      numTopCards += !willDraw;
      topCard_ = it->pickUpAndDiscard(willDraw ? deal() : topCard_, wildNumber_);
      uint32_t score = it->optimalRemainder(wildNumber_).score(wildNumber_);
      stats.scores.push_back(score);
    }
    stats.numPlays = numPlays;
    stats.numTopCards = numTopCards;
    return stats;
  }

 private:
  CardSet deal() {
    uint32_t numElements = std::distance(deck_.begin(), deck_.end());
    uint32_t index = std::uniform_int_distribution<>(0, numElements - 1)(randomGen_);
    auto deckIt = deck_.begin() + index;
    CardSet output = *deckIt;
    deck_.erase(deckIt);
    return output;
  }

 private:
  uint32_t wildNumber_;
  std::vector<CardSet> deck_;
  CardSet topCard_;
  std::vector<Player> players_;
  std::mt19937 randomGen_;
};
