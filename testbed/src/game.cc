#include "game.hh"

/**
 * game.cc
 * 
 * Implementation for game-specific functionality
*/

#include <iostream>

static GameState game_state = {};



bool
Pegasus::Game::Create(Game& game) {
  if (game_state.initialized== true) {
    return false;
  }

  std::cout << "GAME INITIALIZED" << std::endl;
  game.state = static_cast<void*>(&game_state);

  return true;
}


bool 
Pegasus::Game::Update(float delta_time) {
  game_state.delta_time = delta_time;
  return true;
}

bool
Pegasus::Game::Render(float delta_time) {
  game_state.delta_time = delta_time;
  return true;
}

void 
Pegasus::Game::Resize(uint32_t width, uint32_t height) {
  std::cout << "GAME RESIZE" << std::endl;
  return;
}

Pegasus::GameObject
Pegasus::GameObject::New(Pegasus::GameObject::id_t id) {
  Pegasus::GameObject obj;
  obj.m_id = id;
  return obj;
}

/**
 * Create a new object for the game
*/
Pegasus::GameObject
Pegasus::Game::NewGameObject() {
  static Pegasus::GameObject::id_t current_id = 0;

  Pegasus::GameObject obj = Pegasus::GameObject::New(current_id);
  current_id++;

  return obj;
}