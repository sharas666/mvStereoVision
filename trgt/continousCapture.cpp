#include <iostream>
#include <string>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
	std::string tag = "MAIN\t";

	LOG(INFO) << tag << "Application started.";
	mvIMPACT::acquire::DeviceManager devMgr;

	Camera *left;
	Camera *right;

	if(!Utility::initCameras(devMgr,left,right))
		return 0;


	Stereosystem stereo(left,right);



	if(!stereo.loadIntrinsic("parameter-backup/intrinsic.yml"))
		return 0;
	if(!stereo.loadExtrinisic("parameter-backup/extrinsic.yml"))
		return 0;

	Stereopair s;

	left->setExposure(24000);
	right->setExposure(24000);

	std::string leftFilePath = "continousCapture/left";
	std::string rightFilePath = "continousCapture/right";

	if(!Utility::createDirectory(leftFilePath))
		return 0;

	if(!Utility::createDirectory(rightFilePath))
		return 0;


	char key = 0;
	bool running = true;
	bool capture = false;
	unsigned int imageNumber = 0;
	unsigned int binning = 0;
	cv::Mat currentFrame;
	cv::VideoCapture cap(0);

	if(!cap.isOpened())
	{
		return 0;
	}
	cv::namedWindow("Left" ,1);
	cv::namedWindow("Right" ,1);

	while(running)
	{

		stereo.getRectifiedImagepair(s);
		cv::imshow("Left", s.mLeft);
		cv::imshow("Right", s.mRight);

		key = cv::waitKey(10);

		if(capture)
		{
			std::string prefix = "";
			if(imageNumber < 10)
			{
				prefix +="000";
			}
			else if ((imageNumber >=10) && (imageNumber <100))
			{
				prefix += "00";
			}
			else if(imageNumber >= 100)
			{
				prefix +="0";
			}
			cv::imwrite(std::string(leftFilePath+"/left_"+prefix+std::to_string(imageNumber)+".jpg"),s.mLeft);
			cv::imwrite(std::string(rightFilePath+"/right_"+prefix+std::to_string(imageNumber)+".jpg"),s.mRight);
			++imageNumber;
		}

		if(key == 'q')
		{
			running = false;
			break;
		}
		else if(key == 'b')
		{
		if (binning == 0)
			binning = 1;
		else
			binning =0;

		 left->setBinning(binning);
		 right->setBinning(binning);
		 stereo.resetRectification();
		}
		else if(key == 'f')
		{
		 std::cout << "FPS: Right "<< left->getFramerate() << " " << "Left " << right->getFramerate() << std::endl;
		}
		else if (key == 'c')
		{
			capture = !capture;
			if(capture)
			{
				LOG(INFO) << tag << "Capture enabled" << std::endl;
			}
			else
			{
				LOG(INFO) << tag << "Capture disabled" << std::endl;
			}
		}
	}
	return 0;
}
