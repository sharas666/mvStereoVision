#ifndef __OBSTACLE_DETECION__H
#define __OBSTACLE_DETECION__H

#include <vector>
#include <iostream>

//OPENCV Stuff
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "subimage.h"

class obstacleDetection
{
  public:
    obstacleDetection();
    obstacleDetection(cv::Mat const&, int const&);
    ~obstacleDetection();

  private:
    cv::Mat                 mDispMap;
    std::vector<cv::Point>  mSamplePoints;
    
};

#endif //__OBSTACLE_DETECION__H