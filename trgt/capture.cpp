#include <iostream>
#include <string>

//system command
#include <cstdlib>

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
	{
		return 0;
	}
	
	
	Stereosystem stereo(left,right);


	if(!stereo.loadIntrinsic("parameter/intrinsic.yml"))
		return 0;
	if(!stereo.loadExtrinisic("parameter/extrinsic.yml"))
		return 0;

	Stereopair s;

	
	std::string dirPath = "capturedImages";
	std::string pathLeft = dirPath+"/left";
	std::string pathRight = dirPath+"/right";

	std::system(std::string("mkdir -p " + pathLeft).c_str());
	std::system(std::string("mkdir -p " + pathRight).c_str());
	
	if(Utility::directoryExist(pathLeft) && Utility::directoryExist(pathRight))
	{
			LOG(INFO) << tag << "Successfully created directories for captured images." << std::endl; 

	}
	else
	{
		LOG(ERROR) << tag << "Unable to create directories for captured images." <<std::endl;
		return 0;
	}

	left->setExposure(18000);
	right->setExposure(18000);

	char key = 0;
 	int binning = 0;
	int imageCounter = 0;
	cv::namedWindow("Left", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Right", cv::WINDOW_AUTOSIZE);
	bool running = true;
	std::string filename = "";
	while(running)
	{
	
		if(!stereo.getImagepair(s))
		{
			break;
		}
		cv::imshow("Left", s.mLeft);
		cv::imshow("Right", s.mRight);
	
		key = cv::waitKey(5);
		
		if(key > 0)
		{
			switch(key)
			{
				case 'q':
					running = false;
					LOG(INFO) << tag << "Exit requested" <<std::endl;
					delete left;
					left = NULL;

					delete right;
					right = NULL;
					break;
				case 'b':
					if (binning == 0)
						binning = 1;
					else
						binning =0;

					left->setBinning(binning);
					right->setBinning(binning);
					stereo.resetRectification();
					break;
				case 'c':
					filename = "";
					if(imageCounter < 10)
					{
						filename+= "00" + std::to_string(imageCounter);
					}
					if(imageCounter >= 10 && imageCounter < 100)
					{
						filename+="0" + std::to_string(imageCounter);
					}

					cv::imwrite(pathLeft+"/left_"+filename+".jpg",s.mLeft);
					cv::imwrite(pathRight+"/right_"+filename+".jpg",s.mRight);
					LOG(INFO) << tag << "Wrote left image to " << std::string(pathLeft+"/left_"+filename+".jpg") <<std::endl;
					LOG(INFO) << tag << "Wrote right image to " << std::string(pathRight+"/right_"+filename+".jpg") <<std::endl;
					++imageCounter;
					break;
				case 'f':
					std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;
					break;
				default:
					std::cout << "Key pressed has no action" <<std::endl;
					break;
			}	
		}
	}
	

	return 0;
}