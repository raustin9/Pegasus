#pragma once
#include "game_types.hh"
#include <qmath/qmath.hh>

struct GameState {
  bool initialized = false;
  float delta_time;
  qmath::Mat4<float> view;
  qmath::Vec3<float> camera_position;
  qmath::Vec3<float> camera_euler;
  bool camera_view_dirty;
};