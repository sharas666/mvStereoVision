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
  nodes.push_back("capturedImagesRectified");

  std::string config = "./configs/default.yml";

  cv::FileStorage fs;


  if(!Utility::checkConfig(config,nodes,fs))
  {
    return 0;
  }


  std::string inputParameter;
  fs["inputParameter"] >> inputParameter;

  std::string outputDirectory;
  fs["capturedImagesRectified"] >> outputDirectory;

  if(!stereo.loadIntrinsic(inputParameter+"/intrinsic.yml"))
    return 0;
  if(!stereo.loadExtrinisic(inputParameter +"/extrinsic.yml"))
    return 0;

  Stereopair s;

  left->setExposure(24000);
  right->setExposure(24000);

  std::string pathLeft = outputDirectory+"/left";
  std::string pathRight = outputDirectory+"/right";

  unsigned int imageNumber = 0;

  if(Utility::directoryExist(pathLeft) || Utility::directoryExist(pathRight))
  {
    std::vector<std::string> tmp1, tmp2;
    Utility::getFiles(pathLeft,tmp1);
    Utility::getFiles(pathRight,tmp2);

    if(tmp1.size() != 0 || tmp2.size() != 0)
    {
      std::cout << "Output directory not empty, clean files? [y/n] " <<std::endl;
      char key = getchar();
        if(key == 'y')
        {
          std::system(std::string("rm " + pathLeft + "/* -f ").c_str());
          std::system(std::string("rm " + pathRight + "/* -f ").c_str());
        }
        else if(key == 'n')
        {
          imageNumber = tmp1.size();
          LOG(INFO) << tag << "Start with image number " << imageNumber << std::endl;
        }
    }
  }

  if(Utility::createDirectory(pathLeft) && Utility::createDirectory(pathRight))
    {
            LOG(INFO) << tag << "Successfully created directories for captured images." << std::endl;
    }
    else
    {
        LOG(ERROR) << tag << "Unable to create directories for captured images." <<std::endl;
        return 0;
    }


  char key = 0;
  bool running = true;
  unsigned int binning = 0;

  cv::namedWindow("Left" ,1);
  cv::namedWindow("Right" ,1);

  while(running)
  {

    stereo.getRectifiedImagepair(s);
    cv::imshow("Left", s.mLeft);
    cv::imshow("Right", s.mRight);

    key = cv::waitKey(10);

    if(key == 'c')
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
      cv::imwrite(std::string(pathLeft+"/left_"+prefix+std::to_string(imageNumber)+".jpg"),s.mLeft);
      cv::imwrite(std::string(pathRight+"/right_"+prefix+std::to_string(imageNumber)+".jpg"),s.mRight);
      ++imageNumber;
    }
    else if(key == 'q')
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
  }
  return 0;
}
