// #include <core/application.hh>
// #include <core/events.hh>
#include <Pegasus.h>
#include "game.hh"

int main() {
    Pegasus::Game game;
    if (!Pegasus::Game::Create(game)) {
        std::cerr << "ERROR: failed to create game" << std::endl;
    }
    Application app = Application(game, "Pegasus Engine", 800, 600);
    app.run();
}
