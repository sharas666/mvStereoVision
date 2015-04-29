#include "obstacleDetection.h"

obstacleDetection::obstacleDetection():
  mDispMap()
{}

obstacleDetection::obstacleDetection(cv::Mat const& disparityMap, int const& binning):
  mDispMap(disparityMap)
{}

obstacleDetection::~obstacleDetection()
{}
