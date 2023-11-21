#pragma once
#include "stdafx.hh"
#include "vkcommon.hh"

// TODO: split this into multiple functions

// Recreate the swapchain
void RecreateSwapchain(
        VKCommonParameters &params, 
        uint32_t* w, 
        uint32_t *h, 
        bool vsync,
        uint32_t& commandBufferCount // TODO: get rid of this?
); 
