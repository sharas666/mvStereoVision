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
    obstacleDetection(cv::Mat const&, int);
    ~obstacleDetection();

    std::vector<std::vector<float>> getMeanMap() const;
    std::vector<std::vector<float>> getDistanceMapMean() const;
    std::vector<std::vector<float>> getDistanceMapMin() const;
    std::vector<Subimage> getSubimages() const;

    void buildMeanMap(cv::Mat const&);

    void buildMeanDistanceMap(cv::Mat const&);
    void buildMinDistanceMap(cv::Mat const&);
    void buildStdDevDistanceMap(cv::Mat const&);

    void detectObstacles();

  private:
    cv::Mat                         mDispMap;
    int                             mBinning;
    std::vector<cv::Point>          mSamplepoints;
    std::vector<Subimage>           mSubimages;
    std::vector<std::vector<float>> mDistanceMapMean;
    std::vector<std::vector<float>> mDistanceMapMin;
    std::vector<std::vector<float>> mMeanMap;
};

#endif //__OBSTACLE_DETECION__H