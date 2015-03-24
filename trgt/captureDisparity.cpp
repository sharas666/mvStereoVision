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
std::mutex disparityLockSGBM,disparityLockBM;
std::condition_variable cond_var;
bool newDisparityMap = false;
bool newDisparityMap2 = false;
bool newDisparityMap3 = false;
bool running = true;
cv::Mat dispMapSGBM, dispMapBM, dispMapNCC;


void disparityCalc(Stereopair const& s, cv::StereoSGBM &disparity)
{
	while(running)
	{
		std::unique_lock<std::mutex> ul(disparityLockSGBM);
		cond_var.wait(ul);
		Disparity::sgbm(s, dispMapSGBM, disparity);
		newDisparityMap=true;
	}
}

void disparityCalcBM( Stereopair const& s, cv::StereoBM &disparity)
{
	while(running)
	{
		std::unique_lock<std::mutex> ul(disparityLockBM);
		cond_var.wait(ul);
		Disparity::bm(s, dispMapBM, disparity);
		newDisparityMap2=true;
	}
}

// void disparityCalcNCC( Stereopair const& s) {
// 	while(running)
// 	{
// 		std::unique_lock<std::mutex> ul(disparityLock);
// 		cond_var.wait(ul);
// 		Disparity::ncc(s, dispMapNCC);
// 		newDisparityMap3=true;
// 	}
// }

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


	std::vector<std::string> nodes;
  nodes.push_back("inputParameter");
  nodes.push_back("capturedDisparities");

  std::string config = "./configs/default.yml";

  cv::FileStorage fs;


  if(!Utility::checkConfig(config,nodes,fs))
  {
    return 0;
  }


  std::string inputParameter;
  fs["inputParameter"] >> inputParameter;

  std::string outputDirectory;
  fs["capturedDisparities"] >> outputDirectory;

int imageNumber = 0;

if(Utility::directoryExist(outputDirectory))
 	{
 		std::vector<std::string> tmp1;
 		Utility::getFiles(outputDirectory,tmp1);

 		if(tmp1.size() != 0)
 		{
 			std::cout << "Output directory not empty, clean files? [y/n] " <<std::endl;
 			char key = getchar();
  			if(key == 'y')
  			{
  				std::system(std::string("rm " + outputDirectory + "/* -f ").c_str());
  			}
  			else if(key == 'n')
  			{
  				imageNumber = tmp1.size();
  				LOG(INFO) << tag << "Start with image number " << imageNumber << std::endl;
  			}
 		}
 	}



  if(Utility::createDirectory(outputDirectory))
    {
            LOG(INFO) << tag << "Successfully created directory for captured disparity maps." << std::endl;
    }
    else
    {
        LOG(ERROR) << tag << "Unable to create directoryfor captured disparity maps." <<std::endl;
        return 0;
    }


	Stereosystem stereo(left,right);


  if(!stereo.loadIntrinsic(inputParameter+"/intrinsic.yml"))
    return 0;
  if(!stereo.loadExtrinisic(inputParameter +"/extrinsic.yml"))
    return 0;



	int key = 0;
 	int binning = 1;
	Stereopair s;

	left->setBinning(binning);
	right->setBinning(binning);
	stereo.resetRectification();

	int numDisp = 16;
	int windSize = 5;

	cv::StereoSGBM disparity(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
	cv::StereoBM disparity2(CV_STEREO_BM_BASIC, numDisp, windSize);

	std::thread disp(disparityCalc,std::ref(s),std::ref(disparity));
	std::thread disp2(disparityCalcBM,std::ref(s),std::ref(disparity2));
	//std::thread disp3(disparityCalcNCC, std::ref(s));
	cv::Mat normalizedSGBM;
	cv::Mat normalizedBM;
	while(running)
	{

		stereo.getRectifiedImagepair(s);
		cv::imshow("Left", s.mLeft);
		cv::imshow("Right", s.mRight);


		if(newDisparityMap)
		{
			cv::normalize(dispMapSGBM,normalizedSGBM,0,255,cv::NORM_MINMAX, CV_8U);
			cv::imshow("Disparity",normalizedSGBM);
			newDisparityMap = false;
		}
		if(newDisparityMap2)
		{
			cv::normalize(dispMapBM,normalizedBM,0,255,cv::NORM_MINMAX, CV_8U);
			cv::imshow("Disparity2",normalizedBM);
			newDisparityMap2 = false;
		}
		cond_var.notify_one();

		key = cv::waitKey(10);

		if(char(key) == 'q')
		{
			running = false;
			cond_var.notify_one();
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
			std::unique_lock<std::mutex> ulSGBM(disparityLockSGBM);
			std::unique_lock<std::mutex> ulBM(disparityLockBM);
			numDisp +=16;
			disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
			disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);

		}
		else if(char(key) == 'd')
		{
			std::unique_lock<std::mutex> ulSGBM(disparityLockSGBM);
			std::unique_lock<std::mutex> ulBM(disparityLockBM);
			if( numDisp > 16)
			{
				numDisp -=16;
				disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
				disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
			}

		}
		else if(char(key) == 'e')
		{
			std::unique_lock<std::mutex> ulSGBM(disparityLockSGBM);
			std::unique_lock<std::mutex> ulBM(disparityLockBM);
			windSize +=2;
			disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
			disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);

		}
		else if(char(key) == 'r')
		{
			std::unique_lock<std::mutex> ulSGBM(disparityLockSGBM);
			std::unique_lock<std::mutex> ulBM(disparityLockBM);
			if( windSize > 5)
			{
			windSize -=2;
			disparity = cv::StereoSGBM(0,numDisp,windSize,8*windSize*windSize,32*windSize*windSize);
			disparity2 = cv::StereoBM (CV_STEREO_BM_BASIC, numDisp, windSize);
			}

		}
		else if (char(key) == 'c')
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

			{
				std::unique_lock<std::mutex> ul(disparityLockSGBM);
				cv::normalize(dispMapSGBM,normalizedSGBM,0,255,cv::NORM_MINMAX, CV_8U);
				cv::imwrite(std::string(outputDirectory+"/sgbm_raw_"+prefix+std::to_string(imageNumber)+".jpg"),dispMapSGBM);
				cv::imwrite(std::string(outputDirectory+"/sgbm_norm_"+prefix+std::to_string(imageNumber)+".jpg"),normalizedSGBM);
			}

			{
				std::unique_lock<std::mutex> ul(disparityLockBM);
				cv::normalize(dispMapBM,normalizedBM,0,255,cv::NORM_MINMAX, CV_8U);
				cv::imwrite(std::string(outputDirectory+"/bm_raw_"+prefix+std::to_string(imageNumber)+".jpg"),dispMapBM);
				cv::imwrite(std::string(outputDirectory+"/bm_norm_"+prefix+std::to_string(imageNumber)+".jpg"),normalizedBM);
			}


			++imageNumber;
		}
		else if(char(key) == 'f')
			std::cout<<left->getFramerate()<<" "<<right->getFramerate()<<std::endl;

	}

	disp.join();
	disp2.join();

	return 0;
}
