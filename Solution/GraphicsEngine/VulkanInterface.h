#pragma once
#include <vulkan.h>

class Vulkan
{
public:
	Vulkan();
	~Vulkan();

	void Create();
	void Destroy();
private:
	void CreateDevice();
	void DestroyDevice();

private:
	VkInstance myInstance;
	VkDevice myDevice;
	VkPhysicalDevice myGPU;
	VkPhysicalDeviceProperties myGPUProperties {};

	uint32_t myGraphicsFamilyIndex;
};

