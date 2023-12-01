#pragma once
#include <iostream>
#include "stdafx.hh"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>

std::string errorString(VkResult errorCode);


// Macro to easily check the result of a vulkan api call to make
// sure that we get the correct result
#define VK_CHECK(f)																			                                  \
{																										                      \
    VkResult res = (f);																					                      \
    if (res != VK_SUCCESS)																				                      \
    {																									                      \
        std::cout << "Fatal : VkResult is \"" << errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
        assert(res == VK_SUCCESS);																		                      \
    }																									                      \
}

// // Makes sure that the result of a vulkan api call is success
// #define VK_CHECK(f)																			                                  \
// {																										                      \
//     VkResult res = (f);																					                      \
//     if (res != VK_SUCCESS)																				                      \
//     {																									                      \
//         std::cout << "Fatal : VkResult is \"" << errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
//         assert(res == VK_SUCCESS);																		                      \
//     }																									                      \
// }