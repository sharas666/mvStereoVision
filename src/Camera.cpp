#include "Camera.h"

Camera::Camera():
	mDevice(NULL)
{}

Camera::Camera(mvIMPACT::acquire::Device* dev):
	mDevice(dev)
{
	std::cout<<"Camera created\n";
	//driver stuff
}

Camera::~Camera()
{
	std::cout<<"Camera destroyed\n";
	//clean up camera stuff
}


std::vector<char> Camera::getImage()
{
	std::cout<<"Image requested\n";
	//image magic
	return std::vector<char>(1,1);
}
