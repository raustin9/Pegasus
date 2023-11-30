#pragma once
#include "defines.hh"
#include "render_types.hh"

bool renderer_backend_create(renderer_backend_type type, RendererBackend** backend);

void renderer_backend_destroy(RendererBackend* backend);