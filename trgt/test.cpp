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

cv::Mat Q, Q_32F;
cv::Mat dMapRaw, dMapNorm;

void drawObstacleGrid(cv::Mat &stream, int binning)
{
  cv::cvtColor(stream,stream,CV_GRAY2BGR);
  if(binning == 0) 
  {
    //vertical lines
    cv::line(stream, cv::Point(250,0), cv::Point(250,stream.rows), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(502,0), cv::Point(502,stream.rows), cv::Scalar(0,0,255), 1);
    //horizontal lines
    cv::line(stream, cv::Point(0,160), cv::Point(stream.cols, 160), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(0,320), cv::Point(stream.cols, 320), cv::Scalar(0,0,255), 1);
  }
  else
  {
    //vertical lines
    cv::line(stream, cv::Point(250/2,0), cv::Point(250/2,stream.rows), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(502/2,0), cv::Point(502/2,stream.rows), cv::Scalar(0,0,255), 1);
    //horizontal lines
    cv::line(stream, cv::Point(0,160/2), cv::Point(stream.cols, 160/2), cv::Scalar(0,0,255), 1);
    cv::line(stream, cv::Point(0,320/2), cv::Point(stream.cols, 320/2), cv::Scalar(0,0,255), 1);
  }
}

void mouseClick(int event, int x, int y,int flags, void* userdata)
{
  if ( event == CV_EVENT_LBUTTONDOWN )
  {
    std::cout << "x: " << x <<"  y: " << y << std::endl;
  }
}

void initWindows()
{
  cv::namedWindow("dMap" ,1);
  cv::setMouseCallback("dMap", mouseClick, NULL);
}

int main(int argc, char const *argv[])
{
  cv::FileStorage fs("disparity.yml",cv::FileStorage::READ);
  fs["Q"] >> Q;
  fs["disparity"] >> dMapRaw;

  Q.convertTo(Q_32F,CV_32F);

  obstacleDetection obst(dMapRaw, 0);
  obst.buildMeanDistanceMap(Q);
  std::vector<std::vector<float>> distances = obst.getDistanceMapMean();

  //built the Subimage 'tree'
  Subimage sub = Subimage(mDispMap, 0);
  sub.subdivide();
  std::vector<Subimage> subimage = sub.getSubdividedImages();
  for (unsigned int i = 0; i < subimage.size(); ++i)
  {
    subimage[i].subdivide();
  }

  subimage[4].getSubdividedImages()[4].getSubMat()


  cv::normalize(dMapRaw,dMapNorm,0,255,cv::NORM_MINMAX, CV_8U);
  initWindows();
  drawObstacleGrid(dMapNorm,0);
  cv::imshow("dMap", dMapNorm);
  cv::waitKey(0);

  return 0;
}