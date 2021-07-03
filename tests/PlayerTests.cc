#include <Player.h>

#include <gtest/gtest.h>

TEST(PlayerTest, ChooseToDraw) {
  Player player(CardSet(
    (__uint128_t)1 |
    (__uint128_t)1 << 10 |
    (__uint128_t)2 << 22 |
    (__uint128_t)1 << 44 |
    (__uint128_t)1 << 66 |
    (__uint128_t)1 << 88 |
    (__uint128_t)1 << 110));
  CardSet topCard((__uint128_t)1 << 104);
  player.reveal(topCard);
  ASSERT_EQ(player.optimalRemainder(5).score(5), 34);
  player.chooseToDraw(topCard, 5);
}

TEST(PlayerTest, ChooseTopCard) {
  Player player(CardSet(
    (__uint128_t)1 << 10 |
    (__uint128_t)1 << 20 |
    (__uint128_t)1 << 22 |
    (__uint128_t)1 << 30 |
    (__uint128_t)1 << 40 |
    (__uint128_t)1 << 50 |
    (__uint128_t)1 << 52 |
    (__uint128_t)1 << 110));
  CardSet topCard((__uint128_t)1 << 32);
  player.reveal(topCard);
  ASSERT_EQ(player.optimalRemainder(6).score(6), 5);
  player.chooseToDraw(topCard, 6);
}
