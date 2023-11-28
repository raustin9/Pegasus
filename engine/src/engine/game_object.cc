/**
 * game.cc
 * 
 * Implementation for game-specific functionality
*/

#include "game_object.hh" 

namespace Pegasus {
  static GameState game_state = {};

  bool
  Game::Create(Game& game) {
    if (game_state.initialized== true) {
      return false;
    }

    std::cout << "GAME INITIALIZED" << std::endl;
    game.state = static_cast<void*>(&game_state);

    return true;
  }


  bool 
  Game::Update(float delta_time) {
    game_state.delta_time = delta_time;
    return true;
  }

  bool
  Game::Render(float delta_time) {
    game_state.delta_time = delta_time;
    return true;
  }

  void 
  Game::Resize(uint32_t width, uint32_t height) {
    std::cout << "GAME RESIZE" << std::endl;
    return;
  }

  GameObject
  GameObject::New(GameObject::id_t id) {
    GameObject obj;
    obj.m_id = id;
    return obj;
  }

  /**
  * Create a new object for the game
  */
  GameObject
  Game::NewGameObject(void) {
    static GameObject::id_t current_id = 0;

    GameObject obj = GameObject::New(current_id);
    current_id++;

    return obj;
  }
}
