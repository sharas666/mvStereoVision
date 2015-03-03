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

void disparityView(Stereopair const& s)
{
	cv::StereoSGBM disparity(0,128,7,8*49,32*49);
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
		return 0;
	if(!stereo.loadExtrinisic("parameter/extrinsic.yml"))
		return 0;

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