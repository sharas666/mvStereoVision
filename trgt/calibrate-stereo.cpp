#include <iostream>
#include <string>
#include <cstdio>

//system command
#include <cstdlib>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char const *argv[])
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
  
  Stereosystem s(left,right);

  std::string dirPath = "capturedImages";
  std::string pathLeft = dirPath+"/left";
  std::string pathRight = dirPath+"/right";

  std::vector<std::string> filenamesLeft;
  std::vector<std::string> filenamesRight;

  Utility::getFiles(pathLeft, filenamesLeft);
  Utility::getFiles(pathRight, filenamesRight);

  std::vector<cv::Mat> imagesLeft, imagesRight;

  if(filenamesLeft.size() == filenamesRight.size()) {
    for(unsigned int i = 0; i < filenamesLeft.size(); ++i){
      imagesLeft.push_back(cv::imread(std::string(pathLeft+"/"+filenamesLeft[i])));
      imagesRight.push_back(cv::imread(std::string(pathRight+"/"+filenamesRight[i])));
    }
  }

  double stereoRMS = s.calibrate(imagesLeft, imagesRight);

  std::cout <<  stereoRMS << std::endl;
  cv::Mat trans;
  s.getTranslationMatrix(trans);
  std::cout << trans << std::endl;

  std::cout << "press 's' to save the new parameters." << std::endl;
  char key = getchar();
  if(key == 's') 
  {
    s.saveIntrinsic("parameter/intrinsic.yml");
    s.saveExtrinsic("parameter/extrinsic.yml");
  }

  return 0;
}