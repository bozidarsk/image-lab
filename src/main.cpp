#include "Image.h"
#include "Vulkan.h"

int main(int argc, char** argv)
{
	Vulkan vk;

	Image image("test.png");
	image.Save("test-out.png");
}
