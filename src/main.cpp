#include <iostream>
#include <string>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP

//Thread stuff
std::mutex m;
std::condition_variable condvar;
bool newImage1 = false;
bool newImage2 = false;
bool running = true;

//settings
int minDISP, numDISP, SADWindowSize, smoothnessFactor1, smoothnessFactor2;

bool loadSettings(std::string file)
{
	cv::FileStorage fs;
 	bool success = fs.open(file, cv::FileStorage::READ);
  fs["minDisparity"] >> minDISP;
  fs["numDisparities"] >> numDISP;
  fs["SADWindowSize"] >> SADWindowSize;
  fs["smoothnessFactor1"] >> smoothnessFactor1;
  fs["smoothnessFactor2"] >> smoothnessFactor2; 
  fs.release();
  return success;
}

void disparityView(Stereopair const& s)
{


	if(!loadSettings("settings.yml"))
	{
		std::cout << "failed loading settings" << std::endl;
		return;
	}

	std::cout << SADWindowSize << std::endl;
	std::cout << minDISP << std::endl;
	std::cout << numDISP << std::endl;

	int dispSmoothness1 = smoothnessFactor1*SADWindowSize*SADWindowSize;
	int dispSmoothness2 = smoothnessFactor2*SADWindowSize*SADWindowSize;

	cv::StereoSGBM disparity(minDISP,numDISP,SADWindowSize,dispSmoothness1,dispSmoothness2);
	// cv::StereoSGBM disparity(10,144,11,8*121, 32*121);
	cv::Mat dispMap;

	while(running)	
	{
		std::unique_lock<std::mutex> lck(m);
		while(!newImage1)
			condvar.wait(lck);
		Disparity::sgbm(s, dispMap, disparity);
		cv::normalize(dispMap,dispMap,0,255,cv::NORM_MINMAX, CV_8U);
		cv::imshow("Disparity",dispMap);		
		newImage1=false;
	}

}

void liveView(Stereopair const& s)
{
	while(running)	
	{
		std::unique_lock<std::mutex> lck(m);
		while(!newImage2)
			condvar.wait(lck);
		
		cv::imshow("Left", s.mLeft);
		cv::imshow("Right", s.mRight);
		newImage2=false;
	}
}

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
	
	Stereosystem stereo(left,right);

	int key = 0;
	int frame = 0;
 	int binning = 0;

	if(!stereo.loadIntrinsic("parameter/intrinsic.yml"))
	{
		std::cout << "failed loading intrinsics" << std::endl;
		return 0;
	}
	if(!stereo.loadExtrinisic("parameter/extrinsic.yml"))
	{
		std::cout << "faile loading extrinsic" << std::endl;
		return 0;
	}

	Stereopair s;

	std::thread disp(disparityView,std::ref(s));
	std::thread live(liveView,std::ref(s));

	left->setExposure(24000);
	right->setExposure(24000);

	while(true)
	{
	
		stereo.getRectifiedImagepair(s);
		newImage1 = true;
		newImage2 = true;
		condvar.notify_all();		

		key = cv::waitKey(10);
		
		if(char(key) == 'q')
		{
			running = false;
			break;
		}
		else if(char(key) == 'b')
		{
			if (binning == 0)
				binning = 1;
			else
				binning =0;

			left->setBinning(binning);
			right->setBinning(binning);
			stereo.resetRectification();
		}
		if(frame % 100 == 0)
			std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;

		++frame;

	}
	
	live.join();
	disp.join();

	return 0;
}