#include <iostream>
#include <string>

//system command
#include <cstdlib>

#include "Stereosystem.h"
#include "disparity.h"
#include "easylogging++.h"
#include "subimage.h"
#include "obstacleDetection.h"

#include <thread>
#include <mutex>
#include <condition_variable>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char const *argv[])
{
  
  cv::Mat dMapRaw, dMapNorm, Q;

  cv::FileStorage fs("disparity.yml",cv::FileStorage::READ);
  fs["Q"] >> Q;
  fs["disparity"] >> dMapRaw;

  obstacleDetection obst(dMapRaw, 0);
  obst.buildMeanDistanceMap(Q);

  std::vector<Subimage> subvec;
  subvec = obst.getSubimages();

  cv::Scalar meanCV = subvec[4].getSubdividedImages()[4].calcMeanStdDev().first;
  float meanSelf = subvec[4].getSubdividedImages()[4].calcMean();

  std::cout << "meanCV:   " << meanCV(0) << std::endl;
  std::cout << "meanSelf: " << meanSelf << std::endl;

  cv::normalize(dMapRaw,dMapNorm,0,255,cv::NORM_MINMAX, CV_8U);
  cv::imshow("dMap", dMapNorm);
  cv::waitKey(0);

  return 0;
}