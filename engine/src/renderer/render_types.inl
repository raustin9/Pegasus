#pragma once
#include "defines.hh"
#include "stdafx.hh"

// The types of renderer backends that we will support
enum : int {
  RENDERER_BACKEND_TYPE_VULKAN,
  RENDERER_BACKEND_TYPE_OPENGL,
  RENDERER_BACKEND_TYPE_DIRECTX,
};