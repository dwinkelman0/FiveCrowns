#include <Game.h>

#include <iostream>

int main() {
  for (int i = 0; i < 100; ++i) {
    CardSet::clearCache();
    Game game(4, 5);
    Game::Stats stats = game.play();
    std::cout << stats.numPlays << "," << stats.numTopCards;
    for (auto it = stats.scores.begin(); it < stats.scores.end(); ++it) {
      std::cout << "," << *it;
    }
    std::cout << std::endl;
  }
  return 0;
}
