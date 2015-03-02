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
	else
	{
		std::cerr << "Wrong camera order. Exit" << std::endl;
		LOG(ERROR) << tag << "Wrong camera order. Exit" << std::endl;
	}
	
	Stereosystem stereo(*(left),*(right));

	int key = 0;
	int frame = 0;
 	int binning = 0;

	while(true)
	{
			std::vector<char> leftImage;
			left->getImage(leftImage);
			std::vector<char> rightImage;
			right->getImage(rightImage);
			cv::Mat leftMat(left->getImageHeight(),left->getImageWidth(), CV_8UC1, &leftImage[0]);
			cv::Mat rightMat(right->getImageHeight(),right->getImageWidth(), CV_8UC1, &rightImage[0]);
			cv::imshow("Left", leftMat);
			cv::imshow("Right", rightMat);
			key = cv::waitKey(1);
			leftMat.release();
			rightMat.release();
			if(char(key) == 'q')
				break;
			else if(char(key) == 'b')
			{
				if (binning == 0)
					binning = 3;
				else
					binning =0;

				left->setBinning(binning);
				right->setBinning(binning);
			}
			if(frame % 100 == 0)
				std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;

			++frame;

	}
	
	std::cout<<"Test"<<std::endl;
	return 0;
}