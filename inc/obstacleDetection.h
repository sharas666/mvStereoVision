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

    std::vector<std::vector<float>> getDistanceMapMean() const;
    std::vector<Subimage> getSubimages() const;

    void buildMeanDistanceMap(cv::Mat const&);
    void buildMinDistanceMap(cv::Mat const&);
    void buildStdDevDistanceMap(cv::Mat const&);

  private:
    cv::Mat                         mDispMap;
    std::vector<cv::Point>          mSamplepoints;
    std::vector<Subimage>           mSubimages;
    std::vector<std::vector<float>> mDistanceMapMean;
};

#endif //__OBSTACLE_DETECION__H