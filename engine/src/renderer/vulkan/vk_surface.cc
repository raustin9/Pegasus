#include "vulkan_backend.hh"
#include "core/qlogger.hh"

bool
VulkanBackend::create_surface() {
  qlogger::Info("Creating Vulkan surface...");
  if (!Platform::create_vulkan_surface(m_context)) {
      return false;
  }
  
  return true;
}