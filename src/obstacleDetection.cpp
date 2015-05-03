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

std::vector<Subimage> obstacleDetection::getSubimages() const {
  return mSubimages;
}

std::vector<std::vector<float>> obstacleDetection::getDistanceMapMean() const
{
  return mDistanceMapMean;
}


void obstacleDetection::buildMeanDistanceMap(cv::Mat const& Q)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> distanceStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMeanStdDev().first[0];
      float distance = Utility::calcDistance(Q, mean);
      distanceStorage.push_back(distance);
    }
  }
  std::vector<float> temp;
  for (unsigned int i = 0; i < distanceStorage.size(); ++i)
  {
    temp.push_back(distanceStorage[i]);
    if (i % 9 == 0)
    {
      mDistanceMapMean.push_back(temp);
      temp.clear();
    }
  }
}

void obstacleDetection::buildMinDistanceMap(cv::Mat const& Q)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> distanceStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMeanStdDev().first[0];
      float distance = Utility::calcDistance(Q, mean);
      distanceStorage.push_back(distance);
    }
  }
}

void obstacleDetection::buildStdDevDistanceMap(cv::Mat const& Q)
{

}
