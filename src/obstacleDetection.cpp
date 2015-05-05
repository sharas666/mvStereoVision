#include "obstacleDetection.h"
#include "utility.h"

obstacleDetection::obstacleDetection():
  mDispMap(),
  mSamplepoints(),
  mSubimages(),
  mDistanceMapMean(),
  mMeanMap()
{}

obstacleDetection::obstacleDetection(cv::Mat const& disparityMap, int const& binning):
  mDispMap(disparityMap),
  mSamplepoints(),
  mSubimages(),
  mDistanceMapMean(),
  mMeanMap()
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


std::vector<std::vector<float>> obstacleDetection::getMeanMap() const
{
  return mMeanMap;
}

std::vector<std::vector<float>> obstacleDetection::getDistanceMapMean() const
{
  return mDistanceMapMean;
}

std::vector<Subimage> obstacleDetection::getSubimages() const
{
  return mSubimages;
}


void obstacleDetection::buildMeanMap(cv::Mat const& Q)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> meanStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMean();
      meanStorage.push_back(mean);
    }
  }
  std::vector<float> temp;
  for (unsigned int i = 0; i < meanStorage.size(); ++i)
  {
    temp.push_back(meanStorage[i]);
    if (i % 9 == 0)
    {
      mMeanMap.push_back(temp);
      temp.clear();
    }
  }
}
 


void obstacleDetection::buildMeanDistanceMap(cv::Mat const& Q,int binning)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> distanceStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMean();
      float distance = Utility::calcDistance(Q, mean,binning);
      distanceStorage.push_back(distance);
    }
    mDistanceMapMean.push_back(distanceStorage);
    distanceStorage.clear();
  }
}


void obstacleDetection::buildMinDistanceMap(cv::Mat const& Q, int binning)
{
  unsigned int numSubimages = mSubimages.size();
  std::vector<float> distanceStorage;

  for (unsigned int i = 0; i < numSubimages; ++i)
  {
    for (unsigned int j = 0; j < numSubimages; ++j)
    {
      float mean = mSubimages[i].getSubdividedImages()[j].calcMeanStdDev().first[0];
      float distance = Utility::calcDistance(Q, mean, binning);
      distanceStorage.push_back(distance);
    }
  }
}

void obstacleDetection::buildStdDevDistanceMap(cv::Mat const& Q)
{

}
