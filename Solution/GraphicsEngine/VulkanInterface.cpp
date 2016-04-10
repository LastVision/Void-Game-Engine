#include "stdafx.h"

#include "VulkanInterface.h"
#include <vector>

Vulkan::Vulkan()
	: myInstance(nullptr)
	, myDevice(nullptr)
	, myGPU(nullptr)
	, myGraphicsFamilyIndex(0)
{
}


Vulkan::~Vulkan()
{
}


void Vulkan::Create()
{
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.apiVersion = VK_API_VERSION;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	applicationInfo.pApplicationName = "VGE Aplication";
	applicationInfo.pEngineName = "Void Game Engine";

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;

	VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &myInstance);
	if (result != VK_SUCCESS)
	{
		DL_ASSERT("Failed to create vulkan instance.");
	}
	CreateDevice();
}

void Vulkan::Destroy()
{
	vkDestroyInstance(myInstance, nullptr);
	myInstance = nullptr;
	DestroyDevice();
}

void Vulkan::CreateDevice()
{
	{
		uint32_t gpu_count = 0;
		vkEnumeratePhysicalDevices(myInstance, &gpu_count, nullptr);
		std::vector<VkPhysicalDevice> gpuList(gpu_count);
		vkEnumeratePhysicalDevices(myInstance, &gpu_count, gpuList.data());
		myGPU = gpuList[0];
		vkGetPhysicalDeviceProperties(myGPU, &myGPUProperties);
	}
	{
		uint32_t family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(myGPU, &family_count, nullptr);
		std::vector<VkQueueFamilyProperties> familyPropertyList(family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(myGPU, &family_count, familyPropertyList.data());

		bool foundGraphicBit = false;
		for (uint32_t i = 0; i < family_count; ++i)
		{
			if (familyPropertyList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				foundGraphicBit = true;
				myGraphicsFamilyIndex = i;
			}
		}
		DL_ASSERT_EXP(foundGraphicBit == true, "Vulkan Error: Queue family supporting graphics not found.");
	}
	float queuePriorities[] {1.0f};
	VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueFamilyIndex = myGraphicsFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

	VkResult result = vkCreateDevice(myGPU, &deviceCreateInfo, nullptr, &myDevice);
	if (result != VK_SUCCESS)
	{
		DL_ASSERT("Vulkan Error: Failed to create device.");
	}
}

void Vulkan::DestroyDevice()
{
	vkDestroyDevice(myDevice, nullptr);
	myDevice = nullptr;
}