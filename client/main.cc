#include <Game.h>

#include <iostream>

int main(int argc, char **argv) {
  assert(argc == 3);
  std::cout << "numPlayers,numPlays,numTopCards,scores" << std::endl;
  for (int i = 0; i < 100; ++i) {
    CardSet::clearCache();
    Game game(std::atoi(argv[1]), std::atoi(argv[2]));
    Game::Stats stats = game.play();
    std::cout << stats.numPlayers << "," << stats.numPlays << "," << stats.numTopCards;
    for (auto it = stats.scores.begin(); it < stats.scores.end(); ++it) {
      std::cout << "," << *it;
    }
    std::cout << std::endl;
  }
  return 0;
}
