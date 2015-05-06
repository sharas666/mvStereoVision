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

  std::vector<std::string> nodes;
  nodes.push_back("inputParameter");

  std::string config = "./configs/default.yml";

  cv::FileStorage fs;


  if(!Utility::checkConfig(config,nodes,fs))
  {
    return 0;
  }


  std::string inputParameter;
  fs["inputParameter"] >> inputParameter;


  if(!stereo.loadIntrinsic(inputParameter+"/intrinsic.yml"))
    return 0;
  if(!stereo.loadExtrinisic(inputParameter +"/extrinsic.yml"))
    return 0;

  left->setExposure(24000);
  right->setExposure(24000);


  char key = 0;
  bool running = true;
  unsigned int binning = 0;

  cv::namedWindow("Left Distorted" ,1);
  cv::namedWindow("Right Distorted" ,1);

  cv::namedWindow("Left Undistorted" ,1);
  cv::namedWindow("Right Undistorted" ,1);

  cv::namedWindow("Left Rectified" ,1);
  cv::namedWindow("Right Rectified" ,1);

  Stereopair distorted;
  Stereopair undistorted;
  Stereopair rectified;



  while(running)
  {

    stereo.getImagepair(distorted);
    stereo.getUndistortedImagepair(undistorted);
    stereo.getRectifiedImagepair(rectified);

    cv::imshow("Left Distorted", distorted.mLeft);
    cv::imshow("Right Distorted", distorted.mRight);

    cv::imshow("Left Undistorted", undistorted.mLeft);
    cv::imshow("Right Undistorted", undistorted.mRight);

    cv::imshow("Left Rectified", rectified.mLeft);
    cv::imshow("Right Rectified", rectified.mRight);

    key = cv::waitKey(10);

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
      cv::imwrite("Left Distorted.jpg",distorted.mLeft);
      cv::imwrite("Right Distorted.jpg",distorted.mRight);

      cv::imwrite("Left Undistorted.jpg",undistorted.mLeft);
      cv::imwrite("Right Undistorted.jpg",undistorted.mRight);

      cv::imwrite("Left Rectified.jpg",rectified.mLeft);
      cv::imwrite("Right Rectified.jpg",rectified.mRight);
    }
  }
  return 0;
}
