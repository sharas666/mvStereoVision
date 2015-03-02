#include <iostream>
#include <string>

#include "Stereosystem.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
	std::string tag = "MAIN\t";

	LOG(INFO) << tag << "Application started.";
	mvIMPACT::acquire::DeviceManager devMgr;    
		
	int order = Utility::initCameras(devMgr);
	std::cout<< order<<std::endl;
	if(order < 0)
	{
		std::cerr <<"Error in camera initialization" << std::endl;
		LOG(ERROR)<< tag <<"Error in camera initialization" << std::endl;
	}
	Camera *left,*right;

	if(order == 0)
	{
		left = new Camera(devMgr[0]);
		right= new Camera(devMgr[1]);	
	}
	else if (order == 1)
	{
		left = new Camera(devMgr[1]);
		right = new Camera(devMgr[0]);
	}
	
	Stereosystem stereo(*(left),*(right));

	std::vector<char> test = left->getImage();


	std::cout<<"Test"<<std::endl;
	return 0;
}