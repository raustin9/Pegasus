#pragma once

#include "vkcommon.hh"
#include "stdafx.hh"

void PickPhysicalDevice(VKCommonParameters &params);

bool CheckPhysicalDeviceProperties(const VkPhysicalDevice &physicalDevice, VKCommonParameters &params);

void GetDeviceQueue(const VkDevice &device, uint32_t graphicsQueueFamilyIndex, VkQueue& graphicsQueue);

VkResult CreateLogicalDevice(
    std::vector<VkDeviceQueueCreateInfo> &queueInfos,
    std::vector<const char*> &deviceExtensions,
    std::vector<std::string> &supportedDeviceExtensions,
    VKCommonParameters &params);
