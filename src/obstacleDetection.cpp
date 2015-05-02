#include "obstacleDetection.h"
#include "utility.h"

obstacleDetection::obstacleDetection():
  mDispMap(),
  mSamplepoints(),
  mSubimages(),
  mDistanceMapMean()
{}

obstacleDetection::obstacleDetection(cv::Mat const& disparityMap, int const& binning):
  mDispMap(disparityMap),
  mSamplepoints(),
  mSubimages(),
  mDistanceMapMean()
{
  //built the Subimage 'tree'
  Subimage sub = Subimage(mDispMap, 0);
  sub.subdivide();
  mSubimages = sub.getSubdividedImages();
  for (unsigned int i = 0; i < mSubimages.size(); ++i)
  {
    mSubimages[i].subdivide();
  }
}

obstacleDetection::~obstacleDetection()
{}

void obstacleDetection::buildMeanDistanceMap(cv::Mat const& Q)
{
  for (unsigned int i = 0; i < mSubimages.size(); ++i)
  {
    for (unsigned int j = 0; j < mSubimages.size(); ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMeanStdDev().first[0];
      float distance = Utility::calcDistance(Q, mean);
      // std::cout << distance << std::endl;
      
    }
  }
}

void obstacleDetection::buildMinDistanceMap(cv::Mat const& Q)
{

}

void obstacleDetection::buildStdDevDistanceMap(cv::Mat const& Q)
{

}
