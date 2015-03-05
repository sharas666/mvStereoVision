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

  double rmsleft = left->calibrate(imagesLeft);
  double rmsright = right->calibrate(imagesRight);

  std::cout <<  rmsleft << std::endl;
  std::cout << rmsright << std::endl;
  
  std::cout << left->getIntrinsic() << std::endl;
  std::cout << right->getIntrinsic() << std::endl;

  return 0;
}