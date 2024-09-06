#pragma once
#include "defines.hh"
#include "renderer/render_types.hh"
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>
/**
 * game_types.hh
 * 
 * This holds the information for a game built with the engine
*/

namespace Pegasus {
  class QAPI GameObject  {
  public:
    using id_t = uint64_t;

    id_t id() { return m_id; }

    glm::vec3 color{};
    std::vector <Vertex> vertices{};
    std::vector <uint32_t> indices{};
    static GameObject New(id_t id); 
    
  private:
    id_t m_id;
    
  };

  class QAPI  Game {
    public:
      Game() {}
      ~Game() {}
      static bool Create(Game& game);
      static GameObject NewGameObject();
      bool Update(float delta_time);
      bool Render(float delta_time);
      void Resize(uint32_t width, uint32_t height);

      void* state;
      void* application_state; // application state
    private:
  };
}