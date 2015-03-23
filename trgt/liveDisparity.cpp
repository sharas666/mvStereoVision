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
std::mutex disparityLock;
std::condition_variable cond_var;
bool newDisparityMap = false;
bool newDisparityMap2 = false;
bool newDisparityMap3 = false;
bool running = true;
cv::Mat dispMap, dispMap2, dispMap3;


void disparityCalc(Stereopair const& s, cv::StereoSGBM &disparity)
{
	while(running)	
	{
		std::unique_lock<std::mutex> ul(disparityLock);
		cond_var.wait(ul);
		Disparity::sgbm(s, dispMap, disparity);
		newDisparityMap=true;
	}
}

void disparityCalcBM( Stereopair const& s, cv::StereoBM &disparity)
{
	while(running)
	{
		std::unique_lock<std::mutex> ul(disparityLock);
		cond_var.wait(ul);
		Disparity::bm(s, dispMap2, disparity);
		newDisparityMap2=true;
	}
}

void disparityCalcNCC( Stereopair const& s) {
	while(running)
	{
		std::unique_lock<std::mutex> ul(disparityLock);
		cond_var.wait(ul);
		Disparity::ncc(s, dispMap3);
		newDisparityMap3=true;
	}
}

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

	int key = 0;
	int frame = 0;
 	int binning = 0;

	if(!stereo.loadIntrinsic("parameter/intrinsic.yml"))
		return 0;
	if(!stereo.loadExtrinisic("parameter/extrinsic.yml"))
		return 0;

	Stereopair s;

	//left->setExposure(24000);
	//right->setExposure(24000);
	
	
	int numDisp = 16;
	int windSize = 5;

	cv::StereoSGBM disparity(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
	cv::StereoBM disparity2(CV_STEREO_BM_BASIC, numDisp, windSize);

	std::thread disp(disparityCalc,std::ref(s),std::ref(disparity));
	std::thread disp2(disparityCalcBM,std::ref(s),std::ref(disparity2));
	std::thread disp3(disparityCalcNCC, std::ref(s));
	
	while(true)
	{
	
		stereo.getRectifiedImagepair(s);
		cv::imshow("Left", s.mLeft);
		cv::imshow("Right", s.mRight);

	
		if(newDisparityMap)
		{
			cv::normalize(dispMap,dispMap,0,255,cv::NORM_MINMAX, CV_8U);
			cv::imshow("Disparity",dispMap);
			newDisparityMap = false;		
		}
		if(newDisparityMap2)
		{
			cv::normalize(dispMap2,dispMap2,0,255,cv::NORM_MINMAX, CV_8U);
			cv::imshow("Disparity2",dispMap2);
			newDisparityMap2 = false;		
		}
		cond_var.notify_one();
	
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
		else if(char(key) == 'i')
		{
			std::unique_lock<std::mutex> ul(disparityLock);
			numDisp +=16;
			disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
			disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);

		}
		else if(char(key) == 'd')
		{
			std::unique_lock<std::mutex> ul(disparityLock);
			if( numDisp > 16)
			{
				numDisp -=16;
				disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
				disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
			}
			
		}
		else if(char(key) == 'e')
		{
			std::unique_lock<std::mutex> ul(disparityLock);
			windSize +=2;
			disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
			disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);

		}
		else if(char(key) == 'r')
		{
			std::unique_lock<std::mutex> ul(disparityLock);
			if( windSize > 2)
			{
			windSize -=2;
			disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
			disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
			}
			
		}
		if(frame % 100 == 0)
			std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;

		++frame;

	}
	
	disp.join();

	return 0;
}