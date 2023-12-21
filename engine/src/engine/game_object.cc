/**
 * game.cc
 * 
 * Implementation for game-specific functionality
*/

#include "game_object.hh" 
#include "core/input.hh"
#include "core/qmemory.hh"
#include "core/qlogger.hh"
#include <cstdint>

#include <renderer/renderer_frontend.hh>

namespace Pegasus {
    static GameState game_state = {};

    void recalculate_camera_view(GameState& state) {
        if (!state.camera_view_dirty) {
            return;
        }

        qmath::Mat4<float> rotation = qmath::Mat4<float>::EulerXYZ(
            state.camera_euler.x,
            state.camera_euler.y,
            state.camera_euler.z
        );

        qmath::Mat4<float> translation = qmath::Mat4<float>::GetTranslation(state.camera_position);

        state.view = rotation * translation;
        state.view.Invert();

        state.camera_view_dirty = false;
        return;
    }

    void camera_yaw(GameState& state, float amount) {
        state.camera_euler.y += amount;
        state.camera_view_dirty = true;
    }
    
    void camera_pitch(GameState& state, float amount) {
        state.camera_euler.x += amount;

        float limit = qmath::deg_to_rad(89);
        // state.camera_euler.x = std::clamp(state.camera_euler.x, limit, limit);
        state.camera_euler.x = std::clamp(state.camera_euler.x, -limit, limit);
        state.camera_view_dirty = true;
    }

    bool
    Game::Create(Game& game) {
        if (game_state.initialized== true) {
            return false;
        }

        std::cout << "GAME INITIALIZED" << std::endl;
        game.state = static_cast<void*>(&game_state);

        // Camera values
        game_state.camera_position = qmath::Vec3<float>::New(0.0f, 0.0f, 30.0f);
        game_state.camera_euler        = qmath::Vec3<float>::Zero();
        game_state.camera_view_dirty = true;

        // Set the view matrix
        game_state.view = qmath::Mat4<float>::GetTranslation(
            game_state.camera_position
        );
        game_state.view.Invert();

        return true;
    }


    bool 
    Game::Update(float delta_time) {
        game_state.delta_time = delta_time;

        static uint64_t alloc_count = 0;
        uint64_t prev_alloc_count = alloc_count;
        alloc_count = QAllocator::AllocationCount();
        if (InputHandler::IsKeyUp(KEY_M) && InputHandler::WasKeyDown(KEY_M)) {
            qlogger::Debug("Allocations: %llu (%llu this frame)", alloc_count, alloc_count - prev_alloc_count);
        }

        // Move the camera around

        // HACK: temp hack to move the camera around
        if (InputHandler::IsKeyDown(KEY_A) || InputHandler::IsKeyDown(KEY_LEFT)) {
            camera_yaw(game_state, 1.0f * delta_time);
        }

        if (InputHandler::IsKeyDown(KEY_D) || InputHandler::IsKeyDown(KEY_RIGHT)) {
            camera_yaw(game_state, -1.0f * delta_time);
        }
        
        if (InputHandler::IsKeyDown(KEY_UP)) {
            camera_pitch(game_state, 1.0f * delta_time);
        }
       
        if (InputHandler::IsKeyDown(KEY_DOWN)) {
            camera_pitch(game_state, -1.0f * delta_time);
        }
        
        float temp_move_speed = 10.0f; 
        qmath::Vec3<float> velocity = qmath::Vec3<float>::Zero();
        if (InputHandler::IsKeyDown(KEY_W)) {
            qmath::Vec3<float> forward = qmath::Mat4<float>::Forward(game_state.view);
            velocity = velocity + forward;
        }

        if (InputHandler::IsKeyDown(KEY_S)) {
            qmath::Vec3<float> backward = qmath::Mat4<float>::Backward(game_state.view);
            velocity = velocity + backward;
        }
        
        if (InputHandler::IsKeyDown(KEY_Q)) {
            qmath::Vec3<float> left = qmath::Mat4<float>::Left(game_state.view);
            velocity = velocity + left;
        }

        if (InputHandler::IsKeyDown(KEY_E)) {
            qmath::Vec3<float> right = qmath::Mat4<float>::Right(game_state.view);
            velocity = velocity + right;
        }

        if (InputHandler::IsKeyDown(KEY_SPACE)) {
            velocity.y += 1.0f;
        }

        if (InputHandler::IsKeyDown(KEY_X)) {
            velocity.y -= 1.0f;
        }

        qmath::Vec3<float> z = qmath::Vec3<float>::Zero();
        if (!qmath::Vec3<float>::Compare(z, velocity, 0.0002f)) {
            velocity.normalize();
            game_state.camera_position.x += velocity.x * temp_move_speed * delta_time;
            game_state.camera_position.y += velocity.y * temp_move_speed * delta_time;
            game_state.camera_position.z += velocity.z * temp_move_speed * delta_time;
            game_state.camera_view_dirty = true;

        }

        
        recalculate_camera_view(game_state); // make sure view is up-to-date

        Renderer::SetView(game_state.view); // HACK: This should not be available outside of the engine

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
